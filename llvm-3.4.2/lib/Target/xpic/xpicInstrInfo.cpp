//===- xpicInstrInfo.cpp - xpic Instruction Information -------*- C++ -*-===//
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

//#include <stdio.h>
#include "xpicInstrInfo.h"
#include "xpic.h"
#include "xpicMachineFunctionInfo.h"
#include "xpicSubtarget.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineMemOperand.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/TargetRegistry.h"

#define GET_INSTRINFO_CTOR_DTOR
#include "xpicGenInstrInfo.inc"

using namespace llvm;


// Pin the vtable to this file.
void xpicInstrInfo::anchor() { }

xpicInstrInfo::xpicInstrInfo(xpicSubtarget &ST)
  : xpicGenInstrInfo(XPIC::ADJCALLSTACKDOWN, XPIC::ADJCALLSTACKUP),
    RI(ST), Subtarget(ST) {
}

static bool isZeroImm(const MachineOperand &op) {
  return op.isImm() && op.getImm() == 0;
}

static bool isZeroReg(const MachineOperand &op) {
  return op.isReg() && op.getReg() == XPIC::z0;
}

static bool isImplicitReg(const MachineOperand &op) {
  return op.isReg() && op.isImplicit();
}

/// isLoadFromStackSlot - If the specified machine instruction is a direct
/// load from a stack slot, return the virtual or physical register number of
/// the destination along with the FrameIndex of the loaded stack slot.  If
/// not, return 0.  This predicate must return 0 if the instruction has
/// any side effects other than loading from the stack slot.
unsigned xpicInstrInfo::isLoadFromStackSlot(const MachineInstr *MI,int &FrameIndex) const 
{
#ifdef DEBUG_SHOW_FNS_NAMES  
printf("xpicInstrInfo::isLoadFromStackSlot()\n");
#endif
  if (MI->getOpcode() == XPIC::xLOADframe) //31.03.2010
  {
    if (MI->getOperand(1).isFI() && MI->getOperand(2).isImm() && MI->getOperand(2).getImm() == 0) 
    {
      FrameIndex = MI->getOperand(1).getIndex();
      return MI->getOperand(0).getReg();
    }
  }
  return 0;
}

/// isStoreToStackSlot - If the specified machine instruction is a direct
/// store to a stack slot, return the virtual or physical register number of
/// the source reg along with the FrameIndex of the loaded stack slot.  If
/// not, return 0.  This predicate must return 0 if the instruction has
/// any side effects other than storing to the stack slot.
unsigned xpicInstrInfo::isStoreToStackSlot(const MachineInstr *MI, int &FrameIndex) const {
#ifdef DEBUG_SHOW_FNS_NAMES  
printf("xpicInstrInfo::isStoreToStackSlot()\n");
#endif
  if (MI->getOpcode() == XPIC::xSTOREframe) //31.03.2010
  {
    if (MI->getOperand(0).isFI() && MI->getOperand(1).isImm() && MI->getOperand(1).getImm() == 0) 
    {
      FrameIndex = MI->getOperand(0).getIndex();
      return MI->getOperand(2).getReg();
    }
  }
  return 0;
}
bool xpicInstrInfo::AnalyzeBranch(MachineBasicBlock &MBB,
                                   MachineBasicBlock *&TBB,
                                   MachineBasicBlock *&FBB,
                                   SmallVectorImpl<MachineOperand> &Cond,
                                   bool AllowModify) const
{

  MachineBasicBlock::iterator I = MBB.end();
  MachineBasicBlock::iterator UnCondBrIter = MBB.end();
  while (I != MBB.begin()) {
    --I;

    if (I->isDebugValue())
      continue;

    // When we see a non-terminator, we are done.
    if (!isUnpredicatedTerminator(I))
      break;

    // Terminator is not a branch.
    if (!I->isBranch())
      return true;

    // Handle Unconditional branches.
    if (I->getOpcode() == XPIC::xBA) {
      UnCondBrIter = I;

      if (!AllowModify) {
        TBB = I->getOperand(0).getMBB();
        continue;
      }

      while (llvm::next(I) != MBB.end())
        llvm::next(I)->eraseFromParent();

      Cond.clear();
      FBB = 0;

      if (MBB.isLayoutSuccessor(I->getOperand(0).getMBB())) {
        TBB = 0;
        I->eraseFromParent();
        I = MBB.end();
        UnCondBrIter = MBB.end();
        continue;
      }

      TBB = I->getOperand(0).getMBB();
      continue;
    }

#if 0
    unsigned Opcode = I->getOpcode();
    if (Opcode != SP::BCOND && Opcode != SP::FBCOND)
      return true; // Unknown Opcode.

    SPCC::CondCodes BranchCode = (SPCC::CondCodes)I->getOperand(1).getImm();

    if (Cond.empty()) {
      MachineBasicBlock *TargetBB = I->getOperand(0).getMBB();
      if (AllowModify && UnCondBrIter != MBB.end() &&
          MBB.isLayoutSuccessor(TargetBB)) {

        // Transform the code
        //
        //    brCC L1
        //    ba L2
        // L1:
        //    ..
        // L2:
        //
        // into
        //
        //   brnCC L2
        // L1:
        //   ...
        // L2:
        //
        BranchCode = GetOppositeBranchCondition(BranchCode);
        MachineBasicBlock::iterator OldInst = I;
        BuildMI(MBB, UnCondBrIter, MBB.findDebugLoc(I), get(Opcode))
          .addMBB(UnCondBrIter->getOperand(0).getMBB()).addImm(BranchCode);
        BuildMI(MBB, UnCondBrIter, MBB.findDebugLoc(I), get(SP::BA))
          .addMBB(TargetBB);

        OldInst->eraseFromParent();
        UnCondBrIter->eraseFromParent();

        UnCondBrIter = MBB.end();
        I = MBB.end();
        continue;
      }
      FBB = TBB;
      TBB = I->getOperand(0).getMBB();
      Cond.push_back(MachineOperand::CreateImm(BranchCode));
      continue;
    }
#endif
    // FIXME: Handle subsequent conditional branches.
    // For now, we can't handle multiple conditional branches.
    return true;
  }
  return false;
}



