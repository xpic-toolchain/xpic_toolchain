//===-- xpicFrameLowering.cpp - xpic Frame Information ------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the xpic implementation of TargetFrameLowering class.
//
//===----------------------------------------------------------------------===//

#include <stdio.h>
#include "xpicFrameLowering.h"
#include "xpicInstrInfo.h"
#include "xpicMachineFunctionInfo.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineModuleInfo.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/IR/Argument.h"
#include "llvm/IR/Attributes.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/CodeGen/AsmPrinter.h"
#include "llvm/IR/Constant.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/GlobalValue.h"
#include "llvm/IR/GlobalVariable.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetOptions.h"
#include "llvm/IR/Type.h"
#include "llvm/ADT/APInt.h"
#include "llvm/ADT/BitVector.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/MC/MachineLocation.h"
#include "llvm/CodeGen/MachineModuleInfo.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/Target/TargetInstrInfo.h"
#include <vector>
#include "Helper/Helper.h"

//#include "llvm/Target/TargetOptions.h"

using namespace llvm;

bool xpicFrameLowering::hasFP(const MachineFunction &MF) const {
  return false;
}

bool xpicFrameLowering::hasReservedCallFrame(const MachineFunction &MF) const {
  return !MF.getFrameInfo()->hasVarSizedObjects();
}

void xpicFrameLowering::eliminateCallFramePseudoInstr(MachineFunction &MF, MachineBasicBlock &MBB, MachineBasicBlock::iterator I) const {
#ifdef DEBUG_SHOW_FNS_NAMES
printf("xpicFrameLowering::eliminateCallFramePseudoInstr\n");
#endif
  MBB.erase(I);
}

int xpicFrameLowering::get_numStackSavedRegisters( void ) const {
    return numStackSavedRegisters;
}

void xpicFrameLowering::set_numStackSavedRegisters( int value ) const {
    numStackSavedRegisters = value;
}

// Stack adjustment of the call frame (non-register arguments for called function)
void xpicFrameLowering::AdjustStackForArguments(MachineFunction &MF) const {

  // has current function call (argument) frame index? (was attached in LowerCall?)
  if(!xpicLowerCallInfo.IsCurrentFn(MF))
    return; // no

  // get frame index after last
  int MaxFI = MF.getFrameInfo()->getObjectIndexEnd();

  // Argument frame index must have the biggest index (will be placed on end of frame from offset 0)
  // Needs adjustment? 
  if((MaxFI-1) == xpicLowerCallInfo.iCallFI)
    return; // no, current Argument frame index is maximal

  // Is frame objects handling correct?
  assert( MaxFI > xpicLowerCallInfo.iCallFI && "AdjustStackForArguments: call frame index is greater than maximal frame index in current function.");

  // Adjust all frame indexes from (xpicLowerCallInfo.iCallFI+1) to (MaxFI-1), decreasing by one 
  // and keep actual sizes of frame objects. Set xpicLowerCallInfo.iCallFI to (MaxFI-1):

  // search for all frame indexes in all instructions

  // MachineBasicBlock's
  for (MachineFunction::iterator MFIt = MF.begin(), FE = MF.end();MFIt != FE; ++MFIt)
  {
    MachineBasicBlock *MBB = MFIt;

    // MachineInstruction's
    for (MachineBasicBlock::iterator MI = (*MBB).begin(); MI != (*MBB).end(); ++MI)
    {

      // MachineOperand's
      for(unsigned op=0; op < MI->getNumOperands() ;op++)
      {
        MachineOperand &MO = MI->getOperand(op);

        if(MO.isFI())
        {// this is a frame index
          int FrameIndex = MO.getIndex();
          if(FrameIndex > xpicLowerCallInfo.iCallFI)
          {
            MO.setIndex(FrameIndex-1);// decrease by one 
            continue;
          }
          if(FrameIndex == xpicLowerCallInfo.iCallFI)
          {
            MO.setIndex(MaxFI-1); // Set xpicLowerCallInfo.iCallFI to (MaxFI-1)
          }
        }
      }
    }
  }

  // adjust sizes of the objects associated with frame indexes
  // get size of the frame with arguments:
  uint64_t SizeCallFI = MF.getFrameInfo()->getObjectSize( xpicLowerCallInfo.iCallFI );
  int fi;
  for(fi = xpicLowerCallInfo.iCallFI+1; fi < MaxFI; fi++)
  {
    MF.getFrameInfo()->setObjectSize(fi-1, MF.getFrameInfo()->getObjectSize(fi) );
  }
  MF.getFrameInfo()->setObjectSize(fi-1, SizeCallFI);

}

