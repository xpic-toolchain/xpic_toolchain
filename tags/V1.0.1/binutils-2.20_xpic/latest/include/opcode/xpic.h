///TODO:
/* Opcode table for the Atmel AVR micro controllers.

   Copyright 2000, 2001, 2004, 2006, 2008 Free Software Foundation, Inc.
   Contributed by Denis Chertykov <denisc@overta.ru>
   
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street - Fifth Floor, Boston, MA 02110-1301, USA.  */
/** ** ** ** ** ** ** ** ** ** ** ** ** ** X P I C ** ** ** ** ** ** ** ** ** ** ** ** ** ** **/
/** ** ** ** ** ** ** ** ** ** ** ** ** ** X P I C ** ** ** ** ** ** ** ** ** ** ** ** ** ** **/
/** ** ** ** ** ** ** ** ** ** ** ** ** ** X P I C ** ** ** ** ** ** ** ** ** ** ** ** ** ** **/



/// Register codes:
#define REG_R0		0x00
#define REG_R1 		0x01
#define REG_R2		0x02
#define REG_R3		0x03
#define REG_R4		0x04
#define REG_R5		0x05
#define REG_R6		0x06
#define REG_R7		0x07
#define REG_U0		0x08
#define REG_U1		0x09
#define REG_U2		0x0a
#define REG_U3		0x0b
#define REG_U4		0x0c
#define REG_PC		0x0d
#define REG_STAT	0x0e
#define REG_ZERO	0x0f
// pseudo registers for multiplication with 64bit result. Will be encoded as pc and z0
#define REG_U10		0x10
#define REG_U32		0x11
/// Memory Acces Size codes:
#define MAS_BYTE   0
#define MAS_WORD   1
#define MAS_DWORD  2
/// Byte Swap
#define BS_NO      0
#define BS_BYTE    1
#define BS_WORD    2
/// Post increment / Pre decrement
#define PP_POSTINCREMENT  1
#define PP_PREDECREMENT   2
/// Symbols
#define SYMB_UNDEF "???"

/// ISA
#define XPIC_ISA   0x0001 /* device XPIC  */
#define XPIC_ISA2  0x0002 /* device XPIC2 (new instructions: bcd2hex, hex2bcd, gie, gsie, sie)*/


