#include "x/sema/sema.hpp"

#include <memory>
#include <utility>
#include <variant>

#include "x/ast/context.hpp"
#include "x/ast/expr.hpp"
#include "x/ast/stmt.hpp"
#include "x/ast/type.hpp"
#include "x/common.hpp"
#include "x/pt/context.hpp"
#include "x/pt/expr.hpp"
#include "x/pt/module.hpp"
#include "x/pt/type.hpp"

namespace x::sema {

Sema::Sema(pt::Context *ctx) : _pt(ctx) {
  for (auto const &[_, module] : ctx->_modules) {
    add(*module);
  }
}

Ptr<ast::Context> Sema::finish() { return std::move(_ast); }

void Sema::add(pt::Module const &module) {
  // first validate all stubs.  that is to allow stubs to cross-link. e.g. fn
  // val stub needs pointer to return type stub

  for (auto const &[_, stub] : module._items) {
    spdlog::info("validating: {}", stub->_name);
    if (stub->_holder.index() == 0) {
      spdlog::error("validating undefined stub");
      std::terminate();
    }

    std::visit(overloaded{
                   [](std::monostate) {
                     spdlog::error("undefined item");
                     std::terminate();
                   },
                   [&, this](pt::Fn *func) {
                     ast::Fn *newfn = ast::Fn::Allocate(*_ast);
                     _ast->_functions.push_back(newfn);
                     _maps.insert(func, newfn);
                   },
                   [&, this](pt::Type *const &type) {
                     // ast::Type *newtype = ast::Type::Allocate(*_ast);
                     // _ast->_types.push_back(newtype);
                     // _maps.insert(type, newtype);
                   },
               },
               stub->_holder);
  }

  for (auto const &[_, stub] : module._items) {
    spdlog::info("adding: {}", stub->_name);
    std::visit(overloaded{[this](auto *item) { check(item); },
                          [](std::monostate) { std::terminate(); }

               },
               stub->_holder);
  }
}

ast::Block *Sema::check(pt::Block &block) {
  std::vector<ast::Stmt *> body;
  for (pt::Stmt &stmt : block._body) {
    std::visit(overloaded{
                   [this, &body](auto stmt) {
                     body.push_back(static_cast<ast::Stmt *>(check(stmt)));
                   },
                   [this, &body](pt::VarDef *stmt) {
                     ast::Expr *val = check(stmt->_val);
                     auto *decl = ast::VarDecl::Create(
                         *_ast, stmt->_stub->name(), val->type());
                     auto *assign = ast::Assign::Create(*_ast, decl, val);
                     body.push_back(decl);
                     body.push_back(assign);
                   },
                   [this, &body](pt::Expr &stmt) {
                     body.push_back(static_cast<ast::Stmt *>(check(stmt)));
                   },
               },
               stmt);
  }

  ast::Expr *terminator =
      block._end.has_value() ? check(block._end.value()) : _ast->_voidExpr;

  return ast::Block::Create(*_ast, std::move(body), terminator);
}

ast::StructLiteral *Sema::check(pt::StructExpr &expr) {
  std::vector<ast::Expr *> fields;
  fields.reserve(expr.fields.size());

  for (auto &[_, value] : expr.fields) {
    fields.push_back(check(value));
  }

  return ast::StructLiteral::Create(*_ast, std::move(fields));
}

ast::Type *Sema::check(pt::Type *type) {
  if (type == nullptr) {
    return _ast->_voidTy;
  }

  auto [astType, wasInitialized] = _maps.get(type, true);
  spdlog::info("initialized {}? {}", fmt::ptr(astType), wasInitialized);
  if (wasInitialized) {
    return astType;
  }

  assert(false);
  astType->Create(ast::Type::Kind::I32);

  return astType;
}

ast::Return *Sema::check(pt::RetStmt *stmt) {
  ast::Expr *retVal = stmt->_retVal.has_value() ? check(stmt->_retVal.value())
                                                : _ast->_voidExpr;

  return ast::Return::Create(*_ast, retVal);
}

not_null<ast::Expr *> Sema::check(pt::Expr &expr) {
  return std::visit(
      overloaded{
          [this](pt::IntegerE *const &expr) -> ast::Expr * {
            return ast::IntegerLiteral::Int32(*_ast, expr->_val);
          },
          [this](pt::Call *const &expr) -> ast::Expr * {
            return ast::FnCall::Create(*_ast, check(expr->fn),
                                       check(*expr->args));
          },
          [this](pt::IfExpr *const &expr) -> ast::Expr * {
            not_null<ast::Expr *> cond = check(expr->cond);
            not_null<ast::Block *> then = check(*expr->then);
            ast::Block *els =
                expr->else_ != nullptr ? check(*expr->else_) : nullptr;

            if (cond->type() != _ast->_boolTy) {
              spdlog::error("if condition must be of type bool");
              std::terminate();
            }
            if (then->type() != els->type()) {
              spdlog::error("if branches must have same type");
              std::terminate();
            }
            return ast::If::Create(*_ast, cond, then, els);
          },
          [this](pt::BinaryExpr *const &expr) -> ast::Expr * {
            ast::Expr *lhs = check(expr->l);
            ast::Expr *rhs = check(expr->r);
            assert(lhs->type() == rhs->type());

            switch (expr->op) {
              case pt::BinaryExpr::Operator::Plus:
                return ast::Builtin::Create(*_ast, ast::Builtin::Op::iAdd,
                                            std::vector{lhs, rhs}, lhs->type());
              case pt::BinaryExpr::Operator::Minus:
              case pt::BinaryExpr::Operator::Star:
              case pt::BinaryExpr::Operator::Slash:
              case pt::BinaryExpr::Operator::Greater:
                assert(false);
              case pt::BinaryExpr::Operator::Less:
                return ast::Builtin::Create(*_ast, ast::Builtin::Op::iLess,
                                            std::vector{lhs, rhs},
                                            _ast->_boolTy);
            }
          },
          [](auto const & /*expr*/) -> ast::Expr * { assert(false); },
      },
      expr);
}

ast::Fn *Sema::check(pt::Fn *func) {
  auto [astFn, wasInitialized] = _maps.get(func, true);
  if (wasInitialized) {
    return astFn;
  }

  std::vector<ast::Fn::Param> params;
  params.reserve(func->_proto.params.size());

  for (auto const &[_, type] : func->_proto.params) {
    params.push_back(ast::Fn::Param(func->name(), check(type)));
  }
  ast::Block *body = check(*func->_body);
  ast::Type *retType = check(func->_proto.ret);

  assert(body->type() != retType);

  astFn->Create(func->name(), std::move(params), body, retType);

  return astFn;
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
