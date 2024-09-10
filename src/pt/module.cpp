#include "x/pt/module.hpp"

#include <exception>
#include <variant>

#include "spdlog/spdlog.h"
#include "x/common.h"
#include "x/pt/context.hpp"

namespace x::pt {

Stub *Module::get_stub(std::string &&local_name) {
  spdlog::info("getting stub: local={}", local_name);
  return get_stub(/* *_path + */ Path({std::move(local_name)}, false));
}

Stub *Module::get_stub(Path &&path) {
  // return _ctx->stub(std::move(path));

  spdlog::info("getting stub: {}", format_as(path));
  Path *stubPath = _ctx->get_or_insert_path(std::move(path));

  auto [it, inserted] =
      _items.insert({stubPath, std::unique_ptr<Stub>(new Stub(stubPath))});

  return it->second.get();
}

Module::Module(Context *ctx, Path *path) : _path{path}, _ctx{ctx} {
  // spdlog::info("module {}", *path);

  get_stub(Path({"I32"}, false))
      ->define_type(std::make_unique<Type>(Type::Kind::Integer));
}

auto Module::validate() -> Ptr<Val> {
  auto val = std::make_unique<Val>();

  // first validate all stubs. don't take out the value until all stubs have
  // been validated. that is to allow stubs to cross-link. e.g. fn val stub
  // needs pointer to return type stub

  for (auto &[_, stub] : _items) {
    spdlog::info("validating: {}", format_as(*stub->_path));
    stub->validate();
  }

  for (auto &[_, stub] : _items) {
    spdlog::info("adding: {}", format_as(*stub->_path));
    std::visit(overloaded{[&val = *val](Ptr<Fn> item) {
                            spdlog::info("adding function: {}",
                                         fmt::ptr(item->_val.get()));
                            val._functions.push_back(std::move(item->_val));
                          },
                          [&val = *val](Ptr<Type> item) {
                            val._types.push_back(std::move(item->_val));
                          },
                          [](std::monostate) {
                            spdlog::error("undefined item");
                            std::terminate();
                          }

               },
               std::move(stub->_holder));
  }

  return std::move(val);
}

//===============
//= Stub
//===============

void Stub::validate() {
  if (_holder.index() == 0) {
    spdlog::error("validating undefined stub");
    std::terminate();
  }

  spdlog::info("validating children");

  std::visit(
      overloaded{[](std::monostate) {}, [](auto &val) { val->validate(); }},
      _holder);

  // TODO: make sure all usages are good
}

void Stub::define_function(Ptr<Fn> func) {
  spdlog::info("defining function {} at {}", func->name(), format_as(*_path));
  if (_holder.index() != 0) {
    spdlog::error("double definition of stub {}", format_as(*_path));
    std::terminate();
  }

  _holder = std::move(func);
}

void Stub::use_type() {}

void Stub::define_type(Ptr<Type> type) {
  spdlog::info("defining type at {}", format_as(*_path));
  if (_holder.index() != 0) {
    spdlog::error("double definition of stub {}", format_as(*_path));
    std::terminate();
  }

  _holder = std::move(type);
}

Stub::Stub(Path *path) : _path{path} {}

}  // namespace x::pt