// Align all stack objects to 4Byte
void xpicFrameLowering::AlignStackObjects(MachineFunction &MF) const {


  // get frame info
  MachineFrameInfo *MFI = MF.getFrameInfo();

  // maximal alignment of the stack is 4!
  MFI->ensureMaxAlignment(4);

  // search for all frame indexes in all instructions

  // MachineBasicBlock's
  for (MachineFunction::iterator MFIt = MF.begin(), FE = MF.end();MFIt != FE; ++MFIt)
  {
    MachineBasicBlock *MBB = MFIt;

    // MachineInstruction's
    for (MachineBasicBlock::iterator MI = (*MBB).begin(); MI != (*MBB).end(); ++MI)
    {

      // MachineOperand's
      for(unsigned op=0; op < MI->getNumOperands() ;op++)
      {
        MachineOperand &MO = MI->getOperand(op);

        if(MO.isFI()  && ( MFI->getObjectAlignment(MO.getIndex()) < 4 ) )
        { // this is a frame index
          MFI->setObjectAlignment(MO.getIndex(),4);
        }
      }
    }
  }
}



/**

     Analyse which registers must be saved to the stack: all USED in function work registers (2.),
     BUT except registers for arguments and scratch registers (3.).
     Process FIQ/IRQ/IRQN register properties in (3.). Function "main" is the first on call tree, do
     not save in "main" registers (1.).
**/
void xpicFrameLowering::processFunctionBeforeFrameFinalized(MachineFunction &MF, RegScavenger *RS ) const {
#ifdef DEBUG_SHOW_FNS_NAMES  
printf("xpicFrameLowering::processFunctionBeforeFrameFinalized\n");
#endif
  // take care of call frame (non-register arguments for called function)
  AdjustStackForArguments(MF);
  // allign all stack objects to 4BytesdjustStackForArguments
  AlignStackObjects(MF);
  // no register in use before analyse:
  static unsigned UsedRegisters[XPIC::NUM_TARGET_REGS];
  // clean array:
  memset(UsedRegisters,0,sizeof(UsedRegisters));
  // save pointer to array, in next use pointer
  pUsedRegisters = UsedRegisters;

  // at first no saved registers:
  numStackSavedRegisters=0;

  /// 1. for function "main" don't save registers:
  if( MF.getFunction()->getName().compare("main")==0 )
  {
    // annotate as "main"
    FA=main;
    return;
  }
  /// 2. Retrieve used regs. Initialize pointer pUsedRegisters;
  getUsedRegisters(MF);

//printf("\n1:  ");for(int j=0;j<XPIC::NUM_TARGET_REGS;j++) printf(" N%d=%d,",j,pUsedRegisters[j]);printf("\n");

  /// 3. Mark out all scrath registers and look for interrupt annotations
  // get annotation, save to the class member FA, use next in prologue and epilog emission
  FA = isAnnotated(MF);
  // mark in/out registers inside functions and interrupt-handlers:
  // special registers - out
  pUsedRegisters[XPIC::r7]=0;
  pUsedRegisters[XPIC::pc]=0;
  pUsedRegisters[XPIC::stat]=0;
  pUsedRegisters[XPIC::z0]=0;

  switch(FA){
   default:{
     // registers-arguments (scratch) - out
     pUsedRegisters[XPIC::r2]=0;
     pUsedRegisters[XPIC::r3]=0;
     pUsedRegisters[XPIC::r4]=0;
     pUsedRegisters[XPIC::r5]=0;
     // IP register (scratch) - out
     pUsedRegisters[XPIC::r6]=0;
   break;  }

   case irqn:// function is a nested interrupt:
   case irq:{// function is an interrupt:
     pUsedRegisters[XPIC::pc  ]=1;   // in 'pc'
     pUsedRegisters[XPIC::stat]=1;   // in 'st'
     bool fHasCall;
     // has function procedure call?
     HasRELIInstruction(MF,fHasCall);
     if(fHasCall) 
     {
       // registers-arguments (scratch) - in
       pUsedRegisters[XPIC::r2]=1;
       pUsedRegisters[XPIC::r3]=1;
       pUsedRegisters[XPIC::r4]=1;
       pUsedRegisters[XPIC::r5]=1;
     }
     // IP register (scratch) - in
     // It is used by LongJump and FarStore!!!
     pUsedRegisters[XPIC::r6]=1;
   break;   }

   case fiq:{// function is a fast interrupt:
     // mark out all 'rx'
     pUsedRegisters[XPIC::r0]=0;
     pUsedRegisters[XPIC::r1]=0;
     pUsedRegisters[XPIC::r2]=0;
     pUsedRegisters[XPIC::r3]=0;
     pUsedRegisters[XPIC::r4]=0;
     pUsedRegisters[XPIC::r5]=0;
     pUsedRegisters[XPIC::r6]=0;
   break;   }
  };

// calculate number of registers to save in stack:
  for(int i=XPIC::NoRegister + 1; i<XPIC::NUM_TARGET_REGS; i++)
  {
    numStackSavedRegisters += pUsedRegisters[i];
  }

}

