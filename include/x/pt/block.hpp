#pragma once

#include <optional>
#include <vector>

#include "fwd_decl.hpp"
#include "x/sema/fwd_decl.hpp"

namespace x::pt {

class Block : public AllowAlloc<Context, Block> {
 public:
  void add(auto stmt) { _body.emplace_back(stmt); }
  void add(Expr &&expr);

  /// the expression returned from the block
  void setTerminator(Expr expr);

 protected:
  friend sema::Sema;
  std::vector<Stmt> _body;
  std::optional<Expr> _end;

 private:
  friend AllowAlloc;
  Block() = default;
};

}  // namespace x::pt
