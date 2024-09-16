#include "x/pt/block.hpp"

#include <cassert>
#include <memory>

#include "x/pt/expr.hpp"
#include "x/pt/stmt.hpp"

namespace x::pt {

void Block::ret(std::optional<Expr> retval) {
  _body.emplace_back(std::make_unique<RetStmt>(std::move(retval)));
}

void Block::expr(Expr expr) { _body.emplace_back(std::move(expr)); }

void Block::setTerminator(Expr expr) {
  assert(!_end.has_value());

  _end = std::move(expr);
}

}  // namespace x::pt
