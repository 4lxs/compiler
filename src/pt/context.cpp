#include "x/pt/context.hpp"

#include <fmt/core.h>
#include <fmt/format.h>
#include <spdlog/spdlog.h>

#include "x/ast/expr.hpp"
#include "x/ast/stmt.hpp"
#include "x/ast/type.hpp"
#include "x/pt/module.hpp"

namespace x::pt {

//============
// Context
//============

Ptr<Context> Context::Create() {
  return std::unique_ptr<Context>(new Context());
}

Module *Context::module(Path &&path) {
  auto [itr, ok] = _modules.insert({path, Module::Create(*this, this)});
  if (!ok) {
    spdlog::error("module already exists");
  }
  itr->second->_path = std::move(path);

  return itr->second;
}

Stub *Context::stub(Path &&path) {
  auto itr = _modules.lower_bound(path);
  if (itr == _modules.end()) {
    spdlog::error("unable to find module for path {}", format_as(path));
    std::terminate();
  }
  Module *module = itr->second;

  return module->get_stub(std::move(path._components.back()));
}

//============
// Path
//============

Path::Path(std::vector<std::string> &&components, bool external)
    : _externalPath(external), _components{std::move(components)} {}

std::string format_as(Path const &path) {
  return fmt::format("{}{}", path._externalPath ? "::" : "",
                     fmt::join(path._components, "::"));
}

}  // namespace x::pt
