#pragma once

#include <map>

#include "x/common.hpp"
#include "x/pt/module.hpp"
#include "x/pt/pt.hpp"
#include "x/sema/sema.hpp"

namespace x::pt {

/// keeps data about the entire parse tree
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

  /// return the module at path and create it if it doesn't
  /// exist
  Module *module(Path &&path);

  /// get stub to any item in any module by it's full path
  Stub *stub(Path &&path);

 private:
  Context() = default;

  friend Module;
  friend sema::Sema;

  std::map<Path, Ptr<Module>> _modules;
};

}  // namespace x::pt
