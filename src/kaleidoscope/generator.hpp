#pragma once

#include <memory>

#include "ast/function.hpp"

class Generator {
  void add(std::unique_ptr<FunctionAST> code);
};
