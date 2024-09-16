#pragma once

#include "x/pt/pt.hpp"

namespace x::ast {

class Type {
 public:
  enum class Kind {
    Bool,
    String,
    I32,
    Void,
  } _kind;

  pt::Type* pt;
};

}  // namespace x::ast