/// This function searches for registers that are used in current function MF
void xpicFrameLowering::getUsedRegisters(MachineFunction &MF) const
{
  // search for all register in use
  for (MachineFunction::iterator FI = MF.begin(), FE = MF.end();FI != FE; ++FI) {
    MachineBasicBlock *MBB = FI;
    for (MachineBasicBlock::iterator MI = (*MBB).begin(); MI != (*MBB).end(); ++MI){
      // Do not analyse subroutines:
      if(MI->getOpcode()==XPIC::xCALL || MI->getOpcode()==XPIC::xCALL_LOAD ||
         MI->getOpcode()==XPIC::xCALL_done || MI->getOpcode()==XPIC::xCALL_LOAD_done)
        continue;

      for(unsigned op=0; op<MI->getNumOperands();op++){
        MachineOperand &MO = MI->getOperand(op);
        if(MO.isReg() && MO.getReg()<XPIC::NUM_TARGET_REGS)
          pUsedRegisters[MO.getReg()]=1;
      }
    }
  }
}


/// register-sequence: registers will be placed in stack in this sequence:
  #define XPIC_STACK_QUEUE_LENGTH 14
  const unsigned xpicRegisterStackQueue[XPIC_STACK_QUEUE_LENGTH]={XPIC::pc, XPIC::stat, XPIC::r0, XPIC::r1, XPIC::r2, XPIC::r3, XPIC::r4, XPIC::r5, XPIC::r6, XPIC::u0, XPIC::u1, XPIC::u2, XPIC::u3, XPIC::u4};



