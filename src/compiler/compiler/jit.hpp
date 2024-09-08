#pragma once

#include <llvm/ADT/StringRef.h>
#include <llvm/ExecutionEngine/Orc/CompileUtils.h>
#include <llvm/ExecutionEngine/Orc/Core.h>
#include <llvm/ExecutionEngine/Orc/ExecutionUtils.h>
#include <llvm/ExecutionEngine/Orc/ExecutorProcessControl.h>
#include <llvm/ExecutionEngine/Orc/IRCompileLayer.h>
#include <llvm/ExecutionEngine/Orc/JITTargetMachineBuilder.h>
#include <llvm/ExecutionEngine/Orc/Mangling.h>
#include <llvm/ExecutionEngine/Orc/RTDyldObjectLinkingLayer.h>
#include <llvm/ExecutionEngine/Orc/Shared/ExecutorSymbolDef.h>
#include <llvm/ExecutionEngine/SectionMemoryManager.h>
#include <llvm/IR/DataLayout.h>
#include <llvm/IR/LLVMContext.h>

#include <memory>

namespace x {

class Jit {
 public:
  Jit(std::unique_ptr<llvm::orc::ExecutionSession> execSess,
      llvm::orc::JITTargetMachineBuilder jtmb, llvm::DataLayout &&layout);
  Jit(const Jit &) = delete;
  Jit(Jit &&) = delete;
  Jit &operator=(const Jit &) = delete;
  Jit &operator=(Jit &&) = delete;
  ~Jit();

  static llvm::Expected<std::unique_ptr<Jit>> Create();

  const llvm::DataLayout &getDataLayout() const { return _dataLayout; }

  llvm::orc::JITDylib &getMainJITDylib() { return _mainJD; }

  llvm::Error addModule(llvm::orc::ThreadSafeModule tsm,
                        llvm::orc::ResourceTrackerSP rsrcTracker = nullptr);

  llvm::Expected<llvm::orc::ExecutorSymbolDef> lookup(llvm::StringRef name);

 private:
  std::unique_ptr<llvm::orc::ExecutionSession> _execSess;

  llvm::DataLayout _dataLayout;
  llvm::orc::MangleAndInterner _mangle;

  llvm::orc::RTDyldObjectLinkingLayer _objLayer;
  llvm::orc::IRCompileLayer _compileLayer;

  llvm::orc::JITDylib &_mainJD;
};

}  // namespace x
