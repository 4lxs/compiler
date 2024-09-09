#include "x/ast/stmt.hpp"

#include <optional>
#include <string>
#include <vector>

#include "x/ast/block.hpp"
#include "x/ast/module.hpp"

namespace x::ast {

Ptr<RetStmtV> RetStmt::validate() {
  return std::make_unique<RetStmtV>(std::move(_retVal));
}

//============================//

StmtV Stmt::validate() {
  return std::visit([](auto &&stmt) { return StmtV{stmt->validate()}; },
                    std::move(_stmt));
}

//============================//

Ptr<CallV> Call::validate() {
  return std::make_unique<CallV>(fn->validate<Fn>()->validate(),
                                 std::move(args));
}

//============================//

Fn::Fn(Module *mod, FnProto &&proto) : Block{mod}, _proto{std::move(proto)} {
  for (const auto &[_, arg] : proto.params) {
    arg->use_type();
  }
};

std::string_view Fn::name() const { return _proto.name; }

FnV *Fn::validate() {
  if (_val != nullptr) {
    return _val.get();
  }
  spdlog::info("validating function {}", name());

  std::vector<FnV::Param> params;
  params.reserve(_proto.params.size());

  for (auto &&[name, type] : _proto.params) {
    params.push_back(FnV::Param(std::move(name), type->validate<Type>()));
  }

  Block::validate();

  Type *ret = _proto.ret != nullptr ? _proto.ret->validate<Type>() : nullptr;

  _val = std::make_unique<FnV>(std::move(_proto.name), std::move(params),
                               std::move(Block::_val), ret);

  return _val.get();
}

}  // namespace x::ast
