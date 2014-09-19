//===-- xpicTargetMachine.h - Define TargetMachine for xpic ---*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the xpic specific subclass of TargetMachine.
//
//===----------------------------------------------------------------------===//

#ifndef XPICTARGETMACHINE_H
#define XPICTARGETMACHINE_H

#include "xpicFrameLowering.h"
#include "xpicISelLowering.h"
#include "xpicInstrInfo.h"
#include "xpicSelectionDAGInfo.h"
#include "xpicSubtarget.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/Target/TargetFrameLowering.h"
#include "llvm/Target/TargetMachine.h"

namespace llvm {

class xpicTargetMachine : public LLVMTargetMachine {
  xpicSubtarget Subtarget;
  const DataLayout DL;       // Calculates type size & alignment
  xpicInstrInfo InstrInfo;
  xpicTargetLowering TLInfo;
  xpicSelectionDAGInfo TSInfo;
  xpicFrameLowering FrameLowering;
public:
  xpicTargetMachine(const Target &T, 
                    StringRef &TT,
                    StringRef CPU,
                    StringRef FS,
                    const TargetOptions &Options,
                    Reloc::Model RM,
                    CodeModel::Model CM,
                    CodeGenOpt::Level OL);

  virtual const xpicInstrInfo         *getInstrInfo() const { return &InstrInfo; }
  virtual const TargetFrameLowering   *getFrameLowering() const { return &FrameLowering; }
  virtual const xpicSubtarget         *getSubtargetImpl() const{ return &Subtarget; }
  virtual const xpicRegisterInfo      *getRegisterInfo() const { return &InstrInfo.getRegisterInfo(); }
  virtual const xpicTargetLowering    *getTargetLowering() const { return &TLInfo; }
  virtual const xpicSelectionDAGInfo  *getSelectionDAGInfo() const { return &TSInfo; }
  virtual const DataLayout            *getDataLayout() const { return &DL; }
  static unsigned                     getModuleMatchQuality(const Module &M);

  // Pass Pipeline Configuration
  virtual TargetPassConfig *createPassConfig( PassManagerBase &PM );


 };
} // end namespace llvm

#endif
