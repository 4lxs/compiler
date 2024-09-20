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

class Decl {
 public:
  enum class DeclKind {
    Type,
    Fn,
    Var,
  };

  [[nodiscard]] DeclKind get_kind() const { return _kind; }

  [[nodiscard]] std::string_view name() const { return _name; }

 protected:
  explicit Decl(DeclKind kind, std::string_view name)
      : _name(name), _kind(kind) {}

  std::string_view _name;

 private:
  DeclKind _kind;
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

class VarDecl : public Stmt, public Decl, public AllowAlloc<Context, VarDecl> {
  friend AllowAlloc;

 public:
  Type* _type;

 private:
  explicit VarDecl(std::string_view name, Type* type)
      : Stmt(StmtKind::SK_VarDecl), Decl(DeclKind::Var, name), _type(type) {}

 public:
  static bool classof(Stmt const* expr) {
    return expr->get_kind() == SK_VarDecl;
  }

  static bool classof(Decl const* decl) {
    return decl->get_kind() == DeclKind::Var;
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

class FnDecl : public Stmt, public Decl, public AllowAlloc<Context, FnDecl> {
  friend AllowAlloc;

 public:
  struct Param {
    std::string_view name;
    Type* type;
  };

  void define(not_null<Block*> block) { _block = block; }

  [[nodiscard]] std::string_view name() const { return _name; }
  [[nodiscard]] Block* block() const { return _block; }
  [[nodiscard]] not_null<Type*> ret() const { return _ret; }
  [[nodiscard]] std::vector<Param> const& params() const { return _params; }

 private:
  FnDecl(std::string_view name, std::vector<Param>&& params,
         not_null<Type*> ret)
      : Stmt(SK_Function),
        Decl(DeclKind::Fn, name),
        _params(std::move(params)),
        _ret(ret) {}

 public:  // TODO: temp
  std::vector<Param> _params;

  not_null<Type*> _ret;

  /// block is null until function is defined
  Block* _block{};

 public:
  static bool classof(Stmt const* expr) {
    return expr->get_kind() == SK_Function;
  }

  static bool classof(Decl const* decl) {
    return decl->get_kind() == DeclKind::Fn;
  }
};

}  // namespace x::ast
