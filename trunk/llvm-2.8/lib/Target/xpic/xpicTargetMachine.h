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

#ifndef xpicTARGETMACHINE_H
#define xpicTARGETMACHINE_H

#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetData.h"
#include "llvm/Target/TargetFrameInfo.h"
#include "xpicInstrInfo.h"
#include "xpicSubtarget.h"
#include "xpicISelLowering.h"
#include "xpicSelectionDAGInfo.h"

namespace llvm {

class xpicTargetMachine : public LLVMTargetMachine {
  const TargetData DataLayout;       // Calculates type size & alignment
  xpicSubtarget Subtarget;
  xpicTargetLowering TLInfo;
  xpicSelectionDAGInfo TSInfo;
  xpicInstrInfo InstrInfo;
  TargetFrameInfo FrameInfo;

public:
  xpicTargetMachine(const Target &T, const std::string &TT,const std::string &FS);

  virtual const xpicInstrInfo *getInstrInfo() const { return &InstrInfo; }
  virtual const TargetFrameInfo  *getFrameInfo() const { return &FrameInfo; }
  virtual const xpicSubtarget   *getSubtargetImpl() const{ return &Subtarget; }
  virtual const xpicRegisterInfo *getRegisterInfo() const {
    return &InstrInfo.getRegisterInfo();
  }
  virtual const xpicTargetLowering* getTargetLowering() const {
    return const_cast<xpicTargetLowering*>(&TLInfo);
  }
  virtual const xpicSelectionDAGInfo* getSelectionDAGInfo() const {
    return &TSInfo;
  }
  virtual const TargetData       *getTargetData() const { return &DataLayout; }
  static unsigned getModuleMatchQuality(const Module &M);

  // Pass Pipeline Configuration
  virtual bool addInstSelector(PassManagerBase &PM, CodeGenOpt::Level OptLevel);
  virtual bool addPreEmitPass(PassManagerBase &PM, CodeGenOpt::Level OptLevel);
 };
} // end namespace llvm

#endif
