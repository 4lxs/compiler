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
#include <utility>
#include <variant>

#include "x/ast/expr.hpp"
#include "x/ast/stmt.hpp"
#include "x/ast/type.hpp"
#include "x/common.hpp"
#include "x/pt/expr.hpp"
#include "x/pt/module.hpp"
#include "x/pt/stmt.hpp"
#include "x/pt/type.hpp"

namespace x {

class Compiler {
 public:
  void compile(ast::Context const& context) {
    for (auto const& fnctn : context._functions) {
      spdlog::info("compiling function {}", fmt::ptr(fnctn));
      // we need to forward declare all functions
      function(*fnctn);
    }

    for (auto const& fnctn : context._functions) {
      llvm::Function* func = _mod.getFunction(fnctn->name());
      _currentFunction = func;
      llvm::BasicBlock* body = llvm::BasicBlock::Create(_ctx, "entry", func);
      _builder.SetInsertPoint(body);

      spdlog::info("body");
      for (auto const& stmt : fnctn->_block->_body) {
        spdlog::info("stmt");
        compile(*stmt);
      }
      if (llvm::verifyFunction(*func, &llvm::errs())) {
        std::terminate();
      }
    }

    _currentFunction = nullptr;
  };

  void compile(ast::Stmt const& stmt) {
    switch (stmt.get_kind()) {
      case ast::Stmt::SK_Return: {
        auto const& ret = llvm::cast<ast::Return>(stmt);
        spdlog::info("ret");
        _builder.CreateRet(ret._val != nullptr ? eval(*ret._val) : nullptr);
      } break;
      case ast::Stmt::SK_Int:
      case ast::Stmt::SK_Bool:
      case ast::Stmt::SK_String:
      case ast::Stmt::SK_Struct:
      case ast::Stmt::SK_If:
      case ast::Stmt::SK_Call:
      case ast::Stmt::SK_Block:
        eval(stmt);
        break;
      case ast::Stmt::SK_Function:
        // all functions should live in context._functions
      case ast::Stmt::SK_ExprBegin:
      case ast::Stmt::SK_ExprEnd:
        std::unreachable();
    }
  }
  //
  // void visitStmt(const std::unique_ptr<pt::Expr>& expr) {
  //   llvm::Value* val = eval(*expr);
  //   if (val != nullptr) {
  //     spdlog::warn("unused value");
  //   }
  // }

  void visitStmt(ast::Expr const& expr) {
    spdlog::info("expr");
    eval(expr);
  }

  // void visitStmt(const auto& stmt) { std::terminate(); }

  /// @param expr required to be an expression. that is kind between ExprBegin
  /// and ExprEnd
  llvm::Value* eval(ast::Stmt const& expr) {
    switch (expr.get_kind()) {
      case ast::Stmt::SK_Int: {
        auto const& inte = llvm::cast<ast::IntegerLiteral>(expr);
        return llvm::ConstantInt::get(_ctx,
                                      llvm::APInt(inte._width, inte._val, 10));
      } break;
      case ast::Stmt::SK_Bool:
      case ast::Stmt::SK_String:
      case ast::Stmt::SK_Struct:
        assert(false);
      case ast::Stmt::SK_If: {
        auto const& ife = llvm::cast<ast::If>(expr);
        llvm::Value* cond = eval(*ife._cond);

        assert(_currentFunction != nullptr);
        auto* then = llvm::BasicBlock::Create(_ctx, "then", _currentFunction);
        auto* els = llvm::BasicBlock::Create(_ctx, "else");
        auto* end = llvm::BasicBlock::Create(_ctx, "end");

        _builder.CreateCondBr(cond, then, els);

        _builder.SetInsertPoint(then);
        llvm::Value* thenVal = eval(*ife._then);
        _builder.CreateBr(end);

        _currentFunction->insert(_currentFunction->end(), els);
        _builder.SetInsertPoint(els);
        llvm::Value* elseVal =
            ife._else != nullptr ? eval(*ife._else) : nullptr;
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
      } break;
      case ast::Stmt::SK_Call: {
        auto const& call = llvm::cast<ast::FnCall>(expr);
        llvm::Function* calee = _mod.getFunction(call._fn->name());
        assert(calee);
        assert(calee->arg_size() == call._args->_fields.size());

        std::vector<llvm::Value*> args;
        args.reserve(calee->arg_size());
        for (ast::Expr* const& field : call._args->_fields) {
          llvm::Value* val = eval(*field);
          assert(val != nullptr);
          args.push_back(val);
        }

        return _builder.CreateCall(calee, args);
      } break;
      case ast::Stmt::SK_Block: {
        auto const& block = llvm::cast<ast::Block>(expr);
        for (ast::Stmt* stmt : block._body) {
          compile(*stmt);
        }

        return block.terminator != nullptr ? eval(*block.terminator) : nullptr;
      } break;
      case ast::Stmt::SK_Builtin: {
        auto const& builtin = llvm::cast<ast::Builtin>(expr);
        switch (builtin._op) {
          case ast::Builtin::Op::iAdd:
            return _builder.CreateAdd(eval(*builtin._args[0]),
                                      eval(*builtin._args[1]));
          case ast::Builtin::Op::iLess:
            return _builder.CreateICmpULT(eval(*builtin._args[0]),
                                          eval(*builtin._args[1]));
          case ast::Builtin::Op::Start2:
          case ast::Builtin::Op::Start3:
            std::unreachable();
        }
      } break;
      default:
        std::unreachable();
    }
  }

  llvm::Function* function(ast::Fn const& proto) {
    spdlog::info("function {}({})", proto.name(), proto._params.size());
    std::vector<llvm::Type*> argTypes{};
    argTypes.reserve(proto._params.size());
    for (auto const& prm : proto._params) {
      argTypes.push_back(to_llvm_type(prm.type));
    }

    auto* funcType = llvm::FunctionType::get(proto._ret != nullptr
                                                 ? to_llvm_type(proto._ret)
                                                 : llvm::Type::getVoidTy(_ctx),
                                             argTypes, false);
    auto* func = llvm::Function::Create(
        funcType, llvm::Function::ExternalLinkage, proto.name(), _mod);

    for (auto const& [arg, fnArg] :
         std::views::zip(func->args(), proto._params)) {
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
