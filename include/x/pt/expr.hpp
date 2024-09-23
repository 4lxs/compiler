#pragma once

#include <vector>

#include "x/common.hpp"
#include "x/pt/block.hpp"
#include "x/pt/fwd_decl.hpp"
#include "x/pt/path.hpp"

namespace x::pt {

class IntegerE : public AllowAlloc<Context, IntegerE> {
 public:
  std::string _val;

 private:
  friend AllowAlloc;
  explicit IntegerE(std::string val) : _val(std::move(val)) {}
};

class BoolE : public AllowAlloc<Context, BoolE> {
 public:
  bool _val;

 private:
  friend AllowAlloc;
  explicit BoolE(bool val) : _val(val) {}
};

class StringE : public AllowAlloc<Context, StringE> {
 public:
  std::string _val;

 private:
  friend AllowAlloc;
  explicit StringE(std::string val) : _val(std::move(val)) {}
};

class IfExpr : public AllowAlloc<Context, IfExpr> {
 public:
  pt::Expr cond;
  not_null<Block *> then;
  Block *else_;

 private:
  friend AllowAlloc;
  IfExpr(pt::Expr cond, not_null<Block *> then, Block *els)
      : cond(cond), then(then), else_(els) {}
};

class BinaryExpr : public AllowAlloc<Context, BinaryExpr> {
 public:
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

 private:
  friend AllowAlloc;
  BinaryExpr(Expr lhs, Expr rhs, Operator opr) : l(lhs), r(rhs), op(opr) {}
};

class ParenExpr : public AllowAlloc<Context, ParenExpr> {
 public:
  Expr inner;

 private:
  friend AllowAlloc;
  explicit ParenExpr(Expr inner) : inner(inner) {}
};

class DeclRef : public AllowAlloc<Context, DeclRef> {
 public:
  Path _var;

 private:
  friend AllowAlloc;
  explicit DeclRef(Path &&path) : _var(std::move(path)) {}
};

class Call : public AllowAlloc<Context, Call> {
 public:
  DeclRef *fn;
  not_null<StructExpr *> args;

 private:
  friend AllowAlloc;
  Call(DeclRef *func, not_null<StructExpr *> args) : fn(func), args(args) {}
};

struct Field {
  std::string name;
  Expr value;
};

class StructExpr : public AllowAlloc<Context, StructExpr> {
 public:
  std::vector<Field> fields;

 private:
  friend AllowAlloc;
  explicit StructExpr(std::vector<Field> &&fields)
      : fields(std::move(fields)) {}
};

class FieldAccess : public Expr, public AllowAlloc<Context, FieldAccess> {
 public:
  Expr base;
  std::string field;

 private:
  friend AllowAlloc;
  FieldAccess(Expr base, std::string field)
      : base(base), field(std::move(field)) {}
};

}  // namespace x::pt
