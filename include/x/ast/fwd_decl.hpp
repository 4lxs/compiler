#pragma once

#include <cstddef>

namespace x::ast {

class Context;

class Type;
class LiteralTy;
class StructTy;

class FnDecl;

class Stmt;
class Return;
class VarDecl;
class Assign;
class While;
class Decl;

class Expr;
class FieldAccess;
class IntegerLiteral;
class StringLiteral;
class BoolE;
class StructLiteral;
class FnCall;
class Block;
class If;
class DeclRef;

}  // namespace x::ast

void *operator new(size_t bytes, x::ast::Context const &ctx,
                   size_t alignment = 8);
