#include "x/ast/context.hpp"

#include "x/ast/expr.hpp"
#include "x/ast/type.hpp"

namespace x::ast {

Context::Context()
    : _int32Ty{ast::Type::Create(*this, ast::Type::Kind::I32, "i32")},
      _int64Ty{ast::Type::Create(*this, ast::Type::Kind::I64, "i64")},
      _boolTy{ast::Type::Create(*this, ast::Type::Kind::Bool, "bool")},
      _voidTy{ast::Type::Create(*this, ast::Type::Kind::Void, "__void")},
      _voidExpr{ast::Expr::Alloc::Create(*this, ast::Stmt::StmtKind::SK_Expr,
                                         _voidTy)} {}

};  // namespace x::ast
