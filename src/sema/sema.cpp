#include "x/sema/sema.hpp"

#include <fmt/format.h>
#include <llvm/Support/Casting.h>

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
  env.add_type(_ast->_int32Ty);

  for (auto const &module : ctx->_modules) {
    add(*module);
  }
}

Ptr<ast::Context> Sema::finish() { return std::move(_ast); }

void Sema::add(pt::Module const &module) {
  std::vector<std::pair<pt::FnDecl *, ast::FnDecl *>> functions;
  std::vector<std::pair<pt::StructDecl *, ast::StructTy *>> structs;
  // std::vector<std::pair<pt::EnumDecl *, ast::EnumTy *>> enums;
  std::vector<std::pair<pt::TypeDecl *, TypeRef>> types;

  // declare top level items. we can't define them yet as they may refer to
  // each other
  for (pt::TopLevelDecl const &decl : module._items) {
    std::visit(overloaded{
                   [&, this](pt::FnDecl *func) {
                     // we hold off on declaring functions as they may resolve
                     // types
                     ast::FnDecl *newfn = ast::FnDecl::Allocate(*_ast);
                     functions.emplace_back(func, newfn);
                   },
                   [&, this](pt::StructDecl *decl) {
                     auto *ast = ast::StructTy::Allocate(*_ast);
                     structs.emplace_back(decl, ast);
                     declare(ast, decl);
                     env.add_type(ast);
                     _ast->_types.push_back(ast);
                   },
                   [&, this](pt::EnumDecl *decl) {
                     // auto *newEnum = ast::EnumTy::Allocate(*_ast);
                     // enums.emplace_back(decl, newEnum);
                     //   declare(ast, pt);
                     //   env.add_type(ast);
                     //   _ast->_enums.push_back(ast);
                   },
                   [&, this](pt::TypeDecl *decl) {
                     TypeRef ref = env.declare_type(decl->_name);
                     types.emplace_back(decl, ref);
                   },
               },
               decl);
  }

  for (auto const &[pt, ast] : functions) {
    declare(ast, pt);
    env.add_decl(ast);
    _ast->_functions.push_back(ast);
  }

  for (auto const &[pt, ast] : structs) {
    define(ast, pt);
  }

  for (auto [pt, ref] : types) {
    ast::Type *ast = define(pt, ref);
    env.define_type(ref, ast);
    _ast->_types.push_back(ast);
  }

  for (auto const &[pt, ast] : functions) {
    define(ast, pt);
  }
}

ast::Block *Sema::check(pt::Block &block) {
  SymbolTable::BlockRef ref = env.scope_begin();

  std::vector<ast::Stmt *> body;
  for (pt::Stmt &stmt : block._body) {
    std::visit(overloaded{
                   [this, &body](auto stmt) {
                     body.push_back(static_cast<ast::Stmt *>(check(stmt)));
                   },
                   [this, &body](pt::VarDecl *stmt) {
                     ast::Type *type = env.resolve_type(stmt->_type);

                     auto *decl =
                         ast::VarDecl::Create(*_ast, stmt->_name, type);
                     body.push_back(decl);

                     // we need to evaluate expression before adding variable to
                     // env to avoid referencing itself
                     if (!stmt->_val.has_value()) {
                       env.add_decl(decl);
                       return;
                     }

                     ast::Expr *lhs = ast::DeclRef::Create(*_ast, decl, type);
                     ast::Expr *val = check(*stmt->_val);
                     auto *assign = ast::Assign::Create(*_ast, lhs, val);

                     body.push_back(assign);
                     env.add_decl(decl);
                   },
                   [this, &body](pt::Expr &stmt) {
                     body.push_back(static_cast<ast::Stmt *>(check(stmt)));
                   },
               },
               stmt);
  }

  not_null<ast::Expr *> terminator =
      block._end.has_value() ? check(block._end.value()) : _ast->_voidExpr;

  env.scope_end(ref);

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
  ast::Expr *retVal =
      stmt->_retVal.has_value() ? check(stmt->_retVal.value()) : nullptr;

  return ast::Return::Create(*_ast, retVal);
}

ast::Assign *Sema::check(pt::Assign *stmt) {
  ast::Expr *assignee = check(stmt->_assignee);
  ast::Expr *val = check(stmt->_value);

  if (!assignee->lvalueable()) {
    spdlog::error("expected lvalue got {}", fmt::underlying(val->get_kind()));
    std::terminate();
  }
  assignee->to_lvalue();

  return ast::Assign::Create(*_ast, assignee, val);
}

ast::While *Sema::check(pt::While *stmt) {
  ast::Expr *cond = check(stmt->_cond);
  ast::Block *body = check(*stmt->_body);

  return ast::While::Create(*_ast, cond, body);
}

