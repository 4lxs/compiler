#pragma once

#include <variant>

#include "x/common.hpp"

namespace x::ast {

class Context;
class Module;

class Type;

class Fn;

class Stmt;
class Ret;

class IntegerE;
class StringE;
class BoolE;
class StructE;
class CallE;
class BlockE;
class IfE;
using Expr = std::variant<Ptr<IntegerE>, Ptr<StringE>, Ptr<BoolE>, Ptr<StructE>,
                          Ptr<CallE>, Ptr<IfE>, Ptr<BlockE>>;

}  // namespace x::ast
