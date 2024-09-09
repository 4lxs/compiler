#pragma once

#include <map>
#include <vector>

#include "x/ast/module.hpp"
#include "x/common.h"

namespace x::ast {

class Fn;

class Path;

/// keeps data through entire compilation process
/// allowing you to get stuff from other modules
/// even if those modules weren't processed yet.
///
/// while processing one module, we get to
/// use a::b::Type
/// you query context->getType(). the context will
/// give you a type. when later processing module a::b,
/// you call context->add(a::b, Type). the context will
/// be able to link those two together
class Context {
 public:
  static Ptr<Context> Create();

  struct Val {
    std::vector<Ptr<Module::Val>> modules;
  };

  /// checks for inconsistencies. undefined stubs, multiple
  /// definitions, ...
  /// it also evaluates items to their definition and links
  /// them with other stubs.
  ///
  /// note that calling this function invalidates the context and all pointers
  /// obtained from it
  static Ptr<Val> validate(Ptr<Context> context);

  /// return the module at path and create it if it doesn't
  /// exist
  Module *module(Path &&path);

 private:
  Context() = default;

  friend Module;

  /// this function is called by module to get stubs
  /// outside itself
  Stub *stub(Path &&path);

  /// return a global path representing
  Path *get_or_insert_path(Path &&global_path);

  std::map<Path, Ptr<Path>> _paths;
  std::map<Path *, Ptr<Module>> _modules;
};

/// path is module-independent way to reference items
/// that path a::b::C in one module always refers to
/// the same item in another module. it should always
/// be immutable after construction by the Context
class Path {
 public:
  /// @param external: ::a::b true, a::b false
  /// @param module: {"a", "b"} in both cases
  Path(std::vector<std::string> &&components, bool external);

  Path operator+(std::string suffix) const {
    std::vector components{_components};
    components.push_back(std::move(suffix));
    return Path{std::move(components), _externalPath};
  }

  constexpr std::strong_ordering operator<=>(const Path &other) const {
    if (_externalPath != other._externalPath) {
      return _externalPath ? std::strong_ordering::greater
                           : std::strong_ordering::less;
    }
    if (_type != other._type) {
      return _type <=> other._type;
    }
    return _components <=> other._components;
  };

 private:
  friend Context;
  friend std::string format_as(const Path &path);

  /// path external to this project. in x they
  /// are represented by leading :: e.g. ::a
  bool _externalPath;

  /// which type is this path for. this is to allow
  /// different kinds of items to have same names.
  /// meaning you can have a function c in namespace a::b
  /// with path a::b::c and a module a::b::c
  ///
  /// default is value. the Context changes it to module
  /// when requesting a module
  enum class Type {
    Module,
    Value,
  } _type = Value;

  using enum Type;

  std::vector<std::string> _components;
};

std::string format_as(const Path &path);

}  // namespace x::ast
