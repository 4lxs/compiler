#include "x/ast/decl.hpp"

#include "x/ast/expr.hpp"

namespace x::ast {

FnDecl::FnDecl(std::string_view name, std::vector<Param>&& params, Rc<Type> ret,
               Ptr<Block> block, std::vector<Rc<ast::Decl>> localvars)
    : ValueDecl(DeclKind::Fn, name, nullptr /* TODO */),
      _params(std::move(params)),
      _ret(std::move(ret)),
      _block(std::move(block)),
      _localvars(std::move(localvars)) {}

}  // namespace x::ast
