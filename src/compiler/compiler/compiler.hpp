#pragma once

#include <fmt/format.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Value.h>
#include <llvm/IR/Verifier.h>
#include <spdlog/spdlog.h>

#include "x/ast/expr.hpp"
#include "x/ast/stmt.hpp"
#include "x/ast/type.hpp"

namespace x {

class Compiler {
 public:
  /// a wrapper over llvm::Value* to provide additional information
  struct Value {
    operator bool() const { return _llvmV != nullptr; }

    [[nodiscard]] bool isPtr() const { return _derefType != nullptr; }

    explicit Value(llvm::Value* llvmVal, llvm::Type* derefType = nullptr)
        : _llvmV(llvmVal), _derefType{derefType} {}
    Value() = default;

    llvm::Value* _llvmV{};

    /// if this is a pointer, this is the type of the value it points
    llvm::Type* _derefType{};
  };

  llvm::Value* get_deref(Value const& value) {
    if (!value.isPtr() || value._llvmV == nullptr) {
      return value._llvmV;
    }
    return _builder.CreateLoad(value._derefType, value._llvmV);
  }

  void compile(ast::Context const& context) {
    for (Rc<ast::Type> const& type : context._types) {
      to_llvm_type(*type);
    }

    spdlog::info("compiling functions {}", context._functions.size());
    for (Rc<ast::FnDecl> const& fnctn : context._functions) {
      spdlog::info("compiling function {} with name {}", fmt::ptr(fnctn),
                   fnctn->name());
      // we need to forward declare all functions
      function(*fnctn);
    }

    for (Rc<ast::FnDecl> const& fnctn : context._functions) {
      llvm::Function* func = _mod.getFunction(fnctn->name());
      spdlog::info("compiling function {}", fnctn->name());
      assert(func != nullptr);
      _currentFunction = func;
      llvm::BasicBlock* body = llvm::BasicBlock::Create(_ctx, "entry", func);
      _builder.SetInsertPoint(body);

      for (Rc<ast::Decl> const& vardecl : fnctn->_localvars) {
        auto& var = llvm::cast<ast::VarDecl>(*vardecl);
        llvm::AllocaInst* allocaInst =
            _builder.CreateAlloca(var.type()->_llvmType, nullptr, var.name());
        var._alloca = allocaInst;
      }

      spdlog::info("body");
      for (Ptr<ast::Stmt> const& stmt : fnctn->_block->_body) {
        spdlog::info("stmt");
        compile(*stmt);
      }

      spdlog::info("function {}", fmt::ptr(func));
      if (llvm::verifyFunction(*func, &llvm::errs())) {
        _mod.print(llvm::outs(), nullptr);
        std::terminate();
      }
    }

    _currentFunction = nullptr;
  };

  void compile(ast::Stmt& stmt) {
    if (stmt.is_expr()) {
      eval(llvm::cast<ast::Expr>(&stmt));
      return;
    }
    switch (stmt.get_kind()) {
      case ast::Stmt::SK_Return: {
        auto const& ret = llvm::cast<ast::Return>(stmt);
        spdlog::info("ret");
        _builder.CreateRet(get_deref(eval(ret._val.get())));
      } break;
        // case ast::Stmt::SK_Int:
        // case ast::Stmt::SK_Bool:
        // case ast::Stmt::SK_String:
        // case ast::Stmt::SK_Struct:
        // case ast::Stmt::SK_If:
        // case ast::Stmt::SK_Call:
        // case ast::Stmt::SK_VarRef:
        // case ast::Stmt::SK_Block:
        //   eval(stmt);
        //   break;
        // case ast::Stmt::SK_Function:
        //   // all functions should live in context._functions
        // case ast::Stmt::SK_Expr:
        // case ast::Stmt::SK_ExprEnd:
        //   throw std::runtime_error("unexpected stmt");
        // case ast::Stmt::SK_Builtin:
        //   break;
        // case ast::Stmt::SK_VarDecl: {
        //   auto* decl = llvm::cast<ast::VarDecl>(stmt);
        //   llvm::AllocaInst* allocaInst = _builder.CreateAlloca(
        //       decl->type()->_llvmType, nullptr, decl->name());
        //   decl->_alloca = allocaInst;
        // } break;
        // case ast::Stmt::SK_FieldAccess: {
        //   assert(false);
        // } break;
        // case ast::Stmt::SK_While:
        //   auto* ass = llvm::cast<ast::While>(stmt);
        //
        //   auto* cond = llvm::BasicBlock::Create(_ctx, "wCond",
        //   _currentFunction); auto* body = llvm::BasicBlock::Create(_ctx,
        //   "wBody"); auto* end = llvm::BasicBlock::Create(_ctx, "wEnd");
        //
        //   _builder.CreateBr(cond);
        //
        //   _builder.SetInsertPoint(cond);
        //   Value condVal = eval(ass->_cond);
        //   _builder.CreateCondBr(get_deref(condVal), body, end);
        //
        //   _builder.SetInsertPoint(body);
        //   _currentFunction->insert(_currentFunction->end(), body);
        //   compile(ass->_body);
        //   _builder.CreateBr(cond);
        //
        //   _currentFunction->insert(_currentFunction->end(), end);
        //   _builder.SetInsertPoint(end);
        //   break;
      default:
        xerr("unimplemented stmt compile {}", fmt::underlying(stmt.get_kind()));
    }
  }