/**
   Save to stack all marked registers (1.)
   Allign stack to 4 bytes (2.)
   Allocate frame in stack (3.)
**/
void xpicFrameLowering::emitPrologue(MachineFunction &MF) const 
{
#ifdef DEBUG_SHOW_FNS_NAMES  
printf("xpicFrameLowering::emitPrologue\n");
#endif
  xpicMachineFunctionInfo *FuncInfo = MF.getInfo<xpicMachineFunctionInfo>();
  const xpicInstrInfo &TII =
    *static_cast<const xpicInstrInfo*>(MF.getTarget().getInstrInfo());
  // get top block (MachineBasicBlock [iterator]) in function (MachineFunction):
  MachineBasicBlock &MBB = MF.front();
  MachineFrameInfo *MFI = MF.getFrameInfo() ;
  // get iterator of this block (MachineBasicBlock::iterator)
  MachineBasicBlock::iterator MBBI = MBB.begin();
  // debug location:
  DebugLoc dl = MBBI != MBB.end() ? MBBI->getDebugLoc() : DebugLoc();
  ///1. Save content of all regs (that are used in this function) to stack at the beginning of this function
  for(int i=0; i<XPIC_STACK_QUEUE_LENGTH; i++)
  {
    if(pUsedRegisters[ xpicRegisterStackQueue[ i ] ]) // used?
    {
      BuildMI(MBB,MBBI,dl, TII.get(XPIC::xSTORE_TO_STACK)).addReg(XPIC::r7).addReg( xpicRegisterStackQueue[ i ] );
    }
  }

  /// 2. Align size of stack: if stack not 4 byte alligned, correct allignment to 4 bytes
  // positive align of  var, to byte number (bn -> 1/2/4 Byte)
  #define  XPIC_ALIGN(var,bn)   (  ( var + bn-1 ) & ~(bn-1)  )
  int StackSize        = MFI->getStackSize();
  int AlignedStackSize = XPIC_ALIGN(StackSize,4);
  MFI->setStackSize(AlignedStackSize);

  /// 3.Rereserve place in stack for frame objects:
  if(MFI->getNumObjects() > 0 && MFI->getStackSize() != 0)
  {
    if(MFI->getStackSize() > XPIC_CONSTANTS::CONST_V_MAX)   // +-14bit
    { /*  Stack Size to big for instruction like 'add r7,#-const, r7',
       *  Generate warning and
       *  use
       *      load r6, #const
       *      sub r7, r7, r6
       *  or
       *      load r6, [pc + #offs_const]d
       *      sub r7, r7, r6
       */
      // warning:
      fprintf(stderr,"warning: big stack frame in function \"%s\". Code is not optimized.\n",MF.getFunction()->getName().data());
      /// can we use 'load r6, const'?
      if(MFI->getStackSize() <= XPIC_CONSTANTS::CONST_Z_MAX) // +-18bit
      {
        // load r6, #-const
        BuildMI(MBB, MBBI,dl,TII.get(XPIC::xLOADi),XPIC::r6).addImm((int)MFI->getStackSize());
      }
      else
      { // load r6, [pc + #offs_const] -> .data -> offs_const: const
        // create new constant, and use it
        const Module *M=MF.getFunction()->getParent();
        // get stack size
        unsigned int val= MFI->getStackSize();
        // get GlobalVariable
        const GlobalVariable *NewGV = xpicHlprGetGlobalVariable(M, val);
        // load r6, [pc + #off_const]d
        BuildMI(MBB, MBBI,dl,TII.get(XPIC::xLDri),XPIC::r6).addReg( XPIC::pc).addGlobalAddress(NewGV);
      }
      // sub r7, r7, r6
      BuildMI(MBB, MBBI,dl,TII.get(XPIC::xSUBrr),XPIC::r7).addReg( XPIC::r7).addReg( XPIC::r6);
    }
    else
    {
      // use pattern: add r7,#-const, r7
      BuildMI(MBB, MBBI,dl,TII.get(XPIC::xADDir),XPIC::r7).addImm(-(int)MFI->getStackSize()).addReg( XPIC::r7);
    }
  }


  MachineModuleInfo &MMI = MF.getMMI();
  const MCRegisterInfo *MRI = MMI.getContext().getRegisterInfo();
  MCSymbol *FrameLabel = MMI.getContext().CreateTempSymbol();
  unsigned regFP = MRI->getDwarfRegNum(XPIC::r7, true);

  // Emit ".cfi_def_cfa_register".
  MMI.addFrameInst(MCCFIInstruction::createDefCfaRegister(FrameLabel,
                                                          regFP));
  // Emit ".cfi_window_save".
  MMI.addFrameInst(MCCFIInstruction::createWindowSave(FrameLabel));

}

