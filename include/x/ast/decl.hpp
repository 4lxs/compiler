#pragma once

#include <llvm/IR/Instructions.h>

#include <string_view>
#include <vector>

#include "x/ast/stmt.hpp"
#include "x/common.hpp"

namespace x::ast {

class Decl {
 public:
  enum class DeclKind {
    ValueBegin,
    Fn,
    Var,
    Field,
    ValueEnd,

    Type,
    Literal,
    Struct,
    Union,
    TypeEnd,
  };

  [[nodiscard]] DeclKind get_kind() const { return _kind; }

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
  explicit ValueDecl(DeclKind kind, std::string_view name, Type* type)
      : Decl(kind, name), _type(type) {}

  [[nodiscard]] not_null<Type*> type() const {
    assert(_type != nullptr);
    return _type;
  }

 private:
  Type* _type;

 public:
  static bool classof(Decl const* decl) {
    return decl->get_kind() > DeclKind::ValueBegin &&
           decl->get_kind() < DeclKind::ValueEnd;
  }
};

class FnDecl : public Stmt,
               public ValueDecl,
               public AllowAlloc<Context, FnDecl> {
  friend AllowAlloc;

 public:
  struct Param {
    std::string_view name;
    Type* type;
  };

  void define(not_null<Block*> block) { _block = block; }

  [[nodiscard]] std::string_view name() const { return _name; }
  [[nodiscard]] Block* block() const { return _block; }
  [[nodiscard]] not_null<Type*> ret() const { return _ret; }
  [[nodiscard]] std::vector<Param> const& params() const { return _params; }

 private:
  FnDecl(std::string_view name, std::vector<Param>&& params,
         not_null<Type*> ret)
      : Stmt(SK_Function),
        ValueDecl(DeclKind::Fn, name, nullptr /* TODO */),
        _params(std::move(params)),
        _ret(ret) {}

 public:  // TODO: temp
  std::vector<Param> _params;

  not_null<Type*> _ret;

  /// block is null until function is defined
  Block* _block{};

 public:
  static bool classof(Stmt const* expr) {
    return expr->get_kind() == SK_Function;
  }

  static bool classof(Decl const* decl) {
    return decl->get_kind() == DeclKind::Fn;
  }
};

class VarDecl : public Stmt,
                public ValueDecl,
                public AllowAlloc<Context, VarDecl> {
  friend AllowAlloc;

 private:
  explicit VarDecl(std::string_view name, Type* type)
      : Stmt(StmtKind::SK_VarDecl), ValueDecl(DeclKind::Var, name, type) {}

 public:
  static bool classof(Stmt const* expr) {
    return expr->get_kind() == SK_VarDecl;
  }

  static bool classof(Decl const* decl) {
    return decl->get_kind() == DeclKind::Var;
  }

  //===
  // compiler data
  //===

  llvm::AllocaInst* _alloca{};
};

class FieldDecl : public ValueDecl, public AllowAlloc<Context, FieldDecl> {
 public:
  /// index of field in struct
  uint8_t _index;

 private:
  friend AllowAlloc;
  FieldDecl(std::string_view name, not_null<Type*> type, uint8_t index)
      : ValueDecl(DeclKind::Field, name, type), _index(index) {}

 public:
  static bool classof(Decl const* decl) {
    return decl->get_kind() == DeclKind::Field;
  }
};

}  // namespace x::ast
