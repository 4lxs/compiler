#pragma once

#include <ParserRuleContext.h>
#include <XBaseVisitor.h>
#include <XParser.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Value.h>

#include "x/pt/context.hpp"
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

  std::any visitAnonStruct(parser::XParser::AnonStructContext *ctx) override;

 private:
  [[nodiscard]]
  std::vector<pt::FnParam> parse_params(parser::XParser::ParamsContext *ctx);

  [[nodiscard]]
  pt::Stub *get_stub(parser::XParser::PathContext *ctx) const;

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
    /// Expr is a variant
    /// emplacing allows you to do push(A) instead of push(Expr(a))
    template <typename... Args>
    constexpr void push(Args &&...args) {
      _stack.emplace_back(std::move(args)...);
    };

    pt::Expr pop();
    std::vector<pt::Expr> pop(size_t cnt);

   private:
    // stack for storing values. std::any doesn't allow non-copyable types
    std::vector<pt::Expr> _stack;
  } _stack;
};

}  // namespace x
