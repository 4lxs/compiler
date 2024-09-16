#include "x/pt/expr.hpp"

#include "x/pt/stmt.hpp"

namespace x::pt {

std::unique_ptr<StructExpr> StructExpr::Create(std::vector<Field> &&fields) {
  return std::make_unique<StructExpr>(std::move(fields));
};

}  // namespace x::pt
