//===- xpicInstrInfo.td - Target Description for xpic Target --------------===//
//===----------------------------------------------------------------------===//
// Instruction format superclass
//===----------------------------------------------------------------------===//
include "xpicInstrFormats.td"

//===----------------------------------------------------------------------===//
// Instruction Pattern Stuff
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// Constant Types
//===----------------------------------------------------------------------===//

// imm5 predicate - True if the imm fits in a 5-bit unsigned extended field.
def imm5  : PatLeaf<(imm), [{
  return (((unsigned)N->getZExtValue() << (32-5)) >> (32-5)) == (unsigned)N->getZExtValue();
}]>;

// simm14 predicate - True if the imm fits in a 15-bit sign extended field.
def simm14 : PatLeaf<(imm), [{
  return (((signed)N->getZExtValue() << (32-15)) >> (32-15)) == (signed)N->getZExtValue();
}]>;

// simm16 predicate - True if the imm fits in a 17-bit sign extended field.
def simm16 : PatLeaf<(imm), [{
  return (((signed)N->getZExtValue() << (32-17)) >> (32-17)) == (signed)N->getZExtValue();
}]>;

// imm18 predicate - True if the imm fits in a 18-bit unsigned extended field.
def imm18 : PatLeaf<(imm), [{
  return (((unsigned)N->getZExtValue() << (32-18)) >> (32-18)) == (unsigned)N->getZExtValue();
}]>;


//===----------------------------------------------------------------------===//
// Stack Frame Access
//===----------------------------------------------------------------------===//

def ADDRframe : ComplexPattern<i32, 3, "SelectADDRframe", [frameindex],[]>;

def MEMf  : Operand<i32> {
  let PrintMethod = "printFrameOperand";
  let MIOperandInfo = (ops IntRegs, WReg, i32imm);
}

//=== Write to stack frame

def xSTOREframe     : IXST < (ins MEMf:$frame, IntRegs:$src),
                             "store [$frame]d, $src",
                             (store IntRegs:$src, ADDRframe:$frame)>;

def xSTOREframei16  : IXST < (ins MEMf:$frame, IntRegs:$src),
                             "store [$frame]w, $src",
                             (truncstorei16 IntRegs:$src, ADDRframe:$frame)>;
                                       
def xSTOREframei8   : IXST < (ins MEMf:$frame, IntRegs:$src),
                             "store [$frame]b, $src",
                             (truncstorei8 IntRegs:$src, ADDRframe:$frame)>;

//=== Read from stack frame

def xLOADframe      : IXLD < (ins MEMf:$frame),
                             "load $dst, [$frame]d",
                             (load ADDRframe:$frame)>;

def xLOADframeSi16  : IXLD < (ins MEMf:$frame),
                             "load $dst, s[$frame]w",
                             (sextloadi16 ADDRframe:$frame)>;

def xLOADframeSi8   : IXLD < (ins MEMf:$frame),
                             "load $dst, s[$frame]b",
                             (sextloadi8 ADDRframe:$frame)>;

def xLOADframeUi16  : IXLD < (ins MEMf:$frame),
                             "load $dst, [$frame]w",
                             (zextloadi16 ADDRframe:$frame)>;

def xLOADframeUi8   : IXLD < (ins MEMf:$frame),
                             "load $dst, [$frame]b",
                             (zextloadi8 ADDRframe:$frame)>;

def xLOADframei16   : IXLD < (ins MEMf:$frame),
                             "load $dst, [$frame]w",
                             (extloadi16 ADDRframe:$frame)>;

def xLOADframei8    : IXLD < (ins MEMf:$frame),
                             "load $dst, [$frame]b",
                             (extloadi8 ADDRframe:$frame)>;

//=== Eliminate frame instruction
def EliminateFrame  : SDTypeProfile<1,1, [SDTCisSameAs<0, 1>, SDTCisInt<0> ]>;
def patElimFrame    : SDNode<"XPICISD::ELIMINATE_FI", EliminateFrame, [SDNPOutGlue]>;
def xEL_FRAME       : IXALU< 0b000000,
                             (ins MEMf:$frame),
                             "add $dst,$frame, r7",
                             [(set IntRegs:$dst, (patElimFrame ADDRframe:$frame))]>;


//===----------------------------------------------------------------------===//
// Return Statements
//===----------------------------------------------------------------------===//

//=== Return from function
def retflag        : SDNode<"XPICISD::RET_FLAG", SDTNone,[SDNPHasChain, SDNPOptInGlue, SDNPVariadic]>;
let isReturn = 1, isTerminator = 1, isBarrier = 1, hasDelaySlot = 1 in
def xRETL          : InstructionXPIC< (outs),
                                      (ins),
                                      "inc pc, [r7++], #8\t\t// load register 'pc' from stack",
                                      [(retflag)]>;

