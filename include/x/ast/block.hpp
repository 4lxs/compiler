#pragma once

#include <x/common.h>

#include <optional>
#include <vector>

#include "expr.hpp"

namespace x::ast {

class Module;
class Stmt;
class Stub;

class Block {
 public:
  void ret(std::optional<Expr> retval = std::nullopt);

  /// call the function at the stub
  void call(Stub *func, Ptr<StructExpr> args);

  explicit Block(Module *mod);

  // private:
  std::vector<Stmt> _body;

  Module *_mod;
};

}  // namespace x::ast
