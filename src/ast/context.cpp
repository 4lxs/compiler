#include "x/ast/context.hpp"

#include <fmt/core.h>
#include <fmt/format.h>
#include <spdlog/spdlog.h>

#include "x/ast/module.hpp"

namespace x::ast {

//============
// Context
//============

Ptr<Context> Context::Create() {
  return std::unique_ptr<Context>(new Context());
}

auto Context::validate(Ptr<Context> context) -> Ptr<Val> {
  auto val = std::make_unique<Val>();

  for (auto &&[_, module] : context->_modules) {
    val->modules.push_back(module->validate());
  }

  return std::move(val);
}

Module *Context::module(Path &&path) {
  path._type = Path::Module;
  Path *global_path = get_or_insert_path(std::move(path));
  auto [itr, ok] = _modules.insert(
      {global_path, Ptr<Module>(new Module(this, global_path))});
  if (!ok) {
    spdlog::error("module already exists");
  }

  return itr->second.get();
}

// Stub *Context::stub(Path &&path) {
//   get_or_insert_path(std::move(path));
// }

Path *Context::get_or_insert_path(Path &&global_path) {
  Ptr<Path> path(new Path(std::move(global_path)));

  auto [itr, inserted] = _paths.insert({*path, std::move(path)});
  if (!inserted) {
    spdlog::info("path already defined");
  }

  spdlog::info("path: {}", fmt::ptr(itr->second.get()));
  return itr->second.get();
}

//============
// Path
//============

Path::Path(std::vector<std::string> &&components, bool external)
    : _externalPath(external), _components{std::move(components)} {}

std::string format_as(const Path &path) {
  return fmt::format("{}{}", path._externalPath ? "::" : "",
                     fmt::join(path._components, "::"));
}

}  // namespace x::ast
