#pragma once

#include <vector>

#include "ast.hpp"
#include "x/common.hpp"

namespace x::ast {

class Module {
 public:
  std::vector<Ptr<Fn>> _functions;
  std::vector<Ptr<Type>> _types;
};

}  // namespace x::ast
