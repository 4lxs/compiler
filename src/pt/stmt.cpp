#include "x/pt/stmt.hpp"

#include <optional>
#include <string>

#include "x/pt/block.hpp"
#include "x/pt/module.hpp"

namespace x::pt {

RetStmt::RetStmt(std::optional<Expr> val) : _retVal{std::move(val)} {};

Fn::Fn(FnProto&& proto, Ptr<Block> body, Stub* stub)
    : _body{std::move(body)}, _proto{std::move(proto)}, _stub{stub} {};

std::string const& Fn::name() const { return _stub->name(); }

}  // namespace x::pt
