#include "x/ast/module.hpp"

#include <exception>

#include "spdlog/spdlog.h"
#include "x/ast/context.hpp"

namespace x::ast {

Stub *Module::get_stub(std::string &&local_name) {
  return get_stub(*_path + std::move(local_name));
}

Stub *Module::get_stub(Path &&path) { return _ctx->stub(std::move(path)); }

Module::Module(Context *ctx, Path *path) : _path{path}, _ctx{ctx} {
  // spdlog::info("module {}", *path);
}

//===============
//= Stub
//===============

Stub::Stub(Path *path) : _path{path} {}

void Stub::define_function(Ptr<Fn> func) {
  if (_holder.index() != 0) {
    spdlog::error("double definition of stub {}", format_as(*_path));
    std::terminate();
  }

  _holder = std::move(func);
}

}  // namespace x::ast
