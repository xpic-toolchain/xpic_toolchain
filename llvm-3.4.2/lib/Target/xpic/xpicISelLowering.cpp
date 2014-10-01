//===-- xpicISelLowering.cpp - xpic DAG Lowering Implementation ---------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the interfaces that xpic uses to lower LLVM code into a
// selection DAG.
//
//===----------------------------------------------------------------------===//
#include "xpic.h"
#include "xpicISelLowering.h"
#include "xpicMachineFunctionInfo.h"
#include "xpicRegisterInfo.h"
#include "xpicTargetMachine.h"
#include "llvm/CodeGen/CallingConvLower.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/SelectionDAG.h"
#include "llvm/CodeGen/TargetLoweringObjectFileImpl.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Module.h"
#include "llvm/CodeGen/LiveInterval.h"
#include "llvm/CodeGen/LiveIntervalAnalysis.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/ErrorHandling.h"
#include <stdio.h>

#include "Helper/Helper.h"

// number of arguments in function that passes through registers:
#define NUM_REG_ARG_IN_FN	4  // (r2 - r5)

using namespace llvm;

//===----------------------------------------------------------------------===//
// Calling Convention Implementation
//===----------------------------------------------------------------------===//
#include "xpicGenCallingConv.inc"

///   R E T U R N
SDValue xpicTargetLowering::LowerReturn(SDValue Chain,
                                        CallingConv::ID CallConv,
                                        bool IsVarArg,
                                        const SmallVectorImpl<ISD::OutputArg> &Outs,
                                        const SmallVectorImpl<SDValue> &OutVals,
                                        SDLoc DL,
                                        SelectionDAG &DAG) const
{
#ifdef DEBUG_SHOW_FNS_NAMES  
printf("xpecTargetLowering::LowerReturn\n");
#endif

  MachineFunction &MF = DAG.getMachineFunction();

  // CCValAssign - represent the assignment of the return value to locations.
  SmallVector<CCValAssign, 16> RVLocs;
  // CCState - Info about the registers and stack slot.
  CCState CCInfo(CallConv, IsVarArg, DAG.getMachineFunction(), DAG.getTarget(),RVLocs, *DAG.getContext());
  // Analize return values of ISD::RET
  CCInfo.AnalyzeReturn(Outs, RetCC_xpic32);
  /* removed georg not needed
  // If this is the first return lowered for this function, add the regs to the
  // liveout set for the functionI
  if (DAG.getMachineFunction().getRegInfo().liveout_empty()) {
    for (unsigned i = 0; i != RVLocs.size(); ++i)
      if (RVLocs[i].isRegLoc())
        DAG.getMachineFunction().getRegInfo().addLiveOut(RVLocs[i].getLocReg());
  }*/

  SDValue Flag;
  SmallVector<SDValue, 4> RetOps(1, Chain);
  // Make room for the return address offset.
  //RetOps.push_back(SDValue());

  // Copy the result values into the output registers.
  for (unsigned i = 0; i != RVLocs.size(); ++i)
  {
    CCValAssign &VA = RVLocs[i];
    assert(VA.isRegLoc() && "Can only return in registers!");
    // ISD::RET => ret chain, (regnum1,val1), ...
    // So i*2+1 index only the regnums.
    
    Chain = DAG.getCopyToReg(Chain, DL, VA.getLocReg(), OutVals[i], Flag);
    
    // Guarantee that all emitted copies are stuck together with flags.
    Flag = Chain.getValue(1);
    RetOps.push_back(DAG.getRegister(VA.getLocReg(), VA.getLocVT()));
  }

  RetOps[0] = Chain;  // Update chain.

  // Add the flag if we have it.
  if (Flag.getNode())
    RetOps.push_back(Flag);

  return DAG.getNode(XPICISD::RET_FLAG, DL, MVT::Other,
                     &RetOps[0], RetOps.size());
}

