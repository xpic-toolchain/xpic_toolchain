// Helper.cpp
#include "Helper.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/GlobalValue.h"
#include "llvm/IR/GlobalVariable.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Intrinsics.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/CFG.h"
#include "llvm/IR/Type.h"
#include "llvm/ADT/SmallPtrSet.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/CodeGen/MachineConstantPool.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/SelectionDAGISel.h"
#include "llvm/Support/Compiler.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/MathExtras.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Target/TargetLowering.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetOptions.h"
#include <stdio.h>
using namespace llvm;

// helper for Debug: shows opcode and operands
void  SDValueInfo(const char *Name,SDValue Op,SelectionDAG *CurDAG)
{
 static int cntr=0;
  printf("+ + %3d ->> ",cntr);
  printf("%s", Name);
  if(Op.getOpcode()<ISD::BUILTIN_OP_END)
  printf(": %s (Opcode=%d), %d Operands:    ",Op.getNode()->getOperationName().data(),Op.getOpcode(),Op.getNumOperands());
  else 
{
//xpicTargetLowering xTL(TM);
  const TargetLowering &TLI = CurDAG->getTargetLoweringInfo();
  printf(": %s (Opcode=%d), %d Operands:    ",TLI.getTargetNodeName(Op.getOpcode()),Op.getOpcode(),Op.getNumOperands());
}



  for(unsigned t=0; t < Op.getNumOperands(); t++)
      printf("Op%d = %s,  ",t,Op.getOperand(t).getNode()->getOperationName().data()); 

  printf("  <<- + + +\n");
  cntr++;
}


// Create a string form an unsigned integer with pattern "Const.hXXXXXXXX"
char *xpicHlprCreateConstName(unsigned int Const)
{
  static char Name[]="Const.hXXXXXXXX";
  static const char hexchars[]="0123456789abcdef";
  for(int i=14;i>6;i--)
  {
    Name[i] = hexchars[Const&0x0f];
    Const=Const>>4;
  }
  return Name;
}

// Get existence GlobalVariable or create new one
GlobalVariable *xpicHlprGetGlobalVariable(const Module *M, unsigned int val)
{
  // create new name for a constant
  char *ConstName = xpicHlprCreateConstName(val);
  // it can be constructed, check for it
  GlobalVariable *NewGV = (GlobalVariable *)(M->getGlobalVariable(ConstName,true));
  if(!NewGV) 
  {// no, create one
    uint64_t val64= val;
    const APInt mAPInt(32, val64, true);
    ConstantInt *CI=ConstantInt::get(getGlobalContext(), mAPInt);
    Constant *c = dyn_cast<Constant>(CI);
    const Type *pt = Type::getInt32Ty(getGlobalContext());
    const Twine tw=ConstName;
    // new GlobalVariable
    NewGV = new GlobalVariable(*((Module *)M), (Type *) pt,true, GlobalValue::InternalLinkage, c, tw,(GlobalVariable*)0 , GlobalVariable::NotThreadLocal, 0U, false);
    // set alignment
    NewGV->setAlignment(4);
    // place  in   .data   section
    NewGV->setSection(".constants");
  }
  return NewGV;
}
