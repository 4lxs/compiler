#pragma once

#include <cassert>
#include <map>
#include <variant>

#include "spdlog/spdlog.h"
#include "x/ast/stmt.hpp"
#include "x/ast/type.hpp"
#include "x/common.h"

namespace x::ast {

class Context;
class Path;

class Stub;

class Module {
 public:
  struct Val {
    std::vector<Ptr<FnV>> _functions;
    std::vector<Ptr<TypeV>> _types;
  };

  /// get stub to a name in this module
  Stub *get_stub(std::string &&local_name);

  /// get stub by a path. this may be defined in current module or it may be
  /// global
  Stub *get_stub(Path &&path);

 private:
  friend Context;

  explicit Module(Context *ctx, Path *path);

  /// this function is called by Context. see it respective docs
  Ptr<Val> validate();

  Path *_path;

  /// holds all items in this module. e.g. fn, var,
  /// type...
  std::map<Path *, Ptr<Stub>> _items;

  Context *_ctx;
};

/// holder for items. holds the definition and all references to some item.
///
/// this allows you to use an item before the definition has been parsed you
/// just use a stub and when the definition is seen, it checks all usages for
/// appropriate use
class Stub {
 public:
  /// use the stub as a function and call it with
  /// specified parameters
  void use_call();

  /// define the stub as a function
  void define_function(Ptr<Fn> func);

  /// use the stub as a type
  void use_type();

  void define_type(Ptr<Type> type);

  template <typename T>
  auto accept(T const &consumer) const {
    assert(_holder.index() != 0);
    return std::visit(consumer, _holder);
  };

  template <typename T>
  T *validate() {
    validate();
    if (const Ptr<T> *val = std::get_if<Ptr<T>>(&_holder)) {
      return val->get();
    }

    spdlog::error("invalid stub access");
    std::terminate();
  }

 private:
  friend Context;
  friend Module;

  explicit Stub(Path *path);

  /// see Contexts corresponding function for docs
  void validate();

  /// all the different types this stub can be when in monostate, it means the
  /// definition has not been provided yet.
  ///
  /// guaranteed to be valid only after a validate call
  using Holder = std::variant<std::monostate, Ptr<Fn>, Ptr<Type>>;
  Holder _holder;

  Path *_path;
};

}  // namespace x::ast