///   C A L L
SDValue xpicTargetLowering::LowerCall(TargetLowering::CallLoweringInfo &CLI,
                                      SmallVectorImpl<SDValue> &InVals) const
{
#ifdef DEBUG_SHOW_FNS_NAMES  
printf("xpicTargetLowering::LowerCall\n");
#endif

  SelectionDAG &DAG                     = CLI.DAG;
  SDLoc &dl                             = CLI.DL;
  SmallVectorImpl<ISD::OutputArg> &Outs = CLI.Outs;
  SmallVectorImpl<SDValue> &OutVals     = CLI.OutVals;
  SmallVectorImpl<ISD::InputArg> &Ins   = CLI.Ins;
  SDValue Chain                         = CLI.Chain;
  SDValue Callee                        = CLI.Callee;
  bool &isTailCall                      = CLI.IsTailCall;
  CallingConv::ID CallConv              = CLI.CallConv;
  bool IsVarArg                         = CLI.IsVarArg;


  MachineFunction &MF = DAG.getMachineFunction();
  // Analyze operands of the call, assigning locations to each operand.
  SmallVector<CCValAssign, 16> ArgLocs;
  CCState CCInfo(CallConv, IsVarArg, DAG.getMachineFunction(), DAG.getTarget(), ArgLocs,*DAG.getContext());
  CCInfo.AnalyzeCallOperands(Outs, CC_xpic32);

  // The size of the outgoing arguments.
  unsigned ArgsSize = ArgLocs.size();

  Chain = DAG.getCALLSEQ_START(Chain, DAG.getIntPtrConstant(ArgsSize, true),dl);

  SmallVector<std::pair<unsigned, SDValue>, 8> RegsToPass;
  SmallVector<SDValue, 8> MemOpChains;

  // frame infex to store non-register arguments
  int ArgFrameIdx;
  // Arguments offset in frame infex. Offset starts always from 0
  int64_t ArgOffset = 0;

  // get pointer to xpic register info:
  const xpicRegisterInfo  *xRI = ( const xpicRegisterInfo  *) MF.getTarget().getRegisterInfo();
  const xpicFrameLowering *xFL = ( const xpicFrameLowering  *) MF.getTarget().getFrameLowering();
  for (unsigned i = 0, e = Outs.size(); i != e; ++i)
  {
    SDValue Val = OutVals[i];
    EVT ObjectVT = Outs[i].VT;
    SDValue ValToStore(0, 0);
    uint64_t ObjSize;

    switch (ObjectVT.getSimpleVT().SimpleTy) {
    default: assert(0 && "Unhandled argument type!");
    case MVT::i32:
      ObjSize = 4;
      if(ArgLocs[i].isRegLoc() &&  !(Outs[i].IsFixed==false && IsVarArg==true) )
      {// assign to arguments registers
        RegsToPass.push_back(std::make_pair( ArgLocs[ RegsToPass.size() ].getLocReg(), Val));
      } 
      else 
      {// will pass through stack
        ValToStore = Val;
      }
      break;
// f32 f64 i64 deleted
    }

    // have we function arguments through stack?
    if (ValToStore.getNode()) 
    {
      // argument must pass through stack:
      if(!xFL->xpicLowerCallInfo.IsCurrentFn(MF))
      {// we are first time in current function (MF), so save its unique ID (or name)
        xFL->xpicLowerCallInfo.uFunctionID = MF.getFunctionNumber();
        xFL->xpicLowerCallInfo.isSet = true;
        // allocate frame for arguments and keep its index
        ArgFrameIdx = MF.getFrameInfo()->CreateStackObject(ObjSize, 4 ,1);
        xFL->xpicLowerCallInfo.iCallFI = ArgFrameIdx;
      }
      else
      {
        // get saved frame for arguments
        ArgFrameIdx = xFL->xpicLowerCallInfo.iCallFI;
        // its size
        int64_t Offset = MF.getFrameInfo()->getObjectSize(ArgFrameIdx);
        // size can be allocated by previous call
        if(ArgOffset >= Offset)
          // if not enouth, resize more
          MF.getFrameInfo()->setObjectSize(ArgFrameIdx,Offset+ObjSize);
      }
      // add to chain construction "ADD FI + ArgOffset"
      SDValue FIPtr = DAG.getFrameIndex(ArgFrameIdx, MVT::i32);
      SDValue PtrOff   = DAG.getConstant(ArgOffset, MVT::i32);
      PtrOff = DAG.getNode(ISD::ADD,dl, MVT::i32, FIPtr, PtrOff);
      MemOpChains.push_back(DAG.getStore(Chain, dl,ValToStore, PtrOff, MachinePointerInfo(),false, false, 0));

      // adjust offset:
      ArgOffset += ObjSize;
    }
  }

  // Emit all stores, make sure the occur before any copies into physregs.
  if (!MemOpChains.empty())
    Chain = DAG.getNode(ISD::TokenFactor,dl, MVT::Other,&MemOpChains[0], MemOpChains.size());

  // Build a sequence of copy-to-reg nodes chained together with token 
  // chain and flag operands which copy the outgoing args into registers.
  // The InFlag in necessary since all emited instructions must be
  // stuck together.
  SDValue InFlag;
  for (unsigned i = 0, e = RegsToPass.size(); i != e; ++i) 
  {
    unsigned Reg = RegsToPass[i].first;
    Chain = DAG.getCopyToReg(Chain, dl, Reg, RegsToPass[i].second, InFlag);
    InFlag = Chain.getValue(1);
  }

  // If the callee is a GlobalAddress node (quite common, every direct call is)
  // turn it into a TargetGlobalAddress node so that legalize doesn't hack it.
  // Likewise ExternalSymbol -> TargetExternalSymbol.
  if (GlobalAddressSDNode *G = dyn_cast<GlobalAddressSDNode>(Callee))
    Callee = DAG.getTargetGlobalAddress(G->getGlobal(), dl, MVT::i32);
  else if (ExternalSymbolSDNode *E = dyn_cast<ExternalSymbolSDNode>(Callee))
    Callee = DAG.getTargetExternalSymbol(E->getSymbol(), MVT::i32);

  // Returns a chain & a flag for retval copy to use.
  SDVTList NodeTys = DAG.getVTList(MVT::Other, MVT::Glue);
  SmallVector<SDValue, 8> Ops;
  Ops.push_back(Chain);
  Ops.push_back(Callee);

  /// these 3 lines are used for output registers handling
  // Assign locations to each value returned by this call.
  SmallVector<CCValAssign, 16> RVLocs;
  CCState RVInfo(CallConv, IsVarArg, DAG.getMachineFunction(), DAG.getTarget(), RVLocs, *DAG.getContext());
  RVInfo.AnalyzeCallResult(Ins, RetCC_xpic32);

  // Add argument registers to the end of the list so that they are
  // known live into the call.
  for (unsigned i = 0, e = RegsToPass.size(); i != e; ++i)
    Ops.push_back(DAG.getRegister(RegsToPass[i].first,
                                  RegsToPass[i].second.getValueType()));

  if (InFlag.getNode())
    Ops.push_back(InFlag);

  //std::vector<EVT> NodeTys;
  //NodeTys.push_back(MVT::Other);   // Returns a chain
  //NodeTys.push_back(MVT::Flag);    // Returns a flag for retval copy to use.
  //SDValue Ops[] = { Chain, Callee, InFlag };

  Chain = DAG.getNode(XPICISD::CALL,dl, NodeTys, &Ops[0], Ops.size());

  InFlag = Chain.getValue(1);

  Chain = DAG.getCALLSEQ_END(Chain, DAG.getIntPtrConstant(ArgsSize, true),DAG.getIntPtrConstant(0, true), InFlag, dl);
  InFlag = Chain.getValue(1);

  ///output registers handling:
  // do not accept tail calls
  isTailCall=false;

  MachineRegisterInfo &OutRegInfo = MF.getRegInfo();
  // Copy all of the result registers out of their specified physreg.
  for (unsigned i = 0; i != RVLocs.size(); ++i) 
  {
    unsigned Reg = RVLocs[i].getLocReg();
    Chain = DAG.getCopyFromReg(Chain,dl,Reg,RVLocs[i].getValVT(), InFlag).getValue(1);
    InFlag = Chain.getValue(2);
    InVals.push_back(Chain.getValue(0));
    OutRegInfo.setPhysRegUsed(Reg);/// set output register as used, it holds output result value
  }

  return Chain;
}