//=== Return from fast interrupt
def RetF           : SDTypeProfile<0, 0, []>;
def patRetF        : SDNode<"XPICISD::RETF", RetF, [SDNPHasChain]>;
def xRETF          : InstructionXPIC< (outs),
                                      (ins),
                                      "retf\t\t\t\t// return from fast interrupt ",
                                      [(patRetF)]>;

//=== Return from interrupt
def RetRelI        : SDTypeProfile<0, 0, []>;
def patRetRelI     : SDNode<"XPICISD::RETRELI", RetRelI, [SDNPHasChain]>;
def xRETRELI       : InstructionXPIC< (outs),
                                      (ins),
                                      "relreti\t\t\t\t// release interrupt and return",
                                      [(patRetRelI)]>;

//=== Return from nested interrupt
def RetI           : SDTypeProfile<0, 0, []>;
def patRetI        : SDNode<"XPICISD::RETI", RetI, [SDNPHasChain]>;
def xRETI          : InstructionXPIC< (outs),
                                      (ins),
                                      "reti\t\t\t\t// return from nested interrupt",
                                      [(patRetI)]>;


//===----------------------------------------------------------------------===//
// Load Immediate (constant) used by xpicDAGToDAGISel::SelectConstant()
//===----------------------------------------------------------------------===//

def xLOADi        : InstructionXPIC< (outs IntRegs:$dst), 
                                     (ins i32imm:$src1, IntRegs:$src2),
                                     "load $dst, $src1", 
                                     []>;


//===----------------------------------------------------------------------===//
// Load / Store
//===----------------------------------------------------------------------===//

def ADDRw : ComplexPattern<i32, 1, "SelectADDRw", [], []>;
                                     
def ADDRri : ComplexPattern<i32, 2, "SelectADDRri", [frameindex, add], []>;
def MEMri : Operand<i32> {
  let PrintMethod = "printMemOperand";
  let MIOperandInfo = (ops IntRegs, i32imm);
}

def ADDRrr : ComplexPattern<i32, 2, "SelectADDRrr", [], []>;
def MEMrr : Operand<i32> {
  let PrintMethod = "printMemOperand";
  let MIOperandInfo = (ops IntRegs, WReg);
}

//=== Load

multiclass IXLD_ri_rr<string asmstr, PatFrag OpNode> {
// load reg, {s}[reg + simm16]{d|w|b}
 def ri           : IXLD < (ins MEMri:$addr),
                           asmstr,
                           (OpNode ADDRri:$addr)>;
 
// load reg, {s}[reg + wreg]{d|w|b}
 def rr           : IXLD < (ins MEMrr:$addr),
                           asmstr,
                           (OpNode ADDRrr:$addr)>;
}

defm xLD    : IXLD_ri_rr < "load $dst, [$addr]d",
                           load>;
                           
defm xLDUH  : IXLD_ri_rr < "load $dst, [$addr]w",
                           zextloadi16>;
                           
defm xLDUB  : IXLD_ri_rr < "load $dst, [$addr]b",
                           zextloadi8>;
                           
defm xLDSH  : IXLD_ri_rr < "load $dst, s[$addr]w",
                           sextloadi16>;
                           
defm xLDSB  : IXLD_ri_rr < "load $dst, s[$addr]b",
                           sextloadi8>;

//=== Store

multiclass IXST_ri_rr<string asmstr, PatFrag OpNode> {
// store [reg + simm16]{d|w|b}, reg
 def ri           : IXST < (ins MEMri:$addr, IntRegs:$src),
                           asmstr,
                           (OpNode IntRegs:$src, ADDRri:$addr)>;
 
// store [reg + wreg]{d|w|b}, reg
 def rr           : IXST < (ins MEMrr:$addr, IntRegs:$src),
                           asmstr,
                           (OpNode IntRegs:$src, ADDRrr:$addr)>;
}

defm xST    : IXST_ri_rr < "store [$addr], $src",
                           store>;
                           
defm xSTH   : IXST_ri_rr < "store [$addr]w, $src",
                           truncstorei16>;
                           
defm xSTB   : IXST_ri_rr < "store [$addr]b, $src",
                           truncstorei8>;


//===----------------------------------------------------------------------===//
// Calls
//===----------------------------------------------------------------------===//

def SDT_XPICCall: SDTypeProfile<0, -1, [SDTCisVT<0, i32>]>;
def call        : SDNode<"XPICISD::CALL",SDT_XPICCall,[SDNPHasChain,SDNPOptInGlue,SDNPOutGlue,SDNPVariadic]>;
def calltarget  : Operand<i32>;

// xCALL has dummy DelaySlot. It is used to insert "save return address" instruction in xpicDelaySlotFiller
let isCall=1, 
    usesCustomInserter=1,  
    Defs=[r2,r3,r4,r5,r6],
    hasDelaySlot=1 in {
  def xCALL : InstructionXPIC<(outs),
                            (ins calltarget:$dst),
                            "jmp $dst",
                            []>;

  def xCALL_LOAD    : InstructionXPIC< (outs), 
                                       (ins IntRegs:$dst),
                                       "mov pc, $dst, z0",
                                       []>;
}

