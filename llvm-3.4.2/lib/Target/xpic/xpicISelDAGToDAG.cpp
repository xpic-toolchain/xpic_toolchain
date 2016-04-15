//===-- xpicISelDAGToDAG.cpp - A dag to dag inst selector for xpic ------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines an instruction selector for the xpic target.
//
//===----------------------------------------------------------------------===//
#include "xpic.h"
#include "xpicRegisterInfo.h"
#include "xpicSubtarget.h"
#include "xpicTargetMachine.h"
#include "xpicMachineFunctionInfo.h"
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
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetOptions.h"
#include <stdio.h>
#include <stdint.h>
#include "Helper/Helper.h"

using namespace llvm;

// number of bits extended for an immediate ALU operand
#define XPIC_SIMM15_EXT_BITS 17

//===----------------------------------------------------------------------===//
// Instruction Selector Implementation
//===----------------------------------------------------------------------===//

//===--------------------------------------------------------------------===//
/// xpicDAGToDAGISel - xpic specific code to select xpic machine
/// instructions for SelectionDAG operations.
///
namespace {
class xpicDAGToDAGISel : public SelectionDAGISel {
  /// Subtarget - Keep a pointer to the xpic Subtarget around so that we can
  /// make the right decision when generating code for different targets.
  const xpicSubtarget &Subtarget;
  xpicTargetMachine &TM;
  bool CreateConstShift(int32_t value, uint32_t *opcode, uint32_t *op1, uint32_t *op2);
public:


  explicit xpicDAGToDAGISel(xpicTargetMachine &tm, CodeGenOpt::Level OptLevel) 
      : SelectionDAGISel(tm,OptLevel),
        Subtarget(tm.getSubtarget<xpicSubtarget>()),
        TM(tm) { 
  }

  SDNode *Select(SDNode *N);

  // Complex Pattern Selectors
  bool SelectADDRrr(SDValue Addr, SDValue &R1, SDValue &R2);
  bool SelectADDRri(SDValue Addr, SDValue &Base,SDValue &Offset);
  bool SelectADDRw(SDValue Addr, SDValue &Out);

  // add rx, [pc + #Offset],rx  // #Offset - constant from memory. Complex Pattern (26.03.2010)
  bool SelectConstMem(SDValue N, SDValue &R);
  // frame+offset // Complex Pattern (3.03.2010)
  bool SelectADDRframe(SDValue Addr,SDValue &Reg,SDValue &Base, SDValue &Offset);
  // frame (3.03.2010)
  SDNode *SelectFrameIndex(SDNode *N);
  SDNode *SelectSpecialADD(SDNode *N);

  // Constant selection (07.02.2010)
  SDNode *SelectConstant(SDNode *N);
  // G. Addr (8.03.2010)
  SDNode *SelectAddress(SDValue);


  bool SelectADDRComplex(SDValue Addr,SDValue &R1, SDValue &R2, SDValue &R3);

  bool RemapSwitch(uint64_t val, SDValue  *N2);


  virtual const char *getPassName() const {
    return "xpic DAG->DAG Pattern Instruction Selection";
  } 
  // Include the pieces autogenerated from the target description.
  #include "xpicGenDAGISel.inc"
};
}  // end anonymous namespace

