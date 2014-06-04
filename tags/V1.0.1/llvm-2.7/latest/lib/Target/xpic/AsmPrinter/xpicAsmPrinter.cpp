//===-- xpicAsmPrinter.cpp - xpic LLVM assembly writer ------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains a printer that converts from our internal representation
// of machine-dependent LLVM code to GAS-format xpic assembly language.
//
//===----------------------------------------------------------------------===//
#define DEBUG_TYPE "asm-printer"

#include "xpic.h"
#include "xpicInstrInfo.h"
#include "xpicSubtarget.h"
#include "llvm/Constants.h"
#include "llvm/DerivedTypes.h"
#include "llvm/Module.h"

#include "llvm/ADT/SmallPtrSet.h"
#include "llvm/ADT/SmallString.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/ADT/StringExtras.h"

#include "llvm/CodeGen/AsmPrinter.h"
#include "llvm/CodeGen/DwarfWriter.h"
#include "llvm/CodeGen/MachineModuleInfo.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineConstantPool.h"
#include "llvm/CodeGen/MachineInstr.h"

#include "llvm/MC/MCAsmInfo.h"
#include "llvm/MC/MCSection.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/MC/MCSymbol.h"
#include "llvm/MC/SectionKind.h"

#include "llvm/Target/TargetData.h"
#include "llvm/Target/TargetLoweringObjectFile.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetRegistry.h"

#include "llvm/Support/CommandLine.h"
#include "llvm/Support/FormattedStream.h"
#include "llvm/Support/MathExtras.h"

#include <cctype>
#include <cstring>
#include <map>
#include <utility>
#include <stdio.h>

using namespace llvm;

namespace {
  class xpicAsmPrinter : public AsmPrinter {

    /// We name each basic block in a Function with a unique number, so
    /// that we can consistently refer to them later. This is cleared
    /// at the beginning of each call to runOnMachineFunction().
    ///
    typedef std::map<const Value *, unsigned> ValueMapTy;
    ValueMapTy NumberForBB;
    unsigned BBNumber;

    const xpicSubtarget &Subtarget;

  public:
    explicit xpicAsmPrinter(formatted_raw_ostream &O, TargetMachine &TM,
                             MCContext &Ctx, MCStreamer &Streamer,
                             const MCAsmInfo *T)
      : AsmPrinter(O, TM, Ctx, Streamer, T), BBNumber(0), Subtarget(TM.getSubtarget<xpicSubtarget>()) {}

    void getAnalysisUsage(AnalysisUsage &AU) const 
    {
      AU.setPreservesAll();
      AU.addRequired<MachineModuleInfo>();
      AU.addRequired<DwarfWriter>();
      AsmPrinter::getAnalysisUsage(AU);
    }

    virtual const char *getPassName() const 
    {
      return "xpic Assembly Printer";
    }


    bool PrintAsmOperand(const MachineInstr *MI, unsigned OpNo,
                         unsigned AsmVariant, const char *ExtraCode);
 //   void PrintGlobalVariable(const GlobalVariable* GVar);
    void printOperand(const MachineInstr *MI, unsigned opNum);
    void printMemOperand(const MachineInstr *MI, int opNum, const char *Modifier = 0);
    void printFrameOperand(const MachineInstr *MI, int opNum,const char *Modifier=0);

    void printCCOperand(const MachineInstr *MI, int opNum);

    virtual void EmitInstruction(const MachineInstr *MI) 
    {
      printInstruction(MI);
      OutStreamer.AddBlankLine();
    }

    void printInstruction(const MachineInstr *MI);  // autogenerated.
    static const char *getRegisterName(unsigned RegNo);

    void printMachineInstruction(const MachineInstr *MI);

//    bool doInitialization(Module &M);
//    bool doFinalization(Module &M);
    void emitGlobalDirective(const std::string &name);
    void EmitAlignment(unsigned NumBits,const GlobalValue *GV,unsigned ForcedAlignBits=0,bool UseFillExpr=true)const;

    void printConstantExpr(const ConstantExpr* CE);