/** constraint letters
registers:
   d - destination reg, (trg) r0…r7,u0…u4,pc,stat,zero (bypassed, i.e. from last command)
   D - destination reg, for 32- bit multiplication : r0..r7,u0…u4,st
                        for 64-bit mutltiplication: u10 (code: z0); u32 (code: pc)
   r - source 1,        (base_reg) r0…r7,u0…u4,pc,stat,zero (bypassed, i.e. from last command)
   w - source 2,        (wreg) r0…r7 (not bypassed, i.e. from the last but one command)
   q - source 2,        (base_reg) r0…r7,u0…u4,pc,stat,zero 

constants:
   I - immediate unsigned value: 0...3.                        (Interrupt enable value)
   M - immediate unsigned value: 1...3.  0 (NULL) not allowed! (interrupt enable Mask)
   T - immediate unsigned value 4 bit
   u - immediate unsigned value 5 bit (10000b+val)
   U - immediate unsigned value 5 bit (00000b+val)
   V - immediate sign value 15 bit (14bit is sign)
   W - immediate unsigned value 15 bit, dword aligned
   X - immediate sign value 15 bit, dword aligned
   Y - immediate relative-addres value 17 bit (2¹⁶, 2¹⁷, 2¹⁸ depending of MAS)
   Z - immediate sign value 19 bit (18bit is sign)

other codes:
   c - condition 
   C - conditional command execution
   v - wcond, workregister condition: r0nz,r1nz,r2nz,r3nz (with automatic decrement of the working register)
   s - sign extention/ 1 bit
   m - memory acces size
   S - swap
   e - bit code of mas, sign and preinc combinations e={m+s+p/P}
   p - preincrement  ( --Reg   )
   P - Postdecrement (   Reg++ )
   j - Jump, relative = pc + -2²⁵...2²⁵ //24 bit [23-sign]
symbols:
   + - Symbol '+' (f.e. load d, [ r _+_ {p}w{P} ])
   [ - start of operands for memory acces
   ] - enf   of operands for memory acces
   x - skip symbolic name 'pc'
*/
/// OPCODES:
/// xpic.xml:
#define XPIC_OPCODE__NOP 0xc0000010
/// 3.2.5 Standard alu commands
#define XPIC_OPCODE__ADD 0x00000000
XPIC_INSN (add1,     "d,[x+X],r"	,"0000000ddddrrrr00XXXXXXXXXXXXXXX", 4, XPIC_ISA, 0x00000000 + XPIC_OPCODE__ADD)
XPIC_INSN (add2,     "d,[W],r"		,"0000000ddddrrrr01WWWWWWWWWWWWWWW", 4, XPIC_ISA, 0x00008000 + XPIC_OPCODE__ADD)
XPIC_INSN (add3,     "d,V,r"		,"0000000ddddrrrr10VVVVVVVVVVVVVVV", 4, XPIC_ISA, 0x00010000 + XPIC_OPCODE__ADD)
XPIC_INSN (add4,     "Cd,r,q"		,"0000000ddddqqqq11000000CCCCCrrrr", 4, XPIC_ISA, 0x00018000 + XPIC_OPCODE__ADD)
XPIC_INSN (add5,     "Cd,s[x+pwP]m,q"	,"0000000ddddqqqq1110eeeeCCCCC0www", 4, XPIC_ISA, 0x0001c000 + XPIC_OPCODE__ADD)
XPIC_INSN (add6,     "Cd,s[pwP]m,q"	,"0000000ddddqqqq1111eeeeCCCCC0www", 4, XPIC_ISA, 0x0001e000 + XPIC_OPCODE__ADD)
#define XPIC_OPCODE__NOR 0x02000000
XPIC_INSN (nor1,     "d,[x+X],r"	  ,"0000000ddddrrrr00XXXXXXXXXXXXXXX", 4, XPIC_ISA, 0x00000000 + XPIC_OPCODE__NOR)
XPIC_INSN (nor2,     "d,[W],r"		  ,"0000000ddddrrrr01WWWWWWWWWWWWWWW", 4, XPIC_ISA, 0x00008000 + XPIC_OPCODE__NOR)
XPIC_INSN (nor3,     "d,V,r"		  ,"0000000ddddrrrr10VVVVVVVVVVVVVVV", 4, XPIC_ISA, 0x00010000 + XPIC_OPCODE__NOR)
XPIC_INSN (nor4,     "Cd,r,q"		  ,"0000000ddddqqqq11000000CCCCCrrrr", 4, XPIC_ISA, 0x00018000 + XPIC_OPCODE__NOR)
XPIC_INSN (nor5,     "Cd,s[x+pwP]m,q"	  ,"0000000ddddqqqq1110eeeeCCCCC0www", 4, XPIC_ISA, 0x0001c000 + XPIC_OPCODE__NOR)
XPIC_INSN (nor6,     "Cd,s[pwP]m,q"	  ,"0000000ddddqqqq1111eeeeCCCCC0www", 4, XPIC_ISA, 0x0001e000 + XPIC_OPCODE__NOR)
#define XPIC_OPCODE__BS 0x04000000
XPIC_INSN (bs1,     "d,[x+X],r"         ,"0000000ddddrrrr00XXXXXXXXXXXXXXX", 4, XPIC_ISA, 0x00000000 + XPIC_OPCODE__BS)
XPIC_INSN (bs2,     "d,[W],r"           ,"0000000ddddrrrr01WWWWWWWWWWWWWWW", 4, XPIC_ISA, 0x00008000 + XPIC_OPCODE__BS)
XPIC_INSN (bs3,     "d,V,r"             ,"0000000ddddrrrr10VVVVVVVVVVVVVVV", 4, XPIC_ISA, 0x00010000 + XPIC_OPCODE__BS)
XPIC_INSN (bs4,     "Cd,r,q"            ,"0000000ddddqqqq11000000CCCCCrrrr", 4, XPIC_ISA, 0x00018000 + XPIC_OPCODE__BS)
XPIC_INSN (bs5,     "Cd,s[x+pwP]m,q"	,"0000000ddddqqqq1110eeeeCCCCC0www", 4, XPIC_ISA, 0x0001c000 + XPIC_OPCODE__BS)
XPIC_INSN (bs6,     "Cd,s[pwP]m,q"	,"0000000ddddqqqq1111eeeeCCCCC0www", 4, XPIC_ISA, 0x0001e000 + XPIC_OPCODE__BS)
#define XPIC_OPCODE__INV 0x06000000
XPIC_INSN (inv1,   "d,[x+X],r"        ,"0000000ddddrrrr00XXXXXXXXXXXXXXX", 4, XPIC_ISA, 0x00000000 + XPIC_OPCODE__INV)
XPIC_INSN (inv2,   "d,[W],r"          ,"0000000ddddrrrr01WWWWWWWWWWWWWWW", 4, XPIC_ISA, 0x00008000 + XPIC_OPCODE__INV)
XPIC_INSN (inv3,   "d,V,r"            ,"0000000ddddrrrr10VVVVVVVVVVVVVVV", 4, XPIC_ISA, 0x00010000 + XPIC_OPCODE__INV)
XPIC_INSN (inv4,   "Cd,r,q"           ,"0000000ddddqqqq11000000CCCCCrrrr", 4, XPIC_ISA, 0x00018000 + XPIC_OPCODE__INV)
XPIC_INSN (inv5,   "Cd,s[x+pwP]m,q"   ,"0000000ddddqqqq1110eeeeCCCCC0www", 4, XPIC_ISA, 0x0001c000 + XPIC_OPCODE__INV)
XPIC_INSN (inv6,   "Cd,s[pwP]m,q"     ,"0000000ddddqqqq1111eeeeCCCCC0www", 4, XPIC_ISA, 0x0001e000 + XPIC_OPCODE__INV)
#define XPIC_OPCODE__NIMP 0x08000000
XPIC_INSN (nimp1,  "d,[x+X],r"         ,"0000000ddddrrrr00XXXXXXXXXXXXXXX", 4, XPIC_ISA, 0x00000000 + XPIC_OPCODE__NIMP)
XPIC_INSN (nimp2,  "d,[W],r"           ,"0000000ddddrrrr01WWWWWWWWWWWWWWW", 4, XPIC_ISA, 0x00008000 + XPIC_OPCODE__NIMP)
XPIC_INSN (nimp3,  "d,V,r"             ,"0000000ddddrrrr10VVVVVVVVVVVVVVV", 4, XPIC_ISA, 0x00010000 + XPIC_OPCODE__NIMP)
XPIC_INSN (nimp4,  "Cd,r,q"            ,"0000000ddddqqqq11000000CCCCCrrrr", 4, XPIC_ISA, 0x00018000 + XPIC_OPCODE__NIMP)
XPIC_INSN (nimp5,  "Cd,s[x+pwP]m,q"    ,"0000000ddddqqqq1110eeeeCCCCC0www", 4, XPIC_ISA, 0x0001c000 + XPIC_OPCODE__NIMP)
XPIC_INSN (nimp6,  "Cd,s[pwP]m,q"      ,"0000000ddddqqqq1111eeeeCCCCC0www", 4, XPIC_ISA, 0x0001e000 + XPIC_OPCODE__NIMP)
#define XPIC_OPCODE__ADDUS 0x0a000000
XPIC_INSN (addus1,     "d,[x+X],r","0000000ddddrrrr00XXXXXXXXXXXXXXX", 4, XPIC_ISA, 0x00000000 + XPIC_OPCODE__ADDUS)
XPIC_INSN (addus2,     "d,[W],r"  ,"0000000ddddrrrr01WWWWWWWWWWWWWWW", 4, XPIC_ISA, 0x00008000 + XPIC_OPCODE__ADDUS)
XPIC_INSN (addus3,     "d,V,r"    ,"0000000ddddrrrr10VVVVVVVVVVVVVVV", 4, XPIC_ISA, 0x00010000 + XPIC_OPCODE__ADDUS)
XPIC_INSN (addus4,     "Cd,r,q"   ,"0000000ddddqqqq11000000CCCCCrrrr", 4, XPIC_ISA, 0x00018000 + XPIC_OPCODE__ADDUS)
XPIC_INSN (addus5,"Cd,s[x+pwP]m,q","0000000ddddqqqq1110eeeeCCCCC0www", 4, XPIC_ISA, 0x0001c000 + XPIC_OPCODE__ADDUS)
XPIC_INSN (addus6,"Cd,s[pwP]m,q"  ,"0000000ddddqqqq1111eeeeCCCCC0www", 4, XPIC_ISA, 0x0001e000 + XPIC_OPCODE__ADDUS)
#define XPIC_OPCODE__XOR 0x0c000000
XPIC_INSN (xor1,     "d,[x+X],r"     ,"0000000ddddrrrr00XXXXXXXXXXXXXXX", 4, XPIC_ISA, 0x00000000 + XPIC_OPCODE__XOR)
XPIC_INSN (xor2,     "d,[W],r"       ,"0000000ddddrrrr01WWWWWWWWWWWWWWW", 4, XPIC_ISA, 0x00008000 + XPIC_OPCODE__XOR)
XPIC_INSN (xor3,     "d,V,r"         ,"0000000ddddrrrr10VVVVVVVVVVVVVVV", 4, XPIC_ISA, 0x00010000 + XPIC_OPCODE__XOR)
XPIC_INSN (xor4,     "Cd,r,q"        ,"0000000ddddqqqq11000000CCCCCrrrr", 4, XPIC_ISA, 0x00018000 + XPIC_OPCODE__XOR)
XPIC_INSN (xor5,     "Cd,s[x+pwP]m,q","0000000ddddqqqq1110eeeeCCCCC0www", 4, XPIC_ISA, 0x0001c000 + XPIC_OPCODE__XOR)
XPIC_INSN (xor6,     "Cd,s[pwP]m,q"  ,"0000000ddddqqqq1111eeeeCCCCC0www", 4, XPIC_ISA, 0x0001e000 + XPIC_OPCODE__XOR)
#define XPIC_OPCODE__NAND 0x0e000000
XPIC_INSN (nand1,     "d,[x+X],r"    ,"0000000ddddrrrr00XXXXXXXXXXXXXXX", 4, XPIC_ISA, 0x00000000 + XPIC_OPCODE__NAND)
XPIC_INSN (nand2,     "d,[W],r"      ,"0000000ddddrrrr01WWWWWWWWWWWWWWW", 4, XPIC_ISA, 0x00008000 + XPIC_OPCODE__NAND)
XPIC_INSN (nand3,     "d,V,r"        ,"0000000ddddrrrr10VVVVVVVVVVVVVVV", 4, XPIC_ISA, 0x00010000 + XPIC_OPCODE__NAND)
XPIC_INSN (nand4,     "Cd,r,q"       ,"0000000ddddqqqq11000000CCCCCrrrr", 4, XPIC_ISA, 0x00018000 + XPIC_OPCODE__NAND)
XPIC_INSN (nand5,    "Cd,s[x+pwP]m,q","0000000ddddqqqq1110eeeeCCCCC0www", 4, XPIC_ISA, 0x0001c000 + XPIC_OPCODE__NAND)
XPIC_INSN (nand6,    "Cd,s[pwP]m,q"  ,"0000000ddddqqqq1111eeeeCCCCC0www", 4, XPIC_ISA, 0x0001e000 + XPIC_OPCODE__NAND)
#define XPIC_OPCODE__AND 0x10000000
XPIC_INSN (and1,     "d,[x+X],r"     ,"0000000ddddrrrr00XXXXXXXXXXXXXXX", 4, XPIC_ISA, 0x00000000 + XPIC_OPCODE__AND)
XPIC_INSN (and2,     "d,[W],r"       ,"0000000ddddrrrr01WWWWWWWWWWWWWWW", 4, XPIC_ISA, 0x00008000 + XPIC_OPCODE__AND)
XPIC_INSN (and3,     "d,V,r"         ,"0000000ddddrrrr10VVVVVVVVVVVVVVV", 4, XPIC_ISA, 0x00010000 + XPIC_OPCODE__AND)
XPIC_INSN (and4,     "Cd,r,q"        ,"0000000ddddqqqq11000000CCCCCrrrr", 4, XPIC_ISA, 0x00018000 + XPIC_OPCODE__AND)
XPIC_INSN (and5,     "Cd,s[x+pwP]m,q","0000000ddddqqqq1110eeeeCCCCC0www", 4, XPIC_ISA, 0x0001c000 + XPIC_OPCODE__AND)
XPIC_INSN (and6,     "Cd,s[pwP]m,q"  ,"0000000ddddqqqq1111eeeeCCCCC0www", 4, XPIC_ISA, 0x0001e000 + XPIC_OPCODE__AND)
#define XPIC_OPCODE__XNOR 0x12000000
XPIC_INSN (xnor1,     "d,[x+X],r"    ,"0000000ddddrrrr00XXXXXXXXXXXXXXX", 4, XPIC_ISA, 0x00000000 + XPIC_OPCODE__XNOR)
XPIC_INSN (xnor2,     "d,[W],r"      ,"0000000ddddrrrr01WWWWWWWWWWWWWWW", 4, XPIC_ISA, 0x00008000 + XPIC_OPCODE__XNOR)
XPIC_INSN (xnor3,     "d,V,r"        ,"0000000ddddrrrr10VVVVVVVVVVVVVVV", 4, XPIC_ISA, 0x00010000 + XPIC_OPCODE__XNOR)
XPIC_INSN (xnor4,     "Cd,r,q"       ,"0000000ddddqqqq11000000CCCCCrrrr", 4, XPIC_ISA, 0x00018000 + XPIC_OPCODE__XNOR)
XPIC_INSN (xnor5,    "Cd,s[x+pwP]m,q","0000000ddddqqqq1110eeeeCCCCC0www", 4, XPIC_ISA, 0x0001c000 + XPIC_OPCODE__XNOR)
XPIC_INSN (xnor6,     "Cd,s[pwP]m,q" ,"0000000ddddqqqq1111eeeeCCCCC0www", 4, XPIC_ISA, 0x0001e000 + XPIC_OPCODE__XNOR)
#define XPIC_OPCODE__ADDSS 0x14000000
XPIC_INSN (addss1,     "d,[x+X],r","0000000ddddrrrr00XXXXXXXXXXXXXXX", 4, XPIC_ISA, 0x00000000 + XPIC_OPCODE__ADDSS)
XPIC_INSN (addss2,     "d,[W],r"  ,"0000000ddddrrrr01WWWWWWWWWWWWWWW", 4, XPIC_ISA, 0x00008000 + XPIC_OPCODE__ADDSS)
XPIC_INSN (addss3,     "d,V,r"    ,"0000000ddddrrrr10VVVVVVVVVVVVVVV", 4, XPIC_ISA, 0x00010000 + XPIC_OPCODE__ADDSS)
XPIC_INSN (addss4,     "Cd,r,q"   ,"0000000ddddqqqq11000000CCCCCrrrr", 4, XPIC_ISA, 0x00018000 + XPIC_OPCODE__ADDSS)
XPIC_INSN (addss5,"Cd,s[x+pwP]m,q","0000000ddddqqqq1110eeeeCCCCC0www", 4, XPIC_ISA, 0x0001c000 + XPIC_OPCODE__ADDSS)
XPIC_INSN (addss6,  "Cd,s[pwP]m,q","0000000ddddqqqq1111eeeeCCCCC0www", 4, XPIC_ISA, 0x0001e000 + XPIC_OPCODE__ADDSS)
#define XPIC_OPCODE__IMP 0x16000000
XPIC_INSN (imp1,     "d,[x+X],r"     ,"0000000ddddrrrr00XXXXXXXXXXXXXXX", 4, XPIC_ISA, 0x00000000 + XPIC_OPCODE__IMP)
XPIC_INSN (imp2,     "d,[W],r"       ,"0000000ddddrrrr01WWWWWWWWWWWWWWW", 4, XPIC_ISA, 0x00008000 + XPIC_OPCODE__IMP)
XPIC_INSN (imp3,     "d,V,r"         ,"0000000ddddrrrr10VVVVVVVVVVVVVVV", 4, XPIC_ISA, 0x00010000 + XPIC_OPCODE__IMP)
XPIC_INSN (imp4,     "Cd,r,q"        ,"0000000ddddqqqq11000000CCCCCrrrr", 4, XPIC_ISA, 0x00018000 + XPIC_OPCODE__IMP)
XPIC_INSN (imp5,     "Cd,s[x+pwP]m,q","0000000ddddqqqq1110eeeeCCCCC0www", 4, XPIC_ISA, 0x0001c000 + XPIC_OPCODE__IMP)
XPIC_INSN (imp6,     "Cd,s[pwP]m,q"  ,"0000000ddddqqqq1111eeeeCCCCC0www", 4, XPIC_ISA, 0x0001e000 + XPIC_OPCODE__IMP)
#define XPIC_OPCODE__MOV 0x18000000
XPIC_INSN (mov1,     "d,[x+X],r"     ,"0000000ddddrrrr00XXXXXXXXXXXXXXX", 4, XPIC_ISA, 0x00000000 + XPIC_OPCODE__MOV)
XPIC_INSN (mov2,     "d,[W],r"       ,"0000000ddddrrrr01WWWWWWWWWWWWWWW", 4, XPIC_ISA, 0x00008000 + XPIC_OPCODE__MOV)
XPIC_INSN (mov3,     "d,V,r"         ,"0000000ddddrrrr10VVVVVVVVVVVVVVV", 4, XPIC_ISA, 0x00010000 + XPIC_OPCODE__MOV)
XPIC_INSN (mov4,     "Cd,r,q"        ,"0000000ddddqqqq11000000CCCCCrrrr", 4, XPIC_ISA, 0x00018000 + XPIC_OPCODE__MOV)
XPIC_INSN (mov5,     "Cd,s[x+pwP]m,q","0000000ddddqqqq1110eeeeCCCCC0www", 4, XPIC_ISA, 0x0001c000 + XPIC_OPCODE__MOV)
XPIC_INSN (mov6,     "Cd,s[pwP]m,q"  ,"0000000ddddqqqq1111eeeeCCCCC0www", 4, XPIC_ISA, 0x0001e000 + XPIC_OPCODE__MOV)
#define XPIC_OPCODE__BBE 0x1a000000
XPIC_INSN (bbe1,     "d,[x+X],r"    ,"0000000ddddrrrr00XXXXXXXXXXXXXXX", 4, XPIC_ISA, 0x00000000 + XPIC_OPCODE__BBE)
XPIC_INSN (bbe2,     "d,[W],r"      ,"0000000ddddrrrr01WWWWWWWWWWWWWWW", 4, XPIC_ISA, 0x00008000 + XPIC_OPCODE__BBE)
XPIC_INSN (bbe3,     "d,V,r"        ,"0000000ddddrrrr10VVVVVVVVVVVVVVV", 4, XPIC_ISA, 0x00010000 + XPIC_OPCODE__BBE)
XPIC_INSN (bbe4,     "Cd,r,q"       ,"0000000ddddqqqq11000000CCCCCrrrr", 4, XPIC_ISA, 0x00018000 + XPIC_OPCODE__BBE)
XPIC_INSN (bbe5,    "Cd,s[x+pwP]m,q","0000000ddddqqqq1110eeeeCCCCC0www", 4, XPIC_ISA, 0x0001c000 + XPIC_OPCODE__BBE)
XPIC_INSN (bbe6,     "Cd,s[pwP]m,q" ,"0000000ddddqqqq1111eeeeCCCCC0www", 4, XPIC_ISA, 0x0001e000 + XPIC_OPCODE__BBE)
#define XPIC_OPCODE__OR 0x1c000000
XPIC_INSN (or1,     "d,[x+X],r"     ,"0000000ddddrrrr00XXXXXXXXXXXXXXX", 4, XPIC_ISA, 0x00000000 + XPIC_OPCODE__OR)
XPIC_INSN (or2,     "d,[W],r"       ,"0000000ddddrrrr01WWWWWWWWWWWWWWW", 4, XPIC_ISA, 0x00008000 + XPIC_OPCODE__OR)
XPIC_INSN (or3,     "d,V,r"         ,"0000000ddddrrrr10VVVVVVVVVVVVVVV", 4, XPIC_ISA, 0x00010000 + XPIC_OPCODE__OR)
XPIC_INSN (or4,     "Cd,r,q"        ,"0000000ddddqqqq11000000CCCCCrrrr", 4, XPIC_ISA, 0x00018000 + XPIC_OPCODE__OR)
XPIC_INSN (or5,     "Cd,s[x+pwP]m,q","0000000ddddqqqq1110eeeeCCCCC0www", 4, XPIC_ISA, 0x0001c000 + XPIC_OPCODE__OR)
XPIC_INSN (or6,     "Cd,s[pwP]m,q"  ,"0000000ddddqqqq1111eeeeCCCCC0www", 4, XPIC_ISA, 0x0001e000 + XPIC_OPCODE__OR)
#define XPIC_OPCODE__ADDC 0x1e000000
XPIC_INSN (addc1,     "d,[x+X],r"    ,"0000000ddddrrrr00XXXXXXXXXXXXXXX", 4, XPIC_ISA, 0x00000000 + XPIC_OPCODE__ADDC)
XPIC_INSN (addc2,     "d,[W],r"      ,"0000000ddddrrrr01WWWWWWWWWWWWWWW", 4, XPIC_ISA, 0x00008000 + XPIC_OPCODE__ADDC)
XPIC_INSN (addc3,     "d,V,r"        ,"0000000ddddrrrr10VVVVVVVVVVVVVVV", 4, XPIC_ISA, 0x00010000 + XPIC_OPCODE__ADDC)
XPIC_INSN (addc4,     "Cd,r,q"       ,"0000000ddddqqqq11000000CCCCCrrrr", 4, XPIC_ISA, 0x00018000 + XPIC_OPCODE__ADDC)
XPIC_INSN (addc5,     "Cd,s[x+pwP]m,q","0000000ddddqqqq1110eeeeCCCCC0www", 4, XPIC_ISA, 0x0001c000 + XPIC_OPCODE__ADDC)
XPIC_INSN (addc6,     "Cd,s[pwP]m,q" ,"0000000ddddqqqq1111eeeeCCCCC0www", 4, XPIC_ISA, 0x0001e000 + XPIC_OPCODE__ADDC)
#define XPIC_OPCODE__SUB 0x20000000
XPIC_INSN (sub1,     "d,[x+X],r"     ,"0000000ddddrrrr00XXXXXXXXXXXXXXX", 4, XPIC_ISA, 0x00000000 + XPIC_OPCODE__SUB)
XPIC_INSN (sub2,     "d,[W],r"       ,"0000000ddddrrrr01WWWWWWWWWWWWWWW", 4, XPIC_ISA, 0x00008000 + XPIC_OPCODE__SUB)
XPIC_INSN (sub3,     "d,V,r"         ,"0000000ddddrrrr10VVVVVVVVVVVVVVV", 4, XPIC_ISA, 0x00010000 + XPIC_OPCODE__SUB)
XPIC_INSN (sub4,     "Cd,r,q"        ,"0000000ddddqqqq11000000CCCCCrrrr", 4, XPIC_ISA, 0x00018000 + XPIC_OPCODE__SUB)
XPIC_INSN (sub5,     "Cd,s[x+pwP]m,q","0000000ddddqqqq1110eeeeCCCCC0www", 4, XPIC_ISA, 0x0001c000 + XPIC_OPCODE__SUB)
XPIC_INSN (sub6,     "Cd,s[pwP]m,q"  ,"0000000ddddqqqq1111eeeeCCCCC0www", 4, XPIC_ISA, 0x0001e000 + XPIC_OPCODE__SUB)
#define XPIC_OPCODE__SUBUS 0x22000000
XPIC_INSN (subus1,     "d,[x+X],r","0000000ddddrrrr00XXXXXXXXXXXXXXX", 4, XPIC_ISA, 0x00000000 + XPIC_OPCODE__SUBUS)
XPIC_INSN (subus2,     "d,[W],r"  ,"0000000ddddrrrr01WWWWWWWWWWWWWWW", 4, XPIC_ISA, 0x00008000 + XPIC_OPCODE__SUBUS)
XPIC_INSN (subus3,     "d,V,r"    ,"0000000ddddrrrr10VVVVVVVVVVVVVVV", 4, XPIC_ISA, 0x00010000 + XPIC_OPCODE__SUBUS)
XPIC_INSN (subus4,     "Cd,r,q"   ,"0000000ddddqqqq11000000CCCCCrrrr", 4, XPIC_ISA, 0x00018000 + XPIC_OPCODE__SUBUS)
XPIC_INSN (subus5,"Cd,s[x+pwP]m,q","0000000ddddqqqq1110eeeeCCCCC0www", 4, XPIC_ISA, 0x0001c000 + XPIC_OPCODE__SUBUS)
XPIC_INSN (subus6,"Cd,s[pwP]m,q"  ,"0000000ddddqqqq1111eeeeCCCCC0www", 4, XPIC_ISA, 0x0001e000 + XPIC_OPCODE__SUBUS)
#define XPIC_OPCODE__SUBSS 0x24000000
XPIC_INSN (subss1,     "d,[x+X],r","0000000ddddrrrr00XXXXXXXXXXXXXXX", 4, XPIC_ISA, 0x00000000 + XPIC_OPCODE__SUBSS)
XPIC_INSN (subss2,     "d,[W],r"  ,"0000000ddddrrrr01WWWWWWWWWWWWWWW", 4, XPIC_ISA, 0x00008000 + XPIC_OPCODE__SUBSS)
XPIC_INSN (subss3,     "d,V,r"    ,"0000000ddddrrrr10VVVVVVVVVVVVVVV", 4, XPIC_ISA, 0x00010000 + XPIC_OPCODE__SUBSS)
XPIC_INSN (subss4,     "Cd,r,q"   ,"0000000ddddqqqq11000000CCCCCrrrr", 4, XPIC_ISA, 0x00018000 + XPIC_OPCODE__SUBSS)
XPIC_INSN (subss5,"Cd,s[x+pwP]m,q","0000000ddddqqqq1110eeeeCCCCC0www", 4, XPIC_ISA, 0x0001c000 + XPIC_OPCODE__SUBSS)
XPIC_INSN (subss6,"Cd,s[pwP]m,q"  ,"0000000ddddqqqq1111eeeeCCCCC0www", 4, XPIC_ISA, 0x0001e000 + XPIC_OPCODE__SUBSS)
#define XPIC_OPCODE__SUBC 0x26000000
XPIC_INSN (subc1,     "d,[x+X],r"    ,"0000000ddddrrrr00XXXXXXXXXXXXXXX", 4, XPIC_ISA, 0x00000000 + XPIC_OPCODE__SUBC)
XPIC_INSN (subc2,     "d,[W],r"      ,"0000000ddddrrrr01WWWWWWWWWWWWWWW", 4, XPIC_ISA, 0x00008000 + XPIC_OPCODE__SUBC)
XPIC_INSN (subc3,     "d,V,r"        ,"0000000ddddrrrr10VVVVVVVVVVVVVVV", 4, XPIC_ISA, 0x00010000 + XPIC_OPCODE__SUBC)
XPIC_INSN (subc4,     "Cd,r,q"       ,"0000000ddddqqqq11000000CCCCCrrrr", 4, XPIC_ISA, 0x00018000 + XPIC_OPCODE__SUBC)
XPIC_INSN (subc5,     "Cd,s[x+pwP]m,q","0000000ddddqqqq1110eeeeCCCCC0www", 4, XPIC_ISA, 0x0001c000 + XPIC_OPCODE__SUBC)
XPIC_INSN (subc6,     "Cd,s[pwP]m,q" ,"0000000ddddqqqq1111eeeeCCCCC0www", 4, XPIC_ISA, 0x0001e000 + XPIC_OPCODE__SUBC)
#define XPIC_OPCODE__LSL 0x28000000
XPIC_INSN (lsl1,     "d,[x+X],r"     ,"0000000ddddrrrr00XXXXXXXXXXXXXXX", 4, XPIC_ISA, 0x00000000 + XPIC_OPCODE__LSL)
XPIC_INSN (lsl2,     "d,[W],r"       ,"0000000ddddrrrr01WWWWWWWWWWWWWWW", 4, XPIC_ISA, 0x00008000 + XPIC_OPCODE__LSL)
XPIC_INSN (lsl3,     "d,V,r"         ,"0000000ddddrrrr10VVVVVVVVVVVVVVV", 4, XPIC_ISA, 0x00010000 + XPIC_OPCODE__LSL)
XPIC_INSN (lsl4,     "Cd,r,q"        ,"0000000ddddqqqq11000000CCCCCrrrr", 4, XPIC_ISA, 0x00018000 + XPIC_OPCODE__LSL)
XPIC_INSN (lsl5,     "Cd,s[x+pwP]m,q","0000000ddddqqqq1110eeeeCCCCC0www", 4, XPIC_ISA, 0x0001c000 + XPIC_OPCODE__LSL)
XPIC_INSN (lsl6,     "Cd,s[pwP]m,q"  ,"0000000ddddqqqq1111eeeeCCCCC0www", 4, XPIC_ISA, 0x0001e000 + XPIC_OPCODE__LSL)
#define XPIC_OPCODE__LSR 0x2e000000
XPIC_INSN (lsr1,     "d,[x+X],r"     ,"0000000ddddrrrr00XXXXXXXXXXXXXXX", 4, XPIC_ISA, 0x00000000 + XPIC_OPCODE__LSR)
XPIC_INSN (lsr2,     "d,[W],r"       ,"0000000ddddrrrr01WWWWWWWWWWWWWWW", 4, XPIC_ISA, 0x00008000 + XPIC_OPCODE__LSR)
XPIC_INSN (lsr3,     "d,V,r"         ,"0000000ddddrrrr10VVVVVVVVVVVVVVV", 4, XPIC_ISA, 0x00010000 + XPIC_OPCODE__LSR)
XPIC_INSN (lsr4,     "Cd,r,q"        ,"0000000ddddqqqq11000000CCCCCrrrr", 4, XPIC_ISA, 0x00018000 + XPIC_OPCODE__LSR)
XPIC_INSN (lsr5,     "Cd,s[x+pwP]m,q","0000000ddddqqqq1110eeeeCCCCC0www", 4, XPIC_ISA, 0x0001c000 + XPIC_OPCODE__LSR)
XPIC_INSN (lsr6,     "Cd,s[pwP]m,q"  ,"0000000ddddqqqq1111eeeeCCCCC0www", 4, XPIC_ISA, 0x0001e000 + XPIC_OPCODE__LSR)
#define XPIC_OPCODE__ASR 0x30000000
XPIC_INSN (asr1,     "d,[x+X],r"     ,"0000000ddddrrrr00XXXXXXXXXXXXXXX", 4, XPIC_ISA, 0x00000000 + XPIC_OPCODE__ASR)
XPIC_INSN (asr2,     "d,[W],r"       ,"0000000ddddrrrr01WWWWWWWWWWWWWWW", 4, XPIC_ISA, 0x00008000 + XPIC_OPCODE__ASR)
XPIC_INSN (asr3,     "d,V,r"         ,"0000000ddddrrrr10VVVVVVVVVVVVVVV", 4, XPIC_ISA, 0x00010000 + XPIC_OPCODE__ASR)
XPIC_INSN (asr4,     "Cd,r,q"        ,"0000000ddddqqqq11000000CCCCCrrrr", 4, XPIC_ISA, 0x00018000 + XPIC_OPCODE__ASR)
XPIC_INSN (asr5,     "Cd,s[x+pwP]m,q","0000000ddddqqqq1110eeeeCCCCC0www", 4, XPIC_ISA, 0x0001c000 + XPIC_OPCODE__ASR)
XPIC_INSN (asr6,     "Cd,s[pwP]m,q"  ,"0000000ddddqqqq1111eeeeCCCCC0www", 4, XPIC_ISA, 0x0001e000 + XPIC_OPCODE__ASR)
#define XPIC_OPCODE__ROL 0x32000000
XPIC_INSN (rol1,     "d,[x+X],r"     ,"0000000ddddrrrr00XXXXXXXXXXXXXXX", 4, XPIC_ISA, 0x00000000 + XPIC_OPCODE__ROL)
XPIC_INSN (rol2,     "d,[W],r"       ,"0000000ddddrrrr01WWWWWWWWWWWWWWW", 4, XPIC_ISA, 0x00008000 + XPIC_OPCODE__ROL)
XPIC_INSN (rol3,     "d,V,r"         ,"0000000ddddrrrr10VVVVVVVVVVVVVVV", 4, XPIC_ISA, 0x00010000 + XPIC_OPCODE__ROL)
XPIC_INSN (rol4,     "Cd,r,q"        ,"0000000ddddqqqq11000000CCCCCrrrr", 4, XPIC_ISA, 0x00018000 + XPIC_OPCODE__ROL)
XPIC_INSN (rol5,     "Cd,s[x+pwP]m,q","0000000ddddqqqq1110eeeeCCCCC0www", 4, XPIC_ISA, 0x0001c000 + XPIC_OPCODE__ROL)
XPIC_INSN (rol6,     "Cd,s[pwP]m,q"  ,"0000000ddddqqqq1111eeeeCCCCC0www", 4, XPIC_ISA, 0x0001e000 + XPIC_OPCODE__ROL)
#define XPIC_OPCODE__MAXU 0x4c000000
XPIC_INSN (maxu1,     "d,[x+X],r"    ,"0000000ddddrrrr00XXXXXXXXXXXXXXX", 4, XPIC_ISA, 0x00000000 + XPIC_OPCODE__MAXU)
XPIC_INSN (maxu2,     "d,[W],r"      ,"0000000ddddrrrr01WWWWWWWWWWWWWWW", 4, XPIC_ISA, 0x00008000 + XPIC_OPCODE__MAXU)
XPIC_INSN (maxu3,     "d,V,r"        ,"0000000ddddrrrr10VVVVVVVVVVVVVVV", 4, XPIC_ISA, 0x00010000 + XPIC_OPCODE__MAXU)
XPIC_INSN (maxu4,     "Cd,r,q"       ,"0000000ddddqqqq11000000CCCCCrrrr", 4, XPIC_ISA, 0x00018000 + XPIC_OPCODE__MAXU)
XPIC_INSN (maxu5,     "Cd,s[x+pwP]m,q","0000000ddddqqqq1110eeeeCCCCC0www", 4, XPIC_ISA, 0x0001c000 + XPIC_OPCODE__MAXU)
XPIC_INSN (maxu6,     "Cd,s[pwP]m,q" ,"0000000ddddqqqq1111eeeeCCCCC0www", 4, XPIC_ISA, 0x0001e000 + XPIC_OPCODE__MAXU)
#define XPIC_OPCODE__MAXS 0x4e000000
XPIC_INSN (maxs1,     "d,[x+X],r"    ,"0000000ddddrrrr00XXXXXXXXXXXXXXX", 4, XPIC_ISA, 0x00000000 + XPIC_OPCODE__MAXS)
XPIC_INSN (maxs2,     "d,[W],r"      ,"0000000ddddrrrr01WWWWWWWWWWWWWWW", 4, XPIC_ISA, 0x00008000 + XPIC_OPCODE__MAXS)
XPIC_INSN (maxs3,     "d,V,r"        ,"0000000ddddrrrr10VVVVVVVVVVVVVVV", 4, XPIC_ISA, 0x00010000 + XPIC_OPCODE__MAXS)
XPIC_INSN (maxs4,     "Cd,r,q"       ,"0000000ddddqqqq11000000CCCCCrrrr", 4, XPIC_ISA, 0x00018000 + XPIC_OPCODE__MAXS)
XPIC_INSN (maxs5,     "Cd,s[x+pwP]m,q","0000000ddddqqqq1110eeeeCCCCC0www", 4, XPIC_ISA, 0x0001c000 + XPIC_OPCODE__MAXS)
XPIC_INSN (maxs6,     "Cd,s[pwP]m,q" ,"0000000ddddqqqq1111eeeeCCCCC0www", 4, XPIC_ISA, 0x0001e000 + XPIC_OPCODE__MAXS)
#define XPIC_OPCODE__MINU 0x50000000
XPIC_INSN (minu1,     "d,[x+X],r"    ,"0000000ddddrrrr00XXXXXXXXXXXXXXX", 4, XPIC_ISA, 0x00000000 + XPIC_OPCODE__MINU)
XPIC_INSN (minu2,     "d,[W],r"      ,"0000000ddddrrrr01WWWWWWWWWWWWWWW", 4, XPIC_ISA, 0x00008000 + XPIC_OPCODE__MINU)
XPIC_INSN (minu3,     "d,V,r"        ,"0000000ddddrrrr10VVVVVVVVVVVVVVV", 4, XPIC_ISA, 0x00010000 + XPIC_OPCODE__MINU)
XPIC_INSN (minu4,     "Cd,r,q"       ,"0000000ddddqqqq11000000CCCCCrrrr", 4, XPIC_ISA, 0x00018000 + XPIC_OPCODE__MINU)
XPIC_INSN (minu5,     "Cd,s[x+pwP]m,q","0000000ddddqqqq1110eeeeCCCCC0www", 4, XPIC_ISA, 0x0001c000 + XPIC_OPCODE__MINU)
XPIC_INSN (minu6,     "Cd,s[pwP]m,q" ,"0000000ddddqqqq1111eeeeCCCCC0www", 4, XPIC_ISA, 0x0001e000 + XPIC_OPCODE__MINU)
#define XPIC_OPCODE__MINS 0x52000000
XPIC_INSN (mins1,     "d,[x+X],r"    ,"0000000ddddrrrr00XXXXXXXXXXXXXXX", 4, XPIC_ISA, 0x00000000 + XPIC_OPCODE__MINS)
XPIC_INSN (mins2,     "d,[W],r"      ,"0000000ddddrrrr01WWWWWWWWWWWWWWW", 4, XPIC_ISA, 0x00008000 + XPIC_OPCODE__MINS)
XPIC_INSN (mins3,     "d,V,r"        ,"0000000ddddrrrr10VVVVVVVVVVVVVVV", 4, XPIC_ISA, 0x00010000 + XPIC_OPCODE__MINS)
XPIC_INSN (mins4,     "Cd,r,q"       ,"0000000ddddqqqq11000000CCCCCrrrr", 4, XPIC_ISA, 0x00018000 + XPIC_OPCODE__MINS)
XPIC_INSN (mins5,     "Cd,s[x+pwP]m,q","0000000ddddqqqq1110eeeeCCCCC0www", 4, XPIC_ISA, 0x0001c000 + XPIC_OPCODE__MINS)
XPIC_INSN (mins6,     "Cd,s[pwP]m,q" ,"0000000ddddqqqq1111eeeeCCCCC0www", 4, XPIC_ISA, 0x0001e000 + XPIC_OPCODE__MINS)
#define XPIC_OPCODE__CLZ 0x64000000
XPIC_INSN (clz1,     "d,[x+X],r"     ,"0000000ddddrrrr00XXXXXXXXXXXXXXX", 4, XPIC_ISA, 0x00000000 + XPIC_OPCODE__CLZ )
XPIC_INSN (clz2,     "d,[W],r"       ,"0000000ddddrrrr01WWWWWWWWWWWWWWW", 4, XPIC_ISA, 0x00008000 + XPIC_OPCODE__CLZ )
XPIC_INSN (clz3,     "d,V,r"         ,"0000000ddddrrrr10VVVVVVVVVVVVVVV", 4, XPIC_ISA, 0x00010000 + XPIC_OPCODE__CLZ )
XPIC_INSN (clz4,     "Cd,r,q"        ,"0000000ddddqqqq11000000CCCCCrrrr", 4, XPIC_ISA, 0x00018000 + XPIC_OPCODE__CLZ )
XPIC_INSN (clz5,     "Cd,s[x+pwP]m,q","0000000ddddqqqq1110eeeeCCCCC0www", 4, XPIC_ISA, 0x0001c000 + XPIC_OPCODE__CLZ )
XPIC_INSN (clz6,     "Cd,s[pwP]m,q"  ,"0000000ddddqqqq1111eeeeCCCCC0www", 4, XPIC_ISA, 0x0001e000 + XPIC_OPCODE__CLZ )
#define XPIC_OPCODE__CLO 0x66000000
XPIC_INSN (clo1,     "d,[x+X],r"     ,"0000000ddddrrrr00XXXXXXXXXXXXXXX", 4, XPIC_ISA, 0x00000000 + XPIC_OPCODE__CLO)
XPIC_INSN (clo2,     "d,[W],r"       ,"0000000ddddrrrr01WWWWWWWWWWWWWWW", 4, XPIC_ISA, 0x00008000 + XPIC_OPCODE__CLO)
XPIC_INSN (clo3,     "d,V,r"         ,"0000000ddddrrrr10VVVVVVVVVVVVVVV", 4, XPIC_ISA, 0x00010000 + XPIC_OPCODE__CLO)
XPIC_INSN (clo4,     "Cd,r,q"        ,"0000000ddddqqqq11000000CCCCCrrrr", 4, XPIC_ISA, 0x00018000 + XPIC_OPCODE__CLO)
XPIC_INSN (clo5,     "Cd,s[x+pwP]m,q","0000000ddddqqqq1110eeeeCCCCC0www", 4, XPIC_ISA, 0x0001c000 + XPIC_OPCODE__CLO)
XPIC_INSN (clo6,     "Cd,s[pwP]m,q"  ,"0000000ddddqqqq1111eeeeCCCCC0www", 4, XPIC_ISA, 0x0001e000 + XPIC_OPCODE__CLO)
#define XPIC_OPCODE__MEAN 0x68000000
XPIC_INSN (mean1,     "d,[x+X],r"   ,"0000000ddddrrrr00XXXXXXXXXXXXXXX", 4, XPIC_ISA, 0x00000000 + XPIC_OPCODE__MEAN)
XPIC_INSN (mean2,     "d,[W],r"     ,"0000000ddddrrrr01WWWWWWWWWWWWWWW", 4, XPIC_ISA, 0x00008000 + XPIC_OPCODE__MEAN)
XPIC_INSN (mean3,     "d,V,r"       ,"0000000ddddrrrr10VVVVVVVVVVVVVVV", 4, XPIC_ISA, 0x00010000 + XPIC_OPCODE__MEAN)
XPIC_INSN (mean4,     "Cd,r,q"      ,"0000000ddddqqqq11000000CCCCCrrrr", 4, XPIC_ISA, 0x00018000 + XPIC_OPCODE__MEAN)
XPIC_INSN (mean5,     "Cd,s[x+pwP]m,q","0000000ddddqqqq1110eeeeCCCCC0www", 4, XPIC_ISA, 0x0001c000 + XPIC_OPCODE__MEAN)
XPIC_INSN (mean6,     "Cd,s[pwP]m,q","0000000ddddqqqq1111eeeeCCCCC0www", 4, XPIC_ISA, 0x0001e000 + XPIC_OPCODE__MEAN)
#define XPIC_OPCODE__CLMSB 0x6a000000
XPIC_INSN (clmsb1,     "d,[x+X],r"   ,"0000000ddddrrrr00XXXXXXXXXXXXXXX", 4, XPIC_ISA, 0x00000000 + XPIC_OPCODE__CLMSB)
XPIC_INSN (clmsb2,     "d,[W],r"     ,"0000000ddddrrrr01WWWWWWWWWWWWWWW", 4, XPIC_ISA, 0x00008000 + XPIC_OPCODE__CLMSB)
XPIC_INSN (clmsb3,     "d,V,r"       ,"0000000ddddrrrr10VVVVVVVVVVVVVVV", 4, XPIC_ISA, 0x00010000 + XPIC_OPCODE__CLMSB)
XPIC_INSN (clmsb4,     "Cd,r,q"      ,"0000000ddddqqqq11000000CCCCCrrrr", 4, XPIC_ISA, 0x00018000 + XPIC_OPCODE__CLMSB)
XPIC_INSN (clmsb5,     "Cd,s[x+pwP]m,q","0000000ddddqqqq1110eeeeCCCCC0www", 4, XPIC_ISA, 0x0001c000 + XPIC_OPCODE__CLMSB)
XPIC_INSN (clmsb6,     "Cd,s[pwP]m,q","0000000ddddqqqq1111eeeeCCCCC0www", 4, XPIC_ISA, 0x0001e000 + XPIC_OPCODE__CLMSB)
/// 3.2.6 Shift and rotate commands with small constant 0 to 31
#define XPIC_OPCODE__LSL_H 0x34000000
#define XPIC_OPCODE__LSL_L 0x40000000
XPIC_INSN (lsl7,     "d,[x+X],U"  ,"0000000ddddUUUU00XXXXXXXXXXXXXXX", 4, XPIC_ISA, 0x00000000 + XPIC_OPCODE__LSL_L)
XPIC_INSN (lsl8,     "d,[W],U"    ,"0000000ddddUUUU01WWWWWWWWWWWWWWW", 4, XPIC_ISA, 0x00008000 + XPIC_OPCODE__LSL_L)
XPIC_INSN (lsl9,     "d,V,U"      ,"0000000ddddUUUU10VVVVVVVVVVVVVVV", 4, XPIC_ISA, 0x00010000 + XPIC_OPCODE__LSL_L)
XPIC_INSN (lsla,     "Cd,r,U"     ,"0000000ddddUUUU11000000CCCCCrrrr", 4, XPIC_ISA, 0x00018000 + XPIC_OPCODE__LSL_L)
XPIC_INSN (lslb,  "Cd,s[x+pwP]m,U","0000000ddddUUUU1110eeeeCCCCC0www", 4, XPIC_ISA, 0x0001c000 + XPIC_OPCODE__LSL_L)
XPIC_INSN (lslc,  "Cd,s[pwP]m,U"  ,"0000000ddddUUUU1111eeeeCCCCC0www", 4, XPIC_ISA, 0x0001e000 + XPIC_OPCODE__LSL_L)
//for xpic-dis.c:
XPIC_INSN (lsld,     "d,[x+X],u"  ,"0000000dddduuuu00XXXXXXXXXXXXXXX", 4, XPIC_ISA, 0x00000000 + XPIC_OPCODE__LSL_H)
XPIC_INSN (lsle,     "d,[W],u"    ,"0000000dddduuuu01WWWWWWWWWWWWWWW", 4, XPIC_ISA, 0x00008000 + XPIC_OPCODE__LSL_H)
XPIC_INSN (lslf,     "d,V,u"      ,"0000000dddduuuu10VVVVVVVVVVVVVVV", 4, XPIC_ISA, 0x00010000 + XPIC_OPCODE__LSL_H)
XPIC_INSN (lslg,     "Cd,r,u"     ,"0000000dddduuuu11000000CCCCCrrrr", 4, XPIC_ISA, 0x00018000 + XPIC_OPCODE__LSL_H)
XPIC_INSN (lslh,  "Cd,s[x+pwP]m,u","0000000dddduuuu1110eeeeCCCCC0www", 4, XPIC_ISA, 0x0001c000 + XPIC_OPCODE__LSL_H)
XPIC_INSN (lsli,  "Cd,s[pwP]m,u"  ,"0000000dddduuuu1111eeeeCCCCC0www", 4, XPIC_ISA, 0x0001e000 + XPIC_OPCODE__LSL_H)
#define XPIC_OPCODE__LSR_H 0x3a000000
#define XPIC_OPCODE__LSR_L 0x46000000
XPIC_INSN (lsr7,     "d,[x+X],U"     ,"0000000ddddUUUU00XXXXXXXXXXXXXXX", 4, XPIC_ISA, 0x00000000 + XPIC_OPCODE__LSR_L)
XPIC_INSN (lsr8,     "d,[W],U"       ,"0000000ddddUUUU01WWWWWWWWWWWWWWW", 4, XPIC_ISA, 0x00008000 + XPIC_OPCODE__LSR_L)
XPIC_INSN (lsr9,     "d,V,U"         ,"0000000ddddUUUU10VVVVVVVVVVVVVVV", 4, XPIC_ISA, 0x00010000 + XPIC_OPCODE__LSR_L)
XPIC_INSN (lsra,     "Cd,r,U"        ,"0000000ddddUUUU11000000CCCCCrrrr", 4, XPIC_ISA, 0x00018000 + XPIC_OPCODE__LSR_L)
XPIC_INSN (lsrb,     "Cd,s[x+pwP]m,U","0000000ddddUUUU1110eeeeCCCCC0www", 4, XPIC_ISA, 0x0001c000 + XPIC_OPCODE__LSR_L)
XPIC_INSN (lsrc,     "Cd,s[pwP]m,U"  ,"0000000ddddUUUU1111eeeeCCCCC0www", 4, XPIC_ISA, 0x0001e000 + XPIC_OPCODE__LSR_L)
XPIC_INSN (lsrd,     "d,[x+X],u"     ,"0000000dddduuuu00XXXXXXXXXXXXXXX", 4, XPIC_ISA, 0x00000000 + XPIC_OPCODE__LSR_H)
XPIC_INSN (lsre,     "d,[W],u"       ,"0000000dddduuuu01WWWWWWWWWWWWWWW", 4, XPIC_ISA, 0x00008000 + XPIC_OPCODE__LSR_H)
XPIC_INSN (lsrf,     "d,V,u"         ,"0000000dddduuuu10VVVVVVVVVVVVVVV", 4, XPIC_ISA, 0x00010000 + XPIC_OPCODE__LSR_H)
XPIC_INSN (lsrg,     "Cd,r,u"        ,"0000000dddduuuu11000000CCCCCrrrr", 4, XPIC_ISA, 0x00018000 + XPIC_OPCODE__LSR_H)
XPIC_INSN (lsrh,     "Cd,s[x+pwP]m,u","0000000dddduuuu1110eeeeCCCCC0www", 4, XPIC_ISA, 0x0001c000 + XPIC_OPCODE__LSR_H)
XPIC_INSN (lsri,     "Cd,s[pwP]m,u"  ,"0000000dddduuuu1111eeeeCCCCC0www", 4, XPIC_ISA, 0x0001e000 + XPIC_OPCODE__LSR_H)
#define XPIC_OPCODE__ASR_H 0x3c000000
#define XPIC_OPCODE__ASR_L 0x48000000
XPIC_INSN (asr7,     "d,[x+X],U"     ,"0000000ddddUUUU00XXXXXXXXXXXXXXX", 4, XPIC_ISA, 0x00000000 + XPIC_OPCODE__ASR_L)
XPIC_INSN (asr8,     "d,[W],U"       ,"0000000ddddUUUU01WWWWWWWWWWWWWWW", 4, XPIC_ISA, 0x00008000 + XPIC_OPCODE__ASR_L)
XPIC_INSN (asr9,     "d,V,U"         ,"0000000ddddUUUU10VVVVVVVVVVVVVVV", 4, XPIC_ISA, 0x00010000 + XPIC_OPCODE__ASR_L)
XPIC_INSN (asra,     "Cd,r,U"        ,"0000000ddddUUUU11000000CCCCCrrrr", 4, XPIC_ISA, 0x00018000 + XPIC_OPCODE__ASR_L)
XPIC_INSN (asrb,     "Cd,s[x+pwP]m,U","0000000ddddUUUU1110eeeeCCCCC0www", 4, XPIC_ISA, 0x0001c000 + XPIC_OPCODE__ASR_L)
XPIC_INSN (asrc,     "Cd,s[pwP]m,U"  ,"0000000ddddUUUU1111eeeeCCCCC0www", 4, XPIC_ISA, 0x0001e000 + XPIC_OPCODE__ASR_L)
XPIC_INSN (asrd,     "d,[x+X],u"     ,"0000000dddduuuu00XXXXXXXXXXXXXXX", 4, XPIC_ISA, 0x00000000 + XPIC_OPCODE__ASR_H)
XPIC_INSN (asre,     "d,[W],u"       ,"0000000dddduuuu01WWWWWWWWWWWWWWW", 4, XPIC_ISA, 0x00008000 + XPIC_OPCODE__ASR_H)
XPIC_INSN (asrf,     "d,V,u"         ,"0000000dddduuuu10VVVVVVVVVVVVVVV", 4, XPIC_ISA, 0x00010000 + XPIC_OPCODE__ASR_H)
XPIC_INSN (asrg,     "Cd,r,u"        ,"0000000dddduuuu11000000CCCCCrrrr", 4, XPIC_ISA, 0x00018000 + XPIC_OPCODE__ASR_H)
XPIC_INSN (asrh,     "Cd,s[x+pwP]m,u","0000000dddduuuu1110eeeeCCCCC0www", 4, XPIC_ISA, 0x0001c000 + XPIC_OPCODE__ASR_H)
XPIC_INSN (asri,     "Cd,s[pwP]m,u"  ,"0000000dddduuuu1111eeeeCCCCC0www", 4, XPIC_ISA, 0x0001e000 + XPIC_OPCODE__ASR_H)
#define XPIC_OPCODE__ROL_H 0x3e000000
#define XPIC_OPCODE__ROL_L 0x4a000000
XPIC_INSN (rol7,     "d,[x+X],U"     ,"0000000ddddUUUU00XXXXXXXXXXXXXXX", 4, XPIC_ISA, 0x00000000 + XPIC_OPCODE__ROL_L)
XPIC_INSN (rol8,     "d,[W],U"       ,"0000000ddddUUUU01WWWWWWWWWWWWWWW", 4, XPIC_ISA, 0x00008000 + XPIC_OPCODE__ROL_L)
XPIC_INSN (rol9,     "d,V,U"         ,"0000000ddddUUUU10VVVVVVVVVVVVVVV", 4, XPIC_ISA, 0x00010000 + XPIC_OPCODE__ROL_L)
XPIC_INSN (rola,     "Cd,r,U"        ,"0000000ddddUUUU11000000CCCCCrrrr", 4, XPIC_ISA, 0x00018000 + XPIC_OPCODE__ROL_L)
XPIC_INSN (rolb,     "Cd,s[x+pwP]m,U","0000000ddddUUUU1110eeeeCCCCC0www", 4, XPIC_ISA, 0x0001c000 + XPIC_OPCODE__ROL_L)
XPIC_INSN (rolc,     "Cd,s[pwP]m,U"  ,"0000000ddddUUUU1111eeeeCCCCC0www", 4, XPIC_ISA, 0x0001e000 + XPIC_OPCODE__ROL_L)
XPIC_INSN (rold,     "d,[x+X],u"     ,"0000000dddduuuu00XXXXXXXXXXXXXXX", 4, XPIC_ISA, 0x00000000 + XPIC_OPCODE__ROL_H)
XPIC_INSN (role,     "d,[W],u"       ,"0000000dddduuuu01WWWWWWWWWWWWWWW", 4, XPIC_ISA, 0x00008000 + XPIC_OPCODE__ROL_H)
XPIC_INSN (rolf,     "d,V,u"         ,"0000000dddduuuu10VVVVVVVVVVVVVVV", 4, XPIC_ISA, 0x00010000 + XPIC_OPCODE__ROL_H)
XPIC_INSN (rolg,     "Cd,r,u"        ,"0000000dddduuuu11000000CCCCCrrrr", 4, XPIC_ISA, 0x00018000 + XPIC_OPCODE__ROL_H)
XPIC_INSN (rolh,     "Cd,s[x+pwP]m,u","0000000dddduuuu1110eeeeCCCCC0www", 4, XPIC_ISA, 0x0001c000 + XPIC_OPCODE__ROL_H)
XPIC_INSN (roli,     "Cd,s[pwP]m,u"  ,"0000000dddduuuu1111eeeeCCCCC0www", 4, XPIC_ISA, 0x0001e000 + XPIC_OPCODE__ROL_H)
/// 3.2.7 Multiplication commands
#define XPIC_OPCODE__MULU 0x54000000
#define XPIC_OPCODE__MULS 0x56000000
XPIC_INSN (mulu1,     "D,V,r"        ,"0000000DDDDrrrr10VVVVVVVVVVVVVVV", 4, XPIC_ISA, 0x00010000 + XPIC_OPCODE__MULU)
XPIC_INSN (mulu2,     "CD,r,q"       ,"0000000DDDDqqqq11000000CCCCCrrrr", 4, XPIC_ISA, 0x00018000 + XPIC_OPCODE__MULU)
XPIC_INSN (muls1,     "D,V,r"        ,"0000000DDDDrrrr10VVVVVVVVVVVVVVV", 4, XPIC_ISA, 0x00010000 + XPIC_OPCODE__MULS)
XPIC_INSN (muls2,     "CD,r,q"       ,"0000000DDDDqqqq11000000CCCCCrrrr", 4, XPIC_ISA, 0x00018000 + XPIC_OPCODE__MULS) 
/// 3.2.8 Increment / Decrement commands
#define XPIC_OPCODE__INC 0x6c000000
XPIC_INSN (inc1,     "d,[x+X],T"     ,"0000000ddddTTTT00XXXXXXXXXXXXXXX", 4, XPIC_ISA, 0x00000000 + XPIC_OPCODE__INC)
XPIC_INSN (inc2,     "d,[W],T"       ,"0000000ddddTTTT01WWWWWWWWWWWWWWW", 4, XPIC_ISA, 0x00008000 + XPIC_OPCODE__INC)
XPIC_INSN (inc3,     "d,V,T"         ,"0000000ddddTTTT10VVVVVVVVVVVVVVV", 4, XPIC_ISA, 0x00010000 + XPIC_OPCODE__INC)
XPIC_INSN (inc4,     "Cd,r,T"        ,"0000000ddddTTTT11000000CCCCCrrrr", 4, XPIC_ISA, 0x00018000 + XPIC_OPCODE__INC)
XPIC_INSN (inc5,     "Cd,s[x+pwP]m,T","0000000ddddTTTT1110eeeeCCCCC0www", 4, XPIC_ISA, 0x0001c000 + XPIC_OPCODE__INC)
XPIC_INSN (inc6,     "Cd,s[pwP]m,T"  ,"0000000ddddTTTT1111eeeeCCCCC0www", 4, XPIC_ISA, 0x0001e000 + XPIC_OPCODE__INC)
#define XPIC_OPCODE__DEC 0x6e000000
XPIC_INSN (dec1,     "d,[x+X],T"     ,"0000000ddddTTTT00XXXXXXXXXXXXXXX", 4, XPIC_ISA, 0x00000000 + XPIC_OPCODE__DEC)
XPIC_INSN (dec2,     "d,[W],T"       ,"0000000ddddTTTT01WWWWWWWWWWWWWWW", 4, XPIC_ISA, 0x00008000 + XPIC_OPCODE__DEC)
XPIC_INSN (dec3,     "d,V,T"         ,"0000000ddddTTTT10VVVVVVVVVVVVVVV", 4, XPIC_ISA, 0x00010000 + XPIC_OPCODE__DEC)
XPIC_INSN (dec4,     "Cd,r,T"        ,"0000000ddddTTTT11000000CCCCCrrrr", 4, XPIC_ISA, 0x00018000 + XPIC_OPCODE__DEC)
XPIC_INSN (dec5,     "Cd,s[x+pwP]m,T","0000000ddddTTTT1110eeeeCCCCC0www", 4, XPIC_ISA, 0x0001c000 + XPIC_OPCODE__DEC)
XPIC_INSN (dec6,     "Cd,s[pwP]m,T"  ,"0000000ddddTTTT1111eeeeCCCCC0www", 4, XPIC_ISA, 0x0001e000 + XPIC_OPCODE__DEC)
/// 3.2.9 Load/Store  commands
XPIC_INSN (load1,    "d,s[r+Y]m"    ,"100ddddrrrr0mmsYYYYYYYYYYYYYYYYY", 4, XPIC_ISA, 0x80000000)
XPIC_INSN (load2,    "d,Z"          ,"100dddd000010ZZZZZZZZZZZZZZZZZZZ", 4, XPIC_ISA, 0x80100000)
XPIC_INSN (load3,    "Cd,s[r+pwP]mS","100ddddrrrr11SSxxxxeeeeCCCCCxwww", 4, XPIC_ISA, 0x80180000)
XPIC_INSN (store1,   "[r+Y]m,d"     ,"101ddddrrrr0mm0YYYYYYYYYYYYYYYYY", 4, XPIC_ISA, 0xa0000000)
XPIC_INSN (store2,   "Cs[r+pwP]mS,d","101ddddrrrr11SS0000eeeeCCCCC0www", 4, XPIC_ISA, 0xa0180000)
///3.2.11 Special commands
XPIC_INSN (nop1,      ""                       ,"11000000000000000000000000010000", 4, XPIC_ISA, 0xc0000010)
XPIC_INSN (break1,    ""                       ,"11000100000000000000000000010000", 4, XPIC_ISA, 0xc4000010)
XPIC_INSN (retf1,     ""                       ,"11001000000000000000000000010000", 4, XPIC_ISA, 0xc8000010)
XPIC_INSN (reti1,     ""                       ,"11001100000000000000000000010000", 4, XPIC_ISA, 0xcc000010)
XPIC_INSN (reli1,""      	               ,"11010000000000000000000000010000", 4, XPIC_ISA, 0xd0000010)
XPIC_INSN (relreti1,""          	       ,"11010100000000000000000000010000", 4, XPIC_ISA, 0xd4000010)

