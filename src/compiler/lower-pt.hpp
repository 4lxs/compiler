#pragma once

#include "x/ast/context.hpp"
#include "x/pt/context.hpp"
namespace x {

std::unique_ptr<ast::Context> lower_pt(std::unique_ptr<pt::Context> pt);

}  // namespace x