    // function from base class AsmPrinter (it generates bad alignment)
    void EmitGlobalVariable(const GlobalVariable *GV);
    // function from base class AsmPrinter (it is  private)
    void EmitVisibility(MCSymbol *Sym, unsigned Visibility) const;
    // function from base class AsmPrinter (it is  private)
    void EmitLinkage(unsigned Linkage, MCSymbol *GVSym) const;
  };
} // end of anonymous namespace


/// PrintAsmOperand - Print out an operand for an inline asm expression.
///
bool xpicAsmPrinter::PrintAsmOperand(const MachineInstr *MI, unsigned OpNo,
                                     unsigned AsmVariant,
                                     const char *ExtraCode) {
  if (ExtraCode && ExtraCode[0]) {
    if (ExtraCode[1] != 0) return true; // Unknown modifier.

    switch (ExtraCode[0]) {
    default: return true;  // Unknown modifier.
    case 'r':
     break;
    }
  }

  printOperand(MI, OpNo);

  return false;
}

void xpicAsmPrinter::printConstantExpr(const ConstantExpr* CE) 
{
  const Value *left = 0;
  if (CE->getNumOperands()>=1) left = CE->getOperand(0);

  switch (CE->getOpcode()) {
  case Instruction::Trunc:
  case Instruction::ZExt:
  case Instruction::SExt:
  case Instruction::FPTrunc:
  case Instruction::FPExt:
  case Instruction::UIToFP:
  case Instruction::SIToFP:
  case Instruction::FPToUI:
  case Instruction::FPToSI:
  case Instruction::PtrToInt:
  case Instruction::IntToPtr:
  case Instruction::BitCast:
   if(left->getType()->getTypeID()==Type::IntegerTyID)
   {
     const Constant *C = dyn_cast<Constant>(left);
     if(C) EmitGlobalConstant(C);
     else printf("printConstantExpr: Error!\n");
   }
  break;

  default:
    errs() << "Expression = " << *CE << "\n";
    assert(0 && "Invalid constant expression");
  }
}

#include "xpicGenAsmWriter.inc"

void xpicAsmPrinter::printOperand(const MachineInstr *MI, unsigned opNum) 
{
  assert(opNum < MI->getNumOperands() && "printOperand() opNum out of range!");
  const MachineOperand &MO = MI->getOperand (opNum);

  switch (MO.getType()) {
  case MachineOperand::MO_Register:
    if (TargetRegisterInfo::isPhysicalRegister(MO.getReg()))
      O << getRegisterName(MO.getReg());
    else
      O << "%reg" << MO.getReg();
    break;

  case MachineOperand::MO_Immediate:
    O <<"#"<< (int)MO.getImm();
    break;

  case MachineOperand::MO_MachineBasicBlock:
    O << *MO.getMBB()->getSymbol(OutContext);
    break;

  case MachineOperand::MO_GlobalAddress:
    {
      if(MI->getOpcode() != XPIC::xCALL)
       O<<"$";

      O << *GetGlobalValueSymbol(MO.getGlobal());
    }
    break;
  case MachineOperand::MO_ExternalSymbol:
    if(MI->getOpcode() != XPIC::xCALL)
      O <<"$";
    O<< MO.getSymbolName();
    break;
  case MachineOperand::MO_ConstantPoolIndex:
    O << MAI->getPrivateGlobalPrefix() << "CPI" << getFunctionNumber() << "_"
      << MO.getIndex();
    break;
  default:
    O << "<unknown operand type>"; abort (); break;
  }
}

void xpicAsmPrinter::printFrameOperand(const MachineInstr *MI, int opNum, const char *Modifier)
{
  int N=0;
  if(MI->getOperand(opNum).isReg() && (MI->getOperand(opNum).getReg()!=XPIC::z0))
   { printOperand(MI, opNum); N++;}

  // [r0 + zero] -> [r0]
  if(MI->getOperand(opNum+1).isReg() && (MI->getOperand(opNum+1).getReg()==XPIC::z0))
  return;

  if(N) O << " + ";

  printOperand(MI, opNum+1);
}

void xpicAsmPrinter::printMemOperand(const MachineInstr *MI, int opNum, const char *Modifier)
{
  printOperand(MI, opNum);

  // [r0 + zero] -> [r0]
  if(MI->getOperand(opNum+1).isReg() && (MI->getOperand(opNum+1).getReg()==XPIC::z0))
  return;

  O << " + ";
  printOperand(MI, opNum+1);

}

