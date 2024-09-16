#pragma once

#include <optional>
#include <variant>
#include <vector>

#include "x/ast/ast.hpp"
#include "x/common.hpp"
#include "x/pt/block.hpp"
#include "x/pt/pt.hpp"
#include "x/sema/sema.hpp"

namespace x::pt {

struct IntegerE {
  std::string _val;
};

struct BoolE {
  bool _val;
};

struct StringE {
  std::string _val;
};

struct IfExpr {
  pt::Expr cond;
  Ptr<Block> then;
  Ptr<Block> else_;
};

class BinaryExpr {
 public:
  enum class Operator {
    Plus,
    Minus,
    Star,
    Slash,
    Greater,
    Less,
  };

  Expr l;
  Expr r;
  Operator op;
};

class ParenExpr {
 public:
  Expr inner;
};

class Call {
 public:
  Fn *fn;
  Ptr<StructExpr> args;
};

struct Field {
  std::string name;
  Expr value;
};

class StructExpr {
 public:
  static std::unique_ptr<StructExpr> Create(std::vector<Field> &&fields);

  std::vector<Field> fields;
};

}  // namespace x::pt
