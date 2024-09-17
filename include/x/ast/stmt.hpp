#pragma once

#include <vector>

#include "fwd_decl.hpp"
#include "x/common.hpp"
#include "x/pt/pt.hpp"

namespace x::ast {

class Stmt {
 public:
  enum StmtKind {
    SK_Return,
    SK_Function,
    SK_Builtin,

    SK_ExprBegin,
    SK_Int,
    SK_Bool,
    SK_String,
    SK_Struct,
    SK_If,
    SK_Call,
    SK_Block,
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

 public:  // TODO: temp
  explicit Return(Expr* val = nullptr) : Stmt(SK_Return), _val(val) {};

  Expr* _val;

 public:
  static bool classof(Stmt const* expr) {
    return expr->get_kind() == SK_Return;
  }
};

class Fn : public Stmt, public AllowAlloc<Context, Fn> {
  friend AllowAlloc;

 public:
  struct Param {
    std::string_view name;
    Type* type;
  };

  [[nodiscard]] std::string_view name() const { return _name; }

 private:
  Fn(std::string_view name, std::vector<Param>&& params, Block* body, Type* ret)
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
