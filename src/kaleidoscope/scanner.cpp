#include "scanner.hpp"

Scanner::Scanner(const std::string&& input_program)
    : _program{input_program}, _curr{get_char()} {};

Token Scanner::get_token() {
  skip_whitespace();
  if (end()) {
    return {Token::Type::eof, std::string{}, _loc};
  }

  if (bool(isalpha(_curr))) {
    SourceLocation startLoc = _loc;
    std::string ident{};
    while (bool(isalnum(_curr))) {
      ident += _curr;
      advance();
    }
    if (ident == "fn") {
      return {Token::Type::fn, ident, startLoc};
    }
    return {Token::Type::ident, ident, startLoc};
  }

  if (bool(isdigit(_curr))) {
    SourceLocation startLoc = _loc;
    std::string number{};
    while (bool(isdigit(_curr))) {
      number += _curr;
      advance();
    }
    return {Token::Type::number, number, startLoc};
  }
  auto optok = [this](Token::Type tok) {
    Token ret{tok, std::string{_curr}, _loc};
    advance();
    return ret;
  };

  switch (_curr) {
    case '+':
      return optok(Token::Type::plus);
    case '-':
      return optok(Token::Type::minus);
    case '*':
      return optok(Token::Type::star);
    case '/':
      if (peek_char() == '/') {
        // comment. skip to end of line
        while (!end() && _curr != '\n') {
          advance();
        }
        return get_token();
      }
      return optok(Token::Type::slash);
    case '!':
      return optok(Token::Type::exclam);
    case '(':
      return optok(Token::Type::lparen);
    case ')':
      return optok(Token::Type::rparen);
    default:
  }

  throw std::runtime_error("fuck");
};

void Scanner::skip_whitespace() {
  while (!end() && isspace(_curr) != 0) {
    advance();
  }
}

char Scanner::get_char() const { return end() ? char(EOF) : _program[_pos]; }

char Scanner::peek_char() const {
  return _pos + 1 >= _program.size() ? char(EOF) : _program[_pos + 1];
}

void Scanner::advance() {
  _pos++;
  _curr = get_char();
}

bool Scanner::end() const { return _pos >= _program.size(); }
