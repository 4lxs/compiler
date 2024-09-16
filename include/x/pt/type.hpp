#pragma once

#include "x/ast/ast.hpp"
#include "x/common.hpp"

namespace x::pt {

class Type {
 public:
  friend class Module;

  enum class Kind {
    /// representing a boolean. in code: true, false
    Bool,

    /// representing just a string. in code: "hello"
    String,

    /// this is a generic number type that can be converted to any concrete
    /// number type (i32, f32, ...)
    ///
    /// upon conversion, it will be checked for correctness (making sure it's
    /// not to big for the target type, it's correctly signed...)
    ///
    /// in code, it's any normal number (1, 2., 2.0, ...)
    Number,

    /// following are concrete number types that cannot be implicitly converted
    /// to any other type
    ///
    /// in code, this is a typed number (1i32, 32.f32...)

    // I8,
    // I16,
    // I32,
    // I64,
    // I128,
    //
    // U8,
    // U16,
    // U32,
    // U64,
    // U128,
    //
    // F32,
    // F64,
  } _kind;
};

}  // namespace x::pt
