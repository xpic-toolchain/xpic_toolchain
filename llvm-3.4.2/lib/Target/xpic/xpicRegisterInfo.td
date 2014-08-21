//===- xpicRegisterInfo.td - xpic Register defs ----------*- tablegen -*-===//
// 
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
// 
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
//  Declarations that describe the xpic register file 
//===----------------------------------------------------------------------===//

class xpicReg<bits<16> Enc, string n> : Register<n> {
  let HWEncoding = Enc;
  let Namespace = "XPIC";
}


// Registers are identified with 4-bit ID numbers.
// Ri - 32-bit integer registers
class Ri<bits<16> Enc, string n> : xpicReg<Enc, n>;


// new ABI: Integer registers
def r2    : Ri<2, "r2">,       DwarfRegNum<[2]>; 
def r3    : Ri<3, "r3">,       DwarfRegNum<[3]>;
def r4    : Ri<4, "r4">,       DwarfRegNum<[4]>;
def r5    : Ri<5, "r5">,       DwarfRegNum<[5]>;
def r0    : Ri<0, "r0">,       DwarfRegNum<[0]>;
def r1    : Ri<1, "r1">,       DwarfRegNum<[1]>;
def u0    : Ri<8, "u0">,       DwarfRegNum<[8]>;
def u1    : Ri<9, "u1">,       DwarfRegNum<[9]>;
def u2    : Ri<10,"u2">,       DwarfRegNum<[10]>;
def u3    : Ri<11,"u3">,       DwarfRegNum<[11]>;
def r6    : Ri<6, "r6">,       DwarfRegNum<[6]>;
def u4    : Ri<12,"u4">,       DwarfRegNum<[12]>;
def r7    : Ri<7, "r7">,       DwarfRegNum<[7]>;
def pc    : Ri<13,"pc">,       DwarfRegNum<[13]>;
def stat  : Ri<14,"st">,       DwarfRegNum<[14]>;
def z0    : Ri<15,"z0">,       DwarfRegNum<[15]>;


// Register classes.
// The register order should be defined in terms of the preferred
// allocation order

def IntRegs : RegisterClass<"XPIC", [i32], 32,
                            (add  r2,r3,       // arg1/2, return, scratch
                                  r4,r5,       // arg3/4, scratch
                                  r0,r1,       // special variable
                                  u0,u1,u2,u3, // variable
                                  r6,          // intra-procedure call register, scratch
                                  u4,          // Task Pointer
                                  r7,          // stack/frame ptr
                                  pc,          // programm counter
                                  stat,        // status register
                                  z0           // always zero
                            )>;


// Test
def WReg : RegisterClass<"XPIC", [i32], 32, (add r2,r3,r4,r5,r0,r1)>;
