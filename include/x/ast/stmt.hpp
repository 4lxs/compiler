#pragma once

#include "fwd_decl.hpp"
#include "x/ast/label.hpp"
#include "x/common.hpp"

namespace x::ast {

class Stmt {
 public:
  virtual ~Stmt() = default;

  enum StmtKind {
    SK_Return,
    SK_Function,
    SK_VarDecl,
    SK_Loop,

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
  /// handles all kinds of returns. that is local returns, function returns,
  /// loop breaks, ...
  /// @param block signifies the block from which to return. it needs
  /// to be a parent of this stmt
  explicit Return(Ptr<Expr> val, Rc<Label> block = nullptr);
  ~Return() override;

  Ptr<Expr> _val;
  Rc<Label> _blockLbl;

  static bool classof(Stmt const* expr) {
    return expr->get_kind() == SK_Return;
  }
};

class Loop : public Stmt {
 public:
  Ptr<Block> _body;

  explicit Loop(Ptr<Block> body);
  ~Loop() override;

 public:
  static bool classof(Stmt const* expr) { return expr->get_kind() == SK_Loop; }
};

}  // namespace x::ast
