#pragma once

#include "x/ast/context.hpp"
#include "x/ast/fwd_decl.hpp"
#include "x/common.hpp"
#include "x/pt/fwd_decl.hpp"

namespace x::sema {

/// checks for inconsistencies. undefined stubs, multiple definitions, ...
/// it also evaluates items to their definition and links them with other stubs.
///
/// note that calling this function invalidates the context and all pointers
/// obtained from it except any pt::Path*, which is guaranteed to be alive as
/// long as Sema is alive and can be used to remove/update a node
class Sema {
 public:
  /// @param ctx the initial parse tree
  explicit Sema(pt::Context* ctx);

  Ptr<ast::Context> finish();

  void add(pt::Node& node);

 private:
  // void declare(ast::FnDecl* ast, pt::FnDecl* pt,
  //              pt::MethodDecl* method = nullptr);
  // void define(ast::FnDecl* ast, pt::FnDecl* pt,
  //             pt::MethodDecl* method = nullptr);
  //
  // void declare(ast::StructTy* ast, pt::StructDecl* pt);
  // void define(ast::StructTy* ast, pt::StructDecl* pt);
  //
  // void declare(ast::StructTy* ast, pt::EnumDecl* pt);
  // void define(ast::StructTy* ast, pt::EnumDecl* pt);
  //
  // not_null<ast::Type*> define(pt::TypeDecl* pt, ast::Type* ref);
  //
  // ast::Return* check(pt::Return* stmt);
  // ast::While* check(pt::While* stmt);
  // ast::Assign* check(pt::Assign* stmt);
  // ast::VarDecl* check(pt::VarDecl* stmt);
  // not_null<ast::Expr*> check(pt::Expr& expr);
  // std::variant<ast::VarRefRef, ast::ValueDeclRef, ast::TypeRef> check(
  //     pt::DeclUse* expr);
  //
  // ast::Block* check(pt::Block& block);
  // ast::StructLiteral* check(pt::StructExpr& expr);

  void add(pt::Module const& module);

  Ptr<ast::Context> _ast = std::make_unique<ast::Context>();
  pt::Context* _pt;

  // SymbolTable env;
};

}  // namespace x::sema
