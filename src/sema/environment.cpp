#include "x/sema/environment.hpp"

#include <cassert>

#include "spdlog/spdlog.h"
#include "x/pt/expr.hpp"

namespace x::sema {

ast::NamedDecl Environment::resolve(pt::DeclRef *var) const {
  assert(var->_var._components.size() == 1);
  std::string_view name = var->_var._components.front();
  spdlog::info("resolving variable {}", name);
  auto itr = std::ranges::find_if(
      _decls, [&](Ident const &decl) { return decl.name == name; });

  if (itr == _decls.end()) {
    spdlog::error("undeclared variable {}", name);
    std::terminate();
  }

  return itr->decl;
}

}  // namespace x::sema
