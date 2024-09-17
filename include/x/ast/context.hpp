#pragma once

#include <llvm/Support/Allocator.h>

#include <concepts>
#include <vector>

#include "fwd_decl.hpp"
#include "x/sema/fwd_decl.hpp"

namespace x::ast {

class Context {
 public:
  template <typename T>
    requires std::derived_from<T, Stmt> || std::derived_from<T, Expr> ||
             std::derived_from<T, Type>
  T *allocate(size_t alignment = 8) {
    return reinterpret_cast<T *>(allocate(sizeof(T), alignment));
  }

 public:  // TODO: temp
  friend class sema::Sema;
  std::vector<Fn *> _functions;
  std::vector<Type *> _types;

  friend void * ::operator new(size_t bytes, x::ast::Context const &ctx,
                               size_t alignment);
  void *allocate(size_t size, size_t alignment) const {
    return _allocator.Allocate(size, alignment);
  }

  mutable llvm::BumpPtrAllocator _allocator;
};

}  // namespace x::ast

inline void *operator new(size_t bytes, x::ast::Context const &ctx,
                          size_t alignment) {
  return ctx.allocate(bytes, alignment);
}
