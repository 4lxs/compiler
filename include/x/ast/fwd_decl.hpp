#pragma once

#include <cstddef>
#include <variant>

namespace x::ast {

class Context;

class Type;

class FnDecl;

class Stmt;
class Return;
class VarDecl;
class Assign;
using NamedDecl = std::variant<VarDecl *, FnDecl *, Type *>;

class Expr;
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