not_null<ast::Expr *> Sema::check(pt::Expr &expr) {
  return std::visit(
      overloaded{
          [this](pt::IntegerE *const &expr) -> ast::Expr * {
            spdlog::info("int {}", fmt::ptr(expr));
            return ast::IntegerLiteral::Int32(*_ast, expr->_val);
          },
          [this](pt::Call *const &expr) -> ast::Expr * {
            ast::Decl *decl = env.resolve_decl(expr->fn);

            switch (decl->get_kind()) {
              case ast::Decl::DeclKind::Fn:
                return ast::FnCall::Create(*_ast, llvm::cast<ast::FnDecl>(decl),
                                           check(*expr->args));
              case ast::Decl::DeclKind::Var:
              case ast::Decl::DeclKind::Field:
                throw std::runtime_error("not implemented");
            }
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
            ast::Decl *decl = env.resolve_decl(expr);

            spdlog::info("casting {}", fmt::underlying(decl->get_kind()));
            auto *varDecl = llvm::dyn_cast<ast::VarDecl>(decl);
            if (varDecl == nullptr) {
              spdlog::error("expected variable");
              std::terminate();
            }

            ast::Type *type = varDecl->type();

            return ast::DeclRef::Create(*_ast, varDecl, type);
          },
          [this](pt::Block *expr) -> ast::Expr * { return check(*expr); },
          [this](pt::FieldAccess *expr) -> ast::Expr * {
            ast::Expr *base = check(expr->base);
            assert(base->lvalueable());

            spdlog::info("field access {}",
                         fmt::underlying(base->type()->get_kind()));
            auto *struc = llvm::dyn_cast<ast::StructTy>(base->type());
            if (struc == nullptr) {
              spdlog::error("can't access field of non-struct");
              std::terminate();
            }

            ast::FieldDecl *field = struc->get_field(expr->field);

            return ast::FieldAccess::Create(*_ast, base, field);
          },
          [](auto const & /*expr*/) -> ast::Expr * { assert(false); },
      },
      expr);
}

void Sema::declare(ast::FnDecl *ast, pt::FnDecl *pt) {
  std::vector<ast::FnDecl::Param> params;
  params.reserve(pt->_params.size());

  for (auto const &[_, type] : pt->_params) {
    ast::Type *restype = env.resolve_type(type);
    params.push_back(ast::FnDecl::Param(pt->name(), restype));
  }

  ast::Type *type = env.resolve_type(pt->_retTy);

  ast->Create(pt->name(), std::move(params), type);
}

void Sema::define(ast::FnDecl *ast, pt::FnDecl *pt) {
  ast::Block *body = check(*pt->_body);

  ast->define(body);
}

void Sema::declare(ast::StructTy *ast, pt::StructDecl *pt) {
  ast->Create(pt->_name);

  // {
  //   std::vector<ast::FnDecl::Param> params;
  //   auto *constructor =
  //       ast::FnDecl::Create(*_ast, "constructor", std::move(params), ast);
  //
  //   env.add_struct_method(ast, constructor);
  //
  //   _ast->_functions.push_back(constructor);
  // }
}

void Sema::define(ast::StructTy *ast, pt::StructDecl *pt) {
  std::vector<ast::FieldDecl *> fields;

  assert(pt->_fields.size() <= std::numeric_limits<uint8_t>::max());

  for (pt::StructDecl::Field const &field : pt->_fields) {
    ast::Type *type = env.resolve_type(field.type);
    auto ix = static_cast<uint8_t>(fields.size());
    fields.push_back(ast::FieldDecl::Create(*_ast, field.name, type, ix));
  }

  ast->define(std::move(fields));

  // {
  //   ast::FnDecl *constructor = env.get_struct_method(ast);
  //
  //   std::vector<ast::Stmt *> body;
  //
  //   auto *block = ast::Block::Create(*_ast, std::move(body),
  //   _ast->_voidExpr); constructor->define(block);
  // }
}

void Sema::declare(ast::StructTy *ast, pt::EnumDecl *pt) {
  ast->Create(pt->_name);
}

void Sema::define(ast::StructTy *ast, pt::EnumDecl *pt) {
  std::vector<std::string> variants;
  variants.reserve(pt->_variants.size());

  for (pt::EnumDecl::Variant const &variant : pt->_variants) {
    variants.push_back(variant.name);
  }

  // ast->define(std::move(variants));
}

not_null<ast::Type *> Sema::define(pt::TypeDecl *pt, TypeRef ref) {
  not_null<ast::Type *> ast = env.resolve_type(pt->_type);

  return ast;
}

}  // namespace x::sema