void xpicAsmPrinter::printCCOperand(const MachineInstr *MI, int opNum) {
  int CC = (int)MI->getOperand(opNum).getImm();
  O << xpicCondCodeToString((XPICCC::CondCodes)CC);
}

/*
bool xpicAsmPrinter::doInitialization(Module &M) 
{
  bool Result = AsmPrinter::doInitialization(M);
  return Result;
}
*/

void xpicAsmPrinter::emitGlobalDirective(const std::string &name)
{
  O << MAI->getGlobalDirective() << name;
  O << "\n";
}


// emit custom alignment
void xpicAsmPrinter::EmitAlignment(unsigned NumBits, const GlobalValue *GV,
                               unsigned ForcedAlignBits,
                               bool UseFillExpr) const {

  if (GV && GV->getAlignment())
    NumBits = Log2_32(GV->getAlignment());
  NumBits = std::max(NumBits, ForcedAlignBits);
  
  if (NumBits == 0) return;   // No need to emit alignment.
  if (NumBits  > 2) NumBits = 2; // maximal 4byte aligned!

  if (getCurrentSection()->getKind().isText())
    OutStreamer.EmitCodeAlignment( NumBits);
  else
    OutStreamer.EmitValueToAlignment( NumBits, 0, 1, 0);
}

/// EmitGlobalVariable - Emit the specified global variable to the .s file.
#include "llvm/MC/MCExpr.h"
#include "llvm/Assembly/Writer.h"
void xpicAsmPrinter::EmitGlobalVariable(const GlobalVariable *GV) {
  if (!GV->hasInitializer())   // External globals require no code.
    return;
  
  // Check to see if this is a special global used by LLVM, if so, emit it.
  if (EmitSpecialLLVMGlobal(GV))
    return;

  MCSymbol *GVSym = GetGlobalValueSymbol(GV);
  EmitVisibility(GVSym, GV->getVisibility());

  if (MAI->hasDotTypeDotSizeDirective())
    OutStreamer.EmitSymbolAttribute(GVSym, MCSA_ELF_TypeObject);
  
  SectionKind GVKind = TargetLoweringObjectFile::getKindForGlobal(GV, TM);

  const TargetData *TD = TM.getTargetData();
  unsigned Size = TD->getTypeAllocSize(GV->getType()->getElementType());
  unsigned AlignLog = TD->getPreferredAlignmentLog(GV);

  // set all alignment to 4 bytes:
  if(AlignLog>2)
  AlignLog=2;// 

  // Handle common and BSS local symbols (.lcomm).
  if (GVKind.isCommon() || GVKind.isBSSLocal()) {
    if (Size == 0) Size = 1;   // .comm Foo, 0 is undefined, avoid it.
    
    if (VerboseAsm) {
      WriteAsOperand(OutStreamer.GetCommentOS(), GV,
                     /*PrintType=*/false, GV->getParent());
      OutStreamer.GetCommentOS() << '\n';
    }
    
    // Handle common symbols.
    if (GVKind.isCommon()) {
      // .comm _foo, 42, 4
      OutStreamer.EmitCommonSymbol(GVSym, Size, 1 << AlignLog);
      return;
    }
    
    // Handle local BSS symbols.
    if (MAI->hasMachoZeroFillDirective()) {
      const MCSection *TheSection =
        getObjFileLowering().SectionForGlobal(GV, GVKind, Mang, TM);
      // .zerofill __DATA, __bss, _foo, 400, 5
      OutStreamer.EmitZerofill(TheSection, GVSym, Size, 1 << AlignLog);
      return;
    }
    
    if (MAI->hasLCOMMDirective()) {
      // .lcomm _foo, 42
      OutStreamer.EmitLocalCommonSymbol(GVSym, Size);
      return;
    }
    
    // .local _foo
    OutStreamer.EmitSymbolAttribute(GVSym, MCSA_Local);
    // .comm _foo, 42, 4
    OutStreamer.EmitCommonSymbol(GVSym, Size, 1 << AlignLog);
    return;
  }
  
  const MCSection *TheSection =
    getObjFileLowering().SectionForGlobal(GV, GVKind, Mang, TM);

  // Handle the zerofill directive on darwin, which is a special form of BSS
  // emission.
  if (GVKind.isBSSExtern() && MAI->hasMachoZeroFillDirective()) {
    // .globl _foo
    OutStreamer.EmitSymbolAttribute(GVSym, MCSA_Global);
    // .zerofill __DATA, __common, _foo, 400, 5
    OutStreamer.EmitZerofill(TheSection, GVSym, Size, 1 << AlignLog);
    return;
  }

  OutStreamer.SwitchSection(TheSection);

  EmitLinkage(GV->getLinkage(), GVSym);
  EmitAlignment(AlignLog, GV);

  if (VerboseAsm) {
    WriteAsOperand(OutStreamer.GetCommentOS(), GV,
                   /*PrintType=*/false, GV->getParent());
    OutStreamer.GetCommentOS() << '\n';
  }
  OutStreamer.EmitLabel(GVSym);

  EmitGlobalConstant(GV->getInitializer());

  if (MAI->hasDotTypeDotSizeDirective())
    // .size foo, 42
    OutStreamer.EmitELFSize(GVSym, MCConstantExpr::Create(Size, OutContext));
  
  OutStreamer.AddBlankLine();
}

