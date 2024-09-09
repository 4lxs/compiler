#pragma once

#include <ParserRuleContext.h>
#include <XBaseVisitor.h>
#include <XParser.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Value.h>

#include "x/ast/context.hpp"
#include "x/ast/stmt.hpp"

namespace x {

class Visitor : public parser::XBaseVisitor {
 public:
  explicit Visitor(ast::Context *ctx);

 protected:
  std::any visitModdef(parser::XParser::ModdefContext *ctx) override;

  std::any visitFunction(parser::XParser::FunctionContext *ctx) override;

  std::any visitReturn(parser::XParser::ReturnContext *ctx) override;

  std::any visitCallE(parser::XParser::CallEContext *ctx) override;

  std::any visitIntPE(parser::XParser::IntPEContext *ctx) override;

  std::any visitBinaryE(parser::XParser::BinaryEContext *ctx) override;

  std::any visitAnonStruct(parser::XParser::AnonStructContext *ctx) override;

 private:
  [[nodiscard]]
  std::vector<ast::FnParam> parse_params(parser::XParser::ParamsContext *ctx);

  [[nodiscard]]
  ast::Stub *get_stub(parser::XParser::PathContext *ctx) const;

  ast::Context *_ctx;

  // set in visitModdef. should never be null
 public:
  ast::Module *_module{nullptr};

 private:
  /// this is non-null while we're in a block.
  /// it has the value of the inner-most block
  ast::Block *_block{};

  class Stack_ {
   public:
    /// Expr is a variant
    /// emplacing allows you to do push(A) instead of push(Expr(a))
    template <typename... Args>
    constexpr void push(Args &&...args) {
      _stack.emplace_back(std::move(args)...);
    };

    ast::Expr pop();
    std::vector<ast::Expr> pop(size_t cnt);

   private:
    // stack for storing values. std::any doesn't allow non-copyable types
    std::vector<ast::Expr> _stack;
  } _stack;
};

}  // namespace x
