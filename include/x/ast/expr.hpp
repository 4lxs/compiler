#pragma once

#include <cstdlib>
#include <optional>
#include <variant>
#include <vector>

#include "ast.hpp"
#include "x/common.hpp"
#include "x/pt/expr.hpp"

namespace x::ast {

class IntegerE {
 public:
  static Ptr<IntegerE> Int32(std::string val, uint8_t base = 10) {
    return std::make_unique<IntegerE>(std::move(val), 32, base);
  }

  static Ptr<IntegerE> Int64(std::string val, uint8_t base = 10) {
    return std::make_unique<IntegerE>(std::move(val), 64, base);
  }

  std::string _val;
  uint32_t _width;
  uint8_t _base;
};

class StringE {
 public:
  std::string _val;
};

class BoolE {
 public:
  bool _val;
};

class StructE {
 public:
  std::vector<Expr> _fields;
};

class CallE {
 public:
  Fn *fn;
  Ptr<StructE> args;
};

class BlockE {
 public:
  std::vector<ast::Stmt> body;
  std::optional<Expr> terminator;
};

class IfE {
 public:
  Expr _cond;
  Ptr<BlockE> _then;
  Ptr<BlockE> _else;
};

}  // namespace x::ast
