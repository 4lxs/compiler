#include "x/pt/expr.hpp"

#include "x/pt/context.hpp"
#include "x/pt/node.hpp"
#include "x/pt/sema/nameresolution.hpp"

namespace x::pt {

void Integer::dump(Context& ctx, uint8_t indent) {
  fmt::print("{:{}}Integer: {}\n", "", indent, _val);
}

void Integer::nameres(sema::NameResolver& res) {}

void DeclUse::nameres(sema::NameResolver& res) {
  assert(_var._components.size() == 1);
  spdlog::info("nameres: {}", format_as(_var));
  _def = res.use_name(std::move(_var._components.front())).definition();
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
