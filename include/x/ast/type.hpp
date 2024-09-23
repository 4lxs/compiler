#pragma once

#include <llvm/IR/Type.h>

#include <string_view>

#include "fwd_decl.hpp"
#include "spdlog/spdlog.h"
#include "x/ast/decl.hpp"
#include "x/common.hpp"

namespace x::ast {

class Type : public Decl {
 protected:
  explicit Type(DeclKind kind, std::string_view name) : Decl(kind, name) {};

 public:
  static bool classof(Decl const* decl) {
    return decl->get_kind() > DeclKind::TypeBegin &&
           decl->get_kind() < DeclKind::TypeEnd;
  }

  //===
  // compiler data
  //===

  llvm::Type* _llvmType;
};

class LiteralTy : public Type, public AllowAlloc<Context, LiteralTy> {
  friend AllowAlloc;

 public:
  enum class Kind {
    Bool,
    String,
    I32,
    I64,
    Void,
  } _kind;

  void prettyPrint() const {
    spdlog::info("name: {}", _name);
    switch (_kind) {
      case Kind::Bool:
        spdlog::info("bool");
        break;
      case Kind::String:
        spdlog::info("string");
        break;
      case Kind::I32:
        spdlog::info("i32");
        break;
      case Kind::I64:
        spdlog::info("i64");
        break;
      case Kind::Void:
        spdlog::info("void");
        break;
    }
  }

 private:
  explicit LiteralTy(Kind kind, std::string_view name)
      : Type(DeclKind::LiteralTy, name), _kind(kind) {};

 public:
  static bool classof(Decl const* decl) {
    return decl->get_kind() == DeclKind::LiteralTy;
  }
};

class StructTy : public Type, public AllowAlloc<Context, StructTy> {
 public:
  void define(std::vector<FieldDecl*>&& fields) { _fields = std::move(fields); }

  not_null<FieldDecl*> get_field(std::string_view name) {
    for (FieldDecl* field : _fields) {
      if (field->name() == name) {
        return field;
      }
    }
    spdlog::error("field '{}' not found in '{}'", name, _name);
    std::terminate();
  }

  /// empty until define is called
  std::vector<FieldDecl*> _fields;

 private:
  friend AllowAlloc;
  explicit StructTy(std::string_view name) : Type(DeclKind::StructTy, name) {};
};

}  // namespace x::ast
