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
#include <stdio.h>
#include "xpicInstrInfo.h"
#include "xpicSubtarget.h"
#include "xpic.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "xpicGenInstrInfo.inc"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineFrameInfo.h"


using namespace llvm;


xpicInstrInfo::xpicInstrInfo(xpicSubtarget &ST)
  : TargetInstrInfoImpl(xpicInsts, array_lengthof(xpicInsts)),
    RI(ST, *this), Subtarget(ST) {
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


/// Return true if the instruction is a register to register move and
/// leave the source and dest operands in the passed parameters.
///
bool xpicInstrInfo::isMoveInstr(const MachineInstr &MI,
                                 unsigned &SrcReg, unsigned &DstReg,
                                 unsigned &SrcSR, unsigned &DstSR) const {
#ifdef DEBUG_SHOW_FNS_NAMES  
printf("xpicInstrInfo::isMoveInstr()\n");    
#endif
	SrcSR = DstSR = 0; // No sub-registers.
	// We look for 3 kinds of patterns here:
	// or with zero or 0
	// add with zero or 0
	// fmovs or FpMOVD (pseudo double move).
	if (MI.getOpcode() == XPIC::xORrr || MI.getOpcode() == XPIC::xORir)
	{
		unsigned countDef = 0;
		unsigned lastDef = 0;
		unsigned countUse = 0;
		unsigned lastUse = 0;
		unsigned countArgs = 0;
		for ( unsigned i = 0; i < MI.getNumOperands(); i++ ) 
		{
			MachineOperand op = MI.getOperand(i);
			if ( isImplicitReg(op) || isZeroImm(op) || isZeroReg(op) ) {
				continue;
			} 
			else if ( op.isReg() && op.isUse() ) {
				countUse++;
				lastUse = i;
			} else if ( op.isReg() && op.isDef() ) {
				countDef++;
				lastDef = i;
			} 
			countArgs++;
		}
		if ( (countArgs == ( countUse + countDef )) && countArgs == 2 && countUse == 1 ) {
			DstReg = MI.getOperand(lastDef).getReg();
			SrcReg = MI.getOperand(lastUse).getReg();
			return true;
		} else {
			return false;
		}
	} else if ( MI.getOpcode() == XPIC::xMOV ) {
		DstReg = MI.getOperand(0).getReg();
		SrcReg = MI.getOperand(1).getReg();
		return true;
	}
	return false;
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

unsigned
xpicInstrInfo::InsertBranch(MachineBasicBlock &MBB,MachineBasicBlock *TBB,
                             MachineBasicBlock *FBB,
                             const SmallVectorImpl<MachineOperand> &Cond)const{
#ifdef DEBUG_SHOW_FNS_NAMES  
printf("xpicInstrInfo::InsertBranch()\n");
#endif
  // Can only insert uncond branches so far.
  DebugLoc dl = DebugLoc::getUnknownLoc();
  assert(Cond.empty() && !FBB && TBB && "Can only handle uncond branches!");
  BuildMI(&MBB,dl, get(XPIC::xBA)).addMBB(TBB);
  return 1;
}

bool xpicInstrInfo::copyRegToReg(MachineBasicBlock &MBB,
                                     MachineBasicBlock::iterator I,
                                     unsigned DestReg, unsigned SrcReg,
                                     const TargetRegisterClass *DestRC,
                                     const TargetRegisterClass *SrcRC) const {
#ifdef DEBUG_SHOW_FNS_NAMES  
printf("xpicInstrInfo::copyRegToReg()\n");
#endif

  DebugLoc DL = DebugLoc::getUnknownLoc();
  if (I != MBB.end()) DL = I->getDebugLoc();

  if (DestRC == XPIC::IntRegsRegisterClass || DestRC == XPIC::WRegRegisterClass)
  {
    BuildMI(MBB, I, DL, get(XPIC::xMOV), DestReg).addReg(SrcReg);
  }
  else
  { // Can't copy this register
    return false;
  }
  return true;
}



void xpicInstrInfo::storeRegToStackSlot(MachineBasicBlock &MBB, MachineBasicBlock::iterator I,
unsigned SrcReg, bool isKill, int FI,const TargetRegisterClass *RC) const 
{
#ifdef DEBUG_SHOW_FNS_NAMES  
printf("xpicInstrInfo::storeRegToStackSlot()\n");
#endif
  DebugLoc DL = DebugLoc::getUnknownLoc();
  if (I != MBB.end()) DL = I->getDebugLoc();

  if (RC == XPIC::IntRegsRegisterClass || RC == XPIC::WRegRegisterClass)
    BuildMI(MBB,I,DL, get(XPIC::xSTOREframe)).addReg(XPIC::r7).addFrameIndex(FI).addImm(0).addReg(SrcReg,  RegState::Kill,0);

  else assert(0 && "Can't store this register to stack slot       \"storeRegToStackSlot\"");
}
///---------------------------------------------------------------

void xpicInstrInfo::loadRegFromStackSlot(MachineBasicBlock &MBB, MachineBasicBlock::iterator I,
unsigned DestReg, int FI,const TargetRegisterClass *RC) const 
{
#ifdef DEBUG_SHOW_FNS_NAMES  
printf("xpicInstrInfo::loadRegFromStackSlot()\n");
#endif
  DebugLoc DL = DebugLoc::getUnknownLoc();
  if (I != MBB.end()) DL = I->getDebugLoc();

  if (RC == XPIC::IntRegsRegisterClass || RC == XPIC::WRegRegisterClass)
    BuildMI(MBB, I, DL, get(XPIC::xLOADframe), DestReg).addReg(XPIC::r7).addFrameIndex(FI).addImm(0);//28.03.2010
  else assert(0 && "Can't load this register from stack slot");
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

MachineInstr *xpicInstrInfo::foldMemoryOperand(MachineFunction &MF,
                                               MachineInstr* MI,
                                               SmallVectorImpl<unsigned> &Ops,
                                               int FI) const {
#ifdef DEBUG_SHOW_FNS_NAMES  
printf("xpicInstrInfo::foldMemoryOperand()\n");
#endif

  assert(0 && "\"xpicInstrInfo::foldMemoryOperand\" Is it  really launched by You? Let me know: tema13tema@yahoo.de \n");

  MachineInstr *NewMI = NULL;
  return NewMI;
}
