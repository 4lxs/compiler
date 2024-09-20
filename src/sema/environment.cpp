#include "x/sema/environment.hpp"

#include <cassert>

#include "spdlog/spdlog.h"
#include "x/pt/expr.hpp"

namespace x::sema {

not_null<ast::Decl *> SymbolTable::resolve(not_null<pt::DeclRef *> var) const {
  assert(var->_var._components.size() == 1);
  std::string_view name = var->_var._components.front();
  spdlog::info("resolving variable {}", name);
  auto itr = std::find_if(_decls.rbegin(), _decls.rend(),
                          [&](Ident const &decl) { return decl.name == name; });

  if (itr == _decls.rend()) {
    spdlog::error("undeclared variable {}", name);
    std::terminate();
  }

  return itr->decl;
}

}  // namespace x::sema
