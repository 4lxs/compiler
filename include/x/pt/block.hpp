#pragma once

#include "context.hpp"
#include "fwd_decl.hpp"
#include "node.hpp"
#include "x/sema/fwd_decl.hpp"

namespace x::pt {

class Block : public Node {
 public:
  void add(NodeId stmt) { _body.emplace_back(stmt); }

  void dump(Context& ctx, uint8_t indent) override;

  void nameres(sema::NameResolver& res) override;

  /// the expression returned from the block
  void setTerminator(NodeId expr) { _end = expr; }

  std::vector<NodeId> _body;
  std::optional<NodeId> _end;

 private:
  friend Context;
  Block() : Node(Node::Kind::Block) {}

 public:
  ~Block() override = default;

  static bool classof(Node const* node) {
    return node->kind() == Node::Kind::Block;
  }
};

}  // namespace x::pt
