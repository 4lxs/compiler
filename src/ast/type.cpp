#include "x/ast/type.hpp"

#include "x/ast/context.hpp"

namespace x::ast {

Type* Type::Allocate(Context& ctx) { return ctx.allocate<Type>(); }

void Type::Create() { new (this) Type(); }

};  // namespace x::ast
