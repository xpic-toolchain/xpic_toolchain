//===-- xpic.h - Top-level interface for xpic representation --*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the entry points for global functions defined in the LLVM
// xpic back-end.
//
//===----------------------------------------------------------------------===//

//#define DEBUG_SHOW_FNS_NAMES


#ifndef TARGET_XPIC_H
#define TARGET_XPIC_H

#include "MCTargetDesc/xpicMCTargetDesc.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Target/TargetMachine.h"
#include <cassert>
#include <iosfwd>

namespace llvm {
  class FunctionPass;
  class xpicTargetMachine;
  class formatted_raw_ostream;

  FunctionPass *createxpicISelDag(xpicTargetMachine &TM,CodeGenOpt::Level OptLevel);
  FunctionPass *createxpicDelaySlotFillerPass(TargetMachine &TM);

} // end namespace llvm;

namespace llvm {
  // Enumerations corresponding to sizes of constants, that possible to load from instruction
  namespace XPIC_CONSTANTS 
  {
    enum ConstantsLimits{
    CONST_V_MAX =   16383,	// add r,  #V, r
    CONST_V_MIN =  -16384,	// add r, #-V, r
    CONST_Z_MAX =   262143,	// load r, #Z, r
    CONST_Z_MIN =  -262144	// load r,#-Z, r
                        };
  }

  // Enums corresponding to xpic condition codes, both icc's and fcc's.  These
  // values must be kept in sync with the ones in the .td file.
  namespace XPICCC {
    enum CondCodes {
      ICC_A   =  8   ,  // Always
      ICC_N   =  0   ,  // Never
      ICC_EQ  =  1   ,  // Equal
      ICC_NE  =  9   ,  // Not Equal
      ICC_SM  =  3   ,  // smaller // Less Then
      ICC_GR  = 10   ,  // Greater
      ICC_SE  =  2   ,  // smaller or equal //Less or Equal
      ICC_GE  = 11   ,  // Greater or Equal
      ICC_LU  =  5   ,  // Less Unsigned
      ICC_LEU =  4   ,  // Less or Equal Unsigned
      ICC_GU  = 12   ,  // Greater Unsigned
      ICC_GEU = 13   ,  // Great or Equal Unsigned/ Carry Clear
      ICC_POS = 14   ,  // Positive
      ICC_NEG =  6   ,  // Negative
      ICC_VC  = 15   ,  // Overflow Clear
      ICC_VS  =  7   ,  // Overflow Set


      
      //FCC_A   =  8+16,  // Always
      //FCC_N   =  0+16,  // Never
      FCC_U   =  7+16,  // Unordered
      FCC_G   =  6+16,  // Greater
      FCC_UG  =  5+16,  // Unordered or Greater
      FCC_L   =  4+16,  //Less
      FCC_UL  =  3+16,  // Unordered or Less
      FCC_LG  =  2+16,  // Less or Greater
      FCC_NE  =  1+16,  // Not Equal
      FCC_E   =  9+16,  // Equal
      FCC_UE  = 10+16,  // Unordered or Equal
      FCC_GE  = 11+16,  // Greater or Equal
      FCC_UGE = 12+16,  // Unordered or Greater or Equal
      FCC_LE  = 13+16,  // Less or Equal
      FCC_ULE = 14+16,  // Unordered or Less or Equal
      FCC_O   = 15+16   // Ordered

    };
  }
  
  inline static const char *xpicCondCodeToString(XPICCC::CondCodes CC) {
    switch (CC) {
    case XPICCC::ICC_A:   return "always";//+
    case XPICCC::ICC_N:   return "newer";	//+
    case XPICCC::ICC_EQ:  return "e";  //"eq";	//+ equal
    case XPICCC::ICC_NE:  return "ne";	//+ not equal
    case XPICCC::ICC_SM:  return "ls"; //"slt";	//+ signed less
    case XPICCC::ICC_GR:  return "gs"; //"sgt";	//+ signed greater
    case XPICCC::ICC_SE:  return "les";//"sle";	//+ signed less or equal
    case XPICCC::ICC_GE:  return "ges";//"sge";	//+ signed greater or equal
    case XPICCC::ICC_LU:  return "lu"; //"lt";	//+ unsigned less
    case XPICCC::ICC_LEU: return "leu";//"le";	//+ unsigned less or equal
    case XPICCC::ICC_GU:  return "gu"; //"gt";	//+ unsigned greater
    case XPICCC::ICC_GEU: return "geu";//"ge";	//+ unsigned greater or equal
    case XPICCC::ICC_POS: return "ns";	//+
    case XPICCC::ICC_NEG: return "s";	//+
    case XPICCC::ICC_VC:  return "nc";	//+
    case XPICCC::ICC_VS:  return "c";	//+

    case XPICCC::FCC_U:   return "_u";
    case XPICCC::FCC_G:   return "_g";
    case XPICCC::FCC_UG:  return "_ug";
    case XPICCC::FCC_L:   return "_l";
    case XPICCC::FCC_UL:  return "_ul";
    case XPICCC::FCC_LG:  return "_lg";
    case XPICCC::FCC_NE:  return "_ne";
    case XPICCC::FCC_E:   return "_e";
    case XPICCC::FCC_UE:  return "_ue";
    case XPICCC::FCC_GE:  return "_ge";
    case XPICCC::FCC_UGE: return "_uge";
    case XPICCC::FCC_LE:  return "_le";
    case XPICCC::FCC_ULE: return "_ule";
    case XPICCC::FCC_O:   return "_o";
    }
    llvm_unreachable("Invalid cond code");
  }

}  // end namespace llvm
#endif





















