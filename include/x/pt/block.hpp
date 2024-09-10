#pragma once

#include <x/common.h>

#include <optional>
#include <vector>

#include "expr.hpp"

namespace x::pt {

class Module;
class Stub;
class Stmt;
struct StmtV;
struct BlockV;

class Block {
 public:
  void ret(std::optional<Expr> retval = std::nullopt);

  /// call the function at the stub
  void call(Stub *func, Ptr<StructExpr> args);

 protected:
  explicit Block(Module *mod);

  /// see `Context::validate()`
  BlockV *validate();

  /// see `Module::validate()`
  Ptr<BlockV> _val;

  std::vector<Stmt> _body;

  Module *_mod;
};

struct BlockV {
  std::vector<StmtV> body;
};

}  // namespace x::pt
