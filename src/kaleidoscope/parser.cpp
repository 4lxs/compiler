#include "parser.hpp"

#include <iostream>
#include <map>

#include "spdlog/spdlog.h"

std::optional<int> get_tok_precedence(Token::Type tok) {
  static const std::map<Token::Type, int> binopPrecedenceMap = {
      {Token::Type::plus, 20},
      {Token::Type::minus, 20},
      {Token::Type::star, 40},
      {Token::Type::slash, 40},
  };

  auto precedenceIt = binopPrecedenceMap.find(tok);
  return precedenceIt == binopPrecedenceMap.end()
             ? std::nullopt
             : std::optional(precedenceIt->second);
}

Parser::Parser(std::unique_ptr<Scanner> scanner)
    : _scanner{std::move(scanner)},
      _curr{_scanner->get_token()},
      _next{_scanner->get_token()} {}

bool Parser::parse_toplevel(Generator& /*gen*/) {
  switch (_curr.type) {
    case Token::Type::eof:
      return true;
    case Token::Type::fn:
      // gen.add(parse_function());
      parse_function()->dump(std::cout, 0);
      return false;
    case Token::Type::let:
      // return parse_definition();
    case Token::Type::ident:
    case Token::Type::if_:
    case Token::Type::then:
    case Token::Type::else_:
    case Token::Type::for_:
    case Token::Type::in:
    case Token::Type::number:
    case Token::Type::plus:
    case Token::Type::minus:
    case Token::Type::star:
    case Token::Type::slash:
    case Token::Type::exclam:
    case Token::Type::lparen:
    case Token::Type::rparen:
    case Token::Type::colon:
      throw std::runtime_error("unexpected token: " +
                               Token::to_string(_curr.type));
  }
}

std::unique_ptr<FunctionAST> Parser::parse_function() {
  assert(_curr.type == Token::Type::fn);

  std::unique_ptr<PrototypeAST> proto = parse_prototype();
  assert(proto);

  return std::make_unique<FunctionAST>(std::move(proto), nullptr);

  // std::unique_ptr<ExprAST> body = parse_expression();
  // assert(body);
}

// prototype
//  ::= id '(' (id (, id)* )? ')'
std::unique_ptr<PrototypeAST> Parser::parse_prototype() {
  assert(_curr.type == Token::Type::fn);
  SourceLocation fnLoc = _curr.loc;
  advance();

  if (_curr.type != Token::Type::ident) {
    spdlog::error("expected function name after 'fn'");
    return nullptr;
  }
  std::string fnName = std::move(_curr.value);

  advance();

  if (_curr.type != Token::Type::lparen) {
    spdlog::error("expected '(' after function name");
    return nullptr;
  }
  advance();

  std::vector<std::string> fnArgs;
  for (; _curr.type == Token::Type::ident; advance()) {
    fnArgs.emplace_back(std::move(_curr.value));
  }

  if (_curr.type != Token::Type::rparen) {
    spdlog::error("expected ')' to close function parameter list");
    return nullptr;
  }

  advance();  // eat ')'

  return std::make_unique<PrototypeAST>(std::move(fnName), std::move(fnArgs),
                                        fnLoc);
}

// expression
//  ::= binary
std::unique_ptr<ExprAST> Parser::parse_expression() { return parse_binary(); }

// binary
//  ::= unary (op unary)*
std::unique_ptr<ExprAST> Parser::parse_binary() {
  std::unique_ptr<ExprAST> lhs = parse_unary();
  if (!lhs) {
    return nullptr;
  }

  auto parse_op_rhs = [this](auto& self, std::unique_ptr<ExprAST> lhs,
                             int prec) -> std::unique_ptr<ExprAST> {
    while (true) {
      std::optional<int> currPrec = get_tok_precedence(_curr.type);
      if (!currPrec.has_value() || currPrec < prec) {
        // If this is a binop that binds at least as tightly as the current
        // binop, consume it, otherwise we are done.
        return lhs;
      }

      Token binOp = std::move(_curr);
      advance();  // eat operator

      std::unique_ptr<ExprAST> rhs = parse_unary();
      if (!rhs) {
        return nullptr;
      }

      // If BinOp binds less tightly with RHS than the operator after RHS, let
      // the pending operator take RHS as its LHS.
      std::optional<int> nextPrec = get_tok_precedence(_curr.type);
      if (nextPrec.has_value() && currPrec < nextPrec) {
        rhs = self(self, std::move(rhs), *currPrec + 1);
        if (!rhs) {
          return nullptr;
        }
      }

      // Merge LHS/RHS.
      lhs = std::make_unique<BinaryExprAST>(binOp, std::move(lhs),
                                            std::move(rhs));
    }
  };

  return parse_op_rhs(parse_op_rhs, std::move(lhs), 0);
}

/// unary
///   ::= op unary
///   ::= primary
std::unique_ptr<ExprAST> Parser::parse_unary() {
  switch (_curr.type) {
    case Token::Type::exclam:
    case Token::Type::minus: {
      Token tok{_curr};
      advance();
      std::unique_ptr<ExprAST> rhs = parse_unary();
      return rhs ? std::make_unique<UnaryExprAST>(tok, std::move(rhs))
                 : nullptr;
    }
    default:
      break;
  }

  return parse_primary();
}

std::unique_ptr<ExprAST> Parser::parse_primary() {
  Token tok = _curr;
  switch (tok.type) {
    case Token::Type::ident: {
      advance();
      return std::make_unique<VariableExprAST>(tok);
    }
    case Token::Type::number: {
      advance();
      return std::make_unique<NumberExprAST>(tok);
    }
    case Token::Type::lparen: {
      advance();
      std::unique_ptr<ExprAST> expr = parse_expression();
      if (_curr.type != Token::Type::rparen) {
        spdlog::error("unexpected token: {}. expected ')'",
                      Token::to_string(_curr.type));
        return nullptr;
      }
      advance();
      return expr;
    }
    default: {
      spdlog::error("unexpected token when expecting expression");
      return nullptr;
    }
  }
}

void Parser::advance() {
  _curr = _next;
  _next = _scanner->get_token();
}
