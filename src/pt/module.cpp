#include "x/pt/module.hpp"

#include <exception>
#include <variant>

#include "spdlog/spdlog.h"
#include "x/ast/expr.hpp"
#include "x/ast/module.hpp"
#include "x/ast/stmt.hpp"
#include "x/ast/toplevel.hpp"
#include "x/ast/type.hpp"
#include "x/common.hpp"
#include "x/pt/context.hpp"
#include "x/pt/type.hpp"

namespace x::pt {

Stub *Module::get_stub(std::string &&local_name) {
  auto [itr, inserted] = _items.insert(
      {std::string{local_name},
       std::unique_ptr<Stub>(new Stub(std::move(local_name), this))});
  if (!inserted) {
    spdlog::info("item already defined");
  }

  return itr->second.get();
}

Stub *Module::get_stub(Path &&path) {
  if (path._components.size() == 1) {
    return get_stub(std::move(path._components.front()));
  }
  return _ctx->stub(std::move(path));
}

Module::Module(Context *ctx) : _ctx{ctx}, _path{{} /* updated by context */} {
  get_stub("I32")->define_type(std::make_unique<Type>(Type::Kind::Number));
}

//===============
//= Stub
//===============

Fn *Stub::function(StructExpr const & /*params*/) {
  Fn *ret{};

  if (_holder.index() == 0) {
    auto func =
        std::make_unique<Fn>(FnProto(), std::make_unique<Block>(), this);
    ret = func.get();
    _holder = std::move(func);
  } else if (auto *func = std::get_if<Ptr<Fn>>(&_holder)) {
    ret = func->get();
  } else {
    spdlog::error("different definitions of stub");
    std::terminate();
  }

  return ret;
}

Fn *Stub::function(FnProto &&proto, Ptr<Block> body) {
  spdlog::info("defining function {}", _name);
  if (_holder.index() != 0) {
    std::terminate();
  }

  Fn *ret{};

  if (_holder.index() == 0) {
    auto func = std::make_unique<Fn>(std::move(proto), std::move(body), this);
    ret = func.get();
    _holder = std::move(func);
  } else if (auto *func = std::get_if<Ptr<Fn>>(&_holder)) {
    ret = func->get();
  } else {
    spdlog::error("different definitions of stub");
    std::terminate();
  }

  return ret;
}

Type *Stub::use_type() {
  Type *ret{};

  if (_holder.index() == 0) {
    auto type = std::make_unique<Type>(Type::Kind::Number);
    ret = type.get();
    _holder = std::move(type);
  } else if (auto *type = std::get_if<Ptr<Type>>(&_holder)) {
    ret = type->get();
  } else {
    spdlog::error("different definitions of stub");
    std::terminate();
  }

  return ret;
}

void Stub::define_type(Ptr<Type> type) {
  spdlog::info("defining type {}", _name);
  if (_holder.index() != 0) {
    std::terminate();
  }

  _holder = std::move(type);
}

Stub::Stub(std::string &&name, Module *module)
    : _module{module}, _name{std::move(name)} {
  assert(module != nullptr);
}

}  // namespace x::pt
