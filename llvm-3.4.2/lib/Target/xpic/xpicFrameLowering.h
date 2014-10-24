//===-- xpicFrameLowering.h - Define frame lowering for xpic --*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
//
//
//===----------------------------------------------------------------------===//

#ifndef XPIC_FRAMEINFO_H
#define XPIC_FRAMEINFO_H

#include "llvm/CodeGen/MachineFunction.h"
#include "xpic.h"
#include "xpicSubtarget.h"
#include "llvm/Target/TargetFrameLowering.h"

namespace llvm {
  class xpicSubtarget;

class xpicFrameLowering : public TargetFrameLowering {
  const xpicSubtarget &SubTarget;
public:
  explicit xpicFrameLowering(const xpicSubtarget &ST)
    : TargetFrameLowering(TargetFrameLowering::StackGrowsDown, 4, 0, 4),
      SubTarget(ST) {}

  /// emitProlog/emitEpilog - These methods insert prolog and epilog code into
  /// the function.

  void eliminateCallFramePseudoInstr(MachineFunction &MF,
                                     MachineBasicBlock &MBB,
                                     MachineBasicBlock::iterator I) const;


  void emitPrologue(MachineFunction &MF) const;
  void emitEpilogue(MachineFunction &MF, MachineBasicBlock &MBB) const;

  int get_numStackSavedRegisters( void ) const;
  void set_numStackSavedRegisters( int value ) const;

public:
  /// This structure holds info in current function about frame index with arguments for called function
  /// Valid after xpicTargetLowering::LowerCall()
  mutable  struct {
    bool isSet;
    unsigned uFunctionID;     // Identifier of current function
    int IsCurrentFn(MachineFunction &MF)
    {
     return isSet && (uFunctionID == MF.getFunctionNumber() );
    }
    int iCallFI;                // Frame index that holds function arguments
  } xpicLowerCallInfo;
  /// position correction of the  call frame after register allocation.
  void AdjustStackForArguments(MachineFunction &MF) const;
  /// Align all stack objects to 4Byte
  void AlignStackObjects(MachineFunction &MF) const;


  void processFunctionBeforeFrameFinalized(MachineFunction &MF,
                                       RegScavenger *RS ) const;

  bool hasFP(const MachineFunction &MF) const;

protected: /// Saving to stack content of used in function registers:
  /// registers, allocated in function:
  mutable unsigned    *pUsedRegisters;
  /// number of registers, that must be stored/restored in/from stack in prologue/epilogue
  mutable int numStackSavedRegisters;
  /// function to search for used registers in function MF. Saves to pointer-vector pUsedRegisters.
  void getUsedRegisters(MachineFunction &MF) const;

protected:
  /// This function searches for inline assembly instruction "reli", 
  /// that must be used inside nested interrrupt routine
  bool  HasRELIInstruction(MachineFunction &MF, bool &fCallee) const;

  /// Annotations: __attribute__((annotate("fiq/irq/irqn"))) 
  enum FunctionAnnotations {
   no_annotation, // no annotations
   irq,     // interrupt function
   fiq,     // fast interrupt function
   irqn,    // nested interrupt function
   main     // function with name "main" - first entry in C-project
  };
  struct FunctionAnnotation {
   std::string FunctionName;
   std::string ModuleName;
   FunctionAnnotations Annotation;
  };
  // create one as class member, that holds annotation for current function. Initialized in function 'processFunctionBeforeFrameFinalized':
  mutable FunctionAnnotations FA;
  // Table of annotated functions
  mutable std::vector <FunctionAnnotation> vecFA;
  // If a function MF was annotated as "irq" and suchlike
  // returns respective flag from FunctionAnnotations
  FunctionAnnotations isAnnotated(MachineFunction &MF)const;
  // function-helper: If a constant has no user, it is allowed to delete it
  bool IsDeleteAllowed(const Value *C) const;
  // function-helper: Has uses current Constant? (has = false)
  bool NoUses(const Constant *C) const;
// end of annotations

  };

} // End llvm namespace

#endif