///  I N P U T    A R G U M E N T S    I N    F U N C T I O N
SDValue xpicTargetLowering::LowerFormalArguments(SDValue Chain,
                                                CallingConv::ID CallConv, 
                                                bool IsVarArg,
                                                const SmallVectorImpl<ISD::InputArg>&Ins,
                                                SDLoc dl, 
                                                SelectionDAG &DAG,
                                                SmallVectorImpl<SDValue> &InVals) const
{
#ifdef DEBUG_SHOW_FNS_NAMES  
printf("xpicTargetLowering::LowerFormalArguments\n");
#endif

  MachineFunction &MF = DAG.getMachineFunction();
  MachineRegisterInfo &RegInfo = MF.getRegInfo();
  xpicMachineFunctionInfo *FuncInfo = MF.getInfo<xpicMachineFunctionInfo>();

  // get pointer to xpic register info:
  const xpicRegisterInfo  *xRI = ( const xpicRegisterInfo  *) MF.getTarget().getRegisterInfo();
  const xpicFrameLowering *xFL = ( const xpicFrameLowering  *) MF.getTarget().getFrameLowering();
  // and initialize function ID as NULL (no function)
  // initialized here, used in "LowerCall()" {and "xpicRegisterInfo::AdjustStackForArguments"},
  // because "LowerFormalArguments()" will be called allways before "LowerCall()"
  xFL->xpicLowerCallInfo.uFunctionID = 0;
  xFL->xpicLowerCallInfo.isSet = false;

  // Assign locations to all of the incoming arguments.
  SmallVector<CCValAssign, 16> ArgLocs;
  CCState CCInfo(CallConv, IsVarArg, DAG.getMachineFunction(), getTargetMachine(),ArgLocs, *DAG.getContext());
  CCInfo.AnalyzeFormalArguments(Ins, CC_xpic32);

  // Initial frame offset to load arguments through stack:
  int64_t FrameLoadOffset = 4;

  std::vector<SDValue> OutChains;

  for (unsigned i = 0, e = ArgLocs.size(); i != e; ++i)
  {
    CCValAssign &VA = ArgLocs[i];
    EVT ObjectVT = VA.getValVT();

    switch (ObjectVT.getSimpleVT().SimpleTy)
    {
      default: assert(0 && "Unhandled argument type!");
      case MVT::i1:
      case MVT::i8:
      case MVT::i16:
      case MVT::i32:
        if( i < NUM_REG_ARG_IN_FN )
        {// arguments lives in an incoming GPR
          unsigned VReg = RegInfo.createVirtualRegister(&XPIC::IntRegsRegClass);
          MF.getRegInfo().addLiveIn(VA.getLocReg(), VReg);
          SDValue Arg = DAG.getCopyFromReg(Chain,dl,VReg, MVT::i32);
          if (ObjectVT != MVT::i32)
          {
            unsigned AssertOp = ISD::AssertSext;
            Arg = DAG.getNode(AssertOp,dl, MVT::i32, Arg,DAG.getValueType(ObjectVT));
            Arg = DAG.getNode(ISD::TRUNCATE,dl, ObjectVT, Arg);
          }
          InVals.push_back(Arg);
        }
        else 
        {// arguments lives in stack
          int FrameIdx = MF.getFrameInfo()->CreateFixedObject(4, FrameLoadOffset,0);
          SDValue FIPtr = DAG.getFrameIndex(FrameIdx, MVT::i32);
          // offset or next argument:
          FrameLoadOffset += 4;

          SDValue Load;
          if (ObjectVT == MVT::i32) 
          {
            Load = DAG.getLoad(MVT::i32,dl, Chain, FIPtr,  MachinePointerInfo(), false, false, false, 0);

          }
          else 
          {// I am not sure that programm will comes here
            assert( 0 && "LowerFormalArguments: ObjectVT != MVT::i32");
            ISD::LoadExtType LoadOp = ISD::SEXTLOAD;
            unsigned Offset = 4-std::max(1U, ObjectVT.getSizeInBits()/8);
            FIPtr = DAG.getNode(ISD::ADD, dl, MVT::i32, FIPtr, DAG.getConstant(Offset, MVT::i32));
            Load = DAG.getExtLoad(LoadOp, dl, MVT::i32, Chain, FIPtr, MachinePointerInfo(), ObjectVT,false, false, 0);
            Load = DAG.getNode(ISD::TRUNCATE,dl, ObjectVT, Load);
          }
          InVals.push_back(Load);
        }
      break;
    }
  }

  // if this function with variable arguments, store frame index for VarArg initialisation
  if(IsVarArg )
    FuncInfo->setVarArgsFrameOffset(MF.getFrameInfo()->CreateFixedObject(4, FrameLoadOffset,0));

  if (!OutChains.empty()) 
  {
    OutChains.push_back(Chain);
    Chain = DAG.getNode(ISD::TokenFactor, dl, MVT::Other,&OutChains[0], OutChains.size());
  }
  return Chain;
}


//===----------------------------------------------------------------------===//
// TargetLowering Implementation
//===----------------------------------------------------------------------===//

/// IntCondCCodeToICC - Convert a DAG integer condition code to a xpic ICC
/// condition.
static XPICCC::CondCodes IntCondCCodeToICC(ISD::CondCode CC) {
  switch (CC) {
  default: assert(0 && "Unknown integer condition code!");
  case ISD::SETEQ:  return XPICCC::ICC_EQ;
  case ISD::SETNE:  return XPICCC::ICC_NE;
  case ISD::SETLT:  return XPICCC::ICC_SM;
  case ISD::SETGT:  return XPICCC::ICC_GR;
  case ISD::SETLE:  return XPICCC::ICC_SE;
  case ISD::SETGE:  return XPICCC::ICC_GE;
  case ISD::SETULT: return XPICCC::ICC_LU;
  case ISD::SETULE: return XPICCC::ICC_LEU;
  case ISD::SETUGT: return XPICCC::ICC_GU;
  case ISD::SETUGE: return XPICCC::ICC_GEU;
  }
}

/// FPCondCCodeToFCC - Convert a DAG floatingp oint condition code to a xpic
/// FCC condition.
static XPICCC::CondCodes FPCondCCodeToFCC(ISD::CondCode CC) {
  switch (CC) {
  default: assert(0 && "Unknown fp condition code!");
  case ISD::SETEQ:
  case ISD::SETOEQ: return XPICCC::FCC_E;
  case ISD::SETNE:
  case ISD::SETUNE: return XPICCC::FCC_NE;
  case ISD::SETLT:
  case ISD::SETOLT: return XPICCC::FCC_L;
  case ISD::SETGT:
  case ISD::SETOGT: return XPICCC::FCC_GE;
  case ISD::SETLE:
  case ISD::SETOLE: return XPICCC::FCC_LE;
  case ISD::SETGE:
  case ISD::SETOGE: return XPICCC::FCC_GE;
  case ISD::SETULT: return XPICCC::FCC_UL;
  case ISD::SETULE: return XPICCC::FCC_ULE;
  case ISD::SETUGT: return XPICCC::FCC_UG;
  case ISD::SETUGE: return XPICCC::FCC_UGE;
  case ISD::SETUO:  return XPICCC::FCC_U;
  case ISD::SETO:   return XPICCC::FCC_O;
  case ISD::SETONE: return XPICCC::FCC_LG;
  case ISD::SETUEQ: return XPICCC::FCC_UE;
  }
}



