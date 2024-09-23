#pragma once

#include <utility>
#include <vector>

#include "fwd_decl.hpp"
#include "x/common.hpp"
#include "x/sema/fwd_decl.hpp"

namespace x::pt {

struct FnParam {
  std::string name;
  DeclRef *type;
};

class FnDecl : public AllowAlloc<Context, FnDecl> {
 public:
  [[nodiscard]] std::string const &name() const { return _name; };

  std::vector<FnParam> _params;

  DeclRef *_retTy;

  not_null<Block *> _body;

  std::string _name;

 private:
  friend AllowAlloc;
  FnDecl(std::string name, std::vector<FnParam> &&params, DeclRef *retTy,
         not_null<Block *> body)
      : _params(std::move(params)),
        _retTy(retTy),
        _body(body),
        _name(std::move(name)) {}
};

class VarDecl : public AllowAlloc<Context, VarDecl> {
  friend AllowAlloc;

 public:
  std::string _name;
  not_null<DeclRef *> _type;
  std::optional<Expr> _val;

 private:
  VarDecl(std::string &&name, not_null<DeclRef *> type,
          std::optional<Expr> const &val)
      : _name(std::move(name)), _type(type), _val(val) {};
};

class StructDecl : public AllowAlloc<Context, StructDecl> {
 public:
  struct Field {
    std::string name;
    not_null<DeclRef *> type;
    std::optional<Expr> defaultVal;
  };

  std::string _name;
  std::vector<Field> _fields;

 private:
  friend AllowAlloc;
  explicit StructDecl(std::string name, std::vector<Field> &&fields)
      : _name(std::move(name)), _fields(std::move(fields)) {}
};

}  // namespace x::pt
