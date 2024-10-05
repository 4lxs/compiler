#pragma once

#include <gsl/assert>
#include <utility>
#include <vector>

#include "fwd_decl.hpp"
#include "x/pt/context.hpp"
#include "x/pt/node.hpp"
#include "x/pt/sema/nameresolution.hpp"
#include "x/sema/fwd_decl.hpp"

namespace x::pt {

class Decl : public Node {
 public:
  [[nodiscard]] std::string const &name(Context const &ctx) const;

 protected:
  explicit Decl(Node::Kind kind, std::string &&name)
      : Node(kind), _name(name) {}

 private:
  friend sema::NameResolver;
  /// the name of this decl. always use name() to access this field as it will
  /// get moved into _resolvedName
  std::string _name;

  /// gets set by the sema upon name resolution pass
  OptNodeId _resolvedName;

 public:
  static bool classof(Node const *node) { return node->is_decl(); }
};

class Primitive : public Decl {
 public:
  void nameres(sema::NameResolver &res) override;
  void dump(Context &ctx, uint8_t indent) override;

  enum class Type {
    Int32,
  } _type;

 private:
  friend Context;
  Primitive(std::string &&name, Type type)
      : Decl(Node::Kind::Primitive, std::move(name)), _type(type) {}

 public:
  static bool classof(Node const *node) {
    return node->kind() == Node::Kind::Primitive;
  }
};

class ParamDecl : public Decl {
  friend Context;
  ParamDecl(std::string name, NodeId type)
      : Decl(Node::Kind::ParamDecl, std::move(name)), type(type) {}

 public:
  std::string name;
  NodeId type;
};

class FnDecl : public Decl {
 public:
  void nameres(sema::NameResolver &res) override;

  void dump(Context &ctx, uint8_t indent) override;

  std::vector<NodeId> _params;

  NodeId _retTy;

  NodeId _body;

  std::string _mangledName;

 protected:
  friend Context;
  FnDecl(std::string name, std::vector<NodeId> &&params, NodeId retTy,
         NodeId body, bool isMethod = false)
      : Decl(isMethod ? Node::Kind::MethodDecl : Node::Kind::FnDecl,
             std::move(name)),
        _params(std::move(params)),
        _retTy(retTy),
        _body(body) {}
};

class MethodDecl : public FnDecl {
 public:
  /// the type of the receiver
  /// in Foo::bar(), Foo is the receiver
  NodeId _recv;

  /// if it's not static, it has a self
  bool _isStatic;

 private:
  friend Context;
  MethodDecl(NodeId recv, std::string name, std::vector<NodeId> &&params,
             NodeId retTy, NodeId body, bool isStatic)
      : FnDecl(std::move(name), std::move(params), retTy, body, true),
        _recv(recv),
        _isStatic(isStatic) {}
};

class VarDecl : public Decl {
 public:
  NodeId _type;
  std::optional<NodeId> _val;

 private:
  friend Context;
  VarDecl(std::string &&name, NodeId type, std::optional<NodeId> val)
      : Decl(Node::Kind::VarDecl, std::move(name)), _type(type), _val(val) {};

  void nameres(sema::NameResolver &res) override;
  void dump(Context &ctx, uint8_t indent) override;
};

class StructDecl : public Node {
 public:
  struct Field {
    std::string name;
    NodeId type;

    /// must be expr
    std::optional<NodeId> defaultVal;
  };

  std::string _name;
  std::vector<Field> _fields;

 private:
  friend Context;
  explicit StructDecl(std::string name, std::vector<Field> &&fields)
      : Node(Node::Kind::StructDecl),
        _name(std::move(name)),
        _fields(std::move(fields)) {}
};

class EnumDecl : public Node {
 public:
  struct Variant {
    std::string name;
  };
  std::string _name;
  std::vector<Variant> _variants;

 private:
  friend Context;
  explicit EnumDecl(std::string name, std::vector<Variant> &&variants)
      : Node(Node::Kind::EnumDecl),
        _name(std::move(name)),
        _variants(std::move(variants)) {}
};

class TypeDecl : public Node {
 public:
  std::string _name;
  NodeId _type;

 private:
  friend Context;
  TypeDecl(std::string name, NodeId type)
      : Node(Node::Kind::TypeDecl), _name(std::move(name)), _type(type) {}
};

}  // namespace x::pt
