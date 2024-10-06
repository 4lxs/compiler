#pragma once

#include <llvm/ADT/APInt.h>

#include <cstdint>
#include <cstdlib>
#include <string_view>
#include <utility>
#include <vector>

#include "fwd_decl.hpp"
#include "x/ast/context.hpp"
#include "x/ast/label.hpp"
#include "x/ast/stmt.hpp"
#include "x/ast/type.hpp"
#include "x/common.hpp"
// #include "x/pt/decl.hpp"

namespace x::ast {

class Expr : public Stmt {
 public:
  Expr() = delete;

  [[nodiscard]] Rc<Type> type() const { return _type; }

  //  /// data only valid for lvaluable expressions
  //  struct LValueData {
  //    /// set to false through to_rvalue when we want to assign to this
  //    expression bool toRValue{true};
  //
  //    /// decl that is being accessed. e.g. vardecl for declref or FieldDecl
  //    for
  //      /// FieldAccess
  //      Decl* decl;
  //  } _lvalData;
  //
  //  [[nodiscard]] bool lvalueable() const {
  //    return get_kind() == SK_VarRef || get_kind() == SK_FieldAccess;
  //  }
  //
  //  void to_lvalue() {
  //    assert(lvalueable());
  //
  //    _lvalData.toRValue = false;
  //  }
  //
  //  /// if we inherrit from allowalloc, child classes will have problems. it's
  //  /// only used for voidty anyways
  //  using Alloc = AllowAlloc<Context, Expr>;
  //  friend AllowAlloc<Context, Expr>;
  //
 protected:
  explicit Expr(StmtKind kind, Rc<Type> type)
      : Stmt(kind), _type{std::move(type)} {}

  Rc<Type> _type;

 public:
  static bool classof(Stmt const* expr) { return expr->is_expr(); }
};

class IntegerLiteral : public Expr {
 public:
  /// val needs to be valid for the duration of the ast
  /// it's usually owned by the pt
  IntegerLiteral(llvm::APInt val, Rc<LiteralTy> type)
      : Expr(SK_Int, type), _apint{std::move(val)} {}

  llvm::APInt _apint;

  static bool classof(Stmt const* expr) { return expr->get_kind() == SK_Int; }
};

// // class StringLiteral : public Expr {
// //  private:
// //   explicit StringLiteral(std::string_view val) : Expr(SK_String),
// _val(val)
// //   {}
// //
// //   std::string_view _val;
// //
// //  public:
// //   static bool classof(Stmt const* expr) {
// //     return expr->get_kind() == SK_String;
// //   }
// // };
//
// // class BoolE : public Expr {
// //  private:
// //   explicit BoolE(bool val) : Expr(SK_Bool), _val(val) {}
// //
// //   bool _val;
// //
// //  public:
// //   static bool classof(Stmt const* expr) { return expr->get_kind() ==
// SK_Bool;
// //   }
// // };
//
// class StructLiteral : public Expr, public AllowAlloc<Context, StructLiteral>
// {
//   friend AllowAlloc;
//
//  public:  // TODO: temp
//   explicit StructLiteral(std::vector<Expr*> fields)
//       : Expr(SK_Struct, nullptr), _fields(std::move(fields)) {}
//
//   StructTy* _type;
//   std::vector<Expr*> _fields;
//
//  public:
//   static bool classof(Stmt const* expr) {
//     return expr->get_kind() == SK_Struct;
//   }
// };
//
// class FnCall : public Expr, public AllowAlloc<Context, FnCall> {
//   friend AllowAlloc;
//
//   /// note that the fn needs to be initialized with a return type
//   FnCall(not_null<FnDecl*> func, not_null<StructLiteral*> args)
//       : Expr(SK_Call, func->_ret), _fn(func), _args(args) {}
//
//  public:  // TODO: temp
//   not_null<FnDecl*> _fn;
//   not_null<StructLiteral*> _args;
//
//  public:
//   static bool classof(Stmt const* expr) { return expr->get_kind() == SK_Call;
//   }
// };
//

class Block : public Expr {
 public:
  /// note that terminator needs to be initialized with a type
  explicit Block(std::vector<Ptr<ast::Stmt>> body, Rc<Label> label = nullptr)
      : Expr(SK_Block, nullptr),
        _body(std::move(body)),
        _label(std::move(label)) {
    if (_label) {
      if (_label.use_count() == 1) {
        _label = nullptr;
      } else {
        _label->_block = this;
      }
    }
  }

  ~Block() override {
    if (_label) {
      _label->_block = nullptr;
    }
  };

  std::vector<Ptr<ast::Stmt>> _body;
  Rc<Label> _label;

  // compiler data

  llvm::BasicBlock* _llvmEnd;

  static bool classof(Stmt const* expr) { return expr->get_kind() == SK_Block; }
};

class If : public Expr {
 public:
  Ptr<Expr> _cond;
  Ptr<Block> _then;
  Ptr<Block> _else;  // nullable

  If(Ptr<Expr> cond, Ptr<Block> then, Ptr<Block> els)
      : Expr(SK_If, then->type()),
        _cond(std::move(cond)),
        _then(std::move(then)),
        _else(std::move(els)) {
    assert(!_else || _else->type() == _then->type());
  }

  static bool classof(Stmt const* expr) { return expr->get_kind() == SK_If; }
};

class VarRef : public Expr {
 public:
  Rc<VarDecl> _decl;

  explicit VarRef(Rc<VarDecl> decl, Rc<Type> type)
      : Expr(SK_VarRef, std::move(type)), _decl(std::move(decl)) {}

  static bool classof(Stmt const* expr) {
    return expr->get_kind() == SK_VarRef;
  }
};

// class FieldAccess : public Expr, public AllowAlloc<Context, FieldAccess> {
//  public:
//   not_null<Expr*> _base;
//   not_null<FieldDecl*> _field;
//
//  private:
//   friend AllowAlloc;
//   FieldAccess(not_null<Expr*> base, not_null<FieldDecl*> field)
//       : Expr(SK_FieldAccess, field->type()), _base(base), _field(field) {}
//
//  public:
//   static bool classof(Stmt const* expr) {
//     return expr->get_kind() == SK_FieldAccess;
//   }
// };

class Builtin : public Expr {
 public:
  enum class Op {
    Start1,
    Not,
    Start2,  // 2 argument operations
    Assign,
    iAdd,
    iSub,
    iMul,
    iDiv,
    iLess,
    iGreater,
    Start3,
  };

  static Ptr<Builtin> CreateAssignment(Ptr<Expr> lhs, Ptr<Expr> rhs) {
    // Rc<Type> type = lhs->type();
    std::vector<Ptr<Expr>> args;
    args.push_back(std::move(lhs));
    args.push_back(std::move(rhs));
    return std::make_unique<Builtin>(Op::Assign, std::move(args), nullptr);
  }

  static Ptr<Builtin> CreateNot(Ptr<Expr> expr) {
    std::vector<Ptr<Expr>> args;
    Rc<Type> type = expr->type();
    args.push_back(std::move(expr));
    return std::make_unique<Builtin>(Op::Not, std::move(args), std::move(type));
  }

  std::vector<Ptr<Expr>> _args;

  Op _op;

  Builtin(Op opr, std::vector<Ptr<Expr>>&& args, Rc<Type> returnType)
      : Expr(SK_Builtin, std::move(returnType)),
        _args(std::move(args)),
        _op(opr) {}

  static bool classof(Stmt const* expr) {
    return expr->get_kind() == SK_Builtin;
  }
};

}  // namespace x::ast
