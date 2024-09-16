#pragma once

#include <vector>

#include "ast.hpp"
#include "x/common.hpp"
#include "x/pt/pt.hpp"

namespace x::ast {

class Fn {
 public:
  struct Param {
    Type *type;
  };

  std::vector<Param> params;

  Ptr<BlockE> block;

  Type *ret;

  pt::Fn *pt;
};

}  // namespace x::ast
