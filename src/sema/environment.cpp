#include "x/sema/environment.hpp"

#include <llvm/Support/Casting.h>

#include <cassert>

#include "spdlog/spdlog.h"
#include "x/ast/type.hpp"
#include "x/pt/expr.hpp"

namespace x::sema {

auto SymbolTable::resolve(not_null<pt::DeclRef *> var) const -> Ident {
  assert(var->_var._components.size() == 1);
  std::string_view name = var->_var._components.front();
  spdlog::info("resolving variable {}", name);
  auto itr = std::find_if(_env.rbegin(), _env.rend(),
                          [&](Ident const &decl) { return decl.name == name; });

  if (itr == _env.rend()) {
    spdlog::error("undeclared variable {}", name);
    std::terminate();
  }

  return *itr;
}

}  // namespace x::sema