xpicTargetLowering::xpicTargetLowering(TargetMachine &TM)
  : TargetLowering(TM, new TargetLoweringObjectFileELF()) {
    //Subtarget = &TM.getSubtarget<xpicSubtarget>();


  // Set up the register classes.
  addRegisterClass(MVT::i32, &XPIC::IntRegsRegClass);
  addRegisterClass(MVT::i32, &XPIC::WRegRegClass);
  //addRegisterClass(MVT::f32, XPIC::IntRegsRegisterClass);
  //addRegisterClass(MVT::f64, XPIC::IntRegsRegisterClass);

  // Turn FP extload into load/fextend
  setLoadExtAction(ISD::EXTLOAD, MVT::f32, Expand);
  // xpic doesn't have i1 sign extending load
  setLoadExtAction(ISD::SEXTLOAD, MVT::i1, Promote);
  setLoadExtAction(ISD::ZEXTLOAD, MVT::i1, Promote);
  // Turn FP truncstore into trunc + store.
  setTruncStoreAction(MVT::f64, MVT::f32, Expand);

  // Custom legalize GlobalAddress nodes into LO/HI parts.
  setOperationAction(ISD::GlobalAddress,    MVT::i32, Custom);
  setOperationAction(ISD::GlobalTLSAddress, MVT::i32, Custom);
  setOperationAction(ISD::ConstantPool,     MVT::i32, Custom);
  
  // xpic has no sign_extend_inreg, replace them with shl/sra
  setOperationAction(ISD::SIGN_EXTEND_INREG, MVT::i16, Expand);
  setOperationAction(ISD::SIGN_EXTEND_INREG, MVT::i8 , Expand);
  setOperationAction(ISD::SIGN_EXTEND_INREG, MVT::i1 , Expand);

  // xpic has no REM or DIV operations -> use runtime library function.
  setOperationAction(ISD::SDIV, MVT::i32, Expand);
  setOperationAction(ISD::UDIV, MVT::i32, Expand);
  setOperationAction(ISD::UREM, MVT::i32, Expand);
  setOperationAction(ISD::SREM, MVT::i32, Expand);
  setOperationAction(ISD::SDIVREM, MVT::i32, Expand);
  setOperationAction(ISD::UDIVREM, MVT::i32, Expand);

  // xpic has no type conversion float<->int
  setOperationAction(ISD::FP_TO_SINT, MVT::i32, Expand);
  setOperationAction(ISD::SINT_TO_FP, MVT::i32, Expand);
  setOperationAction(ISD::FP_TO_UINT, MVT::i32, Expand);
  setOperationAction(ISD::UINT_TO_FP, MVT::i32, Expand);
  
  setOperationAction(ISD::BITCAST, MVT::f32, Expand);
  setOperationAction(ISD::BITCAST, MVT::i32, Expand);
  
  // xpic has no select or setcc: expand to SELECT_CC.
  setOperationAction(ISD::SELECT, MVT::i32, Expand);
  setOperationAction(ISD::SELECT, MVT::f32, Expand);
  setOperationAction(ISD::SELECT, MVT::f64, Expand);
  setOperationAction(ISD::SETCC, MVT::i32, Expand);
  setOperationAction(ISD::SETCC, MVT::f32, Expand);
  setOperationAction(ISD::SETCC, MVT::f64, Expand);
  
  // BI: is BRCOND = jmp r0z, label ???
  setOperationAction(ISD::BRCOND, MVT::Other, Expand);
  // BI: is BRIND = mov pc, r0, z0 ???
  setOperationAction(ISD::BRIND, MVT::Other, Expand);
  // BI: is BR_JT = load pc, [r0 + r1] ???
  setOperationAction(ISD::BR_JT, MVT::Other, Expand);
  setOperationAction(ISD::BR_CC, MVT::i32, Custom);
  setOperationAction(ISD::BR_CC, MVT::f32, Custom);
  setOperationAction(ISD::BR_CC, MVT::f64, Custom);
  
  setOperationAction(ISD::SELECT_CC, MVT::i32, Custom);
  setOperationAction(ISD::SELECT_CC, MVT::f32, Custom);
  setOperationAction(ISD::SELECT_CC, MVT::f64, Custom);
  
  // xpic has no intrinsics for these particular operations.
  setOperationAction(ISD::ATOMIC_FENCE, MVT::Other, Expand);

  setOperationAction(ISD::FSIN , MVT::f64, Expand);
  setOperationAction(ISD::FCOS , MVT::f64, Expand);
  setOperationAction(ISD::FREM , MVT::f64, Expand);
  setOperationAction(ISD::FSIN , MVT::f32, Expand);
  setOperationAction(ISD::FCOS , MVT::f32, Expand);
  setOperationAction(ISD::FREM , MVT::f32, Expand);
  setOperationAction(ISD::CTPOP, MVT::i32, Expand);
  setOperationAction(ISD::CTTZ , MVT::i32, Expand);
  setOperationAction(ISD::CTTZ_ZERO_UNDEF, MVT::i32, Expand);
  //BI: clz is supported by xPIC! setOperationAction(ISD::CTLZ , MVT::i32, Expand);
  setOperationAction(ISD::CTLZ_ZERO_UNDEF, MVT::i32, Expand);
  //BI: rol is supported by xPIC! setOperationAction(ISD::ROTL , MVT::i32, Expand);
  setOperationAction(ISD::ROTR , MVT::i32, Expand);
  setOperationAction(ISD::BSWAP, MVT::i32, Expand);
  setOperationAction(ISD::FCOPYSIGN, MVT::f64, Expand);
  setOperationAction(ISD::FCOPYSIGN, MVT::f32, Expand);
  setOperationAction(ISD::FPOW , MVT::f64, Expand);
  setOperationAction(ISD::FPOW , MVT::f32, Expand);

  setOperationAction(ISD::SHL_PARTS, MVT::i32, Expand);
  setOperationAction(ISD::SRA_PARTS, MVT::i32, Expand);
  setOperationAction(ISD::SRL_PARTS, MVT::i32, Expand);

  // FIXME: xpic provides these multiplies, please customize them.
  setOperationAction(ISD::UMUL_LOHI, MVT::i32, Expand);
  setOperationAction(ISD::SMUL_LOHI, MVT::i32, Expand);
  setOperationAction(ISD::MULHU, MVT::i32, Expand);
  setOperationAction(ISD::MULHS, MVT::i32, Expand);


  // VASTART needs to be custom lowered to use the VarArgsFrameIndex.
  setOperationAction(ISD::VASTART           , MVT::Other, Custom);
  setOperationAction(ISD::VAARG             , MVT::Other, Custom);
  
  // Use the default implementation.
  setOperationAction(ISD::VACOPY            , MVT::Other, Expand);
  setOperationAction(ISD::VAEND             , MVT::Other, Expand);
  setOperationAction(ISD::STACKSAVE         , MVT::Other, Expand); 
  setOperationAction(ISD::STACKRESTORE      , MVT::Other, Expand);
  setOperationAction(ISD::DYNAMIC_STACKALLOC, MVT::i32  , Custom); // alloca()?
    
  // We have line number support:
/*
  setOperationAction(ISD::DBG_STOPPOINT, MVT::Other, Expand);
  setOperationAction(ISD::DEBUG_LOC, MVT::Other, Expand);
  setOperationAction(ISD::DBG_LABEL, MVT::Other, Expand);
*/
  setOperationAction(ISD::EH_LABEL, MVT::Other, Expand);
  // Debug info support
  //setOperationAction(ISD::DECLARE, MVT::Other, Expand);
    
  setStackPointerRegisterToSaveRestore(XPIC::r7);
  
  computeRegisterProperties();
}

