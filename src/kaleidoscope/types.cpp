#include "types.hpp"

std::string Token::to_string(Type type) {
  switch (type) {
    case Type::eof:
      return "eof";
    case Type::ident:
      return "ident";
    case Type::if_:
      return "if_";
    case Type::then:
      return "then";
    case Type::else_:
      return "else_";
    case Type::for_:
      return "for_";
    case Type::in:
      return "in";
    case Type::let:
      return "let";
    case Type::number:
      return "number";
    case Type::plus:
      return "plus";
    case Type::minus:
      return "minus";
    case Type::star:
      return "star";
    case Type::slash:
      return "slash";
    case Type::exclam:
      return "exclam";
    case Type::fn:
      return "fn";
    case Type::lparen:
      return "lparen";
    case Type::rparen:
      return "rparen";
    case Type::colon:
      return "colon";
  }
}

auto fmt::formatter<Token>::format(Token token, format_context& ctx) const
    -> format_context::iterator {
  return formatter<string_view>::format(
      fmt::format("Token {}({})@{}:{}", Token::to_string(token.type),
                  token.value, token.loc.col, token.loc.line),
      ctx);
}