unsigned
xpicInstrInfo::InsertBranch(MachineBasicBlock &MBB,MachineBasicBlock *TBB,
                             MachineBasicBlock *FBB,
                             const SmallVectorImpl<MachineOperand> &Cond,
                             DebugLoc dl)const{
#ifdef DEBUG_SHOW_FNS_NAMES  
printf("xpicInstrInfo::InsertBranch()\n");
#endif
  // Can only insert uncond branches so far.
  assert(Cond.empty() && !FBB && TBB && "Can only handle uncond branches!");
  BuildMI(&MBB,dl, get(XPIC::xBA)).addMBB(TBB);
  return 1;
}

unsigned xpicInstrInfo::RemoveBranch(MachineBasicBlock &MBB) const
{
  MachineBasicBlock::iterator I = MBB.end();
  unsigned Count = 0;
  while (I != MBB.begin()) {
    --I;

    if (I->isDebugValue())
      continue;

    if (I->getOpcode() != XPIC::xBA
        && I->getOpcode() != XPIC::xBCOND)
      break; // Not a branch

    I->eraseFromParent();
    I = MBB.end();
    ++Count;
  }
  return Count;
}

void xpicInstrInfo::copyPhysReg(MachineBasicBlock &MBB,
                                 MachineBasicBlock::iterator I, DebugLoc DL,
                                 unsigned DestReg, unsigned SrcReg,
                                 bool KillSrc) const {
#ifdef DEBUG_SHOW_FNS_NAMES  
printf("xpicInstrInfo::copyRegToReg()\n");
#endif

  if (I != MBB.end()) DL = I->getDebugLoc();

  if (XPIC::IntRegsRegClass.contains(DestReg, SrcReg) || XPIC::WRegRegClass.contains(DestReg, SrcReg) )
  {
    BuildMI(MBB, I, DL, get(XPIC::xMOV), DestReg).addReg(SrcReg);
  }
  else
  {
    // Can't copy this register                                                                                      
    llvm_unreachable("Impossible reg-to-reg copy");
  }
}

void xpicInstrInfo::storeRegToStackSlot(MachineBasicBlock &MBB, MachineBasicBlock::iterator I,
unsigned SrcReg, bool isKill, int FI,const TargetRegisterClass *RC, const llvm::TargetRegisterInfo *RI) const 
{
#ifdef DEBUG_SHOW_FNS_NAMES  
printf("xpicInstrInfo::storeRegToStackSlot()\n");
#endif
  DebugLoc DL;
  if (I != MBB.end()) DL = I->getDebugLoc();

  if (RC == &XPIC::IntRegsRegClass || RC == &XPIC::WRegRegClass) {
    BuildMI(MBB,I,DL, get(XPIC::xSTOREframe)).addReg(XPIC::r7).addFrameIndex(FI).addImm(0).addReg(SrcReg,  RegState::Kill,0);
  } else {
    assert(0 && "Can't store this register to stack slot       \"storeRegToStackSlot\"");
  }
}
///---------------------------------------------------------------

void xpicInstrInfo::loadRegFromStackSlot(MachineBasicBlock &MBB, MachineBasicBlock::iterator I,
unsigned DestReg, int FI,const TargetRegisterClass *RC, const llvm::TargetRegisterInfo* RI) const 
{
#ifdef DEBUG_SHOW_FNS_NAMES  
printf("xpicInstrInfo::loadRegFromStackSlot()\n");
#endif
  DebugLoc DL = DebugLoc();
  if (I != MBB.end()) DL = I->getDebugLoc();

  if (RC == &XPIC::IntRegsRegClass || RC == &XPIC::WRegRegClass) {
    BuildMI(MBB, I, DL, get(XPIC::xLOADframe), DestReg).addReg(XPIC::r7).addFrameIndex(FI).addImm(0);//28.03.2010
  } else {
    assert(0 && "Can't load this register from stack slot");
  }
}


void xpicInstrInfo::storeRegToAddr(MachineFunction &MF, unsigned SrcReg,
                                       bool isKill,
                                       SmallVectorImpl<MachineOperand> &Addr,
                                       const TargetRegisterClass *RC,
                                 SmallVectorImpl<MachineInstr*> &NewMIs) const {
#ifdef DEBUG_SHOW_FNS_NAMES  
printf("xpicInstrInfo::storeRegToAddr()\n");
#endif

  assert(0 && "\"xpicInstrInfo::storeRegToAddr\" Is it  really launched by You? Let me know: tema13tema@yahoo.de \n");
}

void xpicInstrInfo::loadRegFromAddr(MachineFunction &MF, unsigned DestReg,
                                    SmallVectorImpl<MachineOperand> &Addr,
                                    const TargetRegisterClass *RC,
                                    SmallVectorImpl<MachineInstr*> &NewMIs) const {
#ifdef DEBUG_SHOW_FNS_NAMES  
printf("xpicInstrInfo::loadRegFromAddr()\n");
#endif

  assert(0 && "\"xpicInstrInfo::loadRegFromAddr\" Is it  really launched by You? Let me know: tema13tema@yahoo.de \n");

  return;
}