/// InstructionSelect - This callback is invoked by
/// SelectionDAGISel when it has created a SelectionDAG for us to codegen.
/// +
bool xpicDAGToDAGISel::SelectADDRframe(SDValue Addr, SDValue  &Reg, SDValue &Base, SDValue &Offset)
{

  SDNode *N = Addr.getNode();
  SDLoc dl(N);
  // [r7 + fi]
  if (FrameIndexSDNode *FIN = dyn_cast<FrameIndexSDNode>(Addr)) 
  {
    Reg    = CurDAG->getRegister(XPIC::r7, MVT::i32);
    Base   = CurDAG->getTargetFrameIndex(FIN->getIndex(), MVT::i32);
    Offset = CurDAG->getTargetConstant(0, MVT::i32);
    return true;
  }

//  if (Addr.getOpcode() == ISD::TargetExternalSymbol||Addr.getOpcode() == ISD::TargetGlobalAddress)
//    return false;  // not our selection
  if (Addr.getOpcode() == ISD::ADD) 
  {
    SDValue Global  = Addr.getOperand(0);
    SDValue Summand = Addr.getOperand(1);

    // load rx= ADD (=ADD $TGC,z0),(Const or reg) -> load rx,$TGC : load[rx + (Const or reg)]
    if(Global.getOpcode()==ISD::ADD)
    {
      SDValue GlobalADD = Global.getOperand(0);
      RegisterSDNode *RN = dyn_cast<RegisterSDNode>(Global.getOperand(1));
      if(RN && (RN->getReg() == XPIC::z0))
      {
        if(GlobalADD.getOpcode()==ISD::TargetExternalSymbol || GlobalADD.getOpcode()==ISD::TargetGlobalAddress) 
        {
          if (ConstantSDNode *CN = dyn_cast<ConstantSDNode>(Summand))
          {
            int ConstVal = CN->getSExtValue();
            if(ConstVal >= -(1 << 16) && ConstVal < (1 << 16))
            {
              if(MemSDNode *MN = dyn_cast<MemSDNode>(N))
              {
                unsigned msk = (MN->getMemoryVT().getSizeInBits() + 7) / 8 - 1;
                //printf("BI: mask: 0x%08x\n", msk);
                if( 0 == ( msk & ConstVal ) )
                {
                  Base = CurDAG->getTargetConstant(Addr.getConstantOperandVal(1), MVT::i32);
                }
              }
            }
            return false;
          }
          else 
          {
            Base = Summand; // another operands
          }
          
          SDValue NewADD    = CurDAG->getNode(ISD::ADD,dl,MVT::i32,GlobalADD,CurDAG->getRegister(XPIC::z0,MVT::i32));
          SDNode *NewADDNode= Select(NewADD.getNode());
          Reg    = SDValue(NewADDNode,0);
          Offset = CurDAG->getTargetConstant(0, MVT::i32);
  
          return true;
        }
      }
    }



    //[r7 + fi+offset]
    if (ConstantSDNode *CN = dyn_cast<ConstantSDNode>(Summand)) 
    {
      //MemSDNode *MN = dyn_cast<MemSDNode>(N);
      //unsigned msk = (MN->getMemoryVT().getSizeInBits() + 7) / 8 - 1;
      //assert( 0 == (msk & CN->getSExtValue()) && "Invalid frame offset" );

      if (FrameIndexSDNode *FIN = dyn_cast<FrameIndexSDNode>(Global))
      {  // Constant offset from frame ref.
        Reg    = CurDAG->getRegister(XPIC::r7, MVT::i32);
        Base = CurDAG->getTargetFrameIndex(FIN->getIndex(), MVT::i32);
        Offset = CurDAG->getTargetConstant(CN->getZExtValue(), MVT::i32);
        return true;
      }
    }
  }
  return false;  // not this selection
//  Base = Addr;
//  Offset = CurDAG->getTargetConstant(0, MVT::i32);
//  return true;
}

/// +?
SDNode *xpicDAGToDAGISel::SelectFrameIndex(SDNode *FI)
{

  if (FrameIndexSDNode *FIN = dyn_cast<FrameIndexSDNode>(FI))
  {
    SDValue Reg    = CurDAG->getRegister(XPIC::z0, MVT::i32);
    SDValue Base   = CurDAG->getTargetFrameIndex(FIN->getIndex(), MVT::i32);
    SDValue Offset = CurDAG->getTargetConstant(0, MVT::i32);
    SDValue Ops0[] = {Reg, Base, Offset};
    return CurDAG->SelectNodeTo(FI, XPIC::xEL_FRAME, MVT::i32, Ops0, 3);
  }
  else return 0;
}

