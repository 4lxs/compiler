#pragma once

#include <concepts>
#include <map>
#include <vector>

#include "fwd_decl.hpp"
#include "spdlog/spdlog.h"
#include "x/ast/decl.hpp"
#include "x/ast/fwd_decl.hpp"
#include "x/ast/type.hpp"
#include "x/pt/expr.hpp"
#include "x/pt/fwd_decl.hpp"

namespace x::sema {

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

  [[nodiscard]] ast::Decl *get_single() const;

  ResultType _kind = ResultType::NotFound;

  std::vector<ast::Decl *> _decls;
};

/// represents a collection of named items that you can lookup
/// it's single level, but it's item can be itself a namespace
///
/// in language, a namespace may be a module, a class, a scope, ... anything
/// that may contain named items
///
/// this class is intended to be subclassed
class Namespace {
  using Item = std::variant<ast::Decl *>;

 public:
  [[nodiscard]] Ptr<LookupResult> lookup(std::span<std::string> path) const;

  [[nodiscard]] Ptr<LookupResult> lookup(std::string &name) const {
    return lookup({&name, 1});
  };

  void insert(ast::Decl *val) { _items.insert({val->name(), val}); }

  /// explicitly don't infer name from decl
  void insert(std::string_view name, ast::Decl *val) {
    auto [itr, inserted] = _items.insert({name, val});
    if (!inserted) {
      throw std::runtime_error("inserted duplicate");
    }
  }

  [[nodiscard]] size_t size() const { return _items.size(); };

 private:
  std::map<std::string_view, ast::Decl *> _items;
};

std::string_view mangle_name(ast::Decl *decl);

class SymbolTable {
 public:
 private:
 public:
  [[nodiscard]] ast::ValueDecl *resolve_decl(not_null<pt::DeclRef *> var) {
    auto *ref = resolve<ast::ValueDecl>(var);
    spdlog::info("resolved decl {}. as {}", ref->name(),
                 fmt::underlying(ref->get_kind()));
    return ref;
  }

  [[nodiscard]] ast::Type *resolve_type(not_null<pt::DeclRef *> var) {
    return resolve<ast::Type>(var);
  }

  void add(not_null<ast::Decl *> decl) {
    spdlog::info("adding decl {} {}.", decl->name(),
                 fmt::underlying(decl->get_kind()));
    _items.push_back(decl);
  }

  void scope_begin() {
    spdlog::info("adding scope.");
    _items.push_back(nullptr);
  }

  void scope_end() {
    spdlog::info("removing scope");
    auto itr = std::find(_items.rbegin(), _items.rend(), nullptr);
    _items.erase(itr.base(), _items.end());
  }

 private:
  [[nodiscard]] Ptr<LookupResult> lookup(not_null<pt::DeclRef *> var) const;
  [[nodiscard]] LookupResult lookup(std::string_view name) const;

  template <typename T>
    requires std::derived_from<T, ast::Decl>
  [[nodiscard]] T *resolve(not_null<pt::DeclRef *> declref) {
    Ptr<LookupResult> ident = lookup(declref);
    if (auto *ref = llvm::dyn_cast<T>(ident->get_single())) {
      return ref;
    }
    throw std::runtime_error("unexpected");
  }

  std::vector<ast::Decl *> _items;
};

};  // namespace x::sema
