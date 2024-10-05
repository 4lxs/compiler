#include "x/ast/stmt.hpp"

#include "x/ast/expr.hpp"

namespace x::ast {

Return::~Return() = default;

Return::Return(Ptr<Expr> val) : Stmt(SK_Return), _val(std::move(val)) {}

};  // namespace x::ast
