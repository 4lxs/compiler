#pragma once

#include <spdlog/spdlog.h>

#include <cassert>
#include <map>
#include <variant>

#include "x/common.hpp"
#include "x/pt/context.hpp"
#include "x/pt/fwd_decl.hpp"
#include "x/pt/path.hpp"
#include "x/pt/stmt.hpp"
#include "x/sema/fwd_decl.hpp"

namespace x::pt {

class Module : public AllowAlloc<Context, Module> {
  friend AllowAlloc;

 public:
  /// get stub to a name in this module
  Stub *get_stub(std::string &&local_name);

  /// get stub by a path. this may be defined in current module or it may be
  /// global
  Stub *get_stub(Path &&path);

 private:
  friend Context;
  friend sema::Sema;

  explicit Module(Context *ctx);

  /// set by the context upon creation
  Path _path;

  /// holds all items in this module. e.g. fn, var, type...
  std::map<std::string, Ptr<Stub>> _items;

 public:
  Context *_ctx;
};

/// holder for items. holds the definition and all references to some item.
///
/// this allows you to use an item before the definition has been parsed you
/// just use a stub and when the definition is seen, it checks all usages for
/// appropriate use
class Stub {
 public:
  /// use the stub as a function with given params. return value isn't specified
  Fn *function(StructExpr const &params);

  /// use the stub as a function with given params
  Fn *function(FnProto &&proto, Block *body);

  /// use the stub as a type
  Type *use_type();

  void define_type(Type *type);

  [[nodiscard]] std::string const &name() const { return _name; };

 private:
  friend Module;
  friend sema::Sema;

  template <typename T>
  auto accept(T const &consumer) const {
    assert(_holder.index() != 0);
    return std::visit(consumer, _holder);
  };

  explicit Stub(std::string &&name, Module *module);

  /// all the different types this stub can be when in monostate, it means the
  /// definition has not been provided yet.
  ///
  /// guaranteed to be valid only after a validate call
  using Holder = std::variant<std::monostate, Fn *, Type *>;
  Holder _holder;

  /// the module this stub is defined in
  Module *_module;

  /// the name of the item this stub represents. e.g. function name
  std::string _name;
};

}  // namespace x::pt
