#pragma once

#include <variant>

#include "x/common.hpp"
namespace x::pt {

class Context;
class Module;

class Stub;
struct Path;
class Type;

class VarExpr;
class StructExpr;
class IntegerE;
class IfExpr;
class BinaryExpr;
class ParenExpr;
class StructExpr;
class Call;
class Block;
using Expr = std::variant<IntegerE*, IfExpr*, BinaryExpr*, ParenExpr*,
                          StructExpr*, Call*, Block*, VarExpr*>;

class Fn;
struct FnParam;

class RetStmt;
class VarDef;
using Stmt = std::variant<Expr, RetStmt*, VarDef*>;

}  // namespace x::pt
