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

#include "x/ast/expr.hpp"
#include "x/ast/module.hpp"
#include "x/ast/stmt.hpp"
#include "x/ast/toplevel.hpp"
#include "x/ast/type.hpp"
#include "x/common.hpp"
#include "x/pt/expr.hpp"
#include "x/pt/module.hpp"
#include "x/pt/stmt.hpp"
#include "x/pt/type.hpp"

namespace x {

class Compiler {
 public:
  void compile(ast::Module const& module) {
    for (auto const& fnctn : module._functions) {
      spdlog::info("compiling function {}", fmt::ptr(fnctn.get()));
      // we need to forward declare all functions
      function(*fnctn);
    }

    for (auto const& fnctn : module._functions) {
      llvm::Function* func = _mod.getFunction(fnctn->pt->name());
      _currentFunction = func;
      llvm::BasicBlock* body = llvm::BasicBlock::Create(_ctx, "entry", func);
      _builder.SetInsertPoint(body);

      spdlog::info("body");
      for (auto const& stmt : fnctn->block->body) {
        spdlog::info("stmt");
        compile(stmt);
      }
      if (llvm::verifyFunction(*func, &llvm::errs())) {
        std::terminate();
      }
    }

    _currentFunction = nullptr;
  };

  void compile(ast::Stmt const& stmt) {
    std::visit([this](auto const& stmt) { visitStmt(stmt); }, stmt._val);
  }
  //
  // void visitStmt(const std::unique_ptr<pt::Expr>& expr) {
  //   llvm::Value* val = eval(*expr);
  //   if (val != nullptr) {
  //     spdlog::warn("unused value");
  //   }
  // }

  void visitStmt(Ptr<ast::Ret> const& ret) {
    spdlog::info("ret");
    _builder.CreateRet(ret->_val.has_value() ? eval(*ret->_val) : nullptr);
  }

  void visitStmt(ast::Expr const& expr) {
    spdlog::info("expr");
    eval(expr);
  }

  void visitStmt(ast::CallE const& stmt) {
    llvm::Function* calee = _mod.getFunction(stmt.fn->pt->name());
    assert(calee);
    assert(calee->arg_size() == stmt.args->_fields.size());

    std::vector<llvm::Value*> args;
    args.reserve(calee->arg_size());
    for (ast::Expr const& field : stmt.args->_fields) {
      llvm::Value* val = eval(field);
      assert(val != nullptr);
      args.push_back(val);
    }

    _builder.CreateCall(calee, args);
  }

  // void visitStmt(const auto& stmt) { std::terminate(); }

  llvm::Value* eval(ast::Expr const& expr) {
    return std::visit(
        overloaded{
            [this](Ptr<ast::IntegerE> const& inte) -> llvm::Value* {
              return llvm::ConstantInt::get(
                  _ctx, llvm::APInt(inte->_width, inte->_val, 10));
            },
            [this](Ptr<ast::CallE> const& expr) -> llvm::Value* {
              llvm::Function* calee = _mod.getFunction(expr->fn->pt->name());
              assert(calee);
              assert(calee->arg_size() == expr->args->_fields.size());

              std::vector<llvm::Value*> args;
              args.reserve(calee->arg_size());
              for (ast::Expr const& field : expr->args->_fields) {
                llvm::Value* val = eval(field);
                assert(val != nullptr);
                args.push_back(val);
              }

              return _builder.CreateCall(calee, args);
            },
            [this](Ptr<ast::IfE> const& expr) -> llvm::Value* {
              llvm::Value* cond =
                  _builder.CreateICmpEQ(eval(expr->_cond), _valueTrue);

              assert(_currentFunction != nullptr);
              auto* then =
                  llvm::BasicBlock::Create(_ctx, "then", _currentFunction);
              auto* els = llvm::BasicBlock::Create(_ctx, "else");
              auto* end = llvm::BasicBlock::Create(_ctx, "end");

              _builder.CreateCondBr(cond, then, els);

              _builder.SetInsertPoint(then);
              llvm::Value* thenVal = eval(std::move(expr->_then));
              _builder.CreateBr(end);

              _currentFunction->insert(_currentFunction->end(), els);
              _builder.SetInsertPoint(els);
              llvm::Value* elseVal = expr->_else != nullptr
                                         ? eval(std::move(expr->_else))
                                         : nullptr;
              _builder.CreateBr(end);

              _currentFunction->insert(_currentFunction->end(), end);
              _builder.SetInsertPoint(end);

              if (thenVal == nullptr || elseVal == nullptr) {
                return nullptr;
              }

              auto* phi = _builder.CreatePHI(thenVal->getType(), 2);

              phi->addIncoming(thenVal, then);
              phi->addIncoming(elseVal, els);

              return phi;
            },
            [this](Ptr<ast::BlockE> const& expr) -> llvm::Value* {
              for (ast::Stmt const& stmt : expr->body) {
                compile(stmt);
              }

              return expr->terminator.has_value()
                         ? eval(expr->terminator.value())
                         : nullptr;
            },
            [](auto const& /*expr*/) -> llvm::Value* { assert(false); },
        },
        expr);
  }

  llvm::Function* function(ast::Fn const& proto) {
    spdlog::info("function {}({})", proto.pt->name(), proto.params.size());
    std::vector<llvm::Type*> argTypes{};
    argTypes.reserve(proto.params.size());
    for (auto const& prm : proto.params) {
      argTypes.push_back(to_llvm_type(prm.type));
    }

    auto* funcType = llvm::FunctionType::get(proto.ret != nullptr
                                                 ? to_llvm_type(proto.ret)
                                                 : llvm::Type::getVoidTy(_ctx),
                                             argTypes, false);
    auto* func = llvm::Function::Create(
        funcType, llvm::Function::ExternalLinkage, proto.pt->name(), _mod);

    for (auto const& [arg, fnArg] :
         std::views::zip(func->args(), proto.pt->_proto.params)) {
      arg.setName(fnArg.name);
    }

    return func;
  }

  llvm::Type* to_llvm_type(ast::Type* type) {
    spdlog::info("getting type: {}", fmt::underlying(type->_kind));
    return llvm::Type::getInt32Ty(_ctx);
  }

  // private:
  llvm::LLVMContext _ctx;
  llvm::Module _mod{"my cool jit", _ctx};
  llvm::IRBuilder<> _builder{_ctx};

  llvm::Value* _valueFalse = _builder.getInt1(false);
  llvm::Value* _valueTrue = _builder.getInt1(true);

  llvm::Function* _currentFunction{};
};

};  // namespace x