// xCALL has dummy DelaySlot. It is used to insert "save return address" instruction in xpicDelaySlotFiller
let isCall=1, 
    Defs=[r2,r3,r4,r5,r6],
    hasDelaySlot=1 in {
  def xCALL_done : InstructionXPIC<(outs),
                            (ins calltarget:$dst),
                            "jmp $dst",
                            []>;

  def xCALL_LOAD_done    : InstructionXPIC< (outs), 
                                       (ins IntRegs:$dst),
                                       "mov pc, $dst, z0",
                                       []>;
}

def : Pat<(call tglobaladdr:$dst ),(xCALL_LOAD      tglobaladdr:$dst)>;
def : Pat<(call texternalsym:$dst),(xCALL_LOAD      texternalsym:$dst)>;
def : Pat<(call IntRegs:$dst),     (xCALL_LOAD IntRegs:$dst)>;


def xMOV    : InstructionXPIC<       (outs IntRegs:$dst), 
                                     (ins IntRegs:$src),
                                     "mov $dst, $src, z0",
                                     []>;


//===----------------------------------------------------------------------===//
// Condition Codes
//===----------------------------------------------------------------------===//

// Note that these values must be kept in sync with the XPICCC::CondCodes enum.

class ICC_VAL<int N> : PatLeaf<(i32 N)>;
def ICC_NE  : ICC_VAL< 9>;  // Not Equal
def ICC_EQ  : ICC_VAL< 1>;  // Equal
def ICC_GR  : ICC_VAL<10>;  // Greater
def ICC_SE  : ICC_VAL< 2>;  // Less or Equal
def ICC_GE  : ICC_VAL<11>;  // Greater or Equal
def ICC_SM  : ICC_VAL< 3>;  // Less
def ICC_GU  : ICC_VAL<12>;  // Greater Unsigned
def ICC_LEU : ICC_VAL< 4>;  // Less or Equal Unsigned
def ICC_CC  : ICC_VAL<13>;  // Carry Clear/Great or Equal Unsigned
def ICC_LU  : ICC_VAL< 5>;  // Less Unsigned / Carry Set
def ICC_POS : ICC_VAL<14>;  // Positive Result
def ICC_NEG : ICC_VAL< 6>;  // Negative Result
def ICC_VC  : ICC_VAL<15>;  // Overflow Clear
def ICC_VS  : ICC_VAL< 7>;  // Overflow Set
def ICC_A   : ICC_VAL< 8>;  // Always
def ICC_N   : ICC_VAL< 0>;  // Never

class FCC_VAL<int N> : PatLeaf<(i32 N)>;
def FCC_U   : FCC_VAL<23>;  // Unordered
def FCC_G   : FCC_VAL<22>;  // Greater
def FCC_UG  : FCC_VAL<21>;  // Unordered or Greater
def FCC_L   : FCC_VAL<20>;  // Less
def FCC_UL  : FCC_VAL<19>;  // Unordered or Less
def FCC_LG  : FCC_VAL<18>;  // Less or Greater
def FCC_NE  : FCC_VAL<17>;  // Not Equal
def FCC_E   : FCC_VAL<25>;  // Equal
def FCC_UE  : FCC_VAL<24>;  // Unordered or Equal
def FCC_GE  : FCC_VAL<25>;  // Greater or Equal
def FCC_UGE : FCC_VAL<26>;  // Unordered or Greater or Equal
def FCC_LE  : FCC_VAL<27>;  // Less or Equal
def FCC_ULE : FCC_VAL<28>;  // Unordered or Less or Equal
def FCC_O   : FCC_VAL<29>;  // Ordered


// Branch targets have OtherVT type.
def brtarget : Operand<OtherVT>;


// Operand for printing out a condition code.
let PrintMethod = "printCCOperand" in
  def CCOp : Operand<i32>;


def SDTXbrcc     : SDTypeProfile<0, 2, [SDTCisVT<0, OtherVT>, SDTCisVT<1, i32>]>;
def SDTXselectcc : SDTypeProfile<1, 3, [SDTCisSameAs<0, 1>, SDTCisSameAs<1, 2>, SDTCisVT<3, i32>]>;


def Xcmpicc      : SDNode<"XPICISD::CMPICC",     SDTIntBinOp,  [SDNPOutGlue]>;
def Xbricc       : SDNode<"XPICISD::BRICC",      SDTXbrcc,     [SDNPHasChain, SDNPInGlue]>;
def Xselecticc   : SDNode<"XPICISD::SELECT_ICC", SDTXselectcc, [SDNPInGlue]>;

