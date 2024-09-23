#pragma once

#include <llvm/Support/Allocator.h>

#include <concepts>
#include <vector>

#include "fwd_decl.hpp"
#include "spdlog/spdlog.h"
#include "x/common.hpp"
#include "x/sema/fwd_decl.hpp"

namespace x::ast {

class Context {
  mutable llvm::BumpPtrAllocator _allocator;

 public:
  // builtin types
  not_null<LiteralTy *> _int32Ty;
  not_null<LiteralTy *> _int64Ty;
  not_null<LiteralTy *> _boolTy;
  not_null<LiteralTy *> _voidTy;

  /// this is an expression with void type. it's used in places that are
  /// expressions, but don't have a value. e.g. a call expr that calls a
  /// function returning void.
  not_null<Expr *> _voidExpr;

  Context();

 private:
 public:  // TODO: temp
  friend class sema::Sema;
  std::vector<FnDecl *> _functions;
  std::vector<LiteralTy *> _types;

 public:  // TODO: friend doesn't work
  template <typename, typename>
  friend class AllowAlloc;

  template <typename T>
    requires std::derived_from<T, Stmt> || std::derived_from<T, Decl>
  T *allocate(size_t alignment = 8) const {
    return reinterpret_cast<T *>(_allocator.Allocate(sizeof(T), alignment));
  }
};

}  // namespace x::ast
