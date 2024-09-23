#pragma once

#include <llvm/IR/Type.h>

#include <string_view>

#include "fwd_decl.hpp"
#include "spdlog/spdlog.h"
#include "x/ast/decl.hpp"
#include "x/common.hpp"

namespace x::ast {

class Type {
 public:
  enum class TypeKind {
    Literal,
    Struct,
  };

  [[nodiscard]] std::string_view name() const { return _name; }

  [[nodiscard]] TypeKind get_kind() const { return _kind; }

 protected:
  explicit Type(TypeKind kind, std::string_view name)
      : _name(name), _kind(kind) {};

 private:
  std::string_view _name;
  TypeKind _kind;

  //===
  // compiler data
  //===

 public:
  llvm::Type* _llvmType{};
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

 private:
  LiteralTy(Kind kind, std::string_view name)
      : Type(TypeKind::Literal, name), _kind(kind) {};

 public:
  static bool classof(Type const* type) {
    return type->get_kind() == TypeKind::Literal;
  }
};

class StructTy : public Type, public AllowAlloc<Context, StructTy> {
 public:
  void define(std::vector<FieldDecl*>&& fields) { _fields = std::move(fields); }

  not_null<FieldDecl*> get_field(std::string_view field_name) {
    for (FieldDecl* field : _fields) {
      if (field->name() == field_name) {
        return field;
      }
    }
    spdlog::error("field '{}' not found in '{}'", field_name, name());
    std::terminate();
  }

  /// empty until define is called
  std::vector<FieldDecl*> _fields;

 private:
  friend AllowAlloc;
  explicit StructTy(std::string_view name) : Type(TypeKind::Struct, name) {};

 public:
  static bool classof(Type const* type) {
    return type->get_kind() == TypeKind::Struct;
  }
};

}  // namespace x::ast
