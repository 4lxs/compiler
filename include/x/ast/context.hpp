#pragma once

#include <vector>

#include "ast.hpp"
#include "x/common.hpp"

namespace x::ast {

class Context {
 public:
  std::vector<Ptr<Module>> _modules;
};

}  // namespace x::ast
