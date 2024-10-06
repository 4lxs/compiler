#include "x/pt/expr.hpp"

#include "x/pt/context.hpp"
#include "x/pt/node.hpp"
#include "x/pt/sema/nameresolution.hpp"

namespace x::pt {

void IfExpr::dump(Context& ctx, uint8_t indent) {
  fmt::print("{:{}}IfExpr\n", "", indent);
  fmt::print("{:{}}Condition:\n", "", indent + 1);
  ctx.get_node(cond).dump(ctx, indent + 2);
  fmt::print("{:{}}Then:\n", "", indent + 1);
  ctx.get_node(then).dump(ctx, indent + 2);
  if (else_.has_value()) {
    fmt::print("{:{}}Else:\n", "", indent + 1);
    ctx.get_node(else_.value()).dump(ctx, indent + 2);
  }
}

void IfExpr::nameres(sema::NameResolver& res) {
  spdlog::info("IfExpr::nameres");
  res._ctx->get_node(cond).nameres(res);
  res._ctx->get_node(then).nameres(res);
  if (else_.has_value()) {
    res._ctx->get_node(else_.value()).nameres(res);
  }
}

void BinaryNode::dump(Context& ctx, uint8_t indent) {
  fmt::print("{:{}}Binary: {}\n", "", indent, fmt::underlying(op));
  fmt::print("{:{}}Left:\n", "", indent + 1);
  ctx.get_node(l).dump(ctx, indent + 2);
  fmt::print("{:{}}Right:\n", "", indent + 1);
  ctx.get_node(r).dump(ctx, indent + 2);
}

void BinaryNode::nameres(sema::NameResolver& res) {
  res._ctx->get_node(l).nameres(res);
  res._ctx->get_node(r).nameres(res);
}

void Integer::dump(Context& /*ctx*/, uint8_t indent) {
  fmt::print("{:{}}Integer: {}\n", "", indent, _val);
}

void Integer::nameres(sema::NameResolver& /*res*/) {}

void DeclUse::nameres(sema::NameResolver& res) {
  assert(_var._components.size() == 1);
  spdlog::info("nameres: {}", format_as(_var));
  _def = res.use_name(_var._components.front()).definition();
}

void DeclUse::dump(Context& ctx, uint8_t indent) {
  if (!_def.has_value()) {
    fmt::print("{:{}}DeclUse: {}\n", "", indent, format_as(_var));
    return;
  }

  Node& node = ctx.get_node(_def.value());
  node.dump(ctx, indent + 2);
}

}  // namespace x::pt
