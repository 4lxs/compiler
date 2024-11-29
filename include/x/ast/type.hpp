#pragma once

#include <llvm/IR/Type.h>

#include <string_view>

#include "fwd_decl.hpp"
#include "x/ast/decl.hpp"

namespace x::ast {

class Type : public Decl {
 public:
  [[nodiscard]] std::string_view name() const { return _name; }

 protected:
  explicit Type(DeclKind kind, std::string_view name) : Decl(kind, name) {};

 private:
 public:
  //===
  // compiler data
  //===

  llvm::Type* _llvmType{};

  static bool classof(Decl const* decl) {
    return decl->get_kind() >= DeclKind::Type &&
           decl->get_kind() < DeclKind::TypeEnd;
  }
};

class LiteralTy : public Type {
 public:
  enum class Kind {
    // Bool,
    // String,
    I32,
    // I64,
    // Void,
  } _litkind;

  LiteralTy(Kind kind, std::string_view name)
      : Type(DeclKind::Literal, name), _litkind(kind) {};

  static bool classof(Decl const* decl) {
    return decl->get_kind() == DeclKind::Literal;
  }
};

class StructTy : public Type {
 public:
  Rc<FieldDecl> get_field(std::string_view field_name) {
    for (Rc<FieldDecl> field : _fields) {
      if (field->name() == field_name) {
        return field;
      }
    }
    xerr("field '{}' not found in '{}'", field_name, name());
  }

  std::vector<Rc<FieldDecl>> _fields;

  /// enums at the language level get converted to structs with two fields in
  /// the ast. one is a number other is UnionTy
  // bool _isEnum = false;

  explicit StructTy(std::string_view name, std::vector<Rc<FieldDecl>>&& fields)
      : Type(DeclKind::Struct, name), _fields(std::move(fields)) {};

  static bool classof(Decl const* decl) {
    return decl->get_kind() == DeclKind::Struct;
  }
};

// class UnionTy : public Type, public AllowAlloc<Context, UnionTy> {
//  public:
//   void define(std::vector<Type*>&& types) { _variants = std::move(types); }
//
//   /// empty until define is called
//   std::vector<Type*> _variants;
//
//  private:
//   friend AllowAlloc;
//   explicit UnionTy(std::string_view name) : Type(DeclKind::Union, name) {};
//
//  public:
//   static bool classof(Decl const* decl) {
//     return decl->get_kind() == DeclKind::Struct;
//   }
// };
//

class FnTy : public Type {
 public:
  struct Param {
    Rc<Type> type;
  };
  FnTy(std::string_view name, std::vector<Param>&& params, Rc<Type> ret)
      : Type(DeclKind::FnTy, name), _params(std::move(params)), _ret(ret) {};
  std::vector<Param> _params;
  Rc<Type> _ret;
  static bool classof(Decl const* decl) {
    return decl->get_kind() == DeclKind::FnTy;
  }
};

}  // namespace x::ast
