#pragma once

#include <variant>

namespace x::pt {

class Context;
class Module;

struct Path;

class VarExpr;
class StructExpr;
class IntegerE;
class IfExpr;
class BinaryExpr;
class ParenExpr;
class StructExpr;
class Call;
class Block;
class DeclRef;
using Expr = std::variant<IntegerE*, IfExpr*, BinaryExpr*, ParenExpr*,
                          StructExpr*, Call*, Block*, VarExpr*, DeclRef*>;

struct FnParam;
class FnDecl;
class VarDecl;
class TypeDecl;
using ValueDecl = std::variant<FnDecl*, VarDecl*>;
using TopLevelDecl = std::variant<FnDecl*>;

class Return;
using Stmt = std::variant<Expr, Return*, VarDecl*>;

}  // namespace x::pt
