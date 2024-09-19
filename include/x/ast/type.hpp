#pragma once

#include "fwd_decl.hpp"
#include "spdlog/spdlog.h"
#include "x/pt/fwd_decl.hpp"

namespace x::ast {

class Type : public AllowAlloc<Context, Type> {
  friend AllowAlloc;

 public:
  enum class Kind {
    Bool,
    String,
    I32,
    I64,
    Void,
  } _kind;
  int i;

  void prettyPrint() const {
    switch (_kind) {
      case Kind::Bool:
        spdlog::info("bool");
        break;
      case Kind::String:
        spdlog::info("string");
        break;
      case Kind::I32:
        spdlog::info("i32");
        break;
      case Kind::I64:
        spdlog::info("i64");
        break;
      case Kind::Void:
        spdlog::info("void");
        break;
    }
  }

 private:
  explicit Type(Kind kind) : _kind(kind) {};
};

}  // namespace x::ast
