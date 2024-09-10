#pragma once

#include <llvm/IR/Constants.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Value.h>
#include <llvm/IR/Verifier.h>
#include <spdlog/spdlog.h>

#include <ranges>
#include <variant>

#include "x/pt/expr.hpp"
#include "x/pt/module.hpp"
#include "x/pt/type.hpp"

namespace x {

class Compiler {
 public:
  void compile(const pt::Module::Val& module) {
    for (const auto& fnctn : module._functions) {
      spdlog::info("compiling function {}", fmt::ptr(fnctn.get()));
      // we need to forward declare all functions
      function(*fnctn);
    }

    for (const auto& fnctn : module._functions) {
      llvm::Function* func = _mod.getFunction(fnctn->name);
      llvm::BasicBlock* body = llvm::BasicBlock::Create(_ctx, "entry", func);
      _builder.SetInsertPoint(body);

      spdlog::info("body");
      for (const auto& stmt : fnctn->block->body) {
        spdlog::info("stmt");
        compile(stmt);
      }
      if (llvm::verifyFunction(*func, &llvm::errs())) {
        std::terminate();
      }
    }
  };

  void compile(const pt::StmtV& stmt) {
    std::visit([this](const auto& stmt) { visitStmt(*stmt); }, stmt.stmt);
  }
  //
  // void visitStmt(const std::unique_ptr<pt::Expr>& expr) {
  //   llvm::Value* val = eval(*expr);
  //   if (val != nullptr) {
  //     spdlog::warn("unused value");
  //   }
  // }

  void visitStmt(const pt::RetStmtV& ret) {
    spdlog::info("ret");
    _builder.CreateRet(ret._val.has_value() ? eval(*ret._val) : nullptr);
  }

  void visitStmt(const pt::CallV& stmt) {
    llvm::Function* calee = _mod.getFunction(stmt.fn->name);
    assert(calee);
    assert(calee->arg_size() == stmt.args->fields.size());

    std::vector<llvm::Value*> args;
    args.reserve(calee->arg_size());
    for (const pt::Field& field : stmt.args->fields) {
      llvm::Value* val = eval(field.value);
      assert(val != nullptr);
      args.push_back(val);
    }

    _builder.CreateCall(calee, args);
  }

  // void visitStmt(const auto& stmt) { std::terminate(); }

  llvm::Value* eval(const pt::Expr& expr) {
    return expr.accept([this](const auto& expr) { return visitExpr(expr); });
  }

  llvm::Value* visitExpr(const std::unique_ptr<pt::PrimaryExpr>& expr) {
    return llvm::ConstantInt::get(_ctx, llvm::APInt(32, expr->_val, 10));
  }

  llvm::Value* visitExpr(const std::unique_ptr<pt::ParenExpr>& expr) {
    return eval(expr->inner);
  }

  llvm::Value* visitExpr(const std::unique_ptr<pt::BinaryExpr>& expr) {
    llvm::Value* lhs = eval(expr->l);
    llvm::Value* rhs = eval(expr->r);

    return _builder.CreateAdd(lhs, rhs);
  }

  llvm::Value* visitExpr(const auto& expr) { assert(false); }

  llvm::Function* function(const pt::FnV& proto) {
    spdlog::info("function {}({})", proto.name, proto.params.size());
    std::vector<llvm::Type*> argTypes{};
    argTypes.reserve(proto.params.size());
    for (const auto& prm : proto.params) {
      argTypes.push_back(to_llvm_type(prm.type));
    }

    auto* funcType = llvm::FunctionType::get(proto.ret != nullptr
                                                 ? to_llvm_type(proto.ret)
                                                 : llvm::Type::getVoidTy(_ctx),
                                             argTypes, false);
    auto* func = llvm::Function::Create(
        funcType, llvm::Function::ExternalLinkage, proto.name, _mod);

    for (const auto& [arg, fnArg] :
         std::views::zip(func->args(), proto.params)) {
      arg.setName(fnArg.name);
    }

    return func;
  }

  llvm::Type* to_llvm_type(pt::Type* type) {
    spdlog::info("getting type: {}", fmt::underlying(type->_kind));
    return llvm::Type::getInt32Ty(_ctx);
  }

  // private:
  llvm::LLVMContext _ctx;
  llvm::Module _mod{"my cool jit", _ctx};
  llvm::IRBuilder<> _builder{_ctx};
};

};  // namespace x
