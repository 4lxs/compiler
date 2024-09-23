#pragma once

#include "fwd_decl.hpp"
#include "x/common.hpp"

namespace x::ast {

class Stmt {
 public:
  enum StmtKind {
    SK_Return,
    SK_Function,
    SK_VarDecl,
    SK_Assign,

    SK_Expr,
    SK_Int,
    SK_Bool,
    SK_String,
    SK_Struct,
    SK_If,
    SK_Call,
    SK_Block,
    SK_Builtin,
    SK_DeclRef,
    SK_FieldAccess,
    SK_ExprEnd,
  };

  [[nodiscard]] StmtKind get_kind() const { return _kind; }

 protected:
  explicit Stmt(StmtKind kind) : _kind(kind) {}

 private:
  StmtKind _kind;
};

class Return : public Stmt, public AllowAlloc<Context, Return> {
  friend AllowAlloc;

 public:
  not_null<Expr*> _val;

 private:
  explicit Return(not_null<Expr*> val) : Stmt(SK_Return), _val(val) {};

 public:
  static bool classof(Stmt const* expr) {
    return expr->get_kind() == SK_Return;
  }
};

class Assign : public Stmt, public AllowAlloc<Context, Assign> {
  friend AllowAlloc;

 public:
  /// needs to be lvalue-able
  not_null<Expr*> _variable;

  not_null<Expr*> _value;

 private:
  Assign(not_null<Expr*> variable, not_null<Expr*> value)
      : Stmt(StmtKind::SK_Assign), _variable(variable), _value(value) {}

 public:
  static bool classof(Stmt const* expr) {
    return expr->get_kind() == SK_Assign;
  }
};

}  // namespace x::ast
