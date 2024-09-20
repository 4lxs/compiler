#pragma once

#include <vector>

#include "fwd_decl.hpp"
#include "x/common.hpp"
#include "x/pt/fwd_decl.hpp"

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

class VarDecl : public Stmt, public AllowAlloc<Context, VarDecl> {
  friend AllowAlloc;

 public:
  [[nodiscard]] std::string_view name() const { return _name; }

  std::string_view _name;
  Type* _type;

 private:
  explicit VarDecl(std::string_view name, Type* type)
      : Stmt(StmtKind::SK_VarDecl), _name(name), _type(type) {}

 public:
  static bool classof(Stmt const* expr) {
    return expr->get_kind() == SK_VarDecl;
  }
};

class Assign : public Stmt, public AllowAlloc<Context, Assign> {
  friend AllowAlloc;

 public:
  not_null<VarDecl*> _variable;
  not_null<Expr*> _value;

 private:
  Assign(not_null<VarDecl*> variable, not_null<Expr*> value)
      : Stmt(StmtKind::SK_Assign), _variable(variable), _value(value) {}

 public:
  static bool classof(Stmt const* expr) {
    return expr->get_kind() == SK_Assign;
  }
};

class FnDecl : public Stmt, public AllowAlloc<Context, FnDecl> {
  friend AllowAlloc;

 public:
  struct Param {
    std::string_view name;
    Type* type;
  };

  [[nodiscard]] std::string_view name() const { return _name; }

 private:
  FnDecl(std::string_view name, std::vector<Param>&& params, Block* body,
         Type* ret)
      : Stmt(SK_Function),
        _name{name},
        _params(std::move(params)),
        _block(body),
        _ret(ret) {}

 public:  // TODO: temp
  std::string_view _name;

  std::vector<Param> _params;

  Block* _block;

  Type* _ret;

 public:
  static bool classof(Stmt const* expr) {
    return expr->get_kind() == SK_Function;
  }
};

}  // namespace x::ast
