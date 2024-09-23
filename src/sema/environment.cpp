#include "x/sema/environment.hpp"

#include <llvm/Support/Casting.h>

#include <cassert>

#include "spdlog/spdlog.h"
#include "x/ast/type.hpp"
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

void SymbolTable::add_struct_method(not_null<ast::StructTy *> type,
                                    not_null<ast::FnDecl *> method) {
  spdlog::info("adding method {} to {}.", method->name(), type->name());
  _methods[type].push_back(method);
}

not_null<ast::FnDecl *> SymbolTable::get_struct_method(
    not_null<ast::StructTy *> type) const {
  spdlog::info("getting method for {}.", type->name());
  return _methods.at(type).front();
}

}  // namespace x::sema
