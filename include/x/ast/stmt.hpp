#pragma once

#include "fwd_decl.hpp"
#include "x/common.hpp"

namespace x::ast {

class Stmt {
 public:
  virtual ~Stmt() = default;

  enum StmtKind {
    SK_Return,
    SK_Function,
    SK_VarDecl,
    SK_While,

    SK_Expr,
    SK_Int,
    SK_Bool,
    SK_String,
    SK_Struct,
    SK_If,
    SK_Call,
    SK_Block,
    SK_Builtin,
    SK_VarRef,
    SK_FieldAccess,
    SK_ExprEnd,
  };

  [[nodiscard]] StmtKind get_kind() const { return _kind; }

  [[nodiscard]] bool is_expr() const {
    return _kind > SK_Expr && _kind < SK_ExprEnd;
  }

 protected:
  explicit Stmt(StmtKind kind) : _kind(kind) {}

 private:
  StmtKind _kind;
};

class Return : public Stmt {
 public:
  ~Return() override;

  Ptr<Expr> _val;

  explicit Return(Ptr<Expr> val);

  static bool classof(Stmt const* expr) {
    return expr->get_kind() == SK_Return;
  }
};

// class While : public Stmt, public AllowAlloc<Context, While> {
//  public:
//   not_null<Expr*> _cond;
//   not_null<Block*> _body;
//
//  private:
//   friend AllowAlloc;
//   While(not_null<Expr*> cond, not_null<Block*> body)
//       : Stmt(StmtKind::SK_While), _cond(cond), _body(body) {}
//
//  public:
//   static bool classof(Stmt const* expr) { return expr->get_kind() ==
//   SK_While; }
// };

}  // namespace x::ast
