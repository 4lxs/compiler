#include "x/pt/module.hpp"

#include <exception>
#include <variant>

#include "spdlog/spdlog.h"
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

Module::Module(Context *ctx) : _path{{} /* updated by context */}, _ctx{ctx} {
  get_stub("I32")->define_type(_ctx->_numTy.get());
}

//===============
//= Stub
//===============

Fn *Stub::function(StructExpr const & /*params*/) {
  Fn *ret{};

  if (_holder.index() == 0) {
    Context &ctx = *_module->_ctx;
    ret = Fn::Create(ctx, FnProto(), Block::Create(ctx), this);
    _holder = ret;
  } else if (auto *func = std::get_if<Fn *>(&_holder)) {
    ret = *func;
  } else {
    spdlog::error("different definitions of stub");
    std::terminate();
  }

  return ret;
}

Fn *Stub::function(FnProto &&proto, Block *body) {
  spdlog::info("defining function {}", _name);
  if (_holder.index() != 0) {
    std::terminate();
  }

  Fn *ret{};

  if (_holder.index() == 0) {
    ret = Fn::Create(*_module->_ctx, std::move(proto), body, this);
    _holder = ret;
  } else if (auto *func = std::get_if<Fn *>(&_holder)) {
    ret = *func;
  } else {
    spdlog::error("different definitions of stub");
    std::terminate();
  }

  return ret;
}

Type *Stub::use_type() {
  Type *ret{};

  if (_holder.index() == 0) {
    auto *type = _module->_ctx->_numTy.get();
    _holder = type;
  } else if (auto *type = std::get_if<Type *>(&_holder)) {
    ret = *type;
  } else {
    spdlog::error("different definitions of stub");
    std::terminate();
  }

  return ret;
}

void Stub::define_type(Type *type) {
  spdlog::info("defining type {}", _name);
  if (_holder.index() != 0) {
    std::terminate();
  }

  _holder = type;
}

Stub::Stub(std::string &&name, Module *module)
    : _module{module}, _name{std::move(name)} {
  assert(module != nullptr);
}

}  // namespace x::pt