/*
def SDTXcmpfcc   : SDTypeProfile<0, 2, [SDTCisFP<0>, SDTCisSameAs<0, 1>]>;
def Xcmpfcc      : SDNode<"XPICISD::CMPFCC",     SDTXcmpfcc,   [SDNPOutGlue]>;
def Xbrfcc       : SDNode<"XPICISD::BRFCC",      SDTXbrcc,     [SDNPHasChain, SDNPInGlue]>;
def Xselectfcc   : SDNode<"XPICISD::SELECT_FCC", SDTXselectcc, [SDNPInGlue]>;
*/

// These are target-independent nodes, but have target-specific formats.
def SDTXCallSeqStart : SDCallSeqStart <[ SDTCisVT<0, i32> ]>;
def SDTXCallSeqEnd   : SDCallSeqEnd   <[ SDTCisVT<0, i32>,
                                         SDTCisVT<1, i32> ]>;

def callseq_start : SDNode<"ISD::CALLSEQ_START", SDTXCallSeqStart,
                           [SDNPHasChain, SDNPOutGlue]>;
def callseq_end   : SDNode<"ISD::CALLSEQ_END",   SDTXCallSeqEnd,
                           [SDNPHasChain, SDNPOptInGlue, SDNPOutGlue]>;



//===----------------------------------------------------------------------===//
// ALU Instruction Class Templates
//===----------------------------------------------------------------------===//

class BiOpFrag <dag res> : PatFrag<(ops node:$src1, node:$src2), res>;
class UnOpFrag <dag res> : PatFrag<(ops node:$src), res>;

// select constants more then 14 bit in ALU instructions
def CONST_MEM  : ComplexPattern<i32, 1, "SelectConstMem", []>;

//===----------------------------------------------------------------------===//
// Instruction Name:
// xNNNkk
// x      - xpic instruction
//  NNN   - instruction name; 
//     k  - r: source register, i: source immediate value, m: source is memory
//      k - r: source register, i: source immediate value

/// multiclass - Define a pattern in one shot

let Defs = [stat] in {
// Standard ALU commands
multiclass IXALU_2op<bits<6> opcode, string OpcStr, PatFrag OpNode, bit Commutable = 0> {
 def ir      : IXALU< opcode, 
                      (ins i32imm:$src1, IntRegs:$src2),
                      !strconcat(OpcStr, " $dst, $src1, $src2"), 
                      [(set IntRegs:$dst, (OpNode simm14:$src1, IntRegs:$src2))]>;


 def mr      : IXALU< opcode, 
                      (ins i32imm:$addr, IntRegs:$src),
                      !strconcat(OpcStr, " $dst, [$addr], $src"), 
                      [(set IntRegs:$dst, (OpNode (load simm14:$addr), IntRegs:$src))]>;

 def rmr     : IXALU< opcode, 
                      (ins WReg:$addr, IntRegs:$src),
                      !strconcat(OpcStr, " $dst, [$addr]d, $src"), 
                      [(set IntRegs:$dst, (OpNode (load ADDRw:$addr), IntRegs:$src))]>
                      { let usesRegisterIndirectAddressing = 1; }

 def rmuhr : IXALU< opcode, 
                      (ins WReg:$addr, IntRegs:$src),
                      !strconcat(OpcStr, " $dst, [$addr]w, $src"), 
                      [(set IntRegs:$dst, (OpNode (zextloadi16 ADDRw:$addr), IntRegs:$src))]>
                      { let usesRegisterIndirectAddressing = 1; }

 def rmubr : IXALU< opcode, 
                      (ins WReg:$addr, IntRegs:$src),
                      !strconcat(OpcStr, " $dst, [$addr]b, $src"), 
                      [(set IntRegs:$dst, (OpNode (zextloadi8 ADDRw:$addr), IntRegs:$src))]>
                      { let usesRegisterIndirectAddressing = 1; }

 def rmshr : IXALU< opcode, 
                      (ins WReg:$addr, IntRegs:$src),
                      !strconcat(OpcStr, " $dst, s[$addr]w, $src"), 
                      [(set IntRegs:$dst, (OpNode (sextloadi16 ADDRw:$addr), IntRegs:$src))]>
                      { let usesRegisterIndirectAddressing = 1; }

 def rmsbr : IXALU< opcode, 
                      (ins WReg:$addr, IntRegs:$src),
                      !strconcat(OpcStr, " $dst, s[$addr]b, $src"), 
                      [(set IntRegs:$dst, (OpNode (sextloadi8 ADDRw:$addr), IntRegs:$src))]>
                      { let usesRegisterIndirectAddressing = 1; }
                      
 def rr      : IXALU< opcode, 
                      (ins IntRegs:$src1, IntRegs:$src2),
                      !strconcat(OpcStr, " $dst, $src1, $src2"), 
                      [(set IntRegs:$dst, (OpNode IntRegs:$src1, IntRegs:$src2))]>
                      { let isCommutable = Commutable; }
}

// Shift and Rotate commands
multiclass IXALU_shift <bits<6> opcode, string OpcStr, SDNode OpNode> {
 def ii5     : IXALU< opcode, 
                      (ins i32imm:$src1, i32imm:$src2),
                      !strconcat(OpcStr, " $dst, $src1, $src2"), 
                      [(set IntRegs:$dst, (OpNode imm:$src1, imm5:$src2))]>;
                                
 def mi5     : IXALU< opcode, 
                      (ins i32imm:$addr1, i32imm:$src2),
                      !strconcat(OpcStr, " $dst, [$addr1], $src2"), 
                      [(set IntRegs:$dst, (OpNode (load simm14:$addr1), imm5:$src2))]>;
                                
 def rmi5    : IXALU< opcode, 
                      (ins WReg:$addr1, i32imm:$src2),
                      !strconcat(OpcStr, " $dst, [$addr1], $src2"), 
                      [(set IntRegs:$dst, (OpNode (load ADDRw:$addr1), imm5:$src2))]>
                      { let usesRegisterIndirectAddressing = 1; }
                                
 def rmuhi5  : IXALU< opcode, 
                      (ins WReg:$addr1, i32imm:$src2),
                      !strconcat(OpcStr, " $dst, [$addr1]w, $src2"), 
                      [(set IntRegs:$dst, (OpNode (zextloadi16 ADDRw:$addr1), imm5:$src2))]>
                      { let usesRegisterIndirectAddressing = 1; }
                                
 def rmubi5  : IXALU< opcode, 
                      (ins WReg:$addr1, i32imm:$src2),
                      !strconcat(OpcStr, " $dst, [$addr1]b, $src2"), 
                      [(set IntRegs:$dst, (OpNode (zextloadi8 ADDRw:$addr1), imm5:$src2))]>
                      { let usesRegisterIndirectAddressing = 1; }
                                
 def rmshi5  : IXALU< opcode, 
                      (ins WReg:$addr1, i32imm:$src2),
                      !strconcat(OpcStr, " $dst, s[$addr1]w, $src2"), 
                      [(set IntRegs:$dst, (OpNode (sextloadi16 ADDRw:$addr1), imm5:$src2))]>
                      { let usesRegisterIndirectAddressing = 1; }
                                
 def rmsbi5  : IXALU< opcode, 
                      (ins WReg:$addr1, i32imm:$src2),
                      !strconcat(OpcStr, " $dst, s[$addr1]b, $src2"), 
                      [(set IntRegs:$dst, (OpNode (sextloadi8 ADDRw:$addr1), imm5:$src2))]>
                      { let usesRegisterIndirectAddressing = 1; }
                                
 def ri5     : IXALU< opcode, 
                      (ins IntRegs:$src1, i32imm:$src2),
                      !strconcat(OpcStr, " $dst, $src1, $src2"), 
                      [(set IntRegs:$dst, (OpNode IntRegs:$src1, imm5:$src2))]>;
}
}

