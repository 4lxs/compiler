#pragma once

#include <optional>
#include <string>
#include <vector>

#include "x/ast/block.hpp"
#include "x/ast/expr.hpp"
#include "x/ast/type.hpp"

namespace x::ast {

class Stub;

struct FnParam {
  std::string name;
  Stub *type;
};

struct FnProto {
  std::string name;

  // non null
  std::vector<FnParam> params;

  Stub *ret;
};

struct RetStmtV {
  std::optional<Expr> _val;
};

class RetStmt {
 public:
  /// @param val: return val;
  ///   nullptr -> return;
  explicit RetStmt(std::optional<Expr> val) : _retVal{std::move(val)} {};

  /// see `Context::validate()`
  Ptr<RetStmtV> validate();

  std::optional<Expr> _retVal;
};

class VarDef {};

class TypeDef {};

class Block;
class Fn;
class Call;

struct FnV;
struct RetStmtV;
struct CallV;

class Stmt {
 public:
  explicit Stmt(auto stmt) : _stmt{std::move(stmt)} {}

 private:
  auto accept(const auto &consumer) { return std::visit(consumer, _stmt); };

  friend class Block;
  /// see `Context::validate()`
  StmtV validate();

  // std::variant<Ptr<Expr>, Ptr<TypeDef>, Ptr<VarDef>, Ptr<Block>,
  // Ptr<RetStmt>, Ptr<Call>>
  std::variant<Ptr<RetStmt>, Ptr<Call>> _stmt;
};

struct StmtV {
  std::variant<Ptr<RetStmtV>, Ptr<CallV>> stmt;
};

class Call {
 public:
  Stub *fn;
  Ptr<StructExpr> args;

  /// see `Context::validate()`
  Ptr<CallV> validate();
};

struct CallV {
  FnV *fn;
  Ptr<StructExpr> args;
};

class Fn : public Block {
 public:
  explicit Fn(Module *mod, FnProto &&proto);

  [[nodiscard]] std::string_view name() const;

 private:
  friend class Stub;
  friend class Call;
  /// see `Context::validate()`
  FnV *validate();

  friend class Module;
  /// see `Module::validate()`
  Ptr<FnV> _val;

  FnProto _proto;
};

struct FnV {
  std::string name;

  struct Param {
    std::string name;
    Type *type;
  };

  std::vector<Param> params;

  Ptr<BlockV> block;

  Type *ret;
};

}  // namespace x::ast