/// xpic2.xml
/// 3.3.8    bcd2hex
#define XPIC_OPCODE__BCD2HEX 0x36000000
XPIC_INSN (bcd2hex1,    "d,[x+X],r"     ,"0000000ddddrrrr00XXXXXXXXXXXXXXX", 4, XPIC_ISA2, 0x00000000 + XPIC_OPCODE__BCD2HEX)
XPIC_INSN (bcd2hex2,    "d,[W],r"       ,"0000000ddddrrrr01WWWWWWWWWWWWWWW", 4, XPIC_ISA2, 0x00008000 + XPIC_OPCODE__BCD2HEX)
XPIC_INSN (bcd2hex3,    "d,V,r"         ,"0000000ddddrrrr10VVVVVVVVVVVVVVV", 4, XPIC_ISA2, 0x00010000 + XPIC_OPCODE__BCD2HEX)
XPIC_INSN (bcd2hex4,    "Cd,r,q"        ,"0000000ddddqqqq11000000CCCCCrrrr", 4, XPIC_ISA2, 0x00018000 + XPIC_OPCODE__BCD2HEX)
XPIC_INSN (bcd2hex5,    "Cd,s[x+pwP]m,q","0000000ddddqqqq1110eeeeCCCCC0www", 4, XPIC_ISA2, 0x0001c000 + XPIC_OPCODE__BCD2HEX)
XPIC_INSN (bcd2hex6,    "Cd,s[pwP]m,q"  ,"0000000ddddqqqq1111eeeeCCCCC0www", 4, XPIC_ISA2, 0x0001e000 + XPIC_OPCODE__BCD2HEX)
/// 3.3.17   hex2bcd
#define XPIC_OPCODE__HEX2BCD 0x38000000
XPIC_INSN (hex2bcd1,    "d,[x+X],r"     ,"0000000ddddrrrr00XXXXXXXXXXXXXXX", 4, XPIC_ISA2, 0x00000000 + XPIC_OPCODE__HEX2BCD)
XPIC_INSN (hex2bcd2,    "d,[W],r"       ,"0000000ddddrrrr01WWWWWWWWWWWWWWW", 4, XPIC_ISA2, 0x00008000 + XPIC_OPCODE__HEX2BCD)
XPIC_INSN (hex2bcd3,    "d,V,r"         ,"0000000ddddrrrr10VVVVVVVVVVVVVVV", 4, XPIC_ISA2, 0x00010000 + XPIC_OPCODE__HEX2BCD)
XPIC_INSN (hex2bcd4,    "Cd,r,q"        ,"0000000ddddqqqq11000000CCCCCrrrr", 4, XPIC_ISA2, 0x00018000 + XPIC_OPCODE__HEX2BCD)
XPIC_INSN (hex2bcd5,    "Cd,s[x+pwP]m,q","0000000ddddqqqq1110eeeeCCCCC0www", 4, XPIC_ISA2, 0x0001c000 + XPIC_OPCODE__HEX2BCD)
XPIC_INSN (hex2bcd6,    "Cd,s[pwP]m,q"  ,"0000000ddddqqqq1111eeeeCCCCC0www", 4, XPIC_ISA2, 0x0001e000 + XPIC_OPCODE__HEX2BCD)