// ir_mr_ir
multiclass IXCMP_multi <bits<6> opcode, string OpcStr, SDNode OpNode, bit Commutable = 0> {
 def ri5     : IXCMP< opcode, 
                      (ins IntRegs:$src1, i32imm:$src2),
                      !strconcat(OpcStr, " z0, $src1, $src2"), 
                      [(OpNode IntRegs:$src1, imm5:$src2)]>;

 def ir      : IXCMP< opcode, 
                      (ins i32imm:$src1, IntRegs:$src2),
                      !strconcat(OpcStr, " z0, $src1, $src2"), 
                      [(OpNode simm14:$src1, IntRegs:$src2)]>;

 def mr      : IXCMP< opcode, 
                      (ins i32imm:$addr1, IntRegs:$src2),
                      !strconcat(OpcStr, " z0, [$addr1]d, $src2"), 
                      [(OpNode (load simm14:$addr1), IntRegs:$src2)]>;

 def rmr     : IXCMP< opcode, 
                      (ins WReg:$addr1, IntRegs:$src2),
                      !strconcat(OpcStr, " z0, [$addr1]d, $src2"), 
                      [(OpNode (load ADDRw:$addr1), IntRegs:$src2)]>
                      { let usesRegisterIndirectAddressing = 1; }

 def rmuhr   : IXCMP< opcode, 
                      (ins WReg:$addr1, IntRegs:$src2),
                      !strconcat(OpcStr, " z0, [$addr1]w, $src2"), 
                      [(OpNode (zextloadi16 ADDRw:$addr1), IntRegs:$src2)]>
                      { let usesRegisterIndirectAddressing = 1; }

 def rmubr   : IXCMP< opcode, 
                      (ins WReg:$addr1, IntRegs:$src2),
                      !strconcat(OpcStr, " z0, [$addr1]b, $src2"), 
                      [(OpNode (zextloadi8 ADDRw:$addr1), IntRegs:$src2)]>
                      { let usesRegisterIndirectAddressing = 1; }

 def rmshr   : IXCMP< opcode, 
                      (ins WReg:$addr1, IntRegs:$src2),
                      !strconcat(OpcStr, " z0, s[$addr1]w, $src2"), 
                      [(OpNode (sextloadi16 ADDRw:$addr1), IntRegs:$src2)]>
                      { let usesRegisterIndirectAddressing = 1; }

 def rmsbr   : IXCMP< opcode, 
                      (ins WReg:$addr1, IntRegs:$src2),
                      !strconcat(OpcStr, " z0, s[$addr1]b, $src2"), 
                      [(OpNode (sextloadi8 ADDRw:$addr1), IntRegs:$src2)]>
                      { let usesRegisterIndirectAddressing = 1; }

 def rr      : IXCMP< opcode, 
                      (ins IntRegs:$src1, IntRegs:$src2),
                      !strconcat(OpcStr, " z0, $src1, $src2"), 
                      [(OpNode IntRegs:$src1, IntRegs:$src2)]>
                      { let isCommutable = Commutable; }
}