  /// @param expr required to be an expression. that is kind between ExprBegin
  /// and ExprEnd
  Value eval(ast::Expr* exprptr) {
    if (exprptr == nullptr) {
      return {};
    }
    ast::Expr& expr = *exprptr;

    switch (expr.get_kind()) {
      case ast::Stmt::SK_Int: {
        auto const& intlit = llvm::cast<ast::IntegerLiteral>(expr);
        return Value{llvm::ConstantInt::get(_ctx, intlit._apint)};
      }
      case ast::Stmt::SK_VarRef: {
        auto const& declRef = llvm::cast<ast::VarRef>(expr);
        llvm::AllocaInst* allocaInst = declRef._decl->_alloca;
        assert(allocaInst != nullptr);
        return Value{allocaInst, allocaInst->getAllocatedType()};
      }
      case ast::Stmt::SK_If: {
        auto const& ife = llvm::cast<ast::If>(expr);
        llvm::Value* cond = get_deref(eval(ife._cond.get()));

        assert(_currentFunction != nullptr);
        auto* then = llvm::BasicBlock::Create(_ctx, "then", _currentFunction);
        auto* els = llvm::BasicBlock::Create(_ctx, "else");
        auto* end = llvm::BasicBlock::Create(_ctx, "end");

        _builder.CreateCondBr(cond, then, els);

        _builder.SetInsertPoint(then);
        Value thenVal = eval(ife._then.get());
        _builder.CreateBr(end);

        _currentFunction->insert(_currentFunction->end(), els);
        _builder.SetInsertPoint(els);
        Value elseVal = eval(ife._else.get());
        _builder.CreateBr(end);

        _currentFunction->insert(_currentFunction->end(), end);
        _builder.SetInsertPoint(end);

        if (!thenVal || !elseVal) {
          return {};
        }

        auto* phi = _builder.CreatePHI(get_deref(thenVal)->getType(), 2);

        phi->addIncoming(get_deref(thenVal), then);
        phi->addIncoming(get_deref(elseVal), els);

        return Value{phi};
      }
      case ast::Stmt::SK_Builtin: {
        auto const& builtin = llvm::cast<ast::Builtin>(expr);
        switch (builtin._op) {
          case ast::Builtin::Op::iAdd:
            return Value{
                _builder.CreateAdd(get_deref(eval(builtin._args.at(0).get())),
                                   get_deref(eval(builtin._args.at(1).get())))};
          case ast::Builtin::Op::iLess:
            return Value{_builder.CreateICmpULT(
                get_deref(eval(builtin._args.at(0).get())),
                get_deref(eval(builtin._args.at(1).get())))};
          case ast::Builtin::Op::Start2:
          case ast::Builtin::Op::Start3:
            throw std::runtime_error("invalid builtin");
          case ast::Builtin::Op::iSub:
            return Value{
                _builder.CreateSub(get_deref(eval(builtin._args.at(0).get())),
                                   get_deref(eval(builtin._args.at(1).get())))};
          case ast::Builtin::Op::iMul:
            return Value{
                _builder.CreateMul(get_deref(eval(builtin._args.at(0).get())),
                                   get_deref(eval(builtin._args.at(1).get())))};
          case ast::Builtin::Op::iDiv:
            return Value{_builder.CreateUDiv(
                get_deref(eval(builtin._args.at(0).get())),
                get_deref(eval(builtin._args.at(1).get())))};
          case ast::Builtin::Op::iGreater:
            return Value{_builder.CreateICmpUGT(
                get_deref(eval(builtin._args.at(0).get())),
                get_deref(eval(builtin._args.at(1).get())))};
          case ast::Builtin::Op::Assign:
            Value lhs = eval(builtin._args.at(0).get());
            Value val = eval(builtin._args.at(1).get());
            assert(lhs.isPtr());

            _builder.CreateStore(get_deref(val), lhs._llvmV);
            return {};
        }
      }
      case ast::Stmt::SK_Block: {
        auto const& block = llvm::cast<ast::Block>(expr);
        for (Ptr<ast::Stmt> const& stmt : block._body) {
          compile(*stmt);
        }

        return Value{};
      }
      default:
        xerr("unhandled expr {}", fmt::underlying(expr.get_kind()));
        // case ast::Stmt::SK_Bool:
        // case ast::Stmt::SK_String:
        // case ast::Stmt::SK_Struct:
        //   assert(false);
        // case ast::Stmt::SK_Call: {
        //   auto const& call = llvm::cast<ast::FnCall>(expr);
        //   llvm::Function* calee = _mod.getFunction(call->_fn->name());
        //   spdlog::info("calling function {} with {} params with {} args",
        //                call->_fn->name(), calee->arg_size(),
        //                call->_args->_fields.size());
        //   assert(calee);
        //   assert(calee->arg_size() == call->_args->_fields.size());
        //
        //   std::vector<llvm::Value*> args;
        //   args.reserve(calee->arg_size());
        //   for (ast::Expr* const& field : call->_args->_fields) {
        //     llvm::Value* val = get_deref(eval(field));
        //     assert(val != nullptr);
        //     args.push_back(val);
        //   }
        //
        //   return Value{_builder.CreateCall(calee, args)};
        // } break;
        // case ast::Stmt::SK_FieldAccess: {
        //   auto const& fieldAccess = llvm::cast<ast::FieldAccess>(expr);
        //   spdlog::info("accessing field {} of struct {}",
        //                fieldAccess->_field->name(),
        //                fieldAccess->_base->type()->name());
        //   Value base = eval(fieldAccess->_base);
        //   assert(base.isPtr());
        //
        //   llvm::Value* ptr = _builder.CreateStructGEP(
        //       fieldAccess->_base->type()->_llvmType, base._llvmV, 0);
        //
        //   return Value{ptr, fieldAccess->type()->_llvmType};
        // }
        // case ast::Stmt::SK_Expr:
        //   // expr is used for void
        //   return Value{llvm::UndefValue::get(llvm::Type::getVoidTy(_ctx))};
        // case ast::Stmt::SK_Return:
        // case ast::Stmt::SK_Function:
        // case ast::Stmt::SK_VarDecl:
        // case ast::Stmt::SK_Assign:
        // case ast::Stmt::SK_ExprEnd:
        // case ast::Stmt::SK_While:
        //   throw std::runtime_error(
        //       fmt::format("unexpected stmt {} when expression was expected",
        //                   fmt::underlying(expr->get_kind())));
    }
  }

