#pragma once

#include <fmt/format.h>

#include <cstddef>
#include <string>
#include <string_view>

struct SourceLocation {
  size_t line;
  size_t col;
};

struct Token {
  enum class Type {
    eof,
    ident,

    // keywords
    if_,
    then,
    else_,
    for_,
    in,
    let,
    fn,

    // brackets
    lparen,
    rparen,

    // literals
    number,

    // operators
    plus,
    minus,
    star,
    slash,
    exclam,

    // other
    colon,
  };

  Token(Type type, std::string value, SourceLocation loc)
      : type(type), value(std::move(value)), loc(loc) {}

  static std::string to_string(Type type);

  Type type;
  std::string value;
  SourceLocation loc;
};

template <>
struct fmt::formatter<Token> : fmt::formatter<std::string> {
  auto format(Token tok, format_context& ctx) const -> format_context::iterator;
};
