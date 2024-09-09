#include <XParser.h>
#include <XVisitor.h>
#include <antlr4-runtime.h>
#include <fmt/core.h>
#include <llvm-c/Target.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/IR/Value.h>
#include <llvm/MC/TargetRegistry.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Target/TargetOptions.h>
#include <llvm/TargetParser/Host.h>
#include <tree/IterativeParseTreeWalker.h>
#include <tree/ParseTreeVisitor.h>

#include <fstream>
#include <iostream>
#include <string_view>

#include "XLexer.h"
#include "antlr/exceptionErrorListener.hpp"
#include "antlr/visitor.hpp"
#include "compiler/compiler.hpp"
#include "handroll.hpp"
#include "spdlog/spdlog.h"
#include "x/ast/context.hpp"

using namespace parser;
using namespace x;

void compile(llvm::Module &module) {
  spdlog::info("compiling");

  module.print(llvm::errs(), nullptr);

  auto TargetTriple = llvm::sys::getDefaultTargetTriple();

  llvm::InitializeAllTargetInfos();
  llvm::InitializeAllTargets();
  llvm::InitializeAllTargetMCs();
  llvm::InitializeAllAsmParsers();
  llvm::InitializeAllAsmPrinters();
  std::string Error;
  const llvm::Target *Target =
      llvm::TargetRegistry::lookupTarget(TargetTriple, Error);

  if (Target == nullptr) {
    llvm::errs() << Error;
    exit(1);
  }
  const std::string_view CPU{"generic"};
  const std::string_view Features{};

  llvm::TargetOptions opt;
  llvm::TargetMachine *TargetMachine = Target->createTargetMachine(
      TargetTriple, CPU, Features, opt, llvm::Reloc::PIC_);

  module.setDataLayout(TargetMachine->createDataLayout());
  module.setTargetTriple(TargetTriple);

  const std::string_view Filename = "output.o";
  std::error_code errcode;
  llvm::raw_fd_ostream dest(Filename, errcode, llvm::sys::fs::OF_None);

  if (errcode) {
    llvm::errs() << "Could not open file: " << errcode.message();
    exit(1);
  }

  llvm::legacy::PassManager pass;
  auto FileType = llvm::CodeGenFileType::ObjectFile;

  if (TargetMachine->addPassesToEmitFile(pass, dest, nullptr, FileType)) {
    llvm::errs() << "TargetMachine can't emit a file of this type";
    exit(1);
  }

  pass.run(module);
  dest.flush();
}

void compile(std::string_view filename) {
  auto ctx = ast::Context::Create();

  Visitor visitor(ctx.get());
  std::ifstream inf(filename.data());
  antlr4::ANTLRInputStream input(inf);

  XLexer lexer(&input);
  lexer.removeErrorListeners();
  ExceptionErrorListener errorListener;
  lexer.addErrorListener(&errorListener);

  antlr4::CommonTokenStream tokens(&lexer);

  tokens.fill();
  for (auto *tok : tokens.getTokens()) {
    spdlog::info("token: {}\n", tok->toString());
  }

  XParser parser(&tokens);
  parser.removeErrorListeners();
  parser.addErrorListener(&errorListener);

  antlr4::tree::ParseTree *tree = nullptr;

  // the custom made ExceptionErrorListener immediately stop
  // the parsing when an invalid expression is inserted,
  // we don't want to execute an invalid expression!
  try {
    tree = parser.program();
    // spdlog::info("parsed: {}", tree->toStringTree(&parser, true));
  } catch (antlr4::ParseCancellationException &e) {
    std::cout << e.what() << std::endl;
    exit(1);
  }

  // interpreter.setParser(&parser);

  visitor.visit(tree);
  // walker.walk(&interpreter, tree);

  // std::vector<llvm::Value *> vals = interpreter.values();
  // for (llvm::Value *val : vals) {
  //   llvm::outs() << "value: ";
  //   val->print(llvm::outs());
  //   llvm::outs() << "\n";
  // }

  auto ctxVal = ast::Context::validate(std::move(ctx));

  Compiler compiler;

  compiler.compile(*ctxVal->modules.front());

  compile(compiler._mod);
}

int main(int argc, char *argv[]) {
  if (argc > 1) {
    compile(argv[1]);
    return 0;
  }

  std::unique_ptr<llvm::Module> module = handroll();
  compile(*module);
  return 0;
}