// rr_ir  for multiplication
multiclass IXALU_2op_mul<bits<6> opcode, string OpcStr, PatFrag OpNode, bit Commutable = 0> {
 def ir      : IXALU< opcode, 
                      (ins i32imm:$src1, IntRegs:$src2),
                      !strconcat(OpcStr, " $dst, $src1, $src2"), 
                      [(set IntRegs:$dst, (OpNode simm14:$src1, IntRegs:$src2))]>;

 def rr      : IXALU< opcode, 
                      (ins IntRegs:$src1, IntRegs:$src2),
                      !strconcat(OpcStr, " $dst, $src1, $src2"), 
                      [(set IntRegs:$dst, (OpNode IntRegs:$src1, IntRegs:$src2))]>
                      { let isCommutable = Commutable; }
}

// rr_mr_ir
multiclass IXALU_1op<bits<6> opcode, string OpcStr, PatFrag OpNode> {
 def i       : IXALU< opcode, 
                      (ins i32imm:$src1),
                      !strconcat(OpcStr, " $dst, $src1, z0"), 
                      [(set IntRegs:$dst, (OpNode simm14:$src1))]>;
 
 def m       : IXALU< opcode, 
                      (ins i32imm:$addr1),
                      !strconcat(OpcStr, " $dst, [$addr1]d, z0"), 
                      [(set IntRegs:$dst, (OpNode (load simm14:$addr1)))]>;
 
 def rm      : IXALU< opcode, 
                      (ins WReg:$addr1),
                      !strconcat(OpcStr, " $dst, [$addr1]d, z0"), 
                      [(set IntRegs:$dst, (OpNode (load ADDRw:$addr1)))]>
                      { let usesRegisterIndirectAddressing = 1; }
 
 def rmuh    : IXALU< opcode, 
                      (ins WReg:$addr1),
                      !strconcat(OpcStr, " $dst, [$addr1]w, z0"), 
                      [(set IntRegs:$dst, (OpNode (zextloadi16 ADDRw:$addr1)))]>
                      { let usesRegisterIndirectAddressing = 1; }
 
 def rmub    : IXALU< opcode, 
                      (ins WReg:$addr1),
                      !strconcat(OpcStr, " $dst, [$addr1]b, z0"), 
                      [(set IntRegs:$dst, (OpNode (zextloadi8 ADDRw:$addr1)))]>
                      { let usesRegisterIndirectAddressing = 1; }
 
 def rmsh    : IXALU< opcode, 
                      (ins WReg:$addr1),
                      !strconcat(OpcStr, " $dst, s[$addr1]w, z0"), 
                      [(set IntRegs:$dst, (OpNode (sextloadi16 ADDRw:$addr1)))]>
                      { let usesRegisterIndirectAddressing = 1; }
 
 def rmsb    : IXALU< opcode, 
                      (ins WReg:$addr1),
                      !strconcat(OpcStr, " $dst, s[$addr1]b, z0"), 
                      [(set IntRegs:$dst, (OpNode (sextloadi8 ADDRw:$addr1)))]>
                      { let usesRegisterIndirectAddressing = 1; }
 
 def r       : IXALU< opcode, 
                      (ins IntRegs:$src1),
                      !strconcat(OpcStr, " $dst, $src1, z0"), 
                      [(set IntRegs:$dst, (OpNode IntRegs:$src1))]>;
}


//===----------------------------------------------------------------------===//
// ALU Operations
//===----------------------------------------------------------------------===//

//=== arithmetic
defm xADD    : IXALU_2op <0b000000, "add" , BiOpFrag<(add  node:$src2, node:$src1)>, 1>;
defm xADDC   : IXALU_2op <0b000000, "add" , BiOpFrag<(addc node:$src2, node:$src1)>, 1>;

