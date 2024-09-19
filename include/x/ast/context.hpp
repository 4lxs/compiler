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
  not_null<Type *> _strTy;
  not_null<Type *> _int32Ty;
  not_null<Type *> _int64Ty;
  not_null<Type *> _boolTy;
  not_null<Type *> _voidTy;

  /// this is an expression with void type. it's used in places that are
  /// expressions, but don't have a value. e.g. a call expr that calls a
  /// function returning void.
  not_null<Expr *> _voidExpr;

  Context();

 private:
 public:  // TODO: temp
  friend class sema::Sema;
  std::vector<Fn *> _functions;
  std::vector<Type *> _types;

 public:  // TODO: friend doesn't work
  template <typename, typename>
  friend class AllowAlloc;

  template <typename T>
    requires std::derived_from<T, Stmt> || std::derived_from<T, Type>
  T *allocate(size_t alignment = 8) const {
    return reinterpret_cast<T *>(_allocator.Allocate(sizeof(T), alignment));
  }
};

}  // namespace x::ast
