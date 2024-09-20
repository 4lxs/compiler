#include "x/pt/context.hpp"

#include <fmt/core.h>
#include <fmt/format.h>
#include <spdlog/spdlog.h>

#include "x/pt/module.hpp"

namespace x::pt {

//============
// Context
//============

Ptr<Context> Context::Create() {
  return std::unique_ptr<Context>(new Context());
}

Module *Context::module(Path &&path) {
  _modules.push_back(Module::Create(*this, this, std::move(path)));

  return _modules.back();
}

}  // namespace x::pt
