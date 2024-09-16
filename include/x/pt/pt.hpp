#pragma once

#include <variant>

#include "x/common.hpp"
namespace x::pt {

class Context;
class Module;

class Stub;
struct Path;
class Type;

class Fn;
struct FnParam;

class Stmt;
class Block;
class RetStmt;

class StructExpr;
struct IntegerE;
struct IfExpr;
class BinaryExpr;
class ParenExpr;
class StructExpr;
class Call;
using Expr =
    std::variant<Ptr<IntegerE>, Ptr<IfExpr>, Ptr<BinaryExpr>, Ptr<ParenExpr>,
                 Ptr<StructExpr>, Ptr<Call>, Ptr<Block>>;

}  // namespace x::pt
