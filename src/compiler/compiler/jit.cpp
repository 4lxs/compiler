#include "jit.hpp"

namespace x {

Jit::Jit(std::unique_ptr<llvm::orc::ExecutionSession> execSess,
         llvm::orc::JITTargetMachineBuilder jtmb, llvm::DataLayout &&layout)
    : _execSess(std::move(execSess)),
      _dataLayout(layout),
      _mangle(*this->_execSess, this->_dataLayout),
      _objLayer(
          *this->_execSess,
          []() { return std::make_unique<llvm::SectionMemoryManager>(); }),
      _compileLayer(
          *this->_execSess, _objLayer,
          std::make_unique<llvm::orc::ConcurrentIRCompiler>(std::move(jtmb))),
      _mainJD(this->_execSess->createBareJITDylib("<main>")) {
  _mainJD.addGenerator(
      cantFail(llvm::orc::DynamicLibrarySearchGenerator::GetForCurrentProcess(
          layout.getGlobalPrefix())));
  if (jtmb.getTargetTriple().isOSBinFormatCOFF()) {
    _objLayer.setOverrideObjectFlagsWithResponsibilityFlags(true);
    _objLayer.setAutoClaimResponsibilityForObjectSymbols(true);
  }
}

Jit::~Jit() {
  if (auto Err = _execSess->endSession()) {
    _execSess->reportError(std::move(Err));
  }
}

llvm::Expected<std::unique_ptr<Jit>> Jit::Create() {
  auto EPC = llvm::orc::SelfExecutorProcessControl::Create();
  if (!EPC) {
    return EPC.takeError();
  }

  auto execSess =
      std::make_unique<llvm::orc::ExecutionSession>(std::move(*EPC));

  llvm::orc::JITTargetMachineBuilder JTMB(
      execSess->getExecutorProcessControl().getTargetTriple());

  auto layout = JTMB.getDefaultDataLayoutForTarget();
  if (!layout) {
    return layout.takeError();
  }

  return std::make_unique<Jit>(std::move(execSess), std::move(JTMB),
                               std::move(*layout));
}

llvm::Error Jit::addModule(llvm::orc::ThreadSafeModule tsm,
                           llvm::orc::ResourceTrackerSP rsrcTracker) {
  if (!rsrcTracker) {
    rsrcTracker = _mainJD.getDefaultResourceTracker();
  }
  return _compileLayer.add(rsrcTracker, std::move(tsm));
}

llvm::Expected<llvm::orc::ExecutorSymbolDef> Jit::lookup(llvm::StringRef name) {
  return _execSess->lookup({&_mainJD}, _mangle(name.str()));
}

}  // namespace x
