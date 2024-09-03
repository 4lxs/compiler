#pragma once

#include <cassert>
#include <memory>
#include <optional>

#include "generator.hpp"
#include "scanner.hpp"

std::optional<int> get_tok_precedence(Token::Type tok);

class Parser {
 public:
  explicit Parser(std::unique_ptr<Scanner> scanner);

  // @return true if ended parsing
  bool parse_toplevel(Generator& /*gen*/);

  std::unique_ptr<FunctionAST> parse_function();

  // prototype
  //  ::= id '(' (id (, id)* )? ')'
  std::unique_ptr<PrototypeAST> parse_prototype();

  // expression
  //  ::= binary
  std::unique_ptr<ExprAST> parse_expression();

  // binary
  //  ::= unary (op unary)*
  std::unique_ptr<ExprAST> parse_binary();

  /// unary
  ///   ::= op unary
  ///   ::= primary
  std::unique_ptr<ExprAST> parse_unary();

  std::unique_ptr<ExprAST> parse_primary();

 private:
  void advance();

  std::unique_ptr<Scanner> _scanner;
  Token _curr;
  Token _next;
};
