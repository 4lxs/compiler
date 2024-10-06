#include "x/pt/sema/nameresolution.hpp"

#include "x/pt/decl.hpp"

namespace x::pt::sema {

Name const &NameResolver::get_name(NameRef ref) const {
  if (ref._scope == NameRef::Scope::Local) {
    spdlog::info("get_name: {} of {}", ref._id, _names.size());
    return std::get<Name>(_names.at(ref._id));
  }

  auto isRef = [ref](Name const &name) { return name._ref._id == ref._id; };
  auto itr = std::ranges::find_if(_globals, isRef);
  if (itr == _globals.end()) {
    xerr("invalid nameref");
  }
  return *itr;
}

Name const &NameResolver::define_name(Decl &decl) {
  spdlog::info("defining: {}", decl.name());
  OptNameRef res = lookup(decl.name(), true);

  if (res.has_value()) {
    Name &name = get_name(res.value());
    if (name._definition.has_value()) {
      xerr("redefinition of item {}", decl.name());
    }

    name._definition = decl.id();
    return name;
  }

  if (_names.empty()) {
    NameRef ref(uint32_t(_globals.size()), NameRef::Scope::Global);
    // decl._resolvedName = ref;

    auto [itr, _] = _globals.insert(Name(ref, decl._name, decl.id()));
    return *itr;
  }
  NameRef ref(uint32_t(_names.size()), NameRef::Scope::Local);
  // decl._resolvedName = ref;

  _names.emplace_back(Name(ref, decl._name, decl.id()));
  return std::get<Name>(_names.back());
}

Name const &NameResolver::use_name(std::string const &name) {
  OptNameRef res = lookup(name, false);
  if (res.has_value()) {
    return get_name(res.value());
  }

  NameRef ref(uint32_t(_globals.size()), NameRef::Scope::Global);
  auto [itr, _] = _globals.insert(Name(ref, name, std::nullopt));

  return *itr;
}

Name &NameResolver::get_name(NameRef ref) {
  return const_cast<Name &>(std::as_const(*this).get_name(ref));
}

}  // namespace x::pt::sema