const char *xpicTargetLowering::getTargetNodeName(unsigned Opcode) const {
  switch (Opcode) {
  default: return "XPICISD::???";
  case XPICISD::CMPICC:     return "XPICISD::CMPICC";
  case XPICISD::CMPFCC:     return "XPICISD::CMPFCC";
  case XPICISD::BRICC:      return "XPICISD::BRICC";
  case XPICISD::BRFCC:      return "XPICISD::BRFCC";
  case XPICISD::SELECT_ICC: return "XPICISD::SELECT_ICC";
  case XPICISD::SELECT_FCC: return "XPICISD::SELECT_FCC";
//  case XPICISD::FTOI:       return "XPICISD::FTOI";
//  case XPICISD::ITOF:       return "XPICISD::ITOF";
  case XPICISD::CALL:       return "XPICISD::CALL";
  case XPICISD::RET_FLAG:   return "XPICISD::RET_FLAG";
  case XPICISD::RETF:       return "XPICISD::RETF";
  case XPICISD::RETRELI:    return "XPICISD::RETRELI";
  case XPICISD::RETI:       return "XPICISD::RETI";
  }
}

/// isMaskedValueZeroForTargetNode - Return true if 'Op & Mask' is known to
/// be zero. Op is expicted to be a target specific node. Used by DAG
/// combiner.
void xpicTargetLowering::computeMaskedBitsForTargetNode(const SDValue Op,
                                                         APInt &KnownZero, 
                                                         APInt &KnownOne,
                                                         const SelectionDAG &DAG,
                                                         unsigned Depth) const {
  APInt KnownZero2, KnownOne2;
  KnownZero = KnownOne = APInt(KnownZero.getBitWidth(), 0);   // Don't know anything.
  
  switch (Op.getOpcode()) {
  default: break;
  case XPICISD::SELECT_ICC:
  case XPICISD::SELECT_FCC:
    DAG.ComputeMaskedBits(Op.getOperand(1), KnownZero, KnownOne, Depth+1);
    DAG.ComputeMaskedBits(Op.getOperand(0), KnownZero2, KnownOne2, Depth+1);

    assert((KnownZero & KnownOne) == 0 && "Bits known to be one AND zero?"); 
    assert((KnownZero2 & KnownOne2) == 0 && "Bits known to be one AND zero?"); 
    
    // Only known if known in both the LHS and RHS.
    KnownOne &= KnownOne2;
    KnownZero &= KnownZero2;
    break;
  }
}

// Look at LHS/RHS/CC and see if they are a lowered setcc instruction.  If so
// set LHS/RHS and XPICCC to the LHS/RHS of the setcc and XPICCC to the condition.
static void LookThroughSetCC(SDValue &LHS, SDValue &RHS,
                             ISD::CondCode CC, unsigned &XPICCC) {
  if (isa<ConstantSDNode>(RHS) &&
      cast<ConstantSDNode>(RHS)->getZExtValue() == 0 &&
      CC == ISD::SETNE && 
      ((LHS.getOpcode() == XPICISD::SELECT_ICC &&
        LHS.getOperand(3).getOpcode() == XPICISD::CMPICC) ||
       (LHS.getOpcode() == XPICISD::SELECT_FCC &&
        LHS.getOperand(3).getOpcode() == XPICISD::CMPFCC)) &&
      isa<ConstantSDNode>(LHS.getOperand(0)) &&
      isa<ConstantSDNode>(LHS.getOperand(1)) &&
      cast<ConstantSDNode>(LHS.getOperand(0))->getZExtValue() == 1 &&
      cast<ConstantSDNode>(LHS.getOperand(1))->getZExtValue() == 0) {
    SDValue CMPCC = LHS.getOperand(3);
    XPICCC = cast<ConstantSDNode>(LHS.getOperand(2))->getZExtValue();
    LHS = CMPCC.getOperand(0);
    RHS = CMPCC.getOperand(1);
  }
}

static SDValue LowerGLOBALADDRESS(SDValue Op, SelectionDAG &DAG) 
{
//SDValueInfo("LowerGLOBALADDRESS",Op,&DAG);
  const GlobalValue *GV = cast<GlobalAddressSDNode>(Op)->getGlobal();
  SDLoc dl(Op);
  SDValue GA = DAG.getTargetGlobalAddress(GV,  dl, MVT::i32);
  // do not lower debug info
//  if (DAG.isVerifiedDebugInfoDesc(Op))
//   return GA;

/*
  SDNode *N = Op.getNode();
  if(N->hasOneUse())
  {
   SDNode *use = *(N->use_begin());
   if(use->getOpcode()==ISD::DECLARE)
   return GA;
  }
*/

  SDValue ZeroReg = DAG.getRegister(XPIC::z0, MVT::i32);
  return DAG.getNode(ISD::ADD,dl, MVT::i32, GA, ZeroReg);
}

