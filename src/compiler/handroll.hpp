#pragma once

#include <fmt/core.h>
#include <llvm-c/Target.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Value.h>
#include <llvm/IR/Verifier.h>
#include <llvm/MC/TargetRegistry.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Target/TargetOptions.h>
#include <llvm/TargetParser/Host.h>
#include <tree/IterativeParseTreeWalker.h>

#include <exception>
#include <ranges>

#include "spdlog/spdlog.h"

struct Arg {
  std::string name;
  llvm::Type* type;
};

std::unique_ptr<llvm::LLVMContext> ctx;
std::unique_ptr<llvm::Module> mod;
std::unique_ptr<llvm::IRBuilder<>> builder;

inline llvm::Function* function(std::string_view name, llvm::Type* retType,
                                std::vector<Arg> args) {
  std::vector<llvm::Type*> argTypes{};
  argTypes.reserve(args.size());
  for (const auto& arg : args) {
    argTypes.push_back(arg.type);
  }

  auto* funcType = llvm::FunctionType::get(retType, argTypes, false);
  auto* func = llvm::Function::Create(funcType, llvm::Function::ExternalLinkage,
                                      name, *mod);

  for (const auto& [arg, fnArg] : std::ranges::zip_view(func->args(), args)) {
    arg.setName(fnArg.name);
  }

  return func;
}

inline std::unique_ptr<llvm::Module> handroll() {
  ctx = std::make_unique<llvm::LLVMContext>();
  mod = std::make_unique<llvm::Module>("my cool jit", *ctx);
  builder = std::make_unique<llvm::IRBuilder<>>(*ctx);

  llvm::Type* dbl = llvm::Type::getDoubleTy(*ctx);

  auto cnst = [](double dbl) {
    return llvm::ConstantFP::get(*ctx, llvm::APFloat(dbl));
  };

  auto* main = function("main", dbl, {{"arg1", dbl}});

  llvm::BasicBlock* mainBody = llvm::BasicBlock::Create(*ctx, "entry", main);
  builder->SetInsertPoint(mainBody);

  llvm::Value* a2 = cnst(8);
  auto* cond = builder->CreateFCmpONE(a2, cnst(0), "ifcond");

  auto* then = llvm::BasicBlock::Create(*ctx, "then", main);
  auto* els = llvm::BasicBlock::Create(*ctx, "else");
  auto* end = llvm::BasicBlock::Create(*ctx, "end");

  builder->CreateCondBr(cond, then, els);

  builder->SetInsertPoint(then);
  auto* thenv = cnst(2.);
  builder->CreateBr(end);
  then = builder->GetInsertBlock();

  main->insert(main->end(), els);
  builder->SetInsertPoint(els);
  builder->CreateBr(end);

  main->insert(main->end(), end);
  builder->SetInsertPoint(end);

  auto* phi = builder->CreatePHI(dbl, 2, "iftmp");

  phi->addIncoming(thenv, then);
  phi->addIncoming(a2, els);

  builder->CreateRet(nullptr);
  spdlog::info("verify");
  if (llvm::verifyFunction(*main, &llvm::errs())) {
    std::terminate();
  }
  spdlog::info("cone handrolling");

  return std::move(mod);
}
