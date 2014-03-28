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

#include "xpicMCAsmInfo.h"
#include "xpicTargetMachine.h"
#include "xpic.h"
#include "llvm/Module.h"
#include "llvm/PassManager.h"
#include "llvm/Target/TargetRegistry.h"

using namespace llvm;
/// xpicTargetMachineModule - Note that this is used on hosts that
/// cannot link in a library unless there are references into the
/// library.  In particular, it seems that it is not possible to get
/// things to work on Win32 without this.  Though it is unused, do not
/// remove it.
extern "C" int xpicTargetMachineModule;
int xpicTargetMachineModule = 0;


extern "C" void LLVMInitializexpicTarget() {
  // Register the target.
  RegisterTargetMachine<xpicTargetMachine> X(TheXpicTarget);
  RegisterAsmInfo<xpicELFMCAsmInfo> Y(TheXpicTarget);
}
/// xpicTargetMachine ctor - Create an architecture model
///
xpicTargetMachine::xpicTargetMachine(const Target &T, const std::string &TT, const std::string &FS)
  : LLVMTargetMachine(T, TT),
    DataLayout("e-p:32:32-i32:32"),
    Subtarget(TT, FS), TLInfo(*this), InstrInfo(Subtarget),
    FrameInfo(TargetFrameInfo::StackGrowsDown, 4, 0) {
}


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
}

bool xpicTargetMachine::addInstSelector(PassManagerBase &PM, CodeGenOpt::Level OptLevel) {
  PM.add(createxpicISelDag(*this, OptLevel));
  return false;
}

/// addPreEmitPass - This pass may be implemented by targets that want to run
/// passes immediately before machine code is emitted.  This should return
/// true if -print-machineinstrs should print out the code after the passes.
bool xpicTargetMachine::addPreEmitPass(PassManagerBase &PM, CodeGenOpt::Level OptLevel) {
//  PM.add(createxpicFPMoverPass(*this));
  PM.add(createxpicDelaySlotFillerPass(*this));
  return true;
}
