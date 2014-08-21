//===- xpicInstrFormats.td - xpic Instruction Formats ----*- tablegen -*-===//
// 
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
// 
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// Base class for all xPIC instructions
//===----------------------------------------------------------------------===//

class InstructionXPIC<dag outs, dag ins, string asmstr, list<dag> pattern> : Instruction 
{
  field bits<32> Inst;
  let Namespace = "XPIC";
  dag OutOperandList = outs;
  dag InOperandList = ins;
  let AsmString   = asmstr;
  let Pattern = pattern;
  bit usesRegisterIndirectAddressing = 0;
  bit isCmp = 0;

  // The layout of TSFlags should be kept in sync with xpicInstrInfo.h.
  let TSFlags{0}   = usesRegisterIndirectAddressing;
  let TSFlags{1}   = isCmp;
}

//===----------------------------------------------------------------------===//
// ALU commands: add, mul, asr, or, ...
//===----------------------------------------------------------------------===//

class IXALU<bits<6> opcode, dag ins, string asmstr, list<dag> pattern>
    : InstructionXPIC<(outs IntRegs:$dst), ins, asmstr, pattern>
{
  let Inst{31} = 0;
  let Inst{30-25} = opcode;
}

class IXCMP<bits<6> opcode, dag ins, string asmstr, list<dag> pattern>
    : InstructionXPIC<(outs), ins, asmstr, pattern>
{
  let Inst{31} = 0;
  let Inst{30-25} = opcode;
}


//===----------------------------------------------------------------------===//
// Load/Store commands: load, store
//===----------------------------------------------------------------------===//

class IXLDST<bits<1> isStore, dag outs, dag ins, string asmstr, list<dag> pattern>
    : InstructionXPIC<outs, ins, asmstr, pattern>
{
  let Inst{31} = 1;
  let Inst{30} = 0;
  let Inst{29} = isStore;
}

// load
class IXLD<dag ins, string asmstr, dag loadPattern>
    : IXLDST<0, (outs IntRegs:$dst), ins, asmstr, [(set IntRegs:$dst, loadPattern)]>;

// store
class IXST<dag ins, string asmstr, dag storePattern>
    : IXLDST<1, (outs), ins, asmstr, [storePattern]>;


//===----------------------------------------------------------------------===//
// Jump (branch) commands: jmp
//===----------------------------------------------------------------------===//

class IXJMP<dag ins, string asmstr, list<dag> pattern> 
: InstructionXPIC<(outs), ins, asmstr, pattern> 
{
  let Inst{31} = 1;
  let Inst{30} = 1;
  
  let isBranch = 1;
  let isTerminator = 1;
}
