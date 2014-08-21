//===-- xpicMCTargetDesc.cpp - Xpic Target Descriptions -----------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file provides Xpic specific target descriptions.
//
//===----------------------------------------------------------------------===//

#include "xpicMCTargetDesc.h"
#include "xpicMCAsmInfo.h"
#include "llvm/MC/MCCodeGenInfo.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/TargetRegistry.h"

#define GET_INSTRINFO_MC_DESC
#include "xpicGenInstrInfo.inc"

#define GET_SUBTARGETINFO_MC_DESC
#include "xpicGenSubtargetInfo.inc"

#define GET_REGINFO_MC_DESC
#include "xpicGenRegisterInfo.inc"

using namespace llvm;

static MCInstrInfo *createXpicMCInstrInfo() {
  MCInstrInfo *X = new MCInstrInfo();
  InitxpicMCInstrInfo(X);
  return X;
}

static MCRegisterInfo *createXpicMCRegisterInfo(StringRef TT) {
  MCRegisterInfo *X = new MCRegisterInfo();
  InitxpicMCRegisterInfo(X, XPIC::r7);
  return X;
}

static MCSubtargetInfo *createXpicMCSubtargetInfo(StringRef TT, StringRef CPU,
                                                   StringRef FS) {
  MCSubtargetInfo *X = new MCSubtargetInfo();
  InitxpicMCSubtargetInfo(X, TT, CPU, FS);
  return X;
}

static MCCodeGenInfo *createXpicMCCodeGenInfo(StringRef TT, Reloc::Model RM,
                                               CodeModel::Model CM,
                                               CodeGenOpt::Level OL) {
  MCCodeGenInfo *X = new MCCodeGenInfo();

  // The default 32-bit code model is abs32/pic32.
  if (CM == CodeModel::Default)
    CM = RM == Reloc::PIC_ ? CodeModel::Medium : CodeModel::Small;

  X->InitMCCodeGenInfo(RM, CM, OL);
  return X;
}

extern "C" void LLVMInitializexpicTargetMC() {
  // Register the MC asm info.
  RegisterMCAsmInfo<xpicELFMCAsmInfo> X(TheXpicTarget);

  // Register the MC codegen info.
  TargetRegistry::RegisterMCCodeGenInfo(TheXpicTarget, createXpicMCCodeGenInfo);

  // Register the MC instruction info.
  TargetRegistry::RegisterMCInstrInfo(TheXpicTarget, createXpicMCInstrInfo);

  // Register the MC register info.
  TargetRegistry::RegisterMCRegInfo(TheXpicTarget, createXpicMCRegisterInfo);

  // Register the MC subtarget info.
  TargetRegistry::RegisterMCSubtargetInfo(TheXpicTarget, createXpicMCSubtargetInfo);
}
