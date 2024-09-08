#pragma once

#include <fmt/format.h>
#include <llvm/IR/Type.h>

#include <algorithm>
#include <compare>
#include <map>
#include <variant>
#include <vector>

#include "../common.hpp"
#include "spdlog/spdlog.h"

namespace x::ast {

class Compiler;
class Context;

/// a::b::c::d
class Path {
 public:
  constexpr std::strong_ordering operator<=>(const Path &other) const {
    if (_inProjScope != other._inProjScope) {
      return _inProjScope ? std::strong_ordering::greater
                          : std::strong_ordering::less;
    }
    return _components <=> other._components;
  };

 private:
  friend Context;
  friend auto format_as(const Path &path);

  /// @param projScope: ::a::b false, a::b true
  /// @param module: {"a", "b"} in both cases
  Path(std::vector<std::string> &&components, bool isProj);

  bool _inProjScope;
  std::vector<std::string> _components;
};

auto format_as(const Path &path);

struct Type {
  std::string path;
};

class PrimaryExpr {
 public:
  enum class Type {
    Int,
    String,
    Bool,
  };

  static Ptr<PrimaryExpr> Bool(bool isTrue);

  static Ptr<PrimaryExpr> Int(std::string val) {
    return Ptr<PrimaryExpr>(new PrimaryExpr(std::move(val), Type::Int));
  };

  // private:
  // PrimaryExpr(std::string val, Type type)
  //     : _val{std::move(val)}, _type{type} {};

  std::string _val;
  Type _type;
};

class IfExpr {};

class ParenExpr;
class BinaryExpr;
class StructExpr;

class Expr {
 public:
  template <typename T>
  auto accept(T const &consumer) const {
    return std::visit(consumer, _val);
  };

  template <typename T>
  [[nodiscard]] auto is() const -> bool {
    return std::holds_alternative<T>(_val);
  }

  template <typename T>
  [[nodiscard]] auto to() const -> Ptr<T> & {
    return *std::get<Ptr<T>>(_val);
  }

  template <typename T>
  [[nodiscard]] auto into() -> Ptr<T> {
    return std::move(std::get<Ptr<T>>(_val));
  }

  std::variant<Ptr<PrimaryExpr>, Ptr<IfExpr>, Ptr<BinaryExpr>, Ptr<ParenExpr>,
               Ptr<StructExpr>>
      _val;
};

class BinaryExpr {
  enum class Operator {
    Plus,
    Minus,
    Star,
    Slash,
    Greater,
    Less,
  };

  Expr l;
  Expr r;
  Operator op;
};

class ParenExpr {
 public:
  Expr inner;
};

struct Field {
  std::string name;
  Expr value;
};

class StructExpr {
 public:
  static std::unique_ptr<StructExpr> Create(std::vector<Field> &&fields) {
    return std::make_unique<StructExpr>(std::move(fields));
  };

  std::vector<Field> fields;
};

struct FnParam {
  std::string name;
  Type *type;
};

struct FnProto {
  std::string name;

  // non null
  std::vector<FnParam> params;

  std::optional<Type *> ret;
};

class RetStmt {
 public:
  /// @param val: return val;
  ///   nullptr -> return;
  explicit RetStmt(std::optional<Expr> val) : _val{std::move(val)} {};

  std::optional<Expr> _val;
};

class VarDef {};

class TypeDef {};

class Block;
class Fn;
class Call;

class Stmt {
  template <typename T>
  auto accept(T const &consumer) {
    return std::visit(consumer, _val);
  };

 public:
  std::variant<Ptr<Expr>, Ptr<TypeDef>, Ptr<VarDef>, Ptr<Fn>, Ptr<Block>,
               Ptr<RetStmt>, Ptr<Call>>
      _val;
};

class Module;

class Call {
 public:
  Fn *fn;
  Ptr<StructExpr> args;
};

class Block {
 public:
  void ret(std::optional<Expr> retval = std::nullopt);
  void call(Path *path, Ptr<StructExpr> args);

  explicit Block(Module *mod);

  // private:
  std::vector<Stmt> _body;

  Module *_mod;
};

class Fn : public Block {
 private:
 public:
  friend class Module;
  explicit Fn(Module *mod, FnProto &&proto)
      : Block{mod}, _proto{std::move(proto)} {};

  [[nodiscard]] const std::string &name() const { return _proto.name; }

  FnProto _proto;
};

class Module {
 public:
  Fn *function(FnProto &&proto);
  void add(Ptr<Fn> func);
  void add(Ptr<VarDef> def);
  void add(Ptr<TypeDef> def);

  Type *type(Path *path);

  Path *path(std::vector<std::string> &&module, bool isProj);

  // private:
  friend Context;

  explicit Module(Context *ctx, Path *path);

  [[nodiscard]] auto findFn(Path *path) const -> Fn * {
    auto itr = _functions.find(path);
    return itr != _functions.end() ? itr->second.get() : nullptr;
  }

  Path *_path;

  std::map<Path *, Ptr<Fn>> _functions;

  Context *_ctx;
};

/// keeps data through entire compilation process
/// allowing you to get stuff from other modules
/// even if those modules weren't processed yet.
///
/// while processing one module, we get to
/// use a::b::Type
/// you query context->getType(). the context will
/// give you a type. when later processing module a::b,
/// you call context->add(a::b, Type). the context will
/// be able to link those two together
class Context {
 public:
  Context() {
    _types.insert({path({"I32"}, true), std::make_unique<Type>("I32")});
  }

  Module *module(std::vector<std::string> &&module, bool isProj);

 private:
  friend Module;

  Type *type(Path *path);

  Path *path(std::vector<std::string> &&components, bool isProj);

  std::map<Path, Ptr<Path>> _paths;
  std::map<Path *, Ptr<Fn>> _functions;
  std::map<Path *, Ptr<Module>> _modules;
  std::map<Path *, Ptr<Type>> _types;
};

};  // namespace x::ast
