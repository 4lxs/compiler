#pragma once

#include <optional>
#include <string>
#include <vector>

#include "pt.hpp"
#include "x/pt/block.hpp"
#include "x/pt/expr.hpp"

namespace x::pt {

struct FnParam {
  std::string name;
  Type *type;
};

struct FnProto {
  std::vector<FnParam> params;

  Type *ret;
};

class RetStmt {
 public:
  /// @param val: return val;
  ///   nullptr -> return;
  explicit RetStmt(std::optional<Expr> val);

  std::optional<Expr> _retVal;
};

class VarDef {
  // Expr _val;
  // Stub *_stub;
};

class TypeDef {};

class Fn {
 public:
  friend Stub;
  friend sema::Sema;
  explicit Fn(FnProto &&proto, Ptr<Block> body, Stub *stub);

  [[nodiscard]] std::string const &name() const;

  Ptr<Block> _body;
  FnProto _proto;

  Stub *_stub;
};

}  // namespace x::pt
