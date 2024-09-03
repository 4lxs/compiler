#include "expr.hpp"

ExprAST::ExprAST(Token tok) : _tok(std::move(tok)) {}

std::ostream& ExprAST::dump(std::ostream& out, size_t /*ind*/) {
  return out << ':' << line() << ':' << col() << '\n';
}

[[nodiscard]] size_t ExprAST::line() const { return _tok.loc.line; }
[[nodiscard]] size_t ExprAST::col() const { return _tok.loc.col; }

//---
// BinaryExprAST
//---

BinaryExprAST::BinaryExprAST(Token tok, std::unique_ptr<ExprAST> lhs,
                             std::unique_ptr<ExprAST> rhs)
    : ExprAST(std::move(tok)), _lhs(std::move(lhs)), _rhs(std::move(rhs)) {}

std::ostream& BinaryExprAST::dump(std::ostream& out, size_t ind) {
  ExprAST::dump(out << "binary" << _tok.value, ind);
  _lhs->dump(indent(out, ind) << "LHS:", ind + 1);
  _rhs->dump(indent(out, ind) << "RHS:", ind + 1);
  return out;
}

//---
// UnaryExprAST
//---

UnaryExprAST::UnaryExprAST(Token tok, std::unique_ptr<ExprAST> rhs)
    : ExprAST(std::move(tok)), _rhs{std::move(rhs)} {}

std::ostream& UnaryExprAST::dump(std::ostream& out, size_t ind) {
  ExprAST::dump(out << "unary" << _tok.value, ind);
  _rhs->dump(out, ind + 1);
  return out;
}

//---
// NumberExprAST
//---

NumberExprAST::NumberExprAST(Token tok) : ExprAST(std::move(tok)) {}

std::ostream& NumberExprAST::dump(std::ostream& out, size_t ind) {
  return ExprAST::dump(out << _tok.value, ind);
}

//---
// VariableExprAST
//---

VariableExprAST::VariableExprAST(Token tok) : ExprAST(std::move(tok)) {}

std::ostream& VariableExprAST::dump(std::ostream& out, size_t ind) {
  return ExprAST::dump(out << _tok.value, ind);
}
