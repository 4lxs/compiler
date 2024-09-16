#include "x/sema/sema.hpp"

#include <memory>
#include <variant>

#include "x/ast/context.hpp"
#include "x/ast/expr.hpp"
#include "x/ast/module.hpp"
#include "x/ast/stmt.hpp"
#include "x/ast/toplevel.hpp"
#include "x/ast/type.hpp"
#include "x/common.hpp"
#include "x/pt/context.hpp"
#include "x/pt/module.hpp"
#include "x/pt/type.hpp"

namespace x::sema {

Sema::Sema(pt::Context const &ctx) {
  for (auto const &[_, module] : ctx._modules) {
    add(*module);
  }
}

Ptr<ast::Context> Sema::finish() {
  auto ast = std::make_unique<ast::Context>();

  ast->_modules = std::move(_modules);

  return std::move(ast);
}

void Sema::add(pt::Module const &module) {
  auto mod = std::make_unique<ast::Module>();

  // first validate all stubs.  that is to allow stubs to cross-link. e.g. fn
  // val stub needs pointer to return type stub

  for (auto const &[_, stub] : module._items) {
    spdlog::info("validating: {}", stub->_name);
    if (stub->_holder.index() == 0) {
      spdlog::error("validating undefined stub");
      std::terminate();
    }

    std::visit(
        overloaded{
            [](std::monostate) {
              spdlog::error("undefined item");
              std::terminate();
            },
            [&, this](Ptr<pt::Fn> const &func) {
              std::unique_ptr<ast::Fn> funcptr(new ast::Fn{.pt = func.get()});
              auto *newtype = funcptr.get();
              mod->_functions.push_back(std::move(funcptr));
              _fnMap.insert({func.get(), newtype});
            },
            [&, this](Ptr<pt::Type> const &type) {
              std::unique_ptr<ast::Type> typeptr(
                  new ast::Type{.pt = type.get()});
              auto *newtype = typeptr.get();
              mod->_types.push_back(std::move(typeptr));
              _typeMap.insert({type.get(), newtype});
            },
        },
        stub->_holder);
  }

  for (auto const &[_, stub] : module._items) {
    spdlog::info("adding: {}", stub->_name);
    std::visit(overloaded{[this](auto const &item) { check(item.get()); },
                          [](std::monostate) { std::terminate(); }

               },
               stub->_holder);
  }

  _modules.push_back(std::move(mod));
}

Ptr<ast::BlockE> Sema::check(pt::Block &block) {
  auto ret = std::make_unique<ast::BlockE>();

  for (pt::Stmt &stmt : block._body) {
    ast::Stmt newStmt = stmt.accept(overloaded{
        [this](auto &stmt) { return ast::Stmt(check(stmt)); },
    });

    ret->body.push_back(std::move(newStmt));
  }

  if (block._end.has_value()) {
    ret->terminator = std::move(check(block._end.value()));
  }

  return std::move(ret);
}

Ptr<ast::StructE> Sema::check(pt::StructExpr &expr) {
  std::vector<ast::Expr> fields;

  for (auto &[_, value] : expr.fields) {
    fields.push_back(check(value));
  }

  return std::make_unique<ast::StructE>(std::move(fields));
}

ast::Type *Sema::check(pt::Type *type) {
  if (type == nullptr) {
    return _voidType.get();
  }

  return _typeMap.at(type);
}

Ptr<ast::Ret> Sema::check(Ptr<pt::RetStmt> &stmt) {
  std::optional<ast::Expr> retVal;
  if (stmt->_retVal.has_value()) {
    retVal = check(stmt->_retVal.value());
  }

  return std::make_unique<ast::Ret>(std::move(retVal));
}

ast::Expr Sema::check(pt::Expr &expr, pt::Type *type) {
  return std::visit(
      overloaded{
          [](Ptr<pt::IntegerE> const &expr) {
            return ast::Expr(ast::IntegerE::Int32(expr->_val));
          },
          [this](Ptr<pt::Call> const &expr) {
            return ast::Expr(std::make_unique<ast::CallE>(check(expr->fn),
                                                          check(*expr->args)));
          },
          [this](Ptr<pt::IfExpr> const &expr) {
            return ast::Expr(std::make_unique<ast::IfE>(
                check(expr->cond), check(*expr->then),
                expr->else_ != nullptr ? check(*expr->else_) : nullptr));
          },
          [](auto const & /*expr*/) -> ast::Expr { assert(false); },
      },
      expr);
}

ast::Fn *Sema::check(pt::Fn *func) {
  ast::Fn *val = _fnMap.at(func);
  val->params.reserve(func->_proto.params.size());

  for (auto const &[_, type] : func->_proto.params) {
    val->params.push_back(ast::Fn::Param(check(type)));
  }
  val->ret = check(func->_proto.ret);

  val->block = check(*func->_body);

  return val;
}

// ast::BlockE *Block::validate() {
//   std::vector<ast::Stmt> body;
//   body.reserve(_body.size());
//
//   for (auto &&stmt : _body) {
//     body.push_back(stmt.validate());
//   }
//
//   _val = std::make_unique<ast::BlockE>(std::move(body));
//
//   return _val.get();
// }

}  // namespace x::sema