let Uses=[stat] in {
defm xADDE   : IXALU_2op <0b001111, "addc", BiOpFrag<(adde node:$src2, node:$src1)>, 1>;
}

defm xSUB    : IXALU_2op <0b010000, "sub" , BiOpFrag<(sub  node:$src1, node:$src2)>>;
defm xSUBC   : IXALU_2op <0b010000, "sub" , BiOpFrag<(subc node:$src1, node:$src2)>>;
let Uses=[stat] in {
defm xSUBE   : IXALU_2op <0b010011, "subc", BiOpFrag<(sube node:$src1, node:$src2)>>;
}

defm xMULS    : IXALU_2op_mul <0b101011, "muls", BiOpFrag<(mul node:$src2, node:$src1)>, 1>;

//=== shifts
defm xLSL    : IXALU_shift <0b010100, "lsl", shl>;
defm xLSL    : IXALU_2op   <0b010100, "lsl", BiOpFrag<(shl  node:$src1, node:$src2)>>;
defm xLSR    : IXALU_shift <0b010111, "lsr", srl>;
defm xLSR    : IXALU_2op   <0b010111, "lsr", BiOpFrag<(srl  node:$src1, node:$src2)>>;
defm xASR    : IXALU_shift <0b011000, "asr", sra>;
defm xASR    : IXALU_2op   <0b011000, "asr", BiOpFrag<(sra  node:$src1, node:$src2)>>;
defm xROL    : IXALU_shift <0b011001, "rol", rotl>;
defm xROL    : IXALU_2op   <0b011000, "rol", BiOpFrag<(rotl node:$src1, node:$src2)>>;

//=== bitwise operations
defm xOR     : IXALU_2op <0b001110, "or",   BiOpFrag<(or  node:$src2, node:$src1)>, 1>;
defm xXOR    : IXALU_2op <0b000110, "xor",  BiOpFrag<(xor node:$src2, node:$src1)>, 1>;
defm xAND    : IXALU_2op <0b001000, "and",  BiOpFrag<(and node:$src2, node:$src1)>, 1>;

defm xNOR    : IXALU_2op <0b000001, "nor",  BiOpFrag<(not (or  node:$src2, node:$src1))>, 1>;
defm xXNOR   : IXALU_2op <0b001001, "xnor", BiOpFrag<(not (xor node:$src2, node:$src1))>, 1>;
defm xNAND   : IXALU_2op <0b000111, "nand", BiOpFrag<(not (and node:$src2, node:$src1))>, 1>;

defm xBS     : IXALU_2op <0b000010, "bs",   BiOpFrag<(and (not node:$src1), node:$src2)>>;
defm xNIMP   : IXALU_2op <0b000100, "nimp", BiOpFrag<(and (not node:$src2), node:$src1)>>;
defm xIMP    : IXALU_2op <0b001011, "imp",  BiOpFrag<(or  (not node:$src1), node:$src2)>>;
defm xBBE    : IXALU_2op <0b001101, "bbe",  BiOpFrag<(or  (not node:$src2), node:$src1)>>;

// pattern for xNIMPir is (and (not x), 5) but this gets canonicalized to (xor (and x, 5), 5)
def : Pat<(xor (and IntRegs:$src1, simm14:$src2), simm14:$src2), (xNIMPir simm14:$src2, IntRegs:$src1)>;

// TODO: pattern for xBBEir is (or (not x), 5) but this gets canonicalized to (xor (or x, 5), -6)

defm xCLZ    : IXALU_1op <0b110010, "clz", UnOpFrag<(ctlz node:$src)>>;
defm xINV    : IXALU_1op <0b000011, "inv", UnOpFrag<(not  node:$src)>>;


//=== compare operation
def SDTIntBinOpX : SDTypeProfile<0, 2, [   // has 0-outputs, 2-inputs
                   SDTCisSameAs<0, 1>, SDTCisInt<0> ]>;

def XPICcmpiccX : SDNode<"XPICISD::CMPICC", SDTIntBinOpX, [SDNPOutGlue]>;

let isTerminator = 1, isCmp = 1 in
defm xCMP  : IXCMP_multi<0b011001, "rol", XPICcmpiccX>;



//===----------------------------------------------------------------------===//
// Stack Push/Pop Instructions
//===----------------------------------------------------------------------===//

// Push

def SDTStoreToStack   : SDTypeProfile<0, 2, [SDTCisSameAs<0, 1>, SDTCisInt<0> ]>;
def patStoreToStack   : SDNode<"XPICISD::STACK_STORE", SDTStoreToStack, [SDNPOutGlue]>;

def xSTORE_TO_STACK   : InstructionXPIC< (outs), 
                                         (ins IntRegs:$dst,IntRegs:$src),
                                         "store [z0 + --$dst], $src\t\t// xSTORE_TO_STACK save register '$src' to stack",
                                         [(patStoreToStack IntRegs:$dst, IntRegs:$src)]>;


// Pop

