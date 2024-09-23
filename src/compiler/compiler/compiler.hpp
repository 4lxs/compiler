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
    context._int32Ty->_llvmType = llvm::Type::getInt32Ty(_ctx);
    context._int64Ty->_llvmType = llvm::Type::getInt64Ty(_ctx);
    context._voidTy->_llvmType = llvm::Type::getVoidTy(_ctx);
    context._boolTy->_llvmType = llvm::Type::getInt1Ty(_ctx);

    for (auto const& fnctn : context._functions) {
      spdlog::info("compiling function {} with name {}", fmt::ptr(fnctn),
                   fnctn->name());
      // we need to forward declare all functions
      function(*fnctn);
    }

    for (auto const& fnctn : context._functions) {
      llvm::Function* func = _mod.getFunction(fnctn->name());
      assert(func != nullptr);
      _currentFunction = func;
      llvm::BasicBlock* body = llvm::BasicBlock::Create(_ctx, "entry", func);
      _builder.SetInsertPoint(body);

      spdlog::info("body");
      for (auto const& stmt : fnctn->_block->_body) {
        spdlog::info("stmt");
        compile(stmt);
      }

      spdlog::info("function {}", fmt::ptr(func));
      if (llvm::verifyFunction(*func, &llvm::errs())) {
        _mod.print(llvm::outs(), nullptr);
        std::terminate();
      }
    }

    _currentFunction = nullptr;
  };

  void compile(ast::Stmt* stmt) {
    switch (stmt->get_kind()) {
      case ast::Stmt::SK_Return: {
        auto const& ret = llvm::cast<ast::Return>(*stmt);
        spdlog::info("ret");
        _builder.CreateRet(get_deref(eval(ret._val)));
      } break;
      case ast::Stmt::SK_Int:
      case ast::Stmt::SK_Bool:
      case ast::Stmt::SK_String:
      case ast::Stmt::SK_Struct:
      case ast::Stmt::SK_If:
      case ast::Stmt::SK_Call:
      case ast::Stmt::SK_DeclRef:
      case ast::Stmt::SK_Block:
        eval(stmt);
        break;
      case ast::Stmt::SK_Function:
        // all functions should live in context._functions
      case ast::Stmt::SK_Expr:
      case ast::Stmt::SK_ExprEnd:
        std::unreachable();
      case ast::Stmt::SK_Builtin:
        break;
      case ast::Stmt::SK_VarDecl: {
        auto* decl = llvm::cast<ast::VarDecl>(stmt);
        llvm::AllocaInst* allocaInst = _builder.CreateAlloca(
            to_llvm_type(decl->_type), nullptr, decl->name());
        decl->_alloca = allocaInst;
      } break;
      case ast::Stmt::SK_Assign: {
        auto* ass = llvm::cast<ast::Assign>(stmt);
        Value val = eval(ass->_value);
        Value lhs = eval(ass->_variable);
        assert(lhs.isPtr());

        _builder.CreateStore(get_deref(val), lhs._llvmV);
      } break;
      case ast::Stmt::SK_FieldAccess: {
        assert(false);
      } break;
    }
  }

  /// @param expr required to be an expression. that is kind between ExprBegin
  /// and ExprEnd
  Value eval(ast::Stmt* expr) {
    if (expr == nullptr) {
      return {};
    }

    switch (expr->get_kind()) {
      case ast::Stmt::SK_Int: {
        auto const& inte = llvm::cast<ast::IntegerLiteral>(expr);
        return Value{llvm::ConstantInt::get(
            _ctx, llvm::APInt(inte->_width, inte->_val, 10))};
      } break;
      case ast::Stmt::SK_Bool:
      case ast::Stmt::SK_String:
      case ast::Stmt::SK_Struct:
        assert(false);
      case ast::Stmt::SK_If: {
        auto const& ife = llvm::cast<ast::If>(expr);
        llvm::Value* cond = get_deref(eval(ife->_cond));

        assert(_currentFunction != nullptr);
        auto* then = llvm::BasicBlock::Create(_ctx, "then", _currentFunction);
        auto* els = llvm::BasicBlock::Create(_ctx, "else");
        auto* end = llvm::BasicBlock::Create(_ctx, "end");

        _builder.CreateCondBr(cond, then, els);

        _builder.SetInsertPoint(then);
        Value thenVal = eval(ife->_then);
        _builder.CreateBr(end);

        _currentFunction->insert(_currentFunction->end(), els);
        _builder.SetInsertPoint(els);
        Value elseVal = eval(ife->_else);
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
      } break;
      case ast::Stmt::SK_Call: {
        auto const& call = llvm::cast<ast::FnCall>(expr);
        llvm::Function* calee = _mod.getFunction(call->_fn->name());
        assert(calee);
        assert(calee->arg_size() == call->_args->_fields.size());

        std::vector<llvm::Value*> args;
        args.reserve(calee->arg_size());
        for (ast::Expr* const& field : call->_args->_fields) {
          llvm::Value* val = get_deref(eval(field));
          assert(val != nullptr);
          args.push_back(val);
        }

        return Value{_builder.CreateCall(calee, args)};
      } break;
      case ast::Stmt::SK_Block: {
        auto const& block = llvm::cast<ast::Block>(expr);
        for (ast::Stmt* stmt : block->_body) {
          compile(stmt);
        }

        return eval(block->terminator);
      } break;
      case ast::Stmt::SK_Builtin: {
        auto const& builtin = llvm::cast<ast::Builtin>(expr);
        switch (builtin->_op) {
          case ast::Builtin::Op::iAdd:
            spdlog::info("iAdd");
            return Value{
                _builder.CreateAdd(get_deref(eval(builtin->_args.at(0))),
                                   get_deref(eval(builtin->_args.at(1))))};
          case ast::Builtin::Op::iLess:
            return Value{
                _builder.CreateICmpULT(get_deref(eval(builtin->_args.at(0))),
                                       get_deref(eval(builtin->_args.at(1))))};
          case ast::Builtin::Op::Start2:
          case ast::Builtin::Op::Start3:
            std::unreachable();
          case ast::Builtin::Op::iSub:
            return Value{
                _builder.CreateSub(get_deref(eval(builtin->_args.at(0))),
                                   get_deref(eval(builtin->_args.at(1))))};
          case ast::Builtin::Op::iMul:
            return Value{
                _builder.CreateMul(get_deref(eval(builtin->_args.at(0))),
                                   get_deref(eval(builtin->_args.at(1))))};
          case ast::Builtin::Op::iDiv:
            return Value{
                _builder.CreateUDiv(get_deref(eval(builtin->_args.at(0))),
                                    get_deref(eval(builtin->_args.at(1))))};
          case ast::Builtin::Op::iGreater:
            return Value{
                _builder.CreateICmpUGT(get_deref(eval(builtin->_args.at(0))),
                                       get_deref(eval(builtin->_args.at(1))))};
        }
      } break;
      case ast::Stmt::SK_DeclRef: {
        auto const& declRef = llvm::cast<ast::DeclRef>(expr);
        llvm::AllocaInst* allocaInst = declRef->_decl->_alloca;
        assert(allocaInst != nullptr);
        return Value{allocaInst, allocaInst->getAllocatedType()};
      }; break;
      case ast::Stmt::SK_FieldAccess: {
        auto const& fieldAccess = llvm::cast<ast::FieldAccess>(expr);
        spdlog::info("accessing field {} of struct {}",
                     fieldAccess->_field->name(),
                     fieldAccess->_base->type()->name());
        Value base = eval(fieldAccess->_base);
        assert(base.isPtr());

        llvm::Value* ptr = _builder.CreateStructGEP(
            to_llvm_type(fieldAccess->_base->type()), base._llvmV, 0);

        return Value{ptr, to_llvm_type(fieldAccess->type())};
      }
      case ast::Stmt::SK_Return:
      case ast::Stmt::SK_Function:
      case ast::Stmt::SK_VarDecl:
      case ast::Stmt::SK_Assign:
      case ast::Stmt::SK_Expr:
      case ast::Stmt::SK_ExprEnd:
        std::unreachable();
    }
  }

  llvm::Function* function(ast::FnDecl const& proto) {
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
    spdlog::info("getting type: {}", type->name());

    if (type->_llvmType != nullptr) {
      return type->_llvmType;
    }

    switch (type->get_kind()) {
      case ast::Decl::DeclKind::StructTy: {
        auto* structTy = llvm::cast<ast::StructTy>(type);
        // structTy->_fields

        std::vector<llvm::Type*> fields;
        fields.reserve(structTy->_fields.size());

        for (ast::FieldDecl* const& field : structTy->_fields) {
          fields.push_back(to_llvm_type(field->_type));
        }
        type->_llvmType =
            llvm::StructType::create(_ctx, fields, structTy->name());

        return type->_llvmType;
      }
      case ast::Decl::DeclKind::LiteralTy:
        // all literal types are created in compile()
      case ast::Decl::DeclKind::TypeBegin:
      case ast::Decl::DeclKind::Fn:
      case ast::Decl::DeclKind::Var:
      case ast::Decl::DeclKind::Field:
      case ast::Decl::DeclKind::TypeEnd:
        std::unreachable();
    }
    return llvm::Type::getInt32Ty(_ctx);
  }

  // private:
  llvm::LLVMContext _ctx;
  llvm::Module _mod{"my cool jit", _ctx};
  llvm::IRBuilder<> _builder{_ctx};

  llvm::Function* _currentFunction{};
};
};  // namespace x
