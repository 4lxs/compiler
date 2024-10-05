#pragma once

#include "fwd_decl.hpp"
#include "x/pt/context.hpp"
#include "x/pt/node.hpp"

namespace x::pt {

class Return : public Node {
 public:
  std::optional<NodeId> _retVal;

  void dump(Context& ctx, uint8_t indent) override;

  void nameres(sema::NameResolver& res) override;

 private:
  friend Context;
  /// @param val: return val;
  ///   nullptr -> return;
  explicit Return(std::optional<NodeId> val)
      : Node(Node::Kind::Return), _retVal(val) {}

 public:
  static bool classof(Node const* node) {
    return node->kind() == Node::Kind::Return;
  }
};

class Assign : public Node {
 public:
  NodeId _assignee;
  NodeId _value;

  void dump(Context& ctx, uint8_t indent) override;

 private:
  friend Context;
  explicit Assign(NodeId assignee, NodeId value)
      : Node(Node::Kind::Assign), _assignee(assignee), _value(value) {}

 public:
  static bool classof(Node const* node) {
    return node->kind() == Node::Kind::Assign;
  }
};

class While : public Node {
 public:
  NodeId _cond;
  NodeId _body;

  void dump(Context& ctx, uint8_t indent) override;

 private:
  friend Context;
  explicit While(NodeId cond, NodeId body)
      : Node(Node::Kind::While), _cond(cond), _body(body) {}

 public:
  static bool classof(Node const* node) {
    return node->kind() == Node::Kind::While;
  }
};

}  // namespace x::pt
