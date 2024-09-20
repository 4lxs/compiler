#include "x/sema/sema.hpp"

#include <memory>
#include <utility>
#include <variant>

#include "x/ast/context.hpp"
#include "x/ast/expr.hpp"
#include "x/ast/fwd_decl.hpp"
#include "x/ast/stmt.hpp"
#include "x/ast/type.hpp"
#include "x/common.hpp"
#include "x/pt/context.hpp"
#include "x/pt/decl.hpp"
#include "x/pt/expr.hpp"
#include "x/pt/module.hpp"
#include "x/pt/stmt.hpp"

namespace x::sema {

Sema::Sema(pt::Context *ctx) : _pt(ctx) {
  for (auto const &module : ctx->_modules) {
    add(*module);
  }
}

Ptr<ast::Context> Sema::finish() { return std::move(_ast); }

void Sema::add(pt::Module const &module) {
  // first validate all stubs.  that is to allow stubs to cross-link. e.g. fn
  // val stub needs pointer to return type stub

  for (pt::TopLevelDecl const &decl : module._items) {
    std::visit(
        overloaded{
            [&, this](pt::FnDecl *func) {
              ast::FnDecl *newfn = ast::FnDecl::Allocate(*_ast);
              _ast->_functions.push_back(newfn);
              _maps.insert(func, newfn);
            },
            // [&, this](pt::Type *const &type) {
            //   // ast::Type *newtype = ast::Type::Allocate(*_ast);
            //   // _ast->_types.push_back(newtype);
            //   // _maps.insert(type, newtype);
            // },
        },
        decl);
  }

  for (auto const &stub : module._items) {
    std::visit(
        overloaded{
            [this](auto *item) { check(item); },
        },
        stub);
  }
}

ast::Block *Sema::check(pt::Block &block) {
  std::vector<ast::Stmt *> body;
  for (pt::Stmt &stmt : block._body) {
    std::visit(overloaded{
                   [this, &body](auto stmt) {
                     body.push_back(static_cast<ast::Stmt *>(check(stmt)));
                   },
                   [this, &body](pt::VarDecl *stmt) {
                     ast::NamedDecl typedecl = env.resolve(stmt->_type);
                     auto *type = std::get_if<ast::Type *>(&typedecl);
                     if (type == nullptr) {
                       spdlog::error("expected type");
                       std::terminate();
                     }
                     auto *decl =
                         ast::VarDecl::Create(*_ast, stmt->_name, *type);
                     body.push_back(decl);
                     env.add(decl);
                     if (!stmt->_val.has_value()) {
                       return;
                     }
                     ast::Expr *val = check(*stmt->_val);
                     auto *assign = ast::Assign::Create(*_ast, decl, val);
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

ast::Return *Sema::check(pt::Return *stmt) {
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
            ast::NamedDecl decl = env.resolve(expr->fn);
            if (auto *func = std::get_if<ast::FnDecl *>(&decl)) {
              return ast::FnCall::Create(*_ast, *func, check(*expr->args));
            }
            spdlog::error("cannot call non-function");
            std::terminate();
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
                return ast::Builtin::Create(*_ast, ast::Builtin::Op::iSub,
                                            std::vector{lhs, rhs}, lhs->type());
              case pt::BinaryExpr::Operator::Star:
                return ast::Builtin::Create(*_ast, ast::Builtin::Op::iMul,
                                            std::vector{lhs, rhs}, lhs->type());
              case pt::BinaryExpr::Operator::Slash:
                return ast::Builtin::Create(*_ast, ast::Builtin::Op::iDiv,
                                            std::vector{lhs, rhs}, lhs->type());
              case pt::BinaryExpr::Operator::Greater:
                return ast::Builtin::Create(*_ast, ast::Builtin::Op::iGreater,
                                            std::vector{lhs, rhs},
                                            _ast->_boolTy);
              case pt::BinaryExpr::Operator::Less:
                return ast::Builtin::Create(*_ast, ast::Builtin::Op::iLess,
                                            std::vector{lhs, rhs},
                                            _ast->_boolTy);
            }
          },
          [this](pt::DeclRef *expr) -> ast::Expr * {
            ast::NamedDecl decl = env.resolve(expr);

            auto **varDecl = std::get_if<ast::VarDecl *>(&decl);
            if (varDecl == nullptr) {
              spdlog::error("expected variable");
              std::terminate();
            }

            ast::Type *type = (*varDecl)->_type;

            return ast::DeclRef::Create(*_ast, *varDecl, type);
          },
          [](auto const & /*expr*/) -> ast::Expr * { assert(false); },
      }  // namespace x::sema
      ,
      expr);
}

ast::FnDecl *Sema::check(pt::FnDecl *func) {
  auto [astFn, wasInitialized] = _maps.get(func, true);
  if (wasInitialized) {
    return astFn;
  }

  std::vector<ast::FnDecl::Param> params;
  params.reserve(func->_params.size());

  for (auto const &[_, type] : func->_params) {
    ast::NamedDecl decl = env.resolve(type);
    if (auto *restype = std::get_if<ast::Type *>(&decl); restype != nullptr) {
      params.push_back(ast::FnDecl::Param(func->name(), *restype));
    } else {
      spdlog::error("expected type");
      std::terminate();
    }
  }
  ast::Block *body = check(*func->_body);
  ast::NamedDecl retDecl = env.resolve(func->_retTy);
  auto *type = std::get_if<ast::Type *>(&retDecl);
  if (type == nullptr) {
    spdlog::error("expected return type");
    std::terminate();
  }
  ast::Type *retType = *type;

  // assert(body->type() == retType);

  astFn->Create(func->name(), std::move(params), body, retType);

  env.add(astFn);

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