/**  In epilogue: free allocated frame in stack (1.), load from stack all used registers (2.) and process annotations (3.).
*/
void xpicFrameLowering::emitEpilogue(MachineFunction &MF,MachineBasicBlock &MBB) const 
{
#ifdef DEBUG_SHOW_FNS_NAMES  
printf("xpicFrameLowering::emitEpilogue\n");
#endif
  const MachineFrameInfo *MFI = MF.getFrameInfo();
  xpicMachineFunctionInfo *FuncInfo = MF.getInfo<xpicMachineFunctionInfo>();
  const xpicInstrInfo &TII =
    *static_cast<const xpicInstrInfo*>(MF.getTarget().getInstrInfo());

  MachineBasicBlock::iterator MBBI = MBB.getLastNonDebugInstr();

  // debug location:
  DebugLoc dl =  MBBI->getDebugLoc() ;

  /// 1.Clear place in stack that was allocated for frame objects:
  if(MFI->getNumObjects() >0 && MFI->getStackSize() != 0)
  {
    if(MFI->getStackSize() > XPIC_CONSTANTS::CONST_V_MAX)   // +-14bit
    { /*  Stack Size to big for instruction like 'add r7,#const, r7',
       *  use
       *      load r6, #const
       *      add r7, r7, r6
       *  or
       *      load r6, [pc + #offs_const]d
       *      add r7, r7, r6
       */
      /// can we use 'load r6, #const'?
      if(MFI->getStackSize() <= XPIC_CONSTANTS::CONST_Z_MAX) // +-18bit
      {
        // load r6, #const
        BuildMI(MBB, MBBI,dl,TII.get(XPIC::xLOADi),XPIC::r6).addImm((int)MFI->getStackSize());
      }
      else
      { // load r6, [pc + #offs_const] -> .data -> offs_const: const
        // create new constant, and use it
        const Module *M=MF.getFunction()->getParent();
        // get stack size
        unsigned int val= MFI->getStackSize();
        // get GlobalVariable
        const GlobalVariable *NewGV = xpicHlprGetGlobalVariable(M, val);
        // load r6, [pc + #off_const]d
        BuildMI(MBB, MBBI,dl,TII.get(XPIC::xLDri),XPIC::r6).addReg( XPIC::pc).addGlobalAddress(NewGV);
      }
      // add r7, r7, r6
      BuildMI(MBB, MBBI,dl,TII.get(XPIC::xADDrr),XPIC::r7).addReg( XPIC::r7).addReg( XPIC::r6);
    }
    else
    {
      // use pattern: add r7,#const, r7 
      BuildMI(MBB, MBBI,dl,TII.get(XPIC::xADDir),XPIC::r7).addImm((int)MFI->getStackSize()).addReg( XPIC::r7);
    }
  }

  ///2. Load (in reversed sequence) content of all regs (that were used in this function) from stack on the end of this function
  for(int i=XPIC_STACK_QUEUE_LENGTH-1; i>=0; i--)
    if(pUsedRegisters[ xpicRegisterStackQueue[ i ] ])// used?
      BuildMI(MBB,MBBI,dl, TII.get(XPIC::xLOAD_FROM_STACK2), xpicRegisterStackQueue[ i ] ).addReg(XPIC::r7);

  ///3. This can be an annotated function:
  if(FA!=no_annotation)
  {
    switch(FA){
    // not interrupt:
    default: return;
    case main:
             return; // main is a normal function now!
    // interrupt
    case irq:{
      BuildMI(MBB,MBBI,dl,TII.get(XPIC::xRETRELI));
    }break;
    // nested interrupt:
    case irqn:{
      bool HasCall;
      BuildMI(MBB,MBBI,dl,TII.get(XPIC::xRETI));
      // has function instruction 'reli' inside?
      if(!HasRELIInstruction(MF,HasCall)) 
       // no, emit warning
       fprintf(stderr,"warning: nested interrupt function must release interrupt with instruction \"reli\" before returns. Use '__asm_(\"reli\")' inside function \"%s\" or inside called function.\n",MF.getFunction()->getName().data());
    }break;
    // fast interrupt:
    case fiq:{
      BuildMI(MBB,MBBI,dl,TII.get(XPIC::xRETF));
    }break;
   };//end switch

   // delete usual return, if it presence:
   if(MBBI->getOpcode()==XPIC::xRETL) 
     MBBI->eraseFromParent();
  }
}

