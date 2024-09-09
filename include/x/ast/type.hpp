#pragma once

#include "x/common.h"

namespace x::ast {

struct TypeV;

class Type {
 public:
  friend class Module;

  enum class Kind {
    Integer,
  } _kind;

  /// see `Context::validate()`
  TypeV* validate();

  /// see `Module::validate()`
  Ptr<TypeV> _val;
};

struct TypeV {
  Type::Kind kind;
};

}  // namespace x::ast
