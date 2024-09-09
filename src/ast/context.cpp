#include "x/ast/context.hpp"

#include <fmt/core.h>
#include <fmt/format.h>
#include <spdlog/spdlog.h>

#include "x/ast/module.hpp"

namespace x::ast {

//============
// Context
//============

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

Path *Context::get_or_insert_path(Path &&global_path) {
  Ptr<Path> path(new Path(std::move(global_path)));

  auto [itr, inserted] = _paths.insert({*path, std::move(path)});
  if (!inserted) {
    spdlog::info("module already defined");
  }

  return itr->second.get();
}

//============
// Path
//============

Path::Path(std::vector<std::string> &&components, bool isProj)
    : _externalPath(isProj), _components{std::move(components)} {
  spdlog::info("created path: {} with {} components. first component: {}",
               format_as(*this), _components.size(), _components.front());
}

std::string format_as(const Path &path) {
  return fmt::format("{}{}", path._externalPath ? "" : "::",
                     fmt::join(path._components, "::"));
}

}  // namespace x::ast