static SDValue LowerCONSTANTPOOL(SDValue Op, SelectionDAG &DAG) 
{
#ifdef DEBUG_SHOW_FNS_NAMES  
printf("LowerCONSTANTPOOL()\n");
#endif
  assert(0 && "\"LowerCONSTANTPOOL\" Is it  really launched by You? Let me know: tema13tema@yahoo.de \n");
}

/*
static SDValue LowerFP_TO_SINT(SDValue Op, SelectionDAG &DAG) {
  // Convert the fp value to integer in an FP register.
  DebugLoc dl = Op.getDebugLoc();
  assert(Op.getValueType() == MVT::i32);
  Op = DAG.getNode(XPICISD::FTOI,dl, MVT::f32, Op.getOperand(0));
  return DAG.getNode(ISD::BIT_CONVERT,dl, MVT::i32, Op);
}

static SDValue LowerSINT_TO_FP(SDValue Op, SelectionDAG &DAG) {
  DebugLoc dl = Op.getDebugLoc();
  assert(Op.getOperand(0).getValueType() == MVT::i32);
  SDValue Tmp = DAG.getNode(ISD::BIT_CONVERT,dl, MVT::f32, Op.getOperand(0));
  // Convert the int value to FP in an FP register.
  return DAG.getNode(XPICISD::ITOF,dl, Op.getValueType(), Tmp);
}
*/

static SDValue LowerBR_CC(SDValue Op, SelectionDAG &DAG) {
#ifdef DEBUG_SHOW_FNS_NAMES  
printf("LowerBR_CC()\n");
#endif
  SDLoc dl(Op);
  SDValue Chain = Op.getOperand(0);
  ISD::CondCode CC = cast<CondCodeSDNode>(Op.getOperand(1))->get();
  SDValue LHS = Op.getOperand(2);
  SDValue RHS = Op.getOperand(3);
  SDValue Dest = Op.getOperand(4);
  unsigned Opc, XPICCC = ~0U;

  // If this is a br_cc of a "setcc", and if the setcc got lowered into
  // an CMP[IF]CC/SELECT_[IF]CC pair, find the original compared values.
  LookThroughSetCC(LHS, RHS, CC, XPICCC);

  // Get the condition flag.
  SDValue CompareFlag;
  if (LHS.getValueType() == MVT::i32) 
  {
    //std::vector<MVT> VTs;
    //VTs.push_back(MVT::i32);
    //VTs.push_back(MVT::Flag);
    if (XPICCC == ~0U) XPICCC = IntCondCCodeToICC(CC);
    Opc = XPICISD::BRICC;
    CompareFlag = DAG.getNode(XPICISD::CMPICC,dl, MVT::Glue, LHS, RHS);
  } 
  else 
  {//float
    CompareFlag = DAG.getNode(XPICISD::CMPFCC,dl, MVT::Glue, LHS, RHS);
    if (XPICCC == ~0U) XPICCC = FPCondCCodeToFCC(CC);
    Opc = XPICISD::BRFCC;
  }

  return DAG.getNode(Opc,dl, MVT::Other, Chain, Dest,DAG.getConstant(XPICCC, MVT::i32), CompareFlag);
}

static SDValue LowerSELECT_CC(SDValue Op, SelectionDAG &DAG) {
#ifdef DEBUG_SHOW_FNS_NAMES  
printf("LowerSELECT_CC()\n");
#endif

  SDLoc dl(Op);
  SDValue LHS = Op.getOperand(0);
  SDValue RHS = Op.getOperand(1);
  ISD::CondCode CC = cast<CondCodeSDNode>(Op.getOperand(4))->get();
  SDValue TrueVal = Op.getOperand(2);
  SDValue FalseVal = Op.getOperand(3);
  unsigned Opc, XPICCC = ~0U;
  
  // If this is a select_cc of a "setcc", and if the setcc got lowered into
  // an CMP[IF]CC/SELECT_[IF]CC pair, find the original compared values.
  LookThroughSetCC(LHS, RHS, CC, XPICCC);
  
  SDValue CompareFlag;
  if (LHS.getValueType() == MVT::i32) {
    //std::vector<MVT> VTs;
 //   VTs.push_back(LHS.getValueType());   // subcc returns a value
    //VTs.push_back(MVT::Flag);
  //  SDValue Ops[2] = { LHS, RHS };
//    CompareFlag = DAG.getNode(XPICISD::CMPICC, VTs, Ops, 2).getValue(1);
    CompareFlag = DAG.getNode(XPICISD::CMPICC,dl, MVT::Glue, LHS, RHS);

    Opc = XPICISD::SELECT_ICC;
    if (XPICCC == ~0U) XPICCC = IntCondCCodeToICC(CC);
  } else {
    CompareFlag = DAG.getNode(XPICISD::CMPFCC,dl, MVT::Glue, LHS, RHS);
    Opc = XPICISD::SELECT_FCC;
    if (XPICCC == ~0U) XPICCC = FPCondCCodeToFCC(CC);
  }
  return DAG.getNode(Opc,dl, TrueVal.getValueType(), TrueVal, FalseVal,DAG.getConstant(XPICCC, MVT::i32), CompareFlag);
}

///  I N I T I A L I S  A T I O N    O F    V A R A R G    I N    F U N C T I O N
static SDValue LowerVASTART(SDValue Op, SelectionDAG &DAG, const xpicTargetLowering &TLI)
{
#ifdef DEBUG_SHOW_FNS_NAMES  
SDValueInfo("LowerVASTART",Op,&DAG); 
#endif 
  MachineFunction &MF = DAG.getMachineFunction();
  xpicMachineFunctionInfo *FuncInfo = MF.getInfo<xpicMachineFunctionInfo>();
  
  // vastart just stores the address of the VarArgsFrameIndex slot into the memory location argument.
  SDLoc dl(Op);
  // Get frame index with arguments:
  SDValue FIPtr = DAG.getFrameIndex(FuncInfo->getVarArgsFrameOffset(), MVT::i32);
  // save it into the memory location argument:
  const Value *SrcValue = cast<SrcValueSDNode>(Op.getOperand(2))->getValue();
  return DAG.getStore(Op.getOperand(0),dl, FIPtr, Op.getOperand(1), MachinePointerInfo(SrcValue), false, false, false, 0);
}

