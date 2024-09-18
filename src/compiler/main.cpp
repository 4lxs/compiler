#include <XLexer.h>
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
#include <spdlog/spdlog.h>
#include <tree/IterativeParseTreeWalker.h>
#include <tree/ParseTreeVisitor.h>

#include <fstream>
#include <iostream>
#include <string_view>

#include "antlr/exceptionErrorListener.hpp"
#include "antlr/visitor.hpp"
#include "compiler/compiler.hpp"
#include "handroll.hpp"
#include "x/ast/context.hpp"
#include "x/ast/type.hpp"
#include "x/pt/context.hpp"
#include "x/sema/sema.hpp"

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
  llvm::Target const *Target =
      llvm::TargetRegistry::lookupTarget(TargetTriple, Error);

  if (Target == nullptr) {
    llvm::errs() << Error;
    std::terminate();
  }
  std::string_view const CPU{"generic"};
  std::string_view const Features{};

  llvm::TargetOptions opt;
  llvm::TargetMachine *TargetMachine = Target->createTargetMachine(
      TargetTriple, CPU, Features, opt, llvm::Reloc::PIC_);

  module.setDataLayout(TargetMachine->createDataLayout());
  module.setTargetTriple(TargetTriple);

  std::string_view const Filename = "output.o";
  std::error_code errcode;
  llvm::raw_fd_ostream dest(Filename, errcode, llvm::sys::fs::OF_None);

  if (errcode) {
    llvm::errs() << "Could not open file: " << errcode.message();
    std::terminate();
  }

  llvm::legacy::PassManager pass;
  auto FileType = llvm::CodeGenFileType::ObjectFile;

  if (TargetMachine->addPassesToEmitFile(pass, dest, nullptr, FileType)) {
    llvm::errs() << "TargetMachine can't emit a file of this type";
    std::terminate();
  }

  pass.run(module);
  dest.flush();
}

void compile(std::string_view filename) {
  auto parsetree = pt::Context::Create();

  Visitor visitor(parsetree.get());
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
    std::terminate();
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

  sema::Sema anal(parsetree.get());

  Ptr<ast::Context> ast = anal.finish();

  Compiler compiler;

  compiler.compile(*ast);

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
