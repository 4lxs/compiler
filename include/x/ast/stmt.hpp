#pragma once

#include <optional>
#include <variant>

#include "ast.hpp"
#include "x/ast/expr.hpp"
#include "x/common.hpp"

namespace x::ast {

class Stmt {
 public:
  std::variant<Ptr<Ret>, Expr> _val;
};

class Ret {
 public:
  std::optional<Expr> _val;
};

}  // namespace x::ast
