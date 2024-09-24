#pragma once

#include <variant>

namespace x::pt {

class Context;
class Module;

struct Path;

class StructExpr;
class IntegerE;
class IfExpr;
class BinaryExpr;
class ParenExpr;
class StructExpr;
class Call;
class Block;
class DeclRef;
class FieldAccess;
using Expr = std::variant<IntegerE*, IfExpr*, BinaryExpr*, ParenExpr*,
                          StructExpr*, Call*, Block*, DeclRef*, FieldAccess*>;

struct FnParam;
class FnDecl;
class StructDecl;
class VarDecl;
class EnumDecl;
class TypeDecl;
using ValueDecl = std::variant<FnDecl*, VarDecl*>;
using TopLevelDecl = std::variant<FnDecl*, StructDecl*, EnumDecl*, TypeDecl*>;

class Return;
class Assign;
class While;
using Stmt = std::variant<Expr, Return*, VarDecl*, Assign*, While*>;

}  // namespace x::pt
