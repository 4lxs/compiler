#include "x/sema/sema.hpp"

#include <fmt/format.h>
#include <llvm/Support/Casting.h>

#include <memory>
#include <ranges>
#include <utility>
#include <variant>

#include "spdlog/common.h"
#include "x/ast/context.hpp"
#include "x/ast/expr.hpp"
#include "x/ast/fwd_decl.hpp"
#include "x/ast/stmt.hpp"
#include "x/ast/type.hpp"
#include "x/common.hpp"
#include "x/pt/context.hpp"
#include "x/pt/decl.hpp"
#include "x/pt/expr.hpp"
#include "x/pt/stmt.hpp"
#include "x/sema/environment.hpp"

namespace x::sema {

Sema::Sema(pt::Context *ctx) : _pt(ctx) {
  for (pt::NodeId item : ctx->_items) {
    add(ctx->get_node(item));
  }
}

Ptr<ast::Context> Sema::finish() { return std::move(_ast); }

void Sema::add(pt::Module const &module) {
  // std::vector<std::pair<pt::FnDecl *, ast::FnDecl *>> functions;
  // std::vector<std::pair<pt::StructDecl *, ast::StructTy *>> structs;
  // std::vector<std::pair<pt::MethodDecl *, ast::FnDecl *>> methods;
  // std::vector<std::pair<pt::EnumDecl *, ast::StructTy *>> enums;
  // std::vector<pt::TypeDecl *> types;
  //
  // // declare top level items. we can't define them yet as they may refer to
  // // each other
  // for (pt::Node *decl : module._items) {
  //   switch (decl->kind()) {
  //     case pt::Node::Kind::FnDecl: {
  //       auto *newfn = ast::FnDecl::Allocate(*_ast);
  //       functions.emplace_back(llvm::cast<pt::FnDecl>(decl), newfn);
  //     } break;
  //     case pt::Node::Kind::StructDecl: {
  //       pt::StructDecl *decl = llvm::cast<pt::StructDecl>(decl);
  //       auto *ast = ast::StructTy::Create(*_ast, decl->_name);
  //       structs.emplace_back(decl, ast);
  //       declare(ast, decl);
  //       _ast->_types.push_back(ast);
  //     } break;
  //     case pt::Node::Kind::EnumDecl: {
  //       // auto *newEnum = ast::StructTy::Create(*_ast, decl->name);
  //       // env.add(newEnum);
  //       // _ast->_types.push_back(newEnum);
  //     } break;
  //     case pt::Node::Kind::TypeDecl: {
  //       types.emplace_back(llvm::cast<pt::TypeDecl>(decl));
  //     }
  //     case pt::Node::Kind::MethodDecl: {
  //       ast::FnDecl *newfn = ast::FnDecl::Allocate(*_ast);
  //       methods.emplace_back(llvm::cast<pt::MethodDecl>(decl), newfn);
  //     } break;
  //     default:
  //       xerr("unexpected top level declkind {}",
  //       fmt::underlying(decl->kind()));
  //   }
  // }
  //
  // /// we hold off due to type aliases
  // for (pt::TypeDecl *type : types) {
  //   // ast::Type *ref = env.resolve_type(type->_type);
  //   // env.alias(type->_name, ref);
  // }
  //
  // for (auto [pt, ast] : enums) {
  //   for (auto const &[i, variant] : pt->_variants | std::views::enumerate) {
  //     auto *var = ast::ConstDecl::Create(
  //         *_ast, env.mangle_join(ast, variant.name), _ast->_int32Ty, i);
  //
  //     env.add(var);
  //   }
  // }
  //
  // // we hold off on declaring functions as they may resolve
  // // types
  // for (auto const &[pt, ast] : functions) {
  //   declare(ast, pt);
  //   _ast->_functions.push_back(ast);
  // }
  //
  // for (auto const &[pt, ast] : methods) {
  //   declare(ast, pt, pt);
  //   _ast->_functions.push_back(ast);
  // }
  //
  // for (auto const &[pt, ast] : structs) {
  //   define(ast, pt);
  // }
  //
  // for (auto const &[pt, ast] : functions) {
  //   define(ast, pt);
  // }
  //
  // for (auto const &[pt, ast] : methods) {
  //   define(ast, pt, pt);
  // }
}

// ast::Block *Sema::check(pt::Block &block) {
//   env.scope_begin();
//
//   std::vector<ast::Stmt *> body;
//   for (pt::Stmt &stmt : block._body) {
//     std::visit(overloaded{
//                    [this, &body](auto stmt) {
//                      body.push_back(static_cast<ast::Stmt *>(check(stmt)));
//                    },
//                    [this, &body](pt::VarDecl *stmt) {
//                      ast::Type *type = env.resolve_type(stmt->_type);
//
//                      auto *decl =
//                          ast::VarDecl::Create(*_ast, stmt->_name, type);
//                      body.push_back(decl);
//
//                      // we need to evaluate expression before adding variable
//                      to
//                      // env to avoid referencing itself
//                      if (!stmt->_val.has_value()) {
//                        env.add(decl);
//                        return;
//                      }
//
//                      ast::Expr *lhs = ast::VarRef::Create(*_ast, decl, type);
//                      ast::Expr *val = check(*stmt->_val);
//                      auto *assign = ast::Assign::Create(*_ast, lhs, val);
//
//                      body.push_back(assign);
//                      env.add(decl);
//                    },
//                    [this, &body](pt::Expr &stmt) {
//                      body.push_back(static_cast<ast::Stmt *>(check(stmt)));
//                    },
//                },
//                stmt);
//   }
//
//   not_null<ast::Expr *> terminator =
//       block._end.has_value() ? check(block._end.value()) : _ast->_voidExpr;
//
//   env.scope_end();
//
//   return ast::Block::Create(*_ast, std::move(body), terminator);
// }
//
// ast::StructLiteral *Sema::check(pt::StructExpr &expr) {
//   std::vector<ast::Expr *> fields;
//   fields.reserve(expr.fields.size());
//
//   for (auto &[_, value] : expr.fields) {
//     fields.push_back(check(value));
//   }
//
//   return ast::StructLiteral::Create(*_ast, std::move(fields));
// }
//
// ast::Return *Sema::check(pt::Return *stmt) {
//   ast::Expr *retVal =
//       stmt->_retVal.has_value() ? check(stmt->_retVal.value()) : nullptr;
//
//   return ast::Return::Create(*_ast, retVal);
// }
//
// ast::Assign *Sema::check(pt::Assign *stmt) {
//   ast::Expr *assignee = check(stmt->_assignee);
//   ast::Expr *val = check(stmt->_value);
//
//   if (!assignee->lvalueable()) {
//     spdlog::error("expected lvalue got {}",
//     fmt::underlying(val->get_kind())); std::terminate();
//   }
//   assignee->to_lvalue();
//
//   return ast::Assign::Create(*_ast, assignee, val);
// }
//
// ast::While *Sema::check(pt::While *stmt) {
//   ast::Expr *cond = check(stmt->_cond);
//   ast::Block *body = check(*stmt->_body);
//
//   return ast::While::Create(*_ast, cond, body);
// }
//
// not_null<ast::Expr *> Sema::check(pt::Expr &expr) {
//   return std::visit(
//       overloaded{
//           [this](pt::Integer *expr) -> ast::Expr * {
//             spdlog::info("int {}", fmt::ptr(expr));
//             return ast::IntegerLiteral::Int32(*_ast, expr->_val);
//           },
//           [this](pt::Call *expr) -> ast::Expr * {
//             ast::VarRef *fnvar{};
//             if (auto **declref = std::get_if<pt::DeclRef *>(&expr->fn)) {
//               auto ref = check(*declref);
//               if (auto *valref = std::get_if<ast::ValueDeclRef>(&ref)) {
//                 auto *fnref = llvm::dyn_cast<ast::FnDecl>(valref->get());
//                 if (fnref == nullptr) {
//                   xerr("expected function reference in {}. got {}",
//                        (*valref)->name(),
//                        fmt::underlying((*valref)->get_kind()));
//                 }
//
//                 return ast::FnCall::Create(*_ast, fnref, check(*expr->args));
//               }
//               fnvar = std::get<ast::VarRefRef>(ref);
//             } else {
//               fnvar = llvm::dyn_cast<ast::VarRef>(check(expr->fn).get());
//               if (fnvar == nullptr) {
//                 xerr("expected var reference");
//               }
//             }
//
//             ast::Decl *decl = fnvar->_decl;
//
//             switch (decl->get_kind()) {
//               case ast::Decl::DeclKind::Fn:
//                 return ast::FnCall::Create(*_ast,
//                 llvm::cast<ast::FnDecl>(decl),
//                                            check(*expr->args));
//               case ast::Decl::DeclKind::Var:
//               case ast::Decl::DeclKind::Field:
//                 throw std::runtime_error("not implemented");
//               default:
//                 throw std::runtime_error(
//                     fmt::format("unexpected call with {}",
//                                 fmt::underlying(decl->get_kind())));
//             }
//           },
//           [this](pt::IfExpr *expr) -> ast::Expr * {
//             not_null<ast::Expr *> cond = check(expr->cond);
//             not_null<ast::Block *> then = check(*expr->then);
//             ast::Block *els =
//                 expr->else_ != nullptr ? check(*expr->else_) : nullptr;
//
//             if (cond->type() != _ast->_boolTy) {
//               spdlog::error("if condition must be of type bool");
//               std::terminate();
//             }
//             if (then->type() != els->type()) {
//               spdlog::error("if branches must have same type");
//               std::terminate();
//             }
//             return ast::If::Create(*_ast, cond, then, els);
//           },
//           [this](pt::BinaryExpr *expr) -> ast::Expr * {
//             ast::Expr *lhs = check(expr->l);
//             ast::Expr *rhs = check(expr->r);
//             assert(lhs->type() == rhs->type());
//
//             switch (expr->op) {
//               case pt::BinaryExpr::Operator::Plus:
//                 return ast::Builtin::Create(*_ast, ast::Builtin::Op::iAdd,
//                                             std::vector{lhs, rhs},
//                                             lhs->type());
//               case pt::BinaryExpr::Operator::Minus:
//                 return ast::Builtin::Create(*_ast, ast::Builtin::Op::iSub,
//                                             std::vector{lhs, rhs},
//                                             lhs->type());
//               case pt::BinaryExpr::Operator::Star:
//                 return ast::Builtin::Create(*_ast, ast::Builtin::Op::iMul,
//                                             std::vector{lhs, rhs},
//                                             lhs->type());
//               case pt::BinaryExpr::Operator::Slash:
//                 return ast::Builtin::Create(*_ast, ast::Builtin::Op::iDiv,
//                                             std::vector{lhs, rhs},
//                                             lhs->type());
//               case pt::BinaryExpr::Operator::Greater:
//                 return ast::Builtin::Create(*_ast,
//                 ast::Builtin::Op::iGreater,
//                                             std::vector{lhs, rhs},
//                                             _ast->_boolTy);
//               case pt::BinaryExpr::Operator::Less:
//                 return ast::Builtin::Create(*_ast, ast::Builtin::Op::iLess,
//                                             std::vector{lhs, rhs},
//                                             _ast->_boolTy);
//             }
//           },
//           [this](pt::DeclUse *expr) -> ast::Expr * {
//             auto ref = check(expr);
//             return std::visit(
//                 overloaded{
//                     [](ast::VarRefRef ref) -> ast::Expr * { return ref; },
//                     [this](ast::ValueDeclRef ref) -> ast::Expr * {
//                       // switch (ref->get_kind()) {
//                       //   case ast::Decl::DeclKind::Field: {
//                       //     auto *field =
//                       //     llvm::cast<ast::FieldDecl>(ref.get()); return
//                       //     ast::FieldAccess::Create(*_ast, *field->type(),
//                       //                                     field);
//                       //   }
//                       // }
//                       xerr("unexpected decl {}",
//                            fmt::underlying(ref->get_kind()));
//                     },
//                     [](ast::TypeRef ref) -> ast::Expr * {
//                       xerr("unexpected type {}",
//                            fmt::underlying(ref->get_kind()));
//                     },
//                 },
//                 ref);
//           },
//           [this](pt::Block *expr) -> ast::Expr * { return check(*expr); },
//           [this](pt::FieldAccess *expr) -> ast::Expr * {
//             ast::Expr *base = check(expr->base);
//             assert(base->lvalueable());
//
//             ast::Decl *decl{};
//             switch (base->get_kind()) {
//               case ast::Stmt::SK_VarRef: {
//                 auto *var = llvm::cast<ast::VarRef>(base);
//                 std::string_view name =
//                     env.mangle_join(var->_decl, expr->field);
//
//                 LookupResult res = env.lookup(name);
//
//                 decl = res.get_single();
//               } break;
//               case ast::Stmt::SK_FieldAccess: {
//                 auto *fieldacc = llvm::cast<ast::FieldAccess>(base);
//                 decl = fieldacc->_field;
//               } break;
//               case ast::Stmt::SK_Return:
//               case ast::Stmt::SK_Function:
//               case ast::Stmt::SK_VarDecl:
//               case ast::Stmt::SK_Assign:
//               case ast::Stmt::SK_While:
//               case ast::Stmt::SK_Expr:
//               case ast::Stmt::SK_Int:
//               case ast::Stmt::SK_Bool:
//               case ast::Stmt::SK_String:
//               case ast::Stmt::SK_Struct:
//               case ast::Stmt::SK_If:
//               case ast::Stmt::SK_Call:
//               case ast::Stmt::SK_Block:
//               case ast::Stmt::SK_Builtin:
//               case ast::Stmt::SK_ExprEnd:
//                 xerr("unexpected stmt kind {}",
//                      fmt::underlying(base->get_kind()));
//             }
//
//             switch (decl->get_kind()) {
//               case ast::Decl::DeclKind::Field: {
//                 auto *field = llvm::cast<ast::FieldDecl>(decl);
//                 auto *struc = llvm::dyn_cast<ast::StructTy>(base->type());
//                 if (struc == nullptr) {
//                   xerr("can't access field of non-struct");
//                 }
//
//                 return ast::FieldAccess::Create(*_ast, base, field);
//               }
//               case ast::Decl::DeclKind::ValueBegin:
//               case ast::Decl::DeclKind::Fn:
//               case ast::Decl::DeclKind::Var:
//               case ast::Decl::DeclKind::Const:
//               case ast::Decl::DeclKind::ValueEnd:
//               case ast::Decl::DeclKind::Type:
//               case ast::Decl::DeclKind::Literal:
//               case ast::Decl::DeclKind::Struct:
//               case ast::Decl::DeclKind::Union:
//               case ast::Decl::DeclKind::TypeEnd:
//                 xerr("unexpected decl kind {}",
//                      fmt::underlying(decl->get_kind()));
//             }
//
//             // auto *fieldDecl = ast::FieldDecl::Create(
//             //     *_ast, env.mangle_join(decl, expr->field), type, ix);
//             //
//             // spdlog::info("field access {}",
//             //              fmt::underlying(base->type()->get_kind()));
//           },
//           [](auto /*expr*/) -> ast::Expr * { assert(false); },
//       },
//       expr);
// }
//
// std::variant<ast::VarRefRef, ast::ValueDeclRef, ast::TypeRef> Sema::check(
//     pt::DeclUse *expr) {
//   spdlog::info("checking decl ref {}", format_as(expr->_var));
//   ast::Decl *decl = env.lookup(expr)->get_single();
//
//   spdlog::info("casting {}", fmt::underlying(decl->get_kind()));
//
//   if (auto *varDecl = llvm::dyn_cast<ast::VarDecl>(decl)) {
//     ast::Type *type = varDecl->type();
//
//     return ast::VarRef::Create(*_ast, varDecl, type);
//   }
//
//   if (auto *ref = llvm::dyn_cast<ast::ValueDecl>(decl)) {
//     return ref;
//   }
//
//   return llvm::cast<ast::Type>(decl);
// }
//
// void Sema::declare(ast::FnDecl *ast, pt::FnDecl *pt, pt::MethodDecl *method)
// {
//   bool hasSelf = method != nullptr && !method->_isStatic;
//
//   std::vector<ast::FnDecl::Param> params;
//   params.reserve(pt->_params.size() + (hasSelf ? 1 : 0));
//
//   if (method != nullptr) {
//     ast::Type *recvtyp = env.resolve_type(method->_recv);
//
//     if (hasSelf) {
//       params.push_back(ast::FnDecl::Param(pt->name(), recvtyp));
//     }
//
//     pt->_mangledName = fmt::format("{}%s{}", recvtyp->name(), pt->name());
//   } else {
//     pt->_mangledName = pt->name();
//   }
//
//   for (auto const &[_, type] : pt->_params) {
//     ast::Type *restype = env.resolve_type(type);
//     params.push_back(ast::FnDecl::Param(pt->name(), restype));
//   }
//
//   ast::Type *type = env.resolve_type(pt->_retTy);
//
//   ast->Create(pt->_mangledName, std::move(params), type);
//
//   env.add(ast);
// }
//
// void Sema::define(ast::FnDecl *ast, pt::FnDecl *pt, pt::MethodDecl *method) {
//   ast::Block *body = check(*pt->_body);
//
//   ast->define(body);
// }
//
// void Sema::declare(ast::StructTy *ast, pt::StructDecl *pt) {
//   env.add(ast);
//
//   // {
//   //   std::vector<ast::FnDecl::Param> params;
//   //   auto *constructor =
//   //       ast::FnDecl::Create(*_ast, "constructor", std::move(params), ast);
//   //
//   //   env.add_struct_method(ast, constructor);
//   //
//   //   _ast->_functions.push_back(constructor);
//   // }
// }
//
// void Sema::define(ast::StructTy *ast, pt::StructDecl *pt) {
//   std::vector<ast::FieldDecl *> fields;
//
//   assert(pt->_fields.size() <= std::numeric_limits<uint8_t>::max());
//
//   for (pt::StructDecl::Field const &field : pt->_fields) {
//     ast::Type *type = env.resolve_type(field.type);
//     auto ix = static_cast<uint8_t>(fields.size());
//
//     auto *fieldDecl = ast::FieldDecl::Create(
//         *_ast, env.mangle_join(ast, field.name), type, ix);
//
//     fields.push_back(fieldDecl);
//     env.add(fieldDecl);
//   }
//
//   ast->define(std::move(fields));
//
//   // {
//   //   ast::FnDecl *constructor = env.get_struct_method(ast);
//   //
//   //   std::vector<ast::Stmt *> body;
//   //
//   //   auto *block = ast::Block::Create(*_ast, std::move(body),
//   //   _ast->_voidExpr); constructor->define(block);
//   // }
// }
//
// void Sema::declare(ast::StructTy *ast, pt::EnumDecl *pt) {
//   ast->Create(pt->_name);
// }
//
// void Sema::define(ast::StructTy *ast, pt::EnumDecl *pt) {
//   std::vector<std::string> variants;
//   variants.reserve(pt->_variants.size());
//
//   for (pt::EnumDecl::Variant const &variant : pt->_variants) {
//     variants.push_back(variant.name);
//   }
//
//   // ast->define(std::move(variants));
// }
//
// not_null<ast::Type *> Sema::define(pt::TypeDecl *pt, ast::Type *ref) {
//   not_null<ast::Type *> ast = env.resolve_type(pt->_type);
//
//   return ast;
// }

}  // namespace x::sema
