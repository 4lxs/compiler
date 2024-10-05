#include "x/pt/decl.hpp"

#include "x/pt/sema/nameresolution.hpp"

namespace x::pt {

std::string const &Decl::name(sema::NameResolver const &res) const {
  return _resolvedName.has_value() ? res.get_name(_resolvedName.value()).str()
                                   : _name;
}

void Primitive::nameres(sema::NameResolver &res) {
  spdlog::info("resolving primitive");
  res.define_name(*this);
}

void Primitive::dump(Context &ctx, uint8_t indent) {
  fmt::print("{:{}}Primitive: {}\n", "", indent, name(ctx.name_resolver()));
}

void FnDecl::nameres(sema::NameResolver &res) {
  res.define_name(*this);

  res.enter_scope();

  auto &ctx = *res._ctx;

  for (NodeId param : _params) {
    ctx.get_node(param).nameres(res);
  }

  ctx.get_node(_retTy).nameres(res);
  ctx.get_node(_body).nameres(res);

  res.exit_scope();
}

void FnDecl::dump(Context &ctx, uint8_t indent) {
  fmt::print("{:{}}FnDecl: {}\n", "", indent, name(ctx.name_resolver()));
  fmt::print("{:{}}ret:\n", "", indent + 1);
  ctx.get_node(_retTy).dump(ctx, indent + 2);
  fmt::print("{:{}}body:\n", "", indent + 1);
  ctx.get_node(_body).dump(ctx, indent + 2);
}

void VarDecl::nameres(sema::NameResolver &res) {
  res._ctx->get_node(_type).nameres(res);

  if (_val.has_value()) {
    res._ctx->get_node(_val.value()).nameres(res);
  }
}

void VarDecl::dump(Context &ctx, uint8_t indent) {
  fmt::print("{:{}}VarDecl: {}\n", "", indent, name(ctx.name_resolver()));
  fmt::print("{:{}}type:\n", "", indent + 1);
  ctx.get_node(_type).dump(ctx, indent + 2);
  if (_val.has_value()) {
    fmt::print("{:{}}val:\n", "", indent + 1);
    ctx.get_node(_val.value()).dump(ctx, indent + 2);
  }
}

}  // namespace x::pt