SDNode *xpicDAGToDAGISel::SelectSpecialADD(SDNode *ADD )
{
  SDValue Summand0 = ADD->getOperand(0);
  SDValue Summand1 = ADD->getOperand(1);
  // TargetGlobalAddress/TargetExternalSymbol + z0  { load r0,$TGA }
  if((Summand0.getOpcode()==ISD::TargetGlobalAddress||Summand0.getOpcode()==ISD::TargetExternalSymbol) && Summand1.getOpcode()==ISD::Register)
  {
    RegisterSDNode *Reg = dyn_cast<RegisterSDNode>( Summand1 );
    if(Reg->getReg()==XPIC::z0)
      return CurDAG->SelectNodeTo(ADD, XPIC::xLOADi, MVT::i32, Summand0, Summand1);
  }

  // FrameIndex + Constant 
  if(Summand0.getOpcode()==ISD::FrameIndex && Summand1.getOpcode()==ISD::Constant)
  {
    SDValue Reg    = CurDAG->getRegister(XPIC::z0, MVT::i32);
    FrameIndexSDNode *FIN = dyn_cast<FrameIndexSDNode>( ADD->getOperand(0) );
    SDValue Base   = CurDAG->getTargetFrameIndex(FIN->getIndex(), MVT::i32);
    SDValue Offset = CurDAG->getTargetConstant(ADD->getConstantOperandVal(1), MVT::i32);
    SDValue Ops0[] = {Reg,Base,Offset};
    return CurDAG->SelectNodeTo(ADD, XPIC::xEL_FRAME, MVT::i32, Ops0, 3);
  }
  return 0;
}

// Create a given constant with a shift operation with two immediate operands
bool xpicDAGToDAGISel::CreateConstShift(int32_t value, uint32_t *opcode, uint32_t *op1, uint32_t *op2)
{
  unsigned i, cntHi, cntLo;
  unsigned lsb = value & 1;
  unsigned msb = value < 0 ? 1 : 0;
  unsigned bbBit, bbLen, bbEnd, curbbBit, curbbLen; // bitblock: A sequence of same consecutive bits

  cntHi = 32;
  bbBit = curbbBit = lsb;
  bbLen = curbbLen = 1;
  bbEnd = 1;

  for(cntLo = i = 1; i < 32; ++i)
  {
    unsigned bit = (value >> i) & 1;
    // count bottom bitblock
    if( cntLo == i && bit == lsb ) cntLo = i+1;
    // count top bitblock
    if( bit != msb ) cntHi = 31 - i;
    // remember the longest bitblock
    if( bit == curbbBit )
    {
      if( ++curbbLen > bbLen )
      {
        bbLen = curbbLen;
        bbEnd = i + 1;
        bbBit = curbbBit;
      }
    }
    else
    {
      curbbBit = bit;
      curbbLen = 1;
    }
  }

  // can we create the value with a shift left?
  if( ( lsb == 0 ) && ( cntHi + cntLo > XPIC_SIMM15_EXT_BITS ) )
  {
    // lsl reg, op1, op2
    *opcode = XPIC::xLSLii5;
    *op2 = cntLo;
    *op1 = ( msb == 0 ) ? (value >> cntLo) : (value / (1<<cntLo));
    //printf("CreateConstShift hi:%d lo:%d; 0x%lx = 0x%lx << %ld\n", cntHi, cntLo, value, *op1, *op2);
    return true;
  }

  // check if wrapped bits from top and bottom bitblock are more than from longest inner bitblock
  if( (cntLo + cntHi) > bbLen && msb == lsb )
  {
    bbLen = cntLo + cntHi;
    bbBit = lsb;
    bbEnd = cntLo;
  }

  // can we create the value with a rotate left?
  if( bbLen > XPIC_SIMM15_EXT_BITS )
  {
    *opcode = XPIC::xROLii5;
    *op2 = bbEnd;
    *op1 = ((unsigned)value >> bbEnd) | (value << (32 - bbEnd));
    //printf("CreateConstShift bbStart:%d bbLen:%d; 0x%lx = 0x%lx rol %ld\n", bbEnd, bbLen, value, *op1, *op2);
    return true;
  }

  // cannot find appropriate immediate operands
  return false;
}

