#pragma once

#include <cstdint>
#include <gsl/assert>
#include <limits>
#include <ranges>
#include <string>
#include <unordered_set>
#include <variant>
#include <vector>

#include "x/pt/fwd_decl.hpp"
#include "x/pt/node.hpp"

namespace x::pt::sema {

class NameRef {
  friend class NameResolver;
  friend class OptNameRef;
  friend class Name;

  enum class Scope : uint8_t {
    Invalid,
    Global,
    Local,
  };

  explicit NameRef(uint32_t id, Scope scope) : _scope(scope), _id(id) {}

  bool operator==(NameRef const &other) const = default;

  Scope _scope;
  uint32_t _id;
};

class OptNameRef {
 public:
  OptNameRef() : _name{0, NameRef::Scope::Invalid} {}
  explicit OptNameRef(NameRef name) : _name(name) {}

  OptNameRef &operator=(NameRef name) {
    _name = name;
    return *this;
  }

  [[nodiscard]] bool has_value() const {
    return _name._scope != NameRef::Scope::Invalid;
  }

  [[nodiscard]] NameRef value() const {
    Expects(has_value());
    return _name;
  }

 private:
  NameRef _name;
};

class LookupResult {
 public:
  enum class ResultType {
    /// didn't find shit
    NotFound,

    /// found a single item
    Found,

    /// found multiple function items that you need to disambiguate
    Overloaded,

    /// found multiple items with the same name that you cannot disambiguate
    Ambiguous,
  };

  [[nodiscard]] NodeId get_single() const;

  [[nodiscard]] bool found() const { return _kind != ResultType::NotFound; }

  ResultType _kind = ResultType::NotFound;

  std::vector<NameRef> _decls;
};

class Name {
 public:
  [[nodiscard]] std::string_view str() const { return _name; }

  [[nodiscard]] pt::NodeId definition() const {
    if (!_definition.has_value()) {
      xerr("no definition for {}", _name);
    }
    return _definition.value();
  }

  [[nodiscard]] NameRef ref() const { return _ref; }

  bool operator==(Name const &other) const { return other._ref == _ref; }
  bool operator==(std::string_view const &other) const {
    return other == _name;
  }

 private:
  friend class NameResolver;
  Name(NameRef ref, std::string_view name, std::optional<pt::NodeId> node,
       OptNameRef parent = OptNameRef())
      : _parent(parent), _name(name), _definition(node), _ref(ref) {}

  /// the namespace that this item belongs to. e.g. for method Struct::m(),
  /// this field would be pointer to name of Struct
  OptNameRef _parent;

  /// the name of the item. owned by pt
  std::string_view _name;

  std::optional<pt::NodeId> _definition;

  NameRef _ref;
};

struct NameHash {
  using hash_type = std::hash<std::string_view>;
  using is_transparent = void;

  std::size_t operator()(char const *str) const { return hash_type{}(str); }
  std::size_t operator()(std::string_view str) const {
    return hash_type{}(str);
  }
  std::size_t operator()(std::string const &str) const {
    return hash_type{}(str);
  }
  std::size_t operator()(Name const &name) const {
    return hash_type{}(name.str());
  }
};

class NameResolver {
  using Scope = std::monostate;
  using Entry = std::variant<Scope, Name>;

 public:
  explicit NameResolver(Context *ctx) : _ctx(ctx) {}

  [[nodiscard]] Name const &get_name(NameRef ref) const;

  void resolve(pt::NodeId node);

  Name const &define_name(Decl &decl);

  Name const &use_name(std::string const &name);

  void enter_scope() {
    spdlog::info("enter scope");
    _names.emplace_back(Scope());
  }

  void exit_scope() {
    spdlog::info("exit scope");
    auto itr = std::find_if(_names.rbegin(), _names.rend(), [](auto const &e) {
      return std::holds_alternative<Scope>(e);
    });
    _names.erase(itr.base(), _names.end());
  }

  Context *_ctx;

 private:
  [[nodiscard]] Name &get_name(NameRef ref);

  [[nodiscard]] OptNameRef lookup(std::string_view name, bool local) const {
    std::vector<NameRef> found;

    for (Entry const &item : _names | std::views::reverse) {
      if (std::holds_alternative<Scope>(item)) {
        if (!found.empty() || local) {
          // end of scope in which we found a match
          break;
        }
        continue;
      }
      Name const &nameItem = std::get<Name>(item);
      if (nameItem.str() == name) {
        found.push_back(nameItem.ref());
      }
    }

    if (found.size() == 1) {
      // return LookupResult(LookupResult::ResultType::Found, std::move(found));
      return OptNameRef(found.front());
    }

    if (!found.empty()) {
      for (NameRef decl : found) {
        spdlog::info("found multiple decls with name {}", get_name(decl).str());
      }

      xerr("unimplemented overloading");
    }

    if (auto itr = _globals.find(name); itr != _globals.end()) {
      return OptNameRef(itr->ref());
    }

    spdlog::warn("not found {} in symbol table", name);
    return {};
  }

  /// nullptr represents start of scope. first item is always a scope so that we
  /// can check if definition is in scope or global
  std::vector<Entry> _names;

  std::unordered_set<Name, NameHash, std::equal_to<>> _globals;

  /// namespaced names. e.g. in Struct.a Struct would be in _names and a would
  /// be in _nsNames
  std::vector<Name> _nsNames;
};

}  // namespace x::pt::sema
