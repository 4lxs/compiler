#pragma once

#include <optional>
#include <string>
#include <vector>

#include "fwd_decl.hpp"
#include "x/common.hpp"
#include "x/pt/block.hpp"
#include "x/pt/context.hpp"

namespace x::pt {

struct FnParam {
  std::string name;
  Type *type;
};

struct FnProto {
  std::vector<FnParam> params;

  Type *ret;
};

class RetStmt : public AllowAlloc<Context, RetStmt> {
  friend AllowAlloc;

 public:
  std::optional<Expr> _retVal;

 private:
  /// @param val: return val;
  ///   nullptr -> return;
  explicit RetStmt(std::optional<Expr> val) : _retVal(val) {}
};

class VarDef : public AllowAlloc<Context, VarDef> {
  friend AllowAlloc;

 public:
  Expr _val;
  Stub *_stub;

 private:
  VarDef(not_null<Stub *> stub, Expr &&val) : _val(val), _stub(stub) {};
};

class TypeDef {};

class Fn : public AllowAlloc<Context, Fn> {
 public:
  friend sema::Sema;

  [[nodiscard]] std::string const &name() const;

  not_null<Block *> _body;
  FnProto _proto;

  not_null<Stub *> _stub;

 private:
  friend AllowAlloc;
  Fn(FnProto &&proto, not_null<Block *> body, not_null<Stub *> stub)
      : _body(body), _proto(std::move(proto)), _stub(stub) {}
};

}  // namespace x::pt
