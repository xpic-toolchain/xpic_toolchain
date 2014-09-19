//===- xpicRegisterInfo.h - xpic Register Information Impl ----*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the xpic implementation of the TargetRegisterInfo class.
//
//===----------------------------------------------------------------------===//

#ifndef XPIC_REGISTERINFO_H
#define XPIC_REGISTERINFO_H

#include "llvm/Target/TargetRegisterInfo.h"

#define GET_REGINFO_HEADER
#include "xpicGenRegisterInfo.inc"

namespace llvm {

class xpicSubtarget;
class TargetInstrInfo;
class Type;

struct xpicRegisterInfo : public xpicGenRegisterInfo {
  xpicSubtarget &Subtarget;

  xpicRegisterInfo(xpicSubtarget &st);

  /// Code Generation virtual methods...  
  const uint16_t *getCalleeSavedRegs(const MachineFunction *MF = 0) const;
  
  BitVector getReservedRegs(const MachineFunction &MF) const;

  const TargetRegisterClass *getPointerRegClass(const MachineFunction &MF,
                                              unsigned Kind) const;

  void eliminateFrameIndex(MachineBasicBlock::iterator II,
                           int SPAdj, unsigned FIOperandNum,
                           RegScavenger *RS = NULL) const;
  
  unsigned getRARegister() const;

  // Debug information queries.
  unsigned getFrameRegister(const MachineFunction &MF) const;
  
};

} // end namespace llvm

#endif
