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
    SK_Assign,
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

// class Assign : public Stmt, public AllowAlloc<Context, Assign> {
//   friend AllowAlloc;
//
//  public:
//   /// needs to be lvalue-able
//   not_null<Expr*> _variable;
//
//   not_null<Expr*> _value;
//
//  private:
//   Assign(not_null<Expr*> variable, not_null<Expr*> value)
//       : Stmt(StmtKind::SK_Assign), _variable(variable), _value(value) {}
//
//  public:
//   static bool classof(Stmt const* expr) {
//     return expr->get_kind() == SK_Assign;
//   }
// };
//
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