  llvm::Function* function(ast::FnDecl const& proto) {
    spdlog::info("function {}({})", proto.name(), proto._params.size());
    std::vector<llvm::Type*> argTypes{};
    argTypes.reserve(proto._params.size());
    for (auto const& prm : proto._params) {
      argTypes.push_back(prm.type->_llvmType);
    }

    spdlog::info("getting ret from {}", fmt::ptr(proto._ret));
    llvm::Type* retType = proto._ret != nullptr ? proto._ret->_llvmType
                                                : llvm::Type::getVoidTy(_ctx);
    spdlog::info("return type: {} from {}", fmt::ptr(retType),
                 fmt::ptr(proto._ret->_llvmType));

    auto* funcType = llvm::FunctionType::get(retType, argTypes, false);
    auto* func = llvm::Function::Create(
        funcType, llvm::Function::ExternalLinkage, proto.name(), _mod);

    // for (auto const& [arg, fnArg] :
    //      std::views::zip(func->args(), proto._params)) {
    //   arg.setName(fnArg.name);
    // }

    return func;
  }

  llvm::Type* to_llvm_type(ast::Type& type) {
    spdlog::info("getting type: {}", type.name());

    if (type._llvmType != nullptr) {
      return type._llvmType;
    }

    switch (type.get_kind()) {
      // case ast::Type::DeclKind::Struct: {
      //   auto* structTy = llvm::cast<ast::StructTy>(type);
      //   // structTy->_fields
      //
      //   std::vector<llvm::Type*> fields;
      //   fields.reserve(structTy->_fields.size());
      //
      //   for (ast::FieldDecl* const& field : structTy->_fields) {
      //     fields.push_back(to_llvm_type(field->type()));
      //   }
      //   type->_llvmType =
      //       llvm::StructType::create(_ctx, fields, structTy->name());
      //
      //   return type->_llvmType;
      // }
      case ast::Type::DeclKind::Literal: {
        auto& litTy = llvm::cast<ast::LiteralTy>(type);
        switch (litTy._litkind) {
          case ast::LiteralTy::Kind::I32:
            return litTy._llvmType = llvm::Type::getInt32Ty(_ctx);
        }
      }
      default:
        xerr("unhandled type {}", fmt::underlying(type.get_kind()));
    }
  }

  // private:
  llvm::LLVMContext _ctx;
  llvm::Module _mod{"my cool jit", _ctx};
  llvm::IRBuilder<> _builder{_ctx};

  llvm::Function* _currentFunction{};

  struct CurrBlock {
  } _currentBlock;
};
};  // namespace x
