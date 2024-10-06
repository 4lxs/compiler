#include "x/ast/stmt.hpp"

#include "x/ast/expr.hpp"

namespace x::ast {

Return::~Return() = default;

Return::Return(Ptr<Expr> val, Rc<Label> block)
    : Stmt(SK_Return), _val(std::move(val)), _blockLbl(std::move(block)) {}

Loop::~Loop() = default;

Loop::Loop(Ptr<Block> body) : Stmt(SK_Loop), _body(std::move(body)) {}

};  // namespace x::ast
