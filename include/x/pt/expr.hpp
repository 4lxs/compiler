#pragma once

#include <vector>

#include "x/pt/fwd_decl.hpp"
#include "x/pt/node.hpp"
#include "x/pt/path.hpp"
#include "x/pt/sema/nameresolution.hpp"

namespace x::pt {

class Integer : public Node {
 public:
  std::string _val;

  void dump(Context &ctx, uint8_t indent = 0) override;

  void nameres(sema::NameResolver &res) override;

 private:
  friend Context;
  explicit Integer(std::string val)
      : Node(Node::Kind::Integer), _val(std::move(val)) {}

 public:
  static bool classof(Node const *node) {
    return node->kind() == Node::Kind::Integer;
  }
};

class BoolE : public Node {
 public:
  bool _val;

 private:
  friend Context;
  explicit BoolE(bool val) : Node(Node::Kind::Bool), _val(val) {}
};

class StringE : public Node {
 public:
  std::string _val;

 private:
  friend Context;
  explicit StringE(std::string val)
      : Node(Node::Kind::String), _val(std::move(val)) {}
};

class IfExpr : public Node {
 public:
  NodeId cond;
  NodeId then;
  std::optional<NodeId> else_;

  void nameres(sema::NameResolver &res) override;

  void dump(Context &ctx, uint8_t indent) override;

 private:
  friend Context;
  IfExpr(NodeId cond, NodeId then, std::optional<NodeId> els)
      : Node(Node::Kind::If), cond(cond), then(then), else_(els) {}

 public:
  static bool classof(Node const *node) {
    return node->kind() == Node::Kind::If;
  }
};

class BinaryNode : public Node {
 public:
  enum class Operator {
    Plus,
    Minus,
    Star,
    Slash,
    Greater,
    Less,
  };

  NodeId l;
  NodeId r;
  Operator op;

  void nameres(sema::NameResolver &res) override;

  void dump(Context &ctx, uint8_t indent) override;

 private:
  friend Context;
  BinaryNode(NodeId lhs, NodeId rhs, Operator opr)
      : Node(Node::Kind::Binary), l(lhs), r(rhs), op(opr) {}

 public:
  static bool classof(Node const *node) {
    return node->kind() == Node::Kind::Binary;
  }
};

class ParenNode : public Node {
 public:
  Node *inner;

 private:
  friend Context;
  explicit ParenNode(Node *inner) : Node(Node::Kind::Paren), inner(inner) {}
};

class DeclUse : public Node {
 public:
  Path _var;

  void nameres(sema::NameResolver &res) override;

  void dump(Context &ctx, uint8_t indent) override;

  NodeId definition() {
    if (!_def.has_value()) {
      xerr("undefined decl: {}", format_as(_var));
    }
    return _def.value();
  }

 private:
  friend Context;
  explicit DeclUse(Path &&path)
      : Node(Node::Kind::DeclUse), _var(std::move(path)) {}

  OptNodeId _def;
  // sema::OptNameRef _def;

 public:
  static bool classof(Node const *node) {
    return node->kind() == Node::Kind::DeclUse;
  }
};

class Call : public Node {
 public:
  NodeId fn;
  NodeId args;

 private:
  friend Context;
  Call(NodeId func, NodeId args)
      : Node(Node::Kind::Call), fn(func), args(args) {}
};

struct Field {
  std::string name;
  NodeId value;
};

class StructExpr : public Node {
 public:
  std::vector<Field> fields;

 private:
  friend Context;
  explicit StructExpr(std::vector<Field> &&fields)
      : Node(Node::Kind::Struct), fields(std::move(fields)) {}

 public:
  static bool classof(Node const *node) {
    return node->kind() == Node::Kind::Struct;
  }
};

class FieldAccess : public Node {
 public:
  NodeId base;
  std::string field;

 private:
  friend Context;
  FieldAccess(NodeId base, std::string field)
      : Node(Node::Kind::FieldAccess), base(base), field(std::move(field)) {}
};

}  // namespace x::pt
