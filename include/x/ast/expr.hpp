#pragma once

#include <cstdint>
#include <cstdlib>
#include <string_view>
#include <vector>

#include "fwd_decl.hpp"
#include "x/ast/stmt.hpp"
#include "x/common.hpp"

namespace x::ast {

class Expr : public Stmt {
 public:
  Expr() = delete;

 protected:
  explicit Expr(StmtKind kind) : Stmt(kind) {}

 public:
  static bool classof(Stmt const* expr) {
    return expr->get_kind() > SK_ExprBegin && expr->get_kind() < SK_ExprEnd;
  }
};

class IntegerLiteral : public Expr,
                       private AllowAlloc<Context, IntegerLiteral> {
  friend AllowAlloc;

 public:
  /// val needs to be valid for the duration of the ast
  /// it's usually owned by the pt
  static IntegerLiteral* Int32(Context& ctx, std::string_view val,
                               uint8_t base = 10) {
    return IntegerLiteral::Create(ctx, 32, val, base);
  }

  static IntegerLiteral* Int64(Context& ctx, std::string_view val,
                               uint8_t base = 10) {
    return IntegerLiteral::Create(ctx, 64, val, base);
  }

 private:
  IntegerLiteral(uint32_t width, std::string_view val, uint8_t base)
      : Expr(SK_Int), _val(val), _width(width), _base(base) {}

 public:  // TODO: temp
  std::string_view _val;
  uint32_t _width;
  uint8_t _base;

 public:
  static bool classof(Stmt const* expr) { return expr->get_kind() == SK_Int; }
};

class StringLiteral : public Expr {
 private:
  explicit StringLiteral(std::string_view val) : Expr(SK_String), _val(val) {}

  std::string_view _val;

 public:
  static bool classof(Stmt const* expr) {
    return expr->get_kind() == SK_String;
  }
};

class BoolE : public Expr {
 private:
  explicit BoolE(bool val) : Expr(SK_Bool), _val(val) {}

  bool _val;

 public:
  static bool classof(Stmt const* expr) { return expr->get_kind() == SK_Bool; }
};

class StructLiteral : public Expr, public AllowAlloc<Context, StructLiteral> {
  friend AllowAlloc;

 public:  // TODO: temp
  explicit StructLiteral(std::vector<Expr*> fields)
      : Expr(SK_Struct), _fields(std::move(fields)) {}

  std::vector<Expr*> _fields;

 public:
  static bool classof(Stmt const* expr) {
    return expr->get_kind() == SK_Struct;
  }
};

class FnCall : public Expr, public AllowAlloc<Context, FnCall> {
  friend AllowAlloc;

  FnCall(Fn* fn, StructLiteral* args) : Expr(SK_Call), _fn(fn), _args(args) {}

 public:  // TODO: temp
  Fn* _fn;
  StructLiteral* _args;

 public:
  static bool classof(Stmt const* expr) { return expr->get_kind() == SK_Call; }
};

class Block : public Expr, public AllowAlloc<Context, Block> {
  friend AllowAlloc;

 public:  // TODO: temp
  Block(std::vector<ast::Stmt*> body, Expr* terminator)
      : Expr(SK_Block), _body(std::move(body)), terminator(terminator) {}

  std::vector<ast::Stmt*> _body;
  Expr* terminator;

 public:
  static bool classof(Stmt const* expr) { return expr->get_kind() == SK_Block; }
};

class If : public Expr, public AllowAlloc<Context, If> {
  friend AllowAlloc;

 public:  // TODO: temp
  Expr* _cond;
  Block* _then;
  Block* _else{};

 private:
  If(Expr* cond, Block* then, Block* els)
      : Expr(SK_If), _cond(cond), _then(then), _else(els) {}

 public:
  static bool classof(Stmt const* expr) { return expr->get_kind() == SK_If; }
};

class Builtin : public Expr, public AllowAlloc<Context, Builtin> {
  friend AllowAlloc;

 public:  // TODO: temp
  enum class Op {
    Start2,  // 2 argument operations
    iAdd,
    iLess,
    Start3,
  };

  std::vector<Expr*> _args;

  Op _op;

 private:
  Builtin(Op op, std::vector<Expr*>&& args)
      : Expr(SK_Builtin), _op(op), _args(args) {}

 public:
  static bool classof(Stmt const* expr) {
    return expr->get_kind() == SK_Builtin;
  }
};

}  // namespace x::ast