///   = = =  A N N O T A T I O N S  (recognition of interrupt functions) = = =

/// This function searches for inline assembly instruction "reli", 
/// that must be used inside nested interrrupt routine
/// But this "reli" can also be called in a callee. If MF function has CALL instruction,
/// fCallee is a flag for caller of 'HasRELIInstruction', that current MF has call instruction
bool  xpicFrameLowering::HasRELIInstruction(MachineFunction &MF, bool &fCallee) const{
  // flag: calls this function another?
  fCallee=false;
  // search for instruction INLINEASM
  for (MachineFunction::iterator FI = MF.begin(), FE = MF.end();FI != FE; ++FI){
    MachineBasicBlock *MBB = FI;
    for (MachineBasicBlock::iterator MI = (*MBB).begin(); MI != (*MBB).end(); ++MI){
      if(MI->getOpcode()==XPIC::xCALL || MI->getOpcode()==XPIC::xCALL_LOAD ||
         MI->getOpcode()==XPIC::xCALL_done || MI->getOpcode()==XPIC::xCALL_LOAD_done) 
        fCallee=true; // function call
      if(MI->getOpcode()==XPIC::INLINEASM){
        std::string AsmStr = MI->getOperand(0).getSymbolName();
        if(AsmStr.find("reli")!=std::string::npos)
        return true;
      }
    }
  }
  if(fCallee) 
    return true;// was function call!
  return false;
}

