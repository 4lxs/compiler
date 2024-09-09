#include <fmt/format.h>

#include <cassert>
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

}  // namespace x::ast
