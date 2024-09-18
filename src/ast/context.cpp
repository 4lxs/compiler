#include "x/ast/context.hpp"

#include "x/ast/expr.hpp"
#include "x/ast/type.hpp"

namespace x::ast {

Context::Context()
    : _strTy{ast::Type::Create(*this, ast::Type::Kind::String)},
      _int32Ty{ast::Type::Create(*this, ast::Type::Kind::I32)},
      _int64Ty{ast::Type::Create(*this, ast::Type::Kind::I64)},
      _boolTy{ast::Type::Create(*this, ast::Type::Kind::Bool)},
      _voidTy{ast::Type::Create(*this, ast::Type::Kind::Void)},
      _voidExpr{ast::Expr::Alloc::Create(*this, ast::Stmt::StmtKind::SK_Expr,
                                         _voidTy)} {}

};  // namespace x::ast
