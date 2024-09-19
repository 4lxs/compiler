#pragma once

#include <cstddef>

namespace x::ast {

class Context;

class Type;

class Fn;

class Stmt;
class Return;
class VarDecl;
class Assign;

class Expr;
class IntegerLiteral;
class StringLiteral;
class BoolE;
class StructLiteral;
class FnCall;
class Block;
class If;

}  // namespace x::ast

void *operator new(size_t bytes, x::ast::Context const &ctx,
                   size_t alignment = 8);
