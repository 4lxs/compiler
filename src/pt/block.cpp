#include "x/pt/block.hpp"

namespace x::pt {

void Block::dump(Context& ctx, uint8_t indent) {
  fmt::print("{:{}}Block:\n", "", indent);
  for (auto const& stmt : _body) {
    ctx.get_node(stmt).dump(ctx, indent + 2);
  }
  if (_end) {
    fmt::print("{:{}}return:\n", "", indent);
    ctx.get_node(*_end).dump(ctx, indent + 1);
  }
}

void Block::nameres(sema::NameResolver& res) {
  for (auto& stmt : _body) {
    res._ctx->get_node(stmt).nameres(res);
  }
  if (_end) {
    res._ctx->get_node(*_end).nameres(res);
  }
}

}  // namespace x::pt
