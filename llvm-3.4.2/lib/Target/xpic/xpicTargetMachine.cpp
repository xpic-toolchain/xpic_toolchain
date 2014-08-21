//===-- xpicTargetMachine.cpp - Define TargetMachine for xpic -----------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
//
//===----------------------------------------------------------------------===//

#include "xpicTargetMachine.h"
#include "xpic.h"
#include "llvm/CodeGen/Passes.h"
//#include "llvm/IR/Module.h"
#include "llvm/PassManager.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/Target/TargetFrameLowering.h"

using namespace llvm;
/// xpicTargetMachineModule - Note that this is used on hosts that
/// cannot link in a library unless there are references into the
/// library.  In particular, it seems that it is not possible to get
/// things to work on Win32 without this.  Though it is unused, do not
/// remove it.
//extern "C" int xpicTargetMachineModule;
//int xpicTargetMachineModule = 0;

extern "C" void LLVMInitializexpicTarget() {
  // Register the target.
  RegisterTargetMachine<xpicTargetMachine> X(TheXpicTarget);
}
/// xpicTargetMachine ctor - Create an architecture model
///
xpicTargetMachine:: xpicTargetMachine(const Target &T, 
                                      StringRef &TT,
                                      StringRef CPU,
                                      StringRef FS,
                                      const TargetOptions &Options,
                                      Reloc::Model RM,
                                      CodeModel::Model CM,
                                      CodeGenOpt::Level OL)
                                      : LLVMTargetMachine(T, TT, CPU, FS, Options, RM, CM, OL),
                                        Subtarget(TT, CPU, FS),
                                        DL("e-p:32:32-i32:32"),
                                        InstrInfo(Subtarget),
                                        TLInfo(*this),
                                        TSInfo(*this),
                                        FrameLowering(Subtarget) { 
  initAsmInfo(); 
}

//old function from LLVM2.8?
/*
unsigned xpicTargetMachine::getModuleMatchQuality(const Module &M) {
  std::string TT = M.getTargetTriple();
  if (TT.size() >= 6 && std::string(TT.begin(), TT.begin()+6) == "xpic-")
    return 20;
  
  // If the target triple is something non-xpic, we don't match.
  if (!TT.empty()) return 0;

  if (M.getEndianness()  == Module::BigEndian &&
      M.getPointerSize() == Module::Pointer32)
#ifdef __xpic__
    return 20;   // BE/32 ==> Prefer xpic on xpic
#else
    return 5;    // BE/32 ==> Prefer ppc elsewhere
#endif
  else if (M.getEndianness() != Module::AnyEndianness ||
           M.getPointerSize() != Module::AnyPointerSize)
    return 0;                                    // Match for some other target

#if defined(__xpic__)
  return 10;
#else
  return 0;
#endif
}*/

namespace {
/// xpic Code Generator Pass Configuration Options.
class xpicPassConfig : public TargetPassConfig {
public:
  xpicPassConfig(xpicTargetMachine *TM, PassManagerBase &PM)
    : TargetPassConfig(TM, PM) {}

  xpicTargetMachine &getxpicTargetMachine() const {
    return getTM<xpicTargetMachine>();
  }

  virtual bool addInstSelector();
  virtual bool addPreEmitPass();
};
} // namespace

TargetPassConfig *xpicTargetMachine::createPassConfig(PassManagerBase &PM) {
  return new xpicPassConfig(this, PM);
}

bool xpicPassConfig::addInstSelector() {
  addPass(createxpicISelDag(getxpicTargetMachine(), getOptLevel()));
  return false;
}

/// addPreEmitPass - This pass may be implemented by targets that want to run
/// passes immediately before machine code is emitted.  This should return
/// true if -print-machineinstrs should print out the code after the passes.
bool xpicPassConfig::addPreEmitPass(){
  addPass(createxpicDelaySlotFillerPass(getxpicTargetMachine()));
  return true;
}

