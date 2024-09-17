#pragma once

#include "fwd_decl.hpp"
#include "x/pt/pt.hpp"

namespace x::ast {

class Type : public AllowAlloc<Context, Type> {
  friend AllowAlloc;

 public:
  enum class Kind {
    Bool,
    String,
    I32,
    Void,
  } _kind;

 private:
  explicit Type(Kind kind) : _kind(kind) {};
};

}  // namespace x::ast
