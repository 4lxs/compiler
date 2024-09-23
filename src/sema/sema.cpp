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
  env.add_decl(_ast->_int32Ty);

  for (auto const &module : ctx->_modules) {
    add(*module);
  }
}

Ptr<ast::Context> Sema::finish() { return std::move(_ast); }

void Sema::add(pt::Module const &module) {
  std::vector<std::pair<pt::FnDecl *, ast::FnDecl *>> functions;
  // std::vector<std::pair<pt::TypeDecl *, ast::Type *>> types;
  std::vector<std::pair<pt::StructDecl *, ast::StructTy *>> structs;

  // first declare all top level decls. this is required for resolving
  // references as they can be used before definition
  for (pt::TopLevelDecl const &decl : module._items) {
    std::visit(overloaded{
                   [&, this](pt::FnDecl *func) {
                     ast::FnDecl *newfn = ast::FnDecl::Allocate(*_ast);
                     functions.emplace_back(func, newfn);
                   },
                   [&, this](pt::StructDecl *decl) {
                     auto *newStruct = ast::StructTy::Allocate(*_ast);
                     structs.emplace_back(decl, newStruct);
                   }
                   // [&, this](pt::Type *const &type) {
                   //   // ast::Type *newtype = ast::Type::Allocate(*_ast);
                   //   // _ast->_types.push_back(newtype);
                   //   // _maps.insert(type, newtype);
                   // },
               },
               decl);
  }

  // TODO: create types first, because functions may resolve them

  for (auto const &[pt, ast] : structs) {
    declare(ast, pt);
  }

  // next declare top level items. we can't define them yet as they may refer to
  // each other
  for (auto const &[pt, ast] : functions) {
    declare(ast, pt);
  }

  for (auto const &[pt, ast] : structs) {
    define(ast, pt);
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
                     ast::Decl *typedecl = env.resolve(stmt->_type);
                     auto *type = llvm::dyn_cast<ast::Type>(typedecl);
                     if (type == nullptr) {
                       spdlog::error("expected type");
                       std::terminate();
                     }

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

  ast::Expr *terminator =
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
  ast::Expr *retVal = stmt->_retVal.has_value() ? check(stmt->_retVal.value())
                                                : _ast->_voidExpr;

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

not_null<ast::Expr *> Sema::check(pt::Expr &expr) {
  return std::visit(
      overloaded{
          [this](pt::IntegerE *const &expr) -> ast::Expr * {
            spdlog::info("int {}", fmt::ptr(expr));
            return ast::IntegerLiteral::Int32(*_ast, expr->_val);
          },
          [this](pt::Call *const &expr) -> ast::Expr * {
            ast::Decl *decl = env.resolve(expr->fn);

            switch (decl->get_kind()) {
              case ast::Decl::DeclKind::Fn:
                return ast::FnCall::Create(*_ast, llvm::cast<ast::FnDecl>(decl),
                                           check(*expr->args));
              case ast::Decl::DeclKind::StructTy: {
                auto *structty = llvm::cast<ast::StructTy>(decl);

                ast::FnDecl *constructor = env.get_struct_method(structty);

                return ast::FnCall::Create(*_ast, constructor,
                                           check(*expr->args));
              }
              case ast::Decl::DeclKind::Var:
              case ast::Decl::DeclKind::Field:
                assert(false);
              case ast::Decl::DeclKind::TypeBegin:
              case ast::Decl::DeclKind::LiteralTy:
              case ast::Decl::DeclKind::TypeEnd:
                spdlog::error("cannot call non-callable: {}", decl->name());
                std::terminate();
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
            ast::Decl *decl = env.resolve(expr);

            spdlog::info("casting {}", fmt::underlying(decl->get_kind()));
            auto *varDecl = llvm::dyn_cast<ast::VarDecl>(decl);
            if (varDecl == nullptr) {
              spdlog::error("expected variable");
              std::terminate();
            }

            ast::Type *type = varDecl->_type;

            return ast::DeclRef::Create(*_ast, varDecl, type);
          },
          [this](pt::Block *expr) -> ast::Expr * { return check(*expr); },
          [this](pt::FieldAccess *expr) -> ast::Expr * {
            auto *base = llvm::dyn_cast<ast::DeclRef>(check(expr->base).get());
            if (base == nullptr) {
              spdlog::error("expected declref");
              std::terminate();
            }

            spdlog::info("field access {}",
                         fmt::underlying(base->_decl->_type->get_kind()));
            auto *struc = llvm::dyn_cast<ast::StructTy>(base->_decl->_type);
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
    ast::Decl *decl = env.resolve(type);
    if (auto *restype = llvm::dyn_cast<ast::LiteralTy>(decl)) {
      params.push_back(ast::FnDecl::Param(pt->name(), restype));
    } else {
      spdlog::error("expected type");
      std::terminate();
    }
  }

  ast::Decl *retDecl = env.resolve(pt->_retTy);
  auto *type = llvm::dyn_cast<ast::LiteralTy>(retDecl);
  if (type == nullptr) {
    spdlog::error("expected return type");
    std::terminate();
  }

  // assert(body->type() == retType);

  ast->Create(pt->name(), std::move(params), type);
  env.add_decl(ast);
  _ast->_functions.push_back(ast);
}

void Sema::define(ast::FnDecl *ast, pt::FnDecl *pt) {
  ast::Block *body = check(*pt->_body);

  ast->define(body);
}

void Sema::declare(ast::StructTy *ast, pt::StructDecl *pt) {
  ast->Create(pt->_name);

  env.add_decl(ast);

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
    ast::Decl *type = env.resolve(field.type);
    if (auto *resType = llvm::dyn_cast<ast::Type>(type)) {
      auto ix = static_cast<uint8_t>(fields.size());
      fields.push_back(ast::FieldDecl::Create(*_ast, field.name, resType, ix));
    } else {
      spdlog::error("expected type");
      std::terminate();
    }
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

}  // namespace x::sema
