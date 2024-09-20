#pragma once

#include <string_view>

#include "fwd_decl.hpp"
#include "spdlog/spdlog.h"
#include "x/ast/stmt.hpp"
#include "x/common.hpp"

namespace x::ast {

class Type : public Decl, public AllowAlloc<Context, Type> {
  friend AllowAlloc;

 public:
  [[nodiscard]] std::string_view name() const;

  enum class Kind {
    Bool,
    String,
    I32,
    I64,
    Void,
  } _kind;

  void prettyPrint() const {
    spdlog::info("name: {}", _name);
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

  std::string_view _name;

 private:
  explicit Type(Kind kind, std::string_view name)
      : Decl(DeclKind::Type, name), _kind(kind) {};

 public:
  static bool classof(Decl const* decl) {
    return decl->get_kind() == DeclKind::Type;
  }
};

}  // namespace x::ast
