#pragma once

#include <optional>
#include <vector>

#include "pt.hpp"
#include "x/sema/fwd_decl.hpp"

namespace x::pt {

class Block {
 public:
  void ret(std::optional<Expr> retval);

  void expr(Expr expr);

  /// the expression returned from the block
  void setTerminator(Expr expr);

 protected:
  friend sema::Sema;
  std::vector<Stmt> _body;
  std::optional<Expr> _end;
};

}  // namespace x::pt