// function from base class AsmPrinter (it is  private)
void xpicAsmPrinter::EmitVisibility(MCSymbol *Sym, unsigned Visibility) const {
  MCSymbolAttr Attr = MCSA_Invalid;
  
  switch (Visibility) {
  default: break;
  case GlobalValue::HiddenVisibility:
    Attr = MAI->getHiddenVisibilityAttr();
    break;
  case GlobalValue::ProtectedVisibility:
    Attr = MAI->getProtectedVisibilityAttr();
    break;
  }

  if (Attr != MCSA_Invalid)
    OutStreamer.EmitSymbolAttribute(Sym, Attr);
}
// function from base class AsmPrinter (it is  private)
void xpicAsmPrinter::EmitLinkage(unsigned Linkage, MCSymbol *GVSym) const {
  switch ((GlobalValue::LinkageTypes)Linkage) {
  case GlobalValue::CommonLinkage:
  case GlobalValue::LinkOnceAnyLinkage:
  case GlobalValue::LinkOnceODRLinkage:
  case GlobalValue::WeakAnyLinkage:
  case GlobalValue::WeakODRLinkage:
  case GlobalValue::LinkerPrivateLinkage:
    if (MAI->getWeakDefDirective() != 0) {
      // .globl _foo
      OutStreamer.EmitSymbolAttribute(GVSym, MCSA_Global);
      // .weak_definition _foo
      OutStreamer.EmitSymbolAttribute(GVSym, MCSA_WeakDefinition);
    } else if (const char *LinkOnce = MAI->getLinkOnceDirective()) {
      // .globl _foo
      OutStreamer.EmitSymbolAttribute(GVSym, MCSA_Global);
      // FIXME: linkonce should be a section attribute, handled by COFF Section
      // assignment.
      // http://sourceware.org/binutils/docs-2.20/as/Linkonce.html#Linkonce
      // .linkonce discard
      // FIXME: It would be nice to use .linkonce samesize for non-common
      // globals.
      O << LinkOnce;
    } else {
      // .weak _foo
      OutStreamer.EmitSymbolAttribute(GVSym, MCSA_Weak);
    }
    break;
  case GlobalValue::DLLExportLinkage:
  case GlobalValue::AppendingLinkage:
    // FIXME: appending linkage variables should go into a section of
    // their name or something.  For now, just emit them as external.
  case GlobalValue::ExternalLinkage:
    // If external or appending, declare as a global symbol.
    // .globl _foo
    OutStreamer.EmitSymbolAttribute(GVSym, MCSA_Global);
    break;
  case GlobalValue::PrivateLinkage:
  case GlobalValue::InternalLinkage:
    break;
  default:
    llvm_unreachable("Unknown linkage type!");
  }
}
// Force static initialization.
extern "C" void LLVMInitializexpicAsmPrinter() { 
  RegisterAsmPrinter<xpicAsmPrinter> X(TheXpicTarget);
}
