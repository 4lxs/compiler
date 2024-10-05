#include "x/pt/expr.hpp"

#include "x/pt/context.hpp"
#include "x/pt/node.hpp"
#include "x/pt/sema/nameresolution.hpp"

namespace x::pt {

void Integer::dump(Context& ctx, uint8_t indent) {
  fmt::print("{:{}}Integer: {}\n", "", indent, _val);
}

void DeclUse::nameres(sema::NameResolver& res) {
  assert(_var._components.size() == 1);
  _def = res.use_name(std::move(_var._components.front())).ref();
}

void DeclUse::dump(Context& ctx, uint8_t indent) {
  if (!_def.has_value()) {
    fmt::print("{:{}}DeclUse: {}\n", "", indent, format_as(_var));
    return;
  }

  sema::Name const& ref = ctx.name_resolver().get_name(_def.value());

  Node& node = ctx.get_node(ref.definition());
  node.dump(ctx, indent + 2);
}

}  // namespace x::pt
