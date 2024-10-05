#pragma once

#include <cstdint>

#include "fwd_decl.hpp"
#include "spdlog/spdlog.h"
#include "x/common.hpp"
#include "x/pt/sema/fwd_decl.hpp"

namespace x::pt {

class Node {
 public:
  Node(Node const&) = delete;
  Node(Node&&) = delete;
  Node& operator=(Node const&) = delete;
  Node& operator=(Node&&) = delete;
  virtual ~Node() = default;

  enum class Kind {
    Primitive,

    Invalid,
    StmtBegin,  // <--- stmt
    ExprBegin,  // <--- expr
    Struct,
    Integer,
    Bool,
    String,
    If,
    Binary,
    Paren,
    Call,
    Block,
    DeclUse,
    FieldAccess,
    ExprEnd,  // <--- /expr

    Return,
    Assign,
    While,

    DeclBegin,  // <--- decl
    VarDecl,
    StmtEnd,  // <--- /stmt

    ParamDecl,

    TopLevelDeclBegin,  // <--- top level
    FnDecl,
    MethodDecl,
    StructDecl,
    EnumDecl,
    TypeDecl,
    TopLevelDeclEnd,  // <--- /top level
    DeclEnd,          // <--- /decl
  };

  virtual void dump(Context& /*ctx*/, uint8_t /*indent*/ = 0) {
    spdlog::info("unimplemented dump for {}", fmt::underlying(_kind));
  }

  [[nodiscard]] Kind kind() const { return _kind; }

  virtual void nameres(sema::NameResolver& /*res*/) {
    xerr("unimplemented nameres for {}", fmt::underlying(_kind));
  }

  [[nodiscard]] bool is_value_decl() const {
    return _kind == Kind::FnDecl || _kind == Kind::VarDecl;
  }

  [[nodiscard]] bool is_decl() const {
    return _kind > Kind::DeclBegin && _kind < Kind::DeclEnd;
  }

  [[nodiscard]] bool is_top_level_decl() const {
    return _kind > Kind::TopLevelDeclBegin && _kind < Kind::TopLevelDeclEnd;
  }

  [[nodiscard]] bool is_expr() const {
    return _kind > Kind::ExprBegin && _kind < Kind::ExprEnd;
  }

  [[nodiscard]] bool is_stmt() const {
    return _kind > Kind::StmtBegin && _kind < Kind::StmtEnd;
  }

  [[nodiscard]] NodeId id() const;

 private:
  Kind _kind;
  friend class Context;
  uint32_t _id{};

 protected:
  explicit Node(Kind kind) : _kind(kind) {};
};

class NodeId {
  friend Context;
  friend Node;
  friend class OptNodeId;
  NodeId(Node::Kind kind, uint32_t nid) : _kind(kind), _id(nid) {}

  Node::Kind _kind;
  uint32_t _id;

 public:
  std::strong_ordering operator<=>(NodeId const&) const = default;
};

class OptNodeId {
 public:
  OptNodeId() : _id{Node::Kind::Invalid, 0} {}

  OptNodeId& operator=(NodeId id) {
    _id = id;
    return *this;
  }

  [[nodiscard]] bool has_value() const {
    return _id._kind != Node::Kind::Invalid;
  }

  [[nodiscard]] NodeId value() const {
    Expects(has_value());
    return _id;
  }

 private:
  NodeId _id;
};

class Stmt {};

class Expr {};

}  // namespace x::pt
