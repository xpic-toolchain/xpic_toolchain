//===- xpicRegisterInfo.cpp - xpic Register Information -------*- C++ -*-===//
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

#include "xpicRegisterInfo.h"
#include "xpic.h"
#include "xpicFrameLowering.h"
#include "xpicMachineFunctionInfo.h"
#include "xpicSubtarget.h"
#include "llvm/ADT/BitVector.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/IR/Type.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Target/TargetInstrInfo.h"
#include "Helper/Helper.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/GlobalValue.h"
#include "llvm/IR/GlobalVariable.h"

#define GET_REGINFO_TARGET_DESC
#include "xpicGenRegisterInfo.inc"

using namespace llvm;

xpicRegisterInfo::xpicRegisterInfo(xpicSubtarget &st)
	: xpicGenRegisterInfo(XPIC::r7), Subtarget(st) {
}

const uint16_t* xpicRegisterInfo::getCalleeSavedRegs(const MachineFunction *MF)
                                                                         const {
  static const uint16_t CalleeSavedRegs[] = { XPIC::r7,0 };
  return CalleeSavedRegs;
}

//return reserved registers:  pc, stat, z0, r7, u4, r6
BitVector xpicRegisterInfo::getReservedRegs(const MachineFunction &MF) const {
#ifdef DEBUG_SHOW_FNS_NAMES  
  printf("xpicRegisterInfo::getReservedRegs\n");
#endif
  BitVector Reserved(getNumRegs());
  Reserved.set(XPIC::pc);
  Reserved.set(XPIC::stat);
  Reserved.set(XPIC::z0);
  Reserved.set(XPIC::r7);
  Reserved.set(XPIC::u4);
  Reserved.set(XPIC::r6);
  return Reserved;
}

const TargetRegisterClass*
xpicRegisterInfo::getPointerRegClass(const MachineFunction &MF,
                                      unsigned Kind) const {
  return &XPIC::IntRegsRegClass;
}

/// When llvm accesses an object on the stack, the index of this object into the stack frame (FrameIndex) is used to address it
/// This function converts the FrameIndex to an immediate value in the xpic instruction
void xpicRegisterInfo::eliminateFrameIndex(MachineBasicBlock::iterator II, int SPAdj, unsigned FIOperandNum, RegScavenger *RS) const {
 #ifdef DEBUG_SHOW_FNS_NAMES  
 printf("eliminateFrameIndex\n");
 #endif
  assert(SPAdj == 0 && "Unexpected");

  MachineInstr &MI = *II;
  DebugLoc dl = MI.getDebugLoc();
  int FrameIndex = MI.getOperand(FIOperandNum).getIndex();
  
  // Get MachineFunction
  // Addressable stack objects are accessed using offsets from $fp (SP-> r7)
  MachineFunction &MF = *MI.getParent()->getParent();
  int Offset =0;

  const TargetInstrInfo &TII = *MF.getTarget().getInstrInfo();

  const xpicFrameLowering *xFL =  (const xpicFrameLowering *) MF.getTarget().getFrameLowering();

  // Get size for current frame
  int FrameSize=(int)MF.getFrameInfo()->getStackSize();
  // Get operator number, that frame is
  unsigned int i=0;
  while (!(*II).getOperand(i).isFI()) 
  {
    ++i;
    assert(i < (*II).getNumOperands() && "Instr doesn't have FrameIndex operand!");
  }

  // Base offset
  // it is negative: for debug locations (locale variables)
  // and   positive: for register saving at register allocation step
  //                 for non register functions arguments
  int Base   = MF.getFrameInfo()->getObjectOffset(FrameIndex);

  int Add=0;
  Add=(*II).getOperand(i+1).getImm();
  Offset = FrameSize + Base + Add;

  if( Base >= 0 )
  {// this correction for incomming function arguments
    Offset += xFL->get_numStackSavedRegisters() *4;
  }



  int fLittleConstant=true;
  if( MI.getOpcode() == XPIC::xEL_FRAME )
       // for: add rx, frame, r7 (V constant)
    fLittleConstant = (XPIC_CONSTANTS::CONST_V_MIN <= Offset  &&  Offset <= XPIC_CONSTANTS::CONST_V_MAX);

  else // for: load rx, [pc + frame] / store[pc + frame], rx (Z constant)
    fLittleConstant = (XPIC_CONSTANTS::CONST_Z_MIN <= Offset  &&  Offset <= XPIC_CONSTANTS::CONST_Z_MAX);

  if( fLittleConstant ) 
  {
    MI.getOperand(i).ChangeToImmediate(Offset); /// replace FI with immediate 
  }
  else
  { /*construct sequence:
       load r6, [pc + #offs_const] -> .data -> offs_const: const
       load reg, [r7 + r6]
     ( store [r7 + r6], reg )
    */
    // create new constant, and use it
    const Module *M=MF.getFunction()->getParent();
    // value to create:
    unsigned int val= Offset;
    // get GlobalVariable
    const GlobalVariable *NewGV = xpicHlprGetGlobalVariable(M, val);
    // debug location:
    dl = (II != (MF.begin())->end() ? II->getDebugLoc() : DebugLoc());
    // Add instruction: load r6, [pc + #off_const]d
    BuildMI(*(II->getParent()),II,dl,TII.get(XPIC::xLDri),XPIC::r6).addReg( XPIC::pc).addGlobalAddress(NewGV);
    //   load reg, [r7 + FI]  :> load reg, [r7 + r6]
    // ( store [r7 + r6], reg :> store [r7 + r6], reg )
    MI.getOperand(i).ChangeToRegister(XPIC::r6,true, false,false,false,false,false); /// replace FI with register
  }
}

unsigned xpicRegisterInfo::getRARegister() const {
  //assert(0 && "What is the return address register");
  return XPIC::r7;
}

unsigned xpicRegisterInfo::getFrameRegister(const MachineFunction &MF) const {
  //assert(0 && "What is the frame register");
  return XPIC::r7;
}

