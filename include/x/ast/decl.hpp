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
    Fn,
    Var,
    Field,

    TypeBegin,
    LiteralTy,
    StructTy,
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

class FnDecl : public Stmt, public Decl, public AllowAlloc<Context, FnDecl> {
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
        Decl(DeclKind::Fn, name),
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

class VarDecl : public Stmt, public Decl, public AllowAlloc<Context, VarDecl> {
  friend AllowAlloc;

 public:
  Type* _type;

 private:
  explicit VarDecl(std::string_view name, Type* type)
      : Stmt(StmtKind::SK_VarDecl), Decl(DeclKind::Var, name), _type(type) {}

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

class FieldDecl : public Decl, public AllowAlloc<Context, FieldDecl> {
 public:
  not_null<Type*> _type;

  /// index of field in struct
  uint8_t _index;

 private:
  friend AllowAlloc;
  FieldDecl(std::string_view name, not_null<Type*> type, uint8_t index)
      : Decl(DeclKind::Field, name), _type(type), _index(index) {}

 public:
  static bool classof(Decl const* decl) {
    return decl->get_kind() == DeclKind::Field;
  }
};

}  // namespace x::ast