def SDTLoadFromStack  : SDTypeProfile<1, 1, [SDTCisSameAs<0, 1>, SDTCisInt<0>]>;//
def patLoadFromStack  : SDNode<"XPICISD::STACK_LOAD", SDTLoadFromStack, [SDNPHasChain]>;

def xLOAD_FROM_STACK2 : InstructionXPIC< (outs IntRegs:$dst), 
                                         (ins IntRegs:$src),
                                         "load $dst, [z0 + $src++]\t\t// xLOAD_FROM_STACK2 load register '$dst' from stack", 
                                         [(set IntRegs:$dst,  (patLoadFromStack IntRegs:$src))]>;

def xLOAD_FROM_STACK  : InstructionXPIC< (outs IntRegs:$dst ), 
                                         (ins i32imm:$src),
                                         "mov $dst, [$src], z0\t\t// xLOAD_FROM_STACK",
                                         [(set IntRegs:$dst, (patLoadFromStack imm:$src))]>;



//===----------------------------------------------------------------------===//
// Branch Instructions
//===----------------------------------------------------------------------===//

let isBarrier = 1 in {
def xBA         : IXJMP< (ins brtarget:$dst),
                         "jmp $dst",
                         [(br bb:$dst)]>;
}

def xBCOND      : IXJMP< (ins brtarget:$dst, CCOp:$cc),
                         "jmp $cc, $dst",
                         [(Xbricc bb:$dst, imm:$cc)]>;


//===----------------------------------------------------------------------===//
// Pseudo Instructions
//===----------------------------------------------------------------------===//

class Pseudo<dag outs, dag ins, string asmstr, list<dag> pattern>
   : InstructionXPIC<outs, ins, asmstr, pattern>;

//let Defs = [sr6], Uses = [sr6] in {
 def ADJCALLSTACKDOWN : Pseudo< (outs),
                                (ins i32imm:$amt),
                                "!ADJCALLSTACKDOWN $amt",
                                [(callseq_start timm:$amt)]>;

 def ADJCALLSTACKUP   : Pseudo< (outs),
                                (ins i32imm:$amt1, i32imm:$amt2),
                                "!ADJCALLSTACKUP $amt1",
                                [(callseq_end timm:$amt1, timm:$amt2)]>;
//}

// SELECT_CC_* - Used to implement the SELECT_CC DAG operation.  Expanded by the
// scheduler into a branch sequence.  This has to handle all permutations of
// selection between i32/f32/f64 on ICC and FCC.
let usesCustomInserter = 1 in {   // Expanded by the scheduler.
 def SELECT_CC_Int_ICC : Pseudo< (outs IntRegs:$dst),
                                 (ins IntRegs:$T, IntRegs:$F, i32imm:$Cond),
                                 "// SELECT_CC_Int_ICC PSEUDO!",
                                 [(set IntRegs:$dst, (Xselecticc IntRegs:$T, IntRegs:$F, imm:$Cond))]>;
                                 
/*
 def SELECT_CC_Int_FCC : Pseudo< (outs IntRegs:$dst),
                                 (ins IntRegs:$T, IntRegs:$F, i32imm:$Cond),
                                 "// SELECT_CC_Int_FCC PSEUDO!",
                                 [(set IntRegs:$dst, (Xselectfcc IntRegs:$T, IntRegs:$F, imm:$Cond))]>;
*/
}



//=== NOP Instruction
def xNOP : InstructionXPIC<(outs), (ins), "nop", []>;


//=== Map integer extload's to zextloads.
def : Pat<(i32 (extloadi1 ADDRrr:$src)),  (xLDUBrr ADDRrr:$src)>;
def : Pat<(i32 (extloadi8 ADDRrr:$src)),  (xLDUBrr ADDRrr:$src)>;
def : Pat<(i32 (extloadi16 ADDRrr:$src)), (xLDUHrr ADDRrr:$src)>;
def : Pat<(i32 (extloadi1 ADDRri:$src)),  (xLDUBri ADDRri:$src)>;
def : Pat<(i32 (extloadi8 ADDRri:$src)),  (xLDUBri ADDRri:$src)>;
def : Pat<(i32 (extloadi16 ADDRri:$src)), (xLDUHri ADDRri:$src)>;

// zextload bool -> zextload byte
def : Pat<(i32 (zextloadi1 ADDRrr:$src)), (xLDUBrr ADDRrr:$src)>;

// Load upper 18 bit of 32 bit immediate symbol
def xLOADUPPER          : InstructionXPIC< (outs IntRegs:$dst),
                                      (ins i32imm:$addr),
                                      "lhi $dst, $addr",
                                      []>;

// Bitwise OR register with lower 14 bit of immediate symbol
def xORLOWER            : InstructionXPIC< (outs IntRegs:$dst),
                                      (ins i32imm:$addr, IntRegs:$src),
                                      "orl $dst, $addr, $src",
                                      []>;
