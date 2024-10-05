#pragma once

#include <llvm/IR/Instructions.h>

#include <string_view>
#include <utility>
#include <vector>

#include "x/ast/fwd_decl.hpp"
#include "x/ast/stmt.hpp"
#include "x/common.hpp"

namespace x::ast {

class Decl {
 public:
  enum class DeclKind {
    ValueBegin,
    Fn,
    Var,
    Const,
    Field,
    ValueEnd,

    Type,
    Literal,
    Struct,
    Union,
    TypeEnd,
  };

  [[nodiscard]] DeclKind get_kind() const { return _kind; }

  [[nodiscard]] bool is_type() const {
    return _kind >= DeclKind::Type && _kind < DeclKind::TypeEnd;
  }

  [[nodiscard]] std::string_view name() const { return _name; }

 protected:
  explicit Decl(DeclKind kind, std::string_view name)
      : _name(name), _kind(kind) {}

  std::string_view _name;

 private:
  DeclKind _kind;
};

class ValueDecl : public Decl {
 public:
  explicit ValueDecl(DeclKind kind, std::string_view name, Rc<Type> type)
      : Decl(kind, name), _type(std::move(type)) {}

  [[nodiscard]] Rc<Type> type() const {
    assert(_type != nullptr);
    return _type;
  }

  Rc<Type> _type;

  static bool classof(Decl const* decl) {
    return decl->get_kind() > DeclKind::ValueBegin &&
           decl->get_kind() < DeclKind::ValueEnd;
  }
};

class FnDecl : public ValueDecl {
 public:
  struct Param {
    Rc<Type> type;
  };

  FnDecl(std::string_view name, std::vector<Param>&& params, Rc<Type> ret,
         Ptr<Block> block, std::vector<Rc<ast::Decl>> localvars);

  std::vector<Param> _params;

  Rc<Type> _ret;

  Ptr<Block> _block;
  std::vector<Rc<ast::Decl>> _localvars;

  static bool classof(Decl const* decl) {
    return decl->get_kind() == DeclKind::Fn;
  }
};

class VarDecl : public Stmt, public ValueDecl {
 public:
  VarDecl(std::string_view name, Rc<Type> type)
      : Stmt(StmtKind::SK_VarDecl),
        ValueDecl(DeclKind::Var, name, std::move(type)) {}

  static bool classof(Decl const* decl) {
    return decl->get_kind() == DeclKind::Var;
  }

  //===
  // compiler data
  //===

  llvm::AllocaInst* _alloca{};
};

// class ConstDecl : public ValueDecl, public AllowAlloc<Context, ConstDecl> {
//  public:
//   int32_t _value;
//
//  private:
//   friend AllowAlloc;
//   ConstDecl(std::string_view name, Type* type, int32_t value)
//       : ValueDecl(DeclKind::Const, name, type), _value(value) {}
//
//  public:
//   static bool classof(Decl const* decl) {
//     return decl->get_kind() == DeclKind::Const;
//   }
// };
//
// class FieldDecl : public ValueDecl, public AllowAlloc<Context, FieldDecl> {
//  public:
//   /// index of field in struct
//   uint8_t _index;
//
//  private:
//   friend AllowAlloc;
//   FieldDecl(std::string_view name, not_null<Type*> type, uint8_t index)
//       : ValueDecl(DeclKind::Field, name, type), _index(index) {}
//
//  public:
//   static bool classof(Decl const* decl) {
//     return decl->get_kind() == DeclKind::Field;
//   }
// };

}  // namespace x::ast
