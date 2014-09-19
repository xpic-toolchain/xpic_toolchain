//===-- DelaySlotFiller.cpp - xpic delay slot filler ---------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This is a simple local pass that fills delay slots with NOPs.
//
//===----------------------------------------------------------------------===//
#define DEBUG_TYPE "delay-slot-filler"
#include "xpic.h"
#include "xpicInstrInfo.h"
#include "xpicTargetMachine.h"
#include "llvm/ADT/SmallSet.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Target/TargetInstrInfo.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetRegisterInfo.h"
#include "llvm/IR/Function.h"
#include <stdio.h>

using namespace llvm;

STATISTIC(FilledSlots, "Number of delay slots filled");

static cl::opt<bool> DisableDelaySlotFiller(
  "disable-xpic-delay-filler",
  cl::init(false),
  cl::desc("Disable the Sparc delay slot filler."),
  cl::Hidden);

namespace {
  struct Filler : public MachineFunctionPass {
    /// Target machine description which we query for reg. names, data
    /// layout, etc.
    ///
    TargetMachine &TM;
    const xpicSubtarget *Subtarget;

    static char ID;
    Filler(TargetMachine &tm) 
      : MachineFunctionPass(ID), TM(tm),
        Subtarget(&TM.getSubtarget<xpicSubtarget>()) {
    }

    virtual const char *getPassName() const {
      return "xpic Delay Slot Filler";
    }

    bool runOnMachineFunction(MachineFunction &F); 
    bool isDataDependence(MachineInstr &MI);
  };
  char Filler::ID = 0;
} // end of anonymous namespace

/// createxpicDelaySlotFillerPass - Returns a pass that fills in delay
/// slots in xpic MachineFunctions
///
FunctionPass *llvm::createxpicDelaySlotFillerPass(TargetMachine &tm) {
  return new Filler(tm);
}


/**
Look for a data dependence, like:
   load    r0, 10
   store [r0], r3
Data dependence within register r0. "nop" instruction is needed between this two:
   load    r0, 10
   nop
   store [r0], r3
That is write!
**/
bool Filler::isDataDependence(MachineInstr &MI)
{ 
 static unsigned PrevTargetReg = XPIC::NoRegister;
 unsigned i = 0;
  bool isDependency = false;

//const char *opName = MI.getDesc().getName();
//printf("Filler::isDataDependence : %d\n",MI.getOpcode());
  switch(MI.getOpcode())
  {
  case XPIC::EH_LABEL:
  case XPIC::GC_LABEL:
  case XPIC::DBG_VALUE:
                return false;

  case XPIC::xRETF:
  case XPIC::xRETRELI:
  case XPIC::xRETI:
  case XPIC::xNOP:
                PrevTargetReg=XPIC::NoRegister;
                return false;

  // return and call's have additional instructions with stack pointer r7
  case XPIC::xCALL:
  case XPIC::xCALL_LOAD:
  case XPIC::xRETL:
                if( PrevTargetReg == XPIC::r7 ) isDependency = true;
                PrevTargetReg = XPIC::NoRegister;
                return isDependency;

  // load instructions
  case XPIC::xLOADframe:
  case XPIC::xLOADframeSi16:
  case XPIC::xLOADframeSi8:
  case XPIC::xLOADframeUi16:
  case XPIC::xLOADframeUi8:
  case XPIC::xLOADframei16:
  case XPIC::xLOADframei8:
  case XPIC::xLDri:
  case XPIC::xLDUHri:
  case XPIC::xLDUBri:
  case XPIC::xLDSHri:
  case XPIC::xLDSBri:
  case XPIC::xLDrr:
  case XPIC::xLDUHrr:
  case XPIC::xLDUBrr:
  case XPIC::xLDSHrr:
  case XPIC::xLDSBrr:
  case XPIC::xLOAD_FROM_STACK2:
    for(i=1; i < MI.getNumOperands() ;i++)
    {
      if( (MI.getOperand(i).isReg()) && (MI.getOperand(i).getReg() == PrevTargetReg))
        isDependency = true;
    }
  break;

  // store instructions
  case XPIC::xSTOREframe:
  case XPIC::xSTOREframei16:
  case XPIC::xSTOREframei8:
  case XPIC::xSTri:
  case XPIC::xSTHri:
  case XPIC::xSTBri:
  case XPIC::xSTrr:
  case XPIC::xSTHrr:
  case XPIC::xSTBrr:
  case XPIC::xSTORE_TO_STACK:
    for(i=0; i < (MI.getNumOperands()-1) ;i++)
    {
      if( (MI.getOperand(i).isReg()) && (MI.getOperand(i).getReg() == PrevTargetReg))
        isDependency = true;
    }
    /* 'store' don't have any target register, that can cause a data dependency */
    PrevTargetReg = XPIC::NoRegister;
    return isDependency;

  // catch all operations that use register indirect addressing
  default:
    if( (MI.getDesc().TSFlags & XPICII::UsesRegisterIndirectAddressing) != 0)
    {
      unsigned addrReg = ((MI.getDesc().TSFlags & XPICII::IsCmp) != 0) ? 0 : 1;
      if( MI.getOperand(addrReg).getReg() == PrevTargetReg )
      {
        isDependency = true;
      }
    }
    break;

  };

  PrevTargetReg = XPIC::NoRegister;

  if( MI.getNumOperands() > 0 )
  {
    if( MI.getOperand(0).isReg() )
    {
      PrevTargetReg = MI.getOperand(0).getReg();
      // z0 Register has never data dependencies
      if (PrevTargetReg == XPIC::z0)
        PrevTargetReg = XPIC::NoRegister;
    }
  }
  else fprintf(stderr,"warning: in \"Filler::isDataDependence\" 0 operands!\n");

  return isDependency;
}

/// When data dependence is found, insert "nop"
/// Instruction can have DelaySlot. This flag comes from "xpicInstrInfo.td". Insert "nop" too.
bool Filler::runOnMachineFunction(MachineFunction &MF)
{

  const TargetInstrInfo *TII = TM.getInstrInfo();

  bool Changed = false;
  for (MachineFunction::iterator FI = MF.begin(), FE = MF.end();FI != FE; )
  {
    MachineBasicBlock *MBB = FI;
    ++FI ;
    for (MachineBasicBlock::iterator MI = (*MBB).begin(); MI != (*MBB).end(); ++MI)
    {
      if( isDataDependence(*MI) )
      {
        DebugLoc dl = MI->getDebugLoc();
        BuildMI((*MBB), MI,dl, TII->get(XPIC::xNOP));
      }

      if (MI->getDesc().hasDelaySlot()) /// delay slot from "xpicInstrInfo.td"
      {
        DebugLoc dl = MI->getDebugLoc();
        MachineBasicBlock::iterator J = MI;

	// xCALL instructions has delay slots, but xCALL has dummy: insert "save return address" instruction
        if(MI->getOpcode()==XPIC::xCALL)
        {
	  BuildMI(*MBB,MI,dl, TII->get(XPIC::xSTORE_TO_STACK)).addReg(XPIC::r7,RegState::Define).addReg(XPIC::pc,RegState::Kill);
	  continue;
        }
        if(MI->getOpcode()==XPIC::xCALL_LOAD)
        {
	  BuildMI(*MBB,MI,dl, TII->get(XPIC::xSTORE_TO_STACK)).addReg(XPIC::r7,RegState::Define).addReg(XPIC::pc,RegState::Kill);
        }
        // insert NOP
        ++J;
        BuildMI((*MBB), J, dl, TII->get(XPIC::xNOP));
        ++FilledSlots;
        Changed = true;
      }
    }
  }

  return Changed;
}