/// ++
SDNode *xpicDAGToDAGISel::SelectConstant(SDNode *Const)
{

  SDLoc dl(Const);

  if (ConstantSDNode *C = dyn_cast<ConstantSDNode>(Const))
  {
    // use z0 register for zero constant values
    if(C->isNullValue())
    {
      return CurDAG->getRegister(XPIC::z0, MVT::i32).getNode();
    }

    int64_t val =  C->getSExtValue();
    SDValue ZeroReg = CurDAG->getRegister(XPIC::z0, MVT::i32);

    // constant fit into signed 19 bit?
    if( val >= -(1 << 18) && val < (1 << 18) )
    {
      SDValue TConst   = CurDAG->getTargetConstant(val, MVT::i32);
      SDValue ZeroReg = CurDAG->getRegister(XPIC::z0, MVT::i32);
      return CurDAG->SelectNodeTo(Const, XPIC::xLOADi, MVT::i32, TConst, ZeroReg);
    }

    // try to construct the value with a shift operation
    // e.g. 0x400000 = 1 << 22 ==> lsl %trg, #1, #22
    uint32_t uShiftOpcode;
    uint32_t uShiftOp1;
    uint32_t uShiftOp2;
    if( CreateConstShift(val, &uShiftOpcode, &uShiftOp1, &uShiftOp2) )
    {
      SDValue ShiftOp1 = CurDAG->getTargetConstant(uShiftOp1, MVT::i32);
      SDValue ShiftOp2 = CurDAG->getTargetConstant(uShiftOp2, MVT::i32);
      return CurDAG->SelectNodeTo(Const, uShiftOpcode, MVT::i32, ShiftOp1, ShiftOp2);
    }

    /**
     * Loading constant into a register using three instructions
     *
     *    load  r, #upper18bits
     *    rol   r, r, #14
     *    or    r, #lower14bits, r
     *
     */
    uint32_t uval = (C->getZExtValue() & 0xffffffffu);
    uint32_t upper = uval >> 14;
    SDValue upperBits = CurDAG->getTargetConstant(upper, MVT::i32);
    SDNode *unshifted_high = CurDAG->getMachineNode(XPIC::xLOADi, dl, MVT::i32, upperBits, ZeroReg);
    SDNode *shifted_high = CurDAG->getMachineNode(XPIC::xROLri5, SDLoc(unshifted_high), MVT::i32,
                                                  SDValue(unshifted_high, 0),
                                                  CurDAG->getTargetConstant(14, MVT::i32));
    uint32_t lower = uval & 0x3FFF;
    SDValue lowerBits = CurDAG->getTargetConstant(lower, MVT::i32);
    SDNode *loadedGA = CurDAG->getMachineNode(XPIC::xORir, SDLoc(shifted_high), MVT::i32, lowerBits,
                                              SDValue(shifted_high, 0));
    return loadedGA;
  }
  return SelectCode(Const);
}

/// ++
bool xpicDAGToDAGISel::SelectConstMem(SDValue Const, SDValue &Result)
{

  SDLoc dl(Const);

  if( ConstantSDNode *C = dyn_cast<ConstantSDNode>(Const) )
  {
    int ConstVal = C->getZExtValue();

    // do not match constant that fits into 14 bit immediate field in ALU instruction
    if(ConstVal >= -(1 << 14) && ConstVal < (1 << 14) )
      return false; // leave this one for the xNNNir pattern

    // create new constant, and use it
    const Module *M=CurDAG->getMachineFunction().getFunction()->getParent();
    // get GlobalVariable
    unsigned int Val= (unsigned int)ConstVal;
    const GlobalVariable *NewGV = xpicHlprGetGlobalVariable(M, Val);

    Result = CurDAG->getTargetGlobalAddress(NewGV, dl, MVT::i32);
    return true;
  }
  return false;
}

