#pragma once

#include <llvm/Support/Allocator.h>

#include <vector>

#include "fwd_decl.hpp"
#include "x/common.hpp"
#include "x/pt/context.hpp"
#include "x/sema/fwd_decl.hpp"

namespace x::ast {

class Context {
 public:
  // builtin types
  // primitives are at the start of pt::Context::_items, so they get set before
  // anything else
  Rc<LiteralTy> _int32Ty;
  // Rc<LiteralTy> _int64Ty;
  // Rc<LiteralTy> _boolTy;
  // Rc<LiteralTy> _voidTy;

  /// this is an expression with void type. it's used in places that are
  /// expressions, but don't have a value. e.g. a call expr that calls a
  /// function returning void.
  // Rc<Expr> _voidExpr;

  Ptr<pt::Context> _pt;

  std::vector<Rc<FnDecl>> _functions;
  std::vector<Rc<Type>> _types;
};

}  // namespace x::ast
