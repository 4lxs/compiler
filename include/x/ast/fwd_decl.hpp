#pragma once

#include <cstddef>

#include "x/common.hpp"

namespace x::ast {

class Context;

class Type;
using TypeRef = not_null<Type *>;
class LiteralTy;
using LiteralTyRef = not_null<LiteralTy *>;
class StructTy;
using StructTyRef = not_null<StructTy *>;

class FnDecl;
using FnDeclRef = not_null<FnDecl *>;

class Stmt;
using StmtRef = not_null<Stmt *>;
class Return;
using ReturnRef = not_null<Return *>;
class VarDecl;
using VarDeclRef = not_null<VarDecl *>;
class Assign;
using AssignRef = not_null<Assign *>;
class Loop;
using WhileRef = not_null<Loop *>;
class ValueDecl;
using ValueDeclRef = not_null<ValueDecl *>;
class Decl;
using DeclRef = not_null<Decl *>;

class Expr;
using ExprRef = not_null<Expr *>;
class FieldAccess;
using FieldAccessRef = not_null<FieldAccess *>;
class IntegerLiteral;
using IntegerLiteralRef = not_null<IntegerLiteral *>;
class StringLiteral;
using StringLiteralRef = not_null<StringLiteral *>;
class BoolE;
using BoolERef = not_null<BoolE *>;
class StructLiteral;
using StructLiteralRef = not_null<StructLiteral *>;
class FnCall;
using FnCallRef = not_null<FnCall *>;
class Block;
using BlockRef = not_null<Block *>;
class If;
using IfRef = not_null<If *>;
class VarRef;
using VarRefRef = not_null<VarRef *>;

}  // namespace x::ast

void *operator new(size_t bytes, x::ast::Context const &ctx,
                   size_t alignment = 8);
