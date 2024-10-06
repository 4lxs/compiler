#include "x/pt/stmt.hpp"

namespace x::pt {

void Return::dump(Context &ctx, uint8_t indent) {
  fmt::print("{:{}}Return\n", "", indent);
  if (_retVal) {
    fmt::print("{:{}}retval:\n", "", indent + 1);
    ctx.get_node(*_retVal).dump(ctx, indent + 2);
  }
}

void Return::nameres(sema::NameResolver &res) {
  if (_retVal.has_value()) {
    res._ctx->get_node(*_retVal).nameres(res);
  }
}

void Assign::dump(Context &ctx, uint8_t indent) {
  fmt::print("{:{}}Assign\n", "", indent);
  fmt::print("{:{}}lhs: \n", "", indent + 1);
  ctx.get_node(_assignee).dump(ctx, indent + 2);
  fmt::print("{:{}}rhs: \n", "", indent + 1);
  ctx.get_node(_value).dump(ctx, indent + 2);
}

void Assign::nameres(sema::NameResolver &res) {
  res._ctx->get_node(_assignee).nameres(res);
  res._ctx->get_node(_value).nameres(res);
}

void While::dump(Context &ctx, uint8_t indent) {
  fmt::print("{:{}}While ", "", indent);
  fmt::print("{:{}}cond: \n", "", indent + 1);
  ctx.get_node(_cond).dump(ctx, indent + 2);
  fmt::print("{:{}}body: \n", "", indent + 1);
  ctx.get_node(_body).dump(ctx, indent + 2);
}

void While::nameres(sema::NameResolver &res) {
  res._ctx->get_node(_cond).nameres(res);
  res._ctx->get_node(_body).nameres(res);
}

}  // namespace x::pt
