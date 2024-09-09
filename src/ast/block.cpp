#include <fmt/format.h>

#include <cassert>
#include <memory>
#include <variant>
#include <x/ast/block.hpp>

#include "x/ast/module.hpp"
#include "x/ast/stmt.hpp"

namespace x::ast {

void Block::ret(std::optional<Expr> retval) {
  _body.emplace_back(std::make_unique<RetStmt>(std::move(retval)));
}

void Block::call(Stub *func, Ptr<StructExpr> args) {
  _body.emplace_back(std::make_unique<Call>(func, std::move(args)));
}

Block::Block(Module *mod) : _mod{mod} {}

auto Block::validate() -> BlockV * {
  std::vector<StmtV> body;
  body.reserve(_body.size());

  for (auto &&stmt : _body) {
    body.push_back(stmt.validate());
  }

  _val = std::make_unique<BlockV>(std::move(body));

  return _val.get();
}

}  // namespace x::ast
