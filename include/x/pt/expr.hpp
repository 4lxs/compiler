#pragma once

#include <x/common.h>

#include <variant>
#include <vector>

namespace x::pt {

class PrimaryExpr {
 public:
  enum class Type {
    Int,
    String,
    Bool,
  };

  static Ptr<PrimaryExpr> Bool(bool isTrue);

  static Ptr<PrimaryExpr> Int(std::string val) {
    return Ptr<PrimaryExpr>(new PrimaryExpr(std::move(val), Type::Int));
  };

  // private:
  // PrimaryExpr(std::string val, Type type)
  //     : _val{std::move(val)}, _type{type} {};

  std::string _val;
  Type _type;
};

class IfExpr {};

class ParenExpr;
class BinaryExpr;
class StructExpr;

class Expr {
 public:
  template <typename T>
  auto accept(T const &consumer) const {
    return std::visit(consumer, _val);
  };

  template <typename T>
  [[nodiscard]] auto is() const -> bool {
    return std::holds_alternative<T>(_val);
  }

  template <typename T>
  [[nodiscard]] auto to() const -> Ptr<T> & {
    return *std::get<Ptr<T>>(_val);
  }

  template <typename T>
  [[nodiscard]] auto into() -> Ptr<T> {
    return std::move(std::get<Ptr<T>>(_val));
  }

  std::variant<Ptr<PrimaryExpr>, Ptr<IfExpr>, Ptr<BinaryExpr>, Ptr<ParenExpr>,
               Ptr<StructExpr>>
      _val;
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

struct Field {
  std::string name;
  Expr value;
};

class StructExpr {
 public:
  static std::unique_ptr<StructExpr> Create(std::vector<Field> &&fields) {
    return std::make_unique<StructExpr>(std::move(fields));
  };

  std::vector<Field> fields;
};

}  // namespace x::pt
