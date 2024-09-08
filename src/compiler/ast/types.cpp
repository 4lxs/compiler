#include "types.hpp"

#include <fmt/format.h>

#include <exception>
#include <memory>

#include "spdlog/spdlog.h"

namespace x::ast {

auto format_as(const Path &path) {
  return fmt::format(
      "{}{}", path._inProjScope ? "" : "::", fmt::join(path._components, "::"));
}

Path::Path(std::vector<std::string> &&components, bool isProj)
    : _inProjScope(isProj), _components{std::move(components)} {
  spdlog::info("created path: {} with {} components. first component: {}",
               format_as(*this), _components.size(), _components.front());
}

void Block::ret(std::optional<Expr> retval) {
  _body.emplace_back(std::make_unique<RetStmt>(std::move(retval)));
}

void Block::call(Path *path, Ptr<StructExpr> args) {
  Fn *func = _mod->findFn(path);
  assert(func);
  _body.emplace_back(std::make_unique<Call>(func, std::move(args)));
}

Block::Block(Module *mod) : _mod{mod} {}

Fn *Module::function(FnProto &&proto) {
  spdlog::info("adding function: {}", proto.name);
  auto func = std::make_unique<Fn>(this, std::move(proto));
  auto [itr, ok] =
      _functions.insert({path({func->name()}, true), std::move(func)});
  if (!ok) {
    spdlog::error("function {} already exists", itr->second->_proto.name);
    std::terminate();
  }

  return itr->second.get();
}

Type *Module::type(Path *path) { return _ctx->type(path); }

Path *Module::path(std::vector<std::string> &&components, bool isProj) {
  return _ctx->path(std::move(components), isProj);
}

Module::Module(Context *ctx, Path *path) : _path{path}, _ctx{ctx} {
  // spdlog::info("module {}", *path);
}

Module *Context::module(std::vector<std::string> &&module, bool isProj) {
  Path *pth = path(std::move(module), isProj);
  auto [itr, ok] = _modules.insert({pth, Ptr<Module>(new Module(this, pth))});
  if (!ok) {
    spdlog::error("module already exists");
    std::terminate();
  }

  return itr->second.get();
}

Type *Context::type(Path *path) {
  assert(path->_components.size() == 1 && path->_components[0] == "I32");

  return _types.at(path).get();
}

Path *Context::path(std::vector<std::string> &&components, bool isProj) {
  spdlog::info("adding path with {} components", components.size());
  Ptr<Path> path(new Path(std::move(components), isProj));
  spdlog::info("adding path {}", format_as(*path));

  auto [itr, inserted] = _paths.insert({*path, std::move(path)});
  spdlog::info("path {} inserted: {}", format_as(itr->first), inserted);

  return itr->second.get();
}

}  // namespace x::ast
