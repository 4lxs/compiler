#pragma once

#include <string>

#include "types.hpp"

class Scanner {
 public:
  explicit Scanner(const std::string&& input_program);

  Token get_token();

 private:
  void skip_whitespace();

  [[nodiscard]] char get_char() const;

  [[nodiscard]] char peek_char() const;

  void advance();

  [[nodiscard]] bool end() const;

  std::string _program;
  size_t _pos{0};
  SourceLocation _loc{};
  char _curr;
};
