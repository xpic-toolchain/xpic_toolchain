//===- xpicInstrInfo.h - xpic Instruction Information ---------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the xpic implementation of the TargetInstrInfo class.
//
//===----------------------------------------------------------------------===//

#ifndef xpicINSTRUCTIONINFO_H
#define xpicINSTRUCTIONINFO_H

#include "llvm/Target/TargetInstrInfo.h"
#include "xpicRegisterInfo.h"
#include <utility>
namespace llvm {

/// XPICII - This namespace holds all of the target specific flags that
/// instruction info tracks.
/// Keep this synchronized to xpic.td::xpicInstrInfo
namespace XPICII {
  enum {
    UsesRegisterIndirectAddressing = (1<<0),
    IsCmp                          = (1<<1)
  };
}

class xpicInstrInfo : public TargetInstrInfoImpl {
  const xpicRegisterInfo RI;
  const xpicSubtarget& Subtarget;


  //pair <unsigned, unsigned> P_FI_Aaddr;
public:

  explicit xpicInstrInfo(xpicSubtarget &ST);

  /// getRegisterInfo - TargetInstrInfo is a superset of MRegister info.  As
  /// such, whenever a client has an instance of instruction info, it should
  /// always be able to get register info as well (through this method).
  ///
  virtual const xpicRegisterInfo &getRegisterInfo() const { return RI; }

  /// Return true if the instruction is a register to register move and return
  /// the source and dest operands and their sub-register indices by reference.
  virtual bool isMoveInstr(const MachineInstr &MI,
                           unsigned &SrcReg, unsigned &DstReg,
                           unsigned &SrcSubIdx, unsigned &DstSubIdx) const;
  
  /// isLoadFromStackSlot - If the specified machine instruction is a direct
  /// load from a stack slot, return the virtual or physical register number of
  /// the destination along with the FrameIndex of the loaded stack slot.  If
  /// not, return 0.  This predicate must return 0 if the instruction has
  /// any side effects other than loading from the stack slot.
  virtual unsigned isLoadFromStackSlot(const MachineInstr *MI, int &FrameIndex) const;

  /// isStoreToStackSlot - If the specified machine instruction is a direct
  /// store to a stack slot, return the virtual or physical register number of
  /// the source reg along with the FrameIndex of the loaded stack slot.  If
  /// not, return 0.  This predicate must return 0 if the instruction has
  /// any side effects other than storing to the stack slot.
  virtual unsigned isStoreToStackSlot(const MachineInstr *MI, int &FrameIndex) const;
  
  
  virtual unsigned InsertBranch(MachineBasicBlock &MBB, MachineBasicBlock *TBB,
                                MachineBasicBlock *FBB,
                            const SmallVectorImpl<MachineOperand> &Cond) const;

  virtual bool copyRegToReg(MachineBasicBlock &MBB,
                            MachineBasicBlock::iterator I,
                            unsigned DestReg, unsigned SrcReg,
                            const TargetRegisterClass *DestRC,
                            const TargetRegisterClass *SrcRC) const;
  
  virtual void storeRegToStackSlot(MachineBasicBlock &MBB,
                                   MachineBasicBlock::iterator MBBI,
                                   unsigned SrcReg, bool isKill, int FrameIndex,
                                   const TargetRegisterClass *RC) const;

  virtual void storeRegToAddr(MachineFunction &MF, unsigned SrcReg, bool isKill,
                              SmallVectorImpl<MachineOperand> &Addr,
                              const TargetRegisterClass *RC,
                              SmallVectorImpl<MachineInstr*> &NewMIs) const;

  virtual void loadRegFromStackSlot(MachineBasicBlock &MBB,
                                    MachineBasicBlock::iterator MBBI,
                                    unsigned DestReg, int FrameIndex,
                                    const TargetRegisterClass *RC) const;

  virtual void loadRegFromAddr(MachineFunction &MF, unsigned DestReg,
                               SmallVectorImpl<MachineOperand> &Addr,
                               const TargetRegisterClass *RC,
                               SmallVectorImpl<MachineInstr*> &NewMIs) const;
  
  virtual MachineInstr* foldMemoryOperand(MachineFunction &MF,
                                          MachineInstr* MI,
                                          SmallVectorImpl<unsigned> &Ops,
                                          int FrameIndex) const;

  virtual MachineInstr* foldMemoryOperand(MachineFunction &MF,
                                          MachineInstr* MI,
                                          SmallVectorImpl<unsigned> &Ops,
                                          MachineInstr* LoadMI) const {
    return 0;
  }
};

}

#endif