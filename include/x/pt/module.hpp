#pragma once

#include <spdlog/spdlog.h>

#include <cassert>

#include "x/common.hpp"
#include "x/pt/fwd_decl.hpp"
#include "x/pt/path.hpp"
#include "x/sema/fwd_decl.hpp"

namespace x::pt {

class Module : private AllowAlloc<Context, Module> {
 public:
  template <typename T>
  void define(T *item) {
    _items.emplace_back(item);
  }

 private:
  friend sema::Sema;

  friend Context;  // should only be created by context
  friend AllowAlloc;
  explicit Module(Context *ctx, Path &&path) : _path(path), _ctx(ctx) {};

  /// set by the context upon creation
  Path _path;

  /// holds all items in this module. e.g. fn, var, type...
  std::vector<TopLevelDecl> _items;

 public:
  Context *_ctx;
};

}  // namespace x::pt
