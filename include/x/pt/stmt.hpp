#pragma once

#include <optional>
#include <string>
#include <vector>

#include "pt.hpp"
#include "x/ast/fwd_decl.hpp"
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

class VarDef {};

class TypeDef {};

class Stmt {
 public:
  explicit Stmt(auto stmt) : _stmt{std::move(stmt)} {}

 private:
  friend sema::Sema;
  auto accept(auto const &consumer) { return std::visit(consumer, _stmt); };

  // std::variant<, Ptr<TypeDef>, Ptr<VarDef>, Ptr<Block>,
  // Ptr<RetStmt>>
  std::variant<Expr, Ptr<RetStmt>> _stmt;
};

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
