#pragma once

#include <ParserRuleContext.h>
#include <XBaseVisitor.h>
#include <XParser.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Value.h>

#include "x/pt/context.hpp"
#include "x/pt/decl.hpp"
#include "x/pt/stmt.hpp"

namespace x {

class Visitor : public parser::XBaseVisitor {
 public:
  explicit Visitor(pt::Context *ctx);

 protected:
  std::any visitModdef(parser::XParser::ModdefContext *ctx) override;

  std::any visitFunction(parser::XParser::FunctionContext *ctx) override;

  std::any visitReturn(parser::XParser::ReturnContext *ctx) override;

  std::any visitCallE(parser::XParser::CallEContext *ctx) override;

  std::any visitIntPE(parser::XParser::IntPEContext *ctx) override;

  std::any visitBinaryE(parser::XParser::BinaryEContext *ctx) override;

  std::any visitStructExpr(parser::XParser::StructExprContext *ctx) override;

  std::any visitStmt(parser::XParser::StmtContext *ctx) override;

  std::any visitIf_(parser::XParser::If_Context *ctx) override;

  std::any visitBlock(parser::XParser::BlockContext *ctx) override;

  std::any visitVarDef(parser::XParser::VarDefContext *ctx) override;

  std::any visitVarAssign(parser::XParser::VarAssignContext *ctx) override;

  std::any visitVarE(parser::XParser::VarEContext *ctx) override;

  std::any visitStructDef(parser::XParser::StructDefContext *ctx) override;

  std::any visitWhile(parser::XParser::WhileContext *ctx) override;

  std::any visitMemberAccess(
      parser::XParser::MemberAccessContext *ctx) override;

 private:
  [[nodiscard]]
  pt::StructDecl::Field parse_field(parser::XParser::StructFieldContext *ctx);

  [[nodiscard]]
  std::vector<pt::FnParam> parse_params(parser::XParser::ParamsContext *ctx);

  [[nodiscard]]
  static pt::Path get_path(parser::XParser::PathContext *ctx);

  pt::Context *_ctx;

  // set in visitModdef. should never be null
 public:
  pt::Module *_module{nullptr};

 private:
  /// this is non-null while we're in a block.
  /// it has the value of the inner-most block
  pt::Block *_block{};

  class Stack_ {
   public:
    /// Expr is a variant. emplacing allows you to do push(A) instead of
    /// push(Expr(a))
    template <typename TExpr>
    constexpr void push(TExpr expr) {
      _stack.emplace_back(expr);
    };

    pt::Expr pop();
    std::vector<pt::Expr> pop(size_t cnt);
    [[nodiscard]] auto size() const { return _stack.size(); }

   private:
    // stack for storing values. std::any doesn't allow non-copyable types
    std::vector<pt::Expr> _stack;
  } _stack;
};

}  // namespace x
