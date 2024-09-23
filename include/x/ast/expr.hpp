#pragma once

#include <cstdint>
#include <cstdlib>
#include <string_view>
#include <vector>

#include "fwd_decl.hpp"
#include "x/ast/context.hpp"
#include "x/ast/stmt.hpp"
#include "x/ast/type.hpp"
#include "x/common.hpp"
#include "x/pt/decl.hpp"

namespace x::ast {

class Expr : public Stmt {
 public:
  Expr() = delete;

  [[nodiscard]] Type* type() const { return _type; }

  /// data only valid for lvaluable expressions
  struct LValueData {
    /// set to false through to_rvalue when we want to assign to this expression
    bool toRValue : 1 {true};
  } _lvalData;

  [[nodiscard]] bool lvalueable() const {
    return get_kind() == SK_DeclRef || get_kind() == SK_FieldAccess;
  }

  void to_lvalue() {
    assert(lvalueable());

    _lvalData.toRValue = false;
  }

  /// if we inherrit from allowalloc, child classes will have problems. it's
  /// only used for voidty anyways
  using Alloc = AllowAlloc<Context, Expr>;
  friend AllowAlloc<Context, Expr>;

 protected:
  explicit Expr(StmtKind kind, Type* type) : Stmt(kind), _type{type} {}

  Type* _type;

 public:
  static bool classof(Stmt const* expr) {
    return expr->get_kind() > SK_Expr && expr->get_kind() < SK_ExprEnd;
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
    return IntegerLiteral::Create(ctx, 32U, val, base, ctx._int32Ty);
  }

  static IntegerLiteral* Int64(Context& ctx, std::string_view val,
                               uint8_t base = 10) {
    return IntegerLiteral::Create(ctx, 64U, val, base, ctx._int64Ty);
  }

 private:
  IntegerLiteral(uint32_t width, std::string_view val, uint8_t base,
                 LiteralTy* type)
      : Expr(SK_Int, type), _val(val), _width(width), _base(base) {}

 public:  // TODO: temp
  std::string_view _val;
  uint32_t _width;
  uint8_t _base;

 public:
  static bool classof(Stmt const* expr) { return expr->get_kind() == SK_Int; }
};

// class StringLiteral : public Expr {
//  private:
//   explicit StringLiteral(std::string_view val) : Expr(SK_String), _val(val)
//   {}
//
//   std::string_view _val;
//
//  public:
//   static bool classof(Stmt const* expr) {
//     return expr->get_kind() == SK_String;
//   }
// };

// class BoolE : public Expr {
//  private:
//   explicit BoolE(bool val) : Expr(SK_Bool), _val(val) {}
//
//   bool _val;
//
//  public:
//   static bool classof(Stmt const* expr) { return expr->get_kind() == SK_Bool;
//   }
// };

class StructLiteral : public Expr, public AllowAlloc<Context, StructLiteral> {
  friend AllowAlloc;

 public:  // TODO: temp
  explicit StructLiteral(std::vector<Expr*> fields)
      : Expr(SK_Struct, nullptr), _fields(std::move(fields)) {}

  StructTy* _type;
  std::vector<Expr*> _fields;

 public:
  static bool classof(Stmt const* expr) {
    return expr->get_kind() == SK_Struct;
  }
};

class FnCall : public Expr, public AllowAlloc<Context, FnCall> {
  friend AllowAlloc;

  /// note that the fn needs to be initialized with a return type
  FnCall(not_null<FnDecl*> func, not_null<StructLiteral*> args)
      : Expr(SK_Call, func->_ret), _fn(func), _args(args) {}

 public:  // TODO: temp
  not_null<FnDecl*> _fn;
  not_null<StructLiteral*> _args;

 public:
  static bool classof(Stmt const* expr) { return expr->get_kind() == SK_Call; }
};

class Block : public Expr, public AllowAlloc<Context, Block> {
  friend AllowAlloc;

 public:  // TODO: temp
  /// note that terminator needs to be initialized with a type
  Block(std::vector<ast::Stmt*> body, not_null<Expr*> terminator)
      : Expr(SK_Block, terminator->type()),
        _body(std::move(body)),
        terminator(terminator) {}

  std::vector<ast::Stmt*> _body;
  not_null<Expr*> terminator;

 public:
  static bool classof(Stmt const* expr) { return expr->get_kind() == SK_Block; }
};

class If : public Expr, public AllowAlloc<Context, If> {
  friend AllowAlloc;

 public:  // TODO: temp
  not_null<Expr*> _cond;
  not_null<Block*> _then;
  Block* _else{};

 private:
  If(not_null<Expr*> cond, not_null<Block*> then, Block* els)
      : Expr(SK_If, then->type()), _cond(cond), _then(then), _else(els) {
    assert(then->type() == els->type());
  }

 public:
  static bool classof(Stmt const* expr) { return expr->get_kind() == SK_If; }
};

class Builtin : public Expr, public AllowAlloc<Context, Builtin> {
  friend AllowAlloc;

 public:  // TODO: temp
  enum class Op {
    Start2,  // 2 argument operations
    iAdd,
    iSub,
    iMul,
    iDiv,
    iLess,
    iGreater,
    Start3,
  };

  std::vector<Expr*> _args;

  Op _op;

 private:
  Builtin(Op opr, std::vector<Expr*>&& args, not_null<Type*> returnType)
      : Expr(SK_Builtin, returnType), _args(args), _op(opr) {}

 public:
  static bool classof(Stmt const* expr) {
    return expr->get_kind() == SK_Builtin;
  }
};

class DeclRef : public Expr, public AllowAlloc<Context, DeclRef> {
 public:
  VarDecl* _decl;

 private:
  friend AllowAlloc;
  explicit DeclRef(VarDecl* decl, Type* type)
      : Expr(SK_DeclRef, type), _decl(decl) {}

 public:
  static bool classof(Stmt const* expr) {
    return expr->get_kind() == SK_DeclRef;
  }
};

class FieldAccess : public Expr, public AllowAlloc<Context, FieldAccess> {
 public:
  not_null<Expr*> _base;
  not_null<FieldDecl*> _field;

 private:
  friend AllowAlloc;
  FieldAccess(not_null<Expr*> base, not_null<FieldDecl*> field)
      : Expr(SK_FieldAccess, field->_type), _base(base), _field(field) {}

 public:
  static bool classof(Stmt const* expr) {
    return expr->get_kind() == SK_FieldAccess;
  }
};

}  // namespace x::ast