/// Important:instructions gie, sie, gsie must be follow before jump instructions! 
/// 3.3.15   gie
#define XPIC_OPCODE__GIE 0xd8001010
XPIC_INSN (gie1,    "w"       ,"11011000000000000001www000010000", 4, XPIC_ISA2, XPIC_OPCODE__GIE)

/// 3.3.44   sie
#define XPIC_OPCODE__SIE 0xd8002010
XPIC_INSN (sie1,    "w"       ,"11011000000000000010www000010000", 4, XPIC_ISA2, XPIC_OPCODE__SIE)
XPIC_INSN (sie2,    "M,I"     ,"11011000000000000010000MMII10000", 4, XPIC_ISA2, XPIC_OPCODE__SIE)

/// 3.3.16   gsie
#define XPIC_OPCODE__GSIE 0xd8003010
XPIC_INSN (gsie1, "w,M,I"     ,"11011000000000000011wwwMMII10000", 4, XPIC_ISA2, XPIC_OPCODE__GSIE)


/// xpic.xml:
/// 3.2.10 Jump commands
XPIC_INSN (jmp1,      "c,j"                    ,"110jjjjjjjjjjjjjjjjjjjjjjjjccccc", 4, XPIC_ISA, 0xc0000000)
//XPIC_INSN (jmp2,      "j"                      ,"110jjjjjjjjjjjjjjjjjjjjjjjjxxxxx", 4, XPIC_ISA, 0xc0000000)
XPIC_INSN (jmpdec1,  "v,j"                    ,"111jjjjjjjjjjjjjjjjjjjjjjjj000vv", 4, XPIC_ISA, 0xe0000000)

/** ** ** ** ** ** ** ** ** ** ** ** ** ** X P I C ** ** ** ** ** ** ** ** ** ** ** ** ** ** **/
/** ** ** ** ** ** ** ** ** ** ** ** ** ** X P I C ** ** ** ** ** ** ** ** ** ** ** ** ** ** **/
/** ** ** ** ** ** ** ** ** ** ** ** ** ** X P I C ** ** ** ** ** ** ** ** ** ** ** ** ** ** **/
