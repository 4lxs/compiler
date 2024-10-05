#pragma once

#include <llvm/ADT/APInt.h>

#include <cstdint>
#include <cstdlib>
#include <string_view>
#include <utility>
#include <vector>

#include "fwd_decl.hpp"
#include "x/ast/context.hpp"
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
  static bool classof(Stmt const* expr) {
    return expr->get_kind() > SK_Expr && expr->get_kind() < SK_ExprEnd;
  }
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
  explicit Block(std::vector<Ptr<ast::Stmt>> body,
                 std::vector<Rc<ast::Decl>> localvars)
      : Expr(SK_Block, nullptr),
        _body(std::move(body)),
        _localvars(std::move(localvars)) {}

  std::vector<Ptr<ast::Stmt>> _body;
  std::vector<Rc<ast::Decl>> _localvars;

 public:
  static bool classof(Stmt const* expr) { return expr->get_kind() == SK_Block; }
};
//
// class If : public Expr, public AllowAlloc<Context, If> {
//   friend AllowAlloc;
//
//  public:  // TODO: temp
//   not_null<Expr*> _cond;
//   not_null<Block*> _then;
//   Block* _else{};
//
//  private:
//   If(not_null<Expr*> cond, not_null<Block*> then, Block* els)
//       : Expr(SK_If, then->type()), _cond(cond), _then(then), _else(els) {
//     assert(then->type() == els->type());
//   }
//
//  public:
//   static bool classof(Stmt const* expr) { return expr->get_kind() == SK_If; }
// };
//
// class Builtin : public Expr, public AllowAlloc<Context, Builtin> {
//   friend AllowAlloc;
//
//  public:  // TODO: temp
//   enum class Op {
//     Start2,  // 2 argument operations
//     iAdd,
//     iSub,
//     iMul,
//     iDiv,
//     iLess,
//     iGreater,
//     Start3,
//   };
//
//   std::vector<Expr*> _args;
//
//   Op _op;
//
//  private:
//   Builtin(Op opr, std::vector<Expr*>&& args, not_null<Type*> returnType)
//       : Expr(SK_Builtin, returnType), _args(args), _op(opr) {}
//
//  public:
//   static bool classof(Stmt const* expr) {
//     return expr->get_kind() == SK_Builtin;
//   }
// };

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

}  // namespace x::ast