static SDValue LowerVAARG(SDValue Op, SelectionDAG &DAG) {
  SDNode *Node = Op.getNode();
  EVT VT = Node->getValueType(0);
  SDValue InChain = Node->getOperand(0);
  SDValue VAListPtr = Node->getOperand(1);
  const Value *SV = cast<SrcValueSDNode>(Node->getOperand(2))->getValue();
  SDLoc dl(Node);

  SDValue VAList = DAG.getLoad(MVT::i32, dl, InChain, VAListPtr, MachinePointerInfo(SV), false, false, false, 0);

  // Increment the pointer, VAList, to the next vaarg
  SDValue NextPtr = DAG.getNode(ISD::ADD, dl, MVT::i32, VAList, DAG.getConstant(VT.getSizeInBits()/8, MVT::i32));

  // Store the incremented VAList to the legalized pointer
  InChain = DAG.getStore(VAList.getValue(1), dl, NextPtr, VAListPtr, MachinePointerInfo(SV), false, false, false, 0);

  // Load the actual argument out of the pointer VAList, unless this is an
  // f64 load.
  if (VT != MVT::f64)
    return DAG.getLoad(VT, dl, InChain, VAList, MachinePointerInfo(), false, false, false, 0);

  // Otherwise, load it as i64, then do a bitconvert.
  SDValue V = DAG.getLoad(MVT::i64, dl, InChain, VAList, MachinePointerInfo(), false, false, false, 0);

  // Bit-Convert the value to f64.
  SDValue Ops[2] = {
    DAG.getNode(ISD::BITCAST, dl, MVT::f64, V),
    V.getValue(1)
  };
  return DAG.getMergeValues(Ops, 2, dl);
}

static SDValue LowerDYNAMIC_STACKALLOC(SDValue Op, SelectionDAG &DAG) {
#ifdef DEBUG_SHOW_FNS_NAMES  
printf("LowerDYNAMIC_STACKALLOC() \n");
#endif
  fprintf(stderr,"error: memory allocation on the stack not supported!\n");
  exit(1);
//  assert(0 && " ");

  SDLoc dl(Op);
  SDValue Chain = Op.getOperand(0);  // Legalize the chain.
  SDValue Size  = Op.getOperand(1);  // Legalize the size.
  
  unsigned SPReg = XPIC::r7;
  SDValue SP = DAG.getCopyFromReg(Chain,dl, SPReg, MVT::i32);
  SDValue NewSP = DAG.getNode(ISD::SUB,dl, MVT::i32, SP, Size);    // Value
  Chain = DAG.getCopyToReg(SP.getValue(1),dl, SPReg, NewSP);      // Output chain
  
  // The resultant pointer is actually 16 words from the bottom of the stack,
  // to provide a register spill area.
  SDValue NewVal = DAG.getNode(ISD::ADD,dl, MVT::i32, NewSP,DAG.getConstant(96, MVT::i32));
  SDValue Ops[2] = { NewVal, Chain };
  return DAG.getMergeValues(Ops, 2,dl);
}


SDValue xpicTargetLowering::LowerOperation(SDValue Op, SelectionDAG &DAG) const {
// xpicInfo("__LowerOperation:",Op,&DAG);
//SDValueInfo("__LowerOperation",Op,&DAG);
  switch (Op.getOpcode()) {
  default: assert(0 && "Should not custom lower this!");
  // Frame & Return address.  Currently unimplemented
  case ISD::RETURNADDR: return SDValue();
  case ISD::FRAMEADDR:  return SDValue();
  case ISD::GlobalTLSAddress:
    assert(0 && "TLS not implemented for xpic.");
  case ISD::GlobalAddress:      return LowerGLOBALADDRESS(Op, DAG);
  case ISD::ConstantPool:       return LowerCONSTANTPOOL(Op, DAG);
//  case ISD::FP_TO_SINT:         return LowerFP_TO_SINT(Op, DAG);
//  case ISD::SINT_TO_FP:         return LowerSINT_TO_FP(Op, DAG);
  case ISD::BR_CC:              return   LowerBR_CC(Op, DAG);
  case ISD::SELECT_CC:          return LowerSELECT_CC(Op, DAG);
  case ISD::VASTART:            return LowerVASTART(Op, DAG, *this);
  case ISD::VAARG:              return LowerVAARG(Op, DAG);
  case ISD::DYNAMIC_STACKALLOC: return LowerDYNAMIC_STACKALLOC(Op, DAG);
 }
}

