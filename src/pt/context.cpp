#include "x/pt/context.hpp"

#include <fmt/core.h>
#include <fmt/format.h>
#include <spdlog/spdlog.h>

#include "x/pt/decl.hpp"
#include "x/pt/sema/nameresolution.hpp"

namespace x::pt {

//============
// Context
//============

// Module *Context::module(Path &&path) {
//   _modules.push_back(Module::Create(*this, this, std::move(path)));
//
//   return _modules.back();
// }
//
void Context::dump() {
  for (NodeId node : _items) {
    get_node(node).dump(*this);
  }
}

void Context::resolve_names() {
  _res = std::make_unique<sema::NameResolver>(this);
  for (NodeId node : _items) {
    get_node(node).nameres(*_res);
  }
}

Context::Context()
    : _items({create<Primitive>("i32", Primitive::Type::Int32).id()}) {}

}  // namespace x::pt
