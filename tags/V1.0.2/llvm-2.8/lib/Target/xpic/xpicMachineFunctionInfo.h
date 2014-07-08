//===- xpicMachineFunctionInfo.h - xpic Machine Function Info -*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares  xpic specific per-machine-function information.
//
//===----------------------------------------------------------------------===//
#ifndef XPICMACHINEFUNCTIONINFO_H
#define XPICMACHINEFUNCTIONINFO_H

#include "llvm/CodeGen/MachineFunction.h"

namespace llvm {

  class xpicMachineFunctionInfo : public MachineFunctionInfo {
  private:
    unsigned GlobalBaseReg;

    /// VarArgsFrameOffset - Frame offset to start of varargs area.
    int VarArgsFrameOffset;

  public:
    xpicMachineFunctionInfo() : GlobalBaseReg(0), VarArgsFrameOffset(0) {}
    explicit xpicMachineFunctionInfo(MachineFunction &MF)
      : GlobalBaseReg(0), VarArgsFrameOffset(0) {}

    unsigned getGlobalBaseReg() const { return GlobalBaseReg; }
    void setGlobalBaseReg(unsigned Reg) { GlobalBaseReg = Reg; }

    int getVarArgsFrameOffset() const { return VarArgsFrameOffset; }
    void setVarArgsFrameOffset(int Offset) { VarArgsFrameOffset = Offset; }
  };
}

#endif
