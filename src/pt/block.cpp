#include <fmt/format.h>

#include <cassert>
#include <memory>
#include <variant>
#include <x/pt/block.hpp>

#include "x/pt/module.hpp"
#include "x/pt/stmt.hpp"

namespace x::pt {

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

}  // namespace x::pt
