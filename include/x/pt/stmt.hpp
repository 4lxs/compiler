#pragma once

#include <optional>

#include "fwd_decl.hpp"
#include "x/common.hpp"
#include "x/pt/context.hpp"

namespace x::pt {

class Return : public AllowAlloc<Context, Return> {
  friend AllowAlloc;

 public:
  std::optional<Expr> _retVal;

 private:
  /// @param val: return val;
  ///   nullptr -> return;
  explicit Return(std::optional<Expr> val) : _retVal(val) {}
};

}  // namespace x::pt