///  Load / Store special selection: load rx, [reg + #const]
/// 31.03.2010
bool xpicDAGToDAGISel::SelectADDRri(SDValue Addr, SDValue &Base, SDValue &Offset)
{
  SDNode *N = Addr.getNode();
  SDLoc DL(N);

  //if (Addr.getOpcode() == ISD::FrameIndex) return false;
  
  // store [ADD rx, const], rs  -> store [rx + const], rs
  if (Addr.getOpcode() == ISD::ADD) 
  {
    if (ConstantSDNode *CN = dyn_cast<ConstantSDNode>(Addr.getOperand(1))) 
    {
      int ConstVal = CN->getSExtValue();
      //if (Predicate_simm16(CN))
      if(ConstVal >= -(1 << 16) && ConstVal < (1 << 16))
      {
        if(MemSDNode *MN = dyn_cast<MemSDNode>(N))
        {
          unsigned msk = (MN->getMemoryVT().getSizeInBits() + 7) / 8 - 1;
          //printf("BI: mask: 0x%08x\n", msk);
          if( 0 == ( msk & ConstVal ) )
          {
            Base = Addr.getOperand(0);
            Offset = CurDAG->getTargetConstant(CN->getZExtValue(), MVT::i32);
            return true;
          }
        }
      }
    }
  }
  // external symbol or global addres
  if( Addr.getOpcode() == ISD::TargetExternalSymbol || 
      Addr.getOpcode() == ISD::TargetGlobalAddress  ||
      Addr.getOpcode() == ISD::TargetGlobalTLSAddress)
    return false;  // direct calls.

  Base   = Addr;
  Offset = CurDAG->getTargetConstant(0, MVT::i32);

  return true;
}



bool xpicDAGToDAGISel::SelectADDRrr(SDValue Addr, SDValue &R1, SDValue &R2){
#ifdef DEBUG_SHOW_FNS_NAMES  
printf("xpicDAGToDAGISel::SelectADDRrr()\n");
#endif
  // TODO: not implemented yet! WReg Register class is under test!
  return false;

  // store [ADD rx, ry], rs  -> store [rx + ry], rs
  if (Addr.getOpcode() == ISD::ADD)
  {
    if (dyn_cast<ConstantSDNode>(Addr.getOperand(0))
      || dyn_cast<ConstantSDNode>(Addr.getOperand(1)) ) {
      R1 = Addr;
      R2 = CurDAG->getRegister(XPIC::z0, MVT::i32);
      //errs() << "SelectADDRrr selected for immediate\n";
      return true;
    }

    R1 = Addr.getOperand(0);
    R2 = Addr.getOperand(1);
    return true;
  }

    if (Addr.getOpcode() == ISD::TargetExternalSymbol ||
        Addr.getOpcode() == ISD::TargetGlobalAddress)
      return false;  // direct calls.
 
  R1 = Addr;
  R2 = CurDAG->getRegister(XPIC::z0, MVT::i32);
  return true;
}

// do not select frameindices or sums for ALU addresses
bool xpicDAGToDAGISel::SelectADDRw(SDValue Addr, SDValue &Out)
{

  if ( (Addr.getOpcode() == ISD::ADD) || (dyn_cast<FrameIndexSDNode>(Addr)))
  {
    return false;
  }

  Out = Addr;
  return true;
}


SDNode *xpicDAGToDAGISel::Select(SDNode *N) 
{
#ifdef DEBUG_SHOW_FNS_NAMES  
printf("xpicDAGToDAGISel::Select()\n");
#endif
  
  SDLoc dl(N);

  if(N->isMachineOpcode()){
    N->setNodeId(-1);
    return NULL;   // Already selected.
  }
//DAG.viewGraph();
  switch (N->getOpcode()) 
  {
    default: break;
    case ISD::FrameIndex:
      return SelectFrameIndex(N);

    case ISD::ADD:
    {
      if(SDNode *SD=SelectSpecialADD(N))
        return SD;// used in SelectADDRframe
      break;
    }

    case ISD::Constant:
      return SelectConstant(N);

  }
  return SelectCode(N);
}

/// createxpicISelDag - This pass converts a legalized DAG into a 
/// xpic-specific DAG, ready for instruction scheduling.
///
FunctionPass *llvm::createxpicISelDag(xpicTargetMachine &TM,llvm::CodeGenOpt::Level OptLevel)
{
  return new xpicDAGToDAGISel(TM, OptLevel);
}