MachineBasicBlock * xpicTargetLowering::EmitInstrWithCustomInserter(MachineInstr *MI,
                                                                    MachineBasicBlock *BB ) const{
#ifdef DEBUG_SHOW_FNS_NAMES  
printf("xpicTargetLowering::EmitInstrWithCustomInserter() \n");
#endif
  const TargetInstrInfo &TII = *getTargetMachine().getInstrInfo();
  unsigned BROpcode;
  unsigned CC;
  DebugLoc dl = MI->getDebugLoc();
  // Figure out the conditional branch opcode to use for this select_cc.
  switch (MI->getOpcode()) {
    default: llvm_unreachable("Unknown SELECT_CC!");
    case XPIC::SELECT_CC_Int_ICC:
      BROpcode = XPIC::xBCOND;
      break;
/*
  case XPIC::SELECT_CC_Int_FCC:
    BROpcode = XPIC::FBCOND;
    break;
*/


    case XPIC::xCALL_LOAD:
    case XPIC::xCALL: // Here added "save pc"- instruction!
#ifdef DEBUG_SHOW_FNS_NAMES  
printf("xpicTargetLowering::EmitInstrWithCustomInserter()::xCALL/xCALL_LOAD \n");
#endif

      MachineBasicBlock *thisMBB = BB;
      MachineBasicBlock::iterator I = MI;
      // add native Machine Instruction:
      if(MI->getOpcode() == XPIC::xCALL) {
        if(MI->getOperand(0).isGlobal()) {
          BuildMI(*thisMBB,I,dl,TII.get(XPIC::xCALL_done)).addGlobalAddress(MI->getOperand(0).getGlobal() ,0);
          I++;
        }

        if(MI->getOperand(0).isSymbol()) {
          BuildMI(*thisMBB,I,dl,TII.get(XPIC::xCALL_done)).addExternalSymbol(MI->getOperand(0).getSymbolName() );
          I++;
        }
      }
      if(MI->getOpcode() == XPIC::xCALL_LOAD) {
        if(MI->getOperand(0).isGlobal()) {
          BuildMI(*thisMBB,I,dl,TII.get(XPIC::xCALL_LOAD_done)).addGlobalAddress(MI->getOperand(0).getGlobal() ,0);
          I++;
        }

        if(MI->getOperand(0).isSymbol()) {
          BuildMI(*thisMBB,I,dl,TII.get(XPIC::xCALL_LOAD_done)).addExternalSymbol(MI->getOperand(0).getSymbolName() );
          I++;
        }
      }

      if(MI->getOpcode() == XPIC::xCALL_LOAD) {       
        BuildMI(*thisMBB,I,dl,TII.get(XPIC::xCALL_LOAD_done)).addReg(MI->getOperand(0).getReg());
      }
      // well done! then return...
      MI->eraseFromParent();
      return BB;
  }

  //fprintf(stderr,"------------> OpCode:%d, NumOp=%d,\n",MI->getOpcode(),MI->getNumOperands());

  CC = (XPICCC::CondCodes)MI->getOperand(3).getImm();
  
  // To "insert" a SELECT_CC instruction, we actually have to insert the diamond
  // control-flow pattern.  The incoming instruction knows the destination vreg
  // to set, the condition code register to branch on, the true/false values to
  // select between, and a branch opcode to use.
  const BasicBlock *LLVM_BB = BB->getBasicBlock();
  MachineFunction::iterator It = BB;
  ++It;
  
  unsigned int TrueVal = MI->getOperand(1).getReg();
  unsigned int FalseVal = MI->getOperand(2).getReg();

  //  thisMBB:
  //  ...
  //   TrueVal = ...
  //   [f]bCC copy1MBB
  //   fallthrough --> copy0MBB
  MachineBasicBlock *thisMBB = BB;
  MachineFunction *F = BB->getParent();
  MachineBasicBlock *copy0MBB = F->CreateMachineBasicBlock(LLVM_BB);
  MachineBasicBlock *sinkMBB = F->CreateMachineBasicBlock(LLVM_BB);
  F->insert(It, copy0MBB);
  F->insert(It, sinkMBB);
  
  // Update machine-CFG edges by transferring all successors of the current
  // block to the new block which will contain the Phi node for the select.
  sinkMBB->splice(sinkMBB->begin(), BB,
                   llvm::next(MachineBasicBlock::iterator(MI)),
                   BB->end());
  sinkMBB->transferSuccessorsAndUpdatePHIs(BB);
  // Next, add the true and fallthrough blocks as its successors.
  thisMBB->addSuccessor(copy0MBB);
  thisMBB->addSuccessor(sinkMBB);


  // insert copy node for physregs before the compare because the PHI node cannot handle physregs
  MachineBasicBlock::iterator I = BB->end();
  if( TrueVal < XPIC::NUM_TARGET_REGS ) {
    int vreg = F->getRegInfo().createVirtualRegister(&XPIC::IntRegsRegClass);
    F->getRegInfo().addLiveIn(TrueVal, vreg);
    if( TrueVal == XPIC::z0 ){
      BuildMI(*thisMBB, --I, dl, TII.get(XPIC::xMOV), vreg).addReg(XPIC::z0);
    }else{
      BuildMI(*thisMBB, --I, dl, TII.get(XPIC::xMOV), vreg).addReg(XPIC::z0).addReg(TrueVal);
    }
    TrueVal = vreg;
  }

  // insert conditional branch for FalseVal
  BuildMI(thisMBB,dl, TII.get(BROpcode)).addMBB(sinkMBB).addImm(CC);
  
  if( FalseVal < XPIC::NUM_TARGET_REGS ) {
    int vreg = F->getRegInfo().createVirtualRegister(&XPIC::IntRegsRegClass);
    F->getRegInfo().addLiveIn(FalseVal, vreg);
    BuildMI(copy0MBB, dl, TII.get(XPIC::xORrr), vreg).addReg(XPIC::z0).addReg(FalseVal);
    FalseVal = vreg;
  }

  //  copy0MBB:
  //   %FalsVal = ...
  //   # fallthrough to sinkMBB
  BB = copy0MBB;
  // Update machine-CFG edges
  BB->addSuccessor(sinkMBB);
  
  //  sinkMBB:
  //   %Result = phi [ %FalseValue, copy0MBB ], [ %TrueValue, thisMBB ]
  //  ...
  BB = sinkMBB;
  BuildMI(*BB, BB->begin(), dl, TII.get(XPIC::PHI), MI->getOperand(0).getReg())
    .addReg(FalseVal).addMBB(copy0MBB)
    .addReg(TrueVal).addMBB(thisMBB);
  
  MI->eraseFromParent(); 
  return BB;
}

/// getFunctionAlignment - Return the Log2 alignment of this function.
unsigned xpicTargetLowering::getFunctionAlignment(const Function *) const {
  return 2;
}

//===----------------------------------------------------------------------===//
//                         xPIC Inline Assembly Support
//===----------------------------------------------------------------------===//

/// getConstraintType - Given a constraint letter, return the type of
/// constraint it is for this target.
#if 0
xpicTargetLowering::ConstraintType
xpicTargetLowering::getConstraintType(const std::string &Constraint) const {
  if (Constraint.size() == 1) {
    switch (Constraint[0]) {
    default:  break;
    case 'r': return C_RegisterClass;
    }
  }

  return TargetLowering::getConstraintType(Constraint);
}
#endif

std::pair<unsigned, const TargetRegisterClass*>
xpicTargetLowering::getRegForInlineAsmConstraint(const std::string &Constraint, MVT VT) const {
  if (Constraint.size() == 1) {
    switch (Constraint[0]) {
    case 'r':
      return std::make_pair(0U, &XPIC::IntRegsRegClass);
    }
  }

  return TargetLowering::getRegForInlineAsmConstraint(Constraint, VT);
}

bool
xpicTargetLowering::isOffsetFoldingLegal(const GlobalAddressSDNode *GA) const {
  // The xpic target isn't yet aware of offsets.
  return false;
}


