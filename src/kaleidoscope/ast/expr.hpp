#pragma once

#include "../types.hpp"
#include "helpers.hpp"

class ExprAST {
 public:
  explicit ExprAST(Token tok);
  virtual ~ExprAST() = default;

  virtual std::ostream& dump(std::ostream& out, size_t /*ind*/);

  [[nodiscard]] size_t line() const;
  [[nodiscard]] size_t col() const;

 protected:
  Token _tok;
};

class BinaryExprAST : public ExprAST {
 public:
  BinaryExprAST(Token tok, std::unique_ptr<ExprAST> lhs,
                std::unique_ptr<ExprAST> rhs);

  std::ostream& dump(std::ostream& out, size_t ind) override;

 private:
  std::unique_ptr<ExprAST> _lhs, _rhs;
};

class UnaryExprAST : public ExprAST {
 public:
  UnaryExprAST(Token tok, std::unique_ptr<ExprAST> rhs);

  std::ostream& dump(std::ostream& out, size_t ind) override;

 private:
  std::unique_ptr<ExprAST> _rhs;
};

class NumberExprAST : public ExprAST {
 public:
  explicit NumberExprAST(Token tok);
  std::ostream& dump(std::ostream& out, size_t ind) override;
};

class VariableExprAST : public ExprAST {
 public:
  explicit VariableExprAST(Token tok);
  std::ostream& dump(std::ostream& out, size_t ind) override;
};