/**
This function determines an annotation of current MachineFunction &MF
It can be annotated for interrupts as "irq" and suchlike.
- Function parses a list of Globals, and search for a Global
 with name "llvm.global.annotations". This Global contains a
 vector of annotation-structures. Each structure has four fields:
  type { i8*, i8*, i8*, i32 } 
   1st: i8* - pointer to annotate-function
   2nd: i8* - pointer to annotation
   3rd: i8* - pointer to name of compilation unit (module)
   4th: i32 - string number in sourse file, where annotation done
 A Vector of Annotated Functions (vecAF) will be constructed from this info.
 Wenn a construction of 'vecAF' is done, all info (Globals), that used 
 for annotations, will be deleted.
-This parse occurs just one time, with next call of 'isAnnotated(..)'
 vector 'vecAF' has already annotation-info
-At the end of function, will be looking for an annotation for current MF.
**/
xpicFrameLowering::FunctionAnnotations xpicFrameLowering::isAnnotated(MachineFunction &MF)const{
std::vector <GlobalVariable *> vecGVtoKill;
 // get current Function
 const Function *F = MF.getFunction();
 // is annotation table constructed?
 if(vecFA.size() == 0)
 {// annotation table is not constructed, make it
  // get the Module
  Module *M = (Module *)F->getParent();
  // all functions are annotated in Globals, so
  // walk through all globals and find sections "llvm.global.annotations"
  for( Module::global_iterator IB_GVL = M->global_begin(), IE_GVL = M->global_end(); IB_GVL!=IE_GVL; ++IB_GVL ){
   // current Global Variable
   GlobalVariable *GVar=IB_GVL;
   // has it name?
   if(!GVar->hasName())continue;// continue if no name specified
   // get it's name
   std::string Vname=GVar->getName();
   // is it annotation?
   if(Vname.compare("llvm.global.annotations"))continue;// continue if not equal
   // yes:
   // Save this GlobalVariable, to delete it in future
   vecGVtoKill.push_back(GVar);
   // must be minimal one operand:
   if(GVar->getNumOperands()==0)continue;// continue if null
   // get the array of Structures:
   if (const ConstantArray *CA = dyn_cast<ConstantArray>(GVar->getOperand(0))) {
    // is it really structure?
    if(CA->getType()->getElementType()->getTypeID() == Type::StructTyID) {
      //yes, look in every structure for a chain function-declaration
      for(unsigned NOp = CA->getNumOperands(),n=0;n<NOp;n++){
       // get a Structure
       ConstantStruct *ConstStr = cast<ConstantStruct>(CA->getOperand(n));
       // in FA will be saved all annotation info
       FunctionAnnotation FA;
       FA.Annotation = no_annotation;// clear
       // get a number of fields and search in this fields actual info
       // (type { i8*, i8*, i8*, i32 }) -> {function, annotation,module_name, string in sourse file}
       for(unsigned StrNOp = ConstStr->getNumOperands(),sn=0;sn<StrNOp;sn++){
        // walk through fields
        Constant *ConstStrField  = cast<Constant>(ConstStr->getOperand(sn));
         // first three fields of structure are a pointers, and they have an operands.
         // fourth is integer - we don't want it, continue
         if(ConstStrField->getNumOperands() == 0 ) continue;
         Constant  *ConstFieldPointer=NULL;
         // get a first oprand of field, that pointer is
         if(!(ConstFieldPointer  = dyn_cast<Constant>(ConstStrField->getOperand(0))) ) continue;
         // first field of structure is annotated function
         if(sn==0){
          // is it an annotation for a function? 
          const Type *type=ConstFieldPointer->getType();
          if(type->getTypeID() ==Type::PointerTyID){
           if(type->getNumContainedTypes()>0)
            if(type->getContainedType(0)->getTypeID() != Type::FunctionTyID )
             // this is an annotation not for a function! go to next structure!
             break;
          }
          // save a name
          FA.FunctionName = ConstFieldPointer->getName();
//printf("FunctionName: %s ",FA.FunctionName.data());
          // go to next field
          continue;
         }
         // here we want get a value from a pointer, must be more operands than null
         if(ConstFieldPointer->getNumOperands()==0) continue;
         Constant *ConstFieldSubPointer  = dyn_cast<Constant>(ConstStrField->getOperand(0));
         if(ConstFieldSubPointer->getNumOperands()==0) continue;
// to kill:!!
GlobalVariable *GV  = (GlobalVariable *) dyn_cast<GlobalVariable>(ConstFieldSubPointer);
if(GV)vecGVtoKill.push_back(GV);
//GlobalVariableVal
         // get and test a pointer for Array, where data saved
         const Constant *ConstFieldSubPointerArray = dyn_cast<Constant>(ConstFieldSubPointer->getOperand(0));
         if(const ConstantDataArray *ConstArr= dyn_cast<ConstantDataArray>(ConstFieldSubPointerArray)){
          // it must be a string, check it
          if(ConstArr->isString()){
           if(sn==1){ // as described above, first comes an annotation
            // annotationis inside a string, get it and compare with "irq"/"fiq"/"irqn",
            // save and go to next structure field with continue
            std::string annotation = ConstArr->getAsString();
//printf(", Annotation: %s",annotation.data());
            if     (strcmp("irq", annotation.data())==0) {FA.Annotation = irq; continue;}
            else if(strcmp("fiq", annotation.data())==0) {FA.Annotation = fiq; continue;}
            else if(strcmp("irqn",annotation.data())==0) {FA.Annotation = irqn;continue;}
            // false annotation 
            FA.Annotation = no_annotation; continue;
           }
           if(sn==2){ // as described above, thhird comes a module name
            // save module name
            FA.ModuleName = ConstArr->getAsString();
//printf(", ModuleName: %s, ",FA.ModuleName.data());
            continue;
           }
          }//ConstArr->isString
         }// *ConstArr
       }// for(sn) - structures
       // the structure FA is full, add it in vector
       vecFA.push_back(FA);
     }// for(), look for a chain function-declaration
    }// Type::StructTyID ?
   }// ConstantDataArray *CA - is it an Array of Structures?
  }// Iterator of Globals

// deleting of Globals, that is used just in annotations
  for(unsigned i=0; i<vecGVtoKill.size();i++){
   GlobalVariable *GV  = vecGVtoKill[i];
   if(GV->getParent())
    if(IsDeleteAllowed(GV))
     GV->eraseFromParent();
  }

 }// vecFA.size

 /// here annotation table is constructed!
 // get Name of current function
 std::string FuncName= F->getName();
 // search for an annotation for current function 
 for(unsigned a=0; a<vecFA.size();a++){
  FunctionAnnotation &FA=vecFA[a];
  // compare functions names
  if(FA.FunctionName.compare(FuncName)==0){
   // Function must be in oure compilation module!
   // get name of compilation module.
   std::string ModuleName=F->getParent()->getModuleIdentifier();
   // ModuleName can be absolute path with slashes and has extension .bc, like /home/myfile.bc
   // or simple name of file with extension .bc
//printf("\n\nModuleName: %s\n",ModuleName.data());
//printf("Annotation_ModuleName: %s\n",FA.ModuleName.data());
   size_t first,last;// first and last cut-points
   first=ModuleName.rfind("\\");// windows
   if(first==std::string::npos)first=ModuleName.rfind("/");// unix
   if(first==std::string::npos)first=(unsigned)-1;// simple name
   last=ModuleName.rfind(".");
   // found?
   if(last==std::string::npos) continue;
   // so Current Module Name
   std::string CMN = ModuleName.substr(first+1, last-first-1);

   // ModuleName in FA extension .c
   // and Annotated Module Name
   first=FA.ModuleName.rfind("\\");// windows
   if(first==std::string::npos)first=FA.ModuleName.rfind("/");// unix
   if(first==std::string::npos)first=(unsigned)-1;// simple name
   last=FA.ModuleName.rfind(".");
   // found?
   if(last==std::string::npos) continue;
   //  so Current Annotated Module Name
   std::string AMN = FA.ModuleName.substr(first+1, last-first-1);
   // and now compare
   if(AMN.compare(CMN)==0)
    return FA.Annotation;
  }// Fcompare of function names
 }
 return no_annotation;
}

// Has current Constant users?
// returns:  false if has
//           true  if not
bool xpicFrameLowering::NoUses(const Constant *C)const {
  if (isa<GlobalValue>(C)) return false; // Cannot remove this
 bool no_uses=false;
  while (!C->use_empty() && !no_uses) {
    const Constant *User = dyn_cast<Constant>(C->use_back());
    if (!User) return false; // Non-constant usage;
    no_uses = NoUses(User);
    if (!no_uses)
      return false; // Constant wasn't dead
  }
  return true;
}
// Test: Can I delete this Value? (if it has uses - no)
bool xpicFrameLowering::IsDeleteAllowed(const Value *V)const {
  Value::const_use_iterator I = V->use_begin(), E = V->use_end();
  if (I == E) return true;
  if (const Constant *User = dyn_cast<Constant>(*I))
   if (NoUses(User)) 
   return true;
  return false;
}
///   = = =  E N D    A N N O T A T I O N  = = =



