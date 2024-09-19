#pragma once

#include <llvm/Support/Allocator.h>

#include <map>

#include "spdlog/spdlog.h"
#include "x/common.hpp"
#include "x/pt/fwd_decl.hpp"
#include "x/pt/path.hpp"
#include "x/pt/type.hpp"
#include "x/sema/fwd_decl.hpp"

namespace x::pt {

/// keeps data about the entire parse tree
/// allowing you to get stuff from other modules
/// even if those modules weren't processed yet.
///
/// while processing one module, we get to
/// use a::b::Type
/// you query context->getType(). the context will
/// give you a type. when later processing module a::b,
/// you call context->add(a::b, Type). the context will
/// be able to link those two together
class Context {
 public:
  static Ptr<Context> Create();

  /// return the module at path and create it if it doesn't
  /// exist
  Module *module(Path &&path);

  /// get stub to any item in any module by it's full path
  Stub *stub(Path &&path);

  Ptr<Type> _boolTy = std::make_unique<Type>(Type::Kind::Bool);
  Ptr<Type> _strTy = std::make_unique<Type>(Type::Kind::String);
  Ptr<Type> _numTy = std::make_unique<Type>(Type::Kind::Number);

 private:
  Context() = default;

 public:  // TODO: friend doesn't work
  template <typename, typename>
  friend class AllowAlloc;

  template <typename T>
  // requires std::derived_from<T, Stmt> || std::derived_from<T, Type>
  T *allocate(size_t alignment = 8) const {
    return reinterpret_cast<T *>(_allocator.Allocate(sizeof(T), alignment));
  }

 private:
  mutable llvm::BumpPtrAllocator _allocator;

  friend sema::Sema;

  std::map<Path, Module *> _modules;
};

}  // namespace x::pt
