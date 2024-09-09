#include <optional>
#include <string>
#include <vector>

#include "x/ast/block.hpp"
#include "x/ast/expr.hpp"

namespace x::ast {

class Type;

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

class Call {
 public:
  Fn *fn;
  Ptr<StructExpr> args;
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

}  // namespace x::ast
