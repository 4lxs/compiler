#include "x/pt/stmt.hpp"

namespace x::pt {

void Return::dump(Context &ctx, uint8_t indent) {
  fmt::print("{:{}}Return\n", "", indent);
  if (_retVal) {
    fmt::print("{:{}}retval:\n", "", indent + 1);
    ctx.get_node(*_retVal).dump(ctx, indent + 2);
  }
}

void Assign::dump(Context &ctx, uint8_t indent) {
  fmt::print("{:{}}Assign\n", "", indent);
  fmt::print("{:{}}lhs: \n", "", indent + 1);
  ctx.get_node(_assignee).dump(ctx, indent + 2);
  fmt::print("{:{}}rhs: \n", "", indent + 1);
  ctx.get_node(_value).dump(ctx, indent + 2);
}

void While::dump(Context &ctx, uint8_t indent) {
  fmt::print("{:{}}While ", "", indent);
  fmt::print("{:{}}cond: \n", "", indent + 1);
  ctx.get_node(_cond).dump(ctx, indent + 2);
  fmt::print("{:{}}body: \n", "", indent + 1);
  ctx.get_node(_body).dump(ctx, indent + 2);
}

}  // namespace x::pt
