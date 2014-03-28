/* XPIC ELF support for BFD.
   Copyright 1999, 2000, 2004, 2006 Free Software Foundation, Inc.
   Contributed by Denis Chertykov <denisc@overta.ru>

   This file is part of BFD, the Binary File Descriptor library.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation, Inc.,
   51 Franklin Street - Fifth Floor, Boston, MA 02110-1301, USA.  */

#ifndef _ELF_XPIC_H
#define _ELF_XPIC_H

#include "elf/reloc-macros.h"

// special definitions for xpic
#define XPIC_MEM_INTPRAM_LEN 0x2000

// instructions:
// nop
#define  XPIC_INS_NOP			0xc0000010
// load  pc, #0
#define  XPIC_INS_LOAD_PC_0		0x9a100000
// load pc, [pc + #8]
#define  XPIC_INS_LOAD_PC_MPC_8		0x9ba80002
// load pc, [pc + #offset]
#define  XPIC_INS_LOAD_PC_MPC_OFF	0x9ba80001
#define  XPIC_INS_LOAD_PC_MPC_OFF_	0x9ba80000
// load rx, [pc + #offset]d
#define  XPIC_INS_LOAD_RX_MPC_OFF	0x81a80000
// store  [ z0 + --r7 ]d, r6
#define  XPIC_INS_STORE_dSP_R6		0xadf81407
// store [z0 + --r7], pc
#define  XPIC_INS_STORE_dSP_PC		0xbbf81407
// add  r6, #4, pc
#define  XPIC_INS_R6_ADD_R6_4_PC	0x00db0004
// add  r6, #8, pc
#define  XPIC_INS_R6_ADD_R6_8_PC	0x00db0008

// alu  RDST, [RSRC1], RSRC2  -> mas,sign,preinc as dafault 00000 (mas=d, unsign, noinc): add r0, [r0]d, r0  - alu_code=add=000000
#define  XPIC_INS_RDST_ALU_MSRC1__RSRC2	0x0001E000

// register, that is used for FarStore: r6  (do not place here any register with code>7, it can cot fit in 3 bits /wreg/)
#define  XPIC_FARSTORE_REG  6
// gets:
#define  XPIC_GET_MAS(instruction)  ( (instruction>>18) & 0x00000003 )

/* Processor specific flags for the ELF header e_flags field.  */
#define EF_XPIC_MACH 0x7F

/* If bit #7 is set, it is assumed that the elf file uses local symbols
   as reference for the relocations so that linker relaxation is possible.  */
#define EF_AVR_LINKRELAX_PREPARED 0x80

// now just one ach of xpic
#define E_XPIC_MACH     1 
/* Relocations.  */
START_RELOC_NUMBERS (elf_xpic_reloc_type)
     RELOC_NUMBER (R_XPIC_NONE,			0)
     RELOC_NUMBER (R_XPIC_32,			1)
     RELOC_NUMBER (R_AVR_7_PCREL,		2)
     RELOC_NUMBER (R_AVR_13_PCREL,		3)
     RELOC_NUMBER (R_AVR_16, 			4)
     RELOC_NUMBER (R_AVR_16_PM, 		5)
     RELOC_NUMBER (R_AVR_LO8_LDI,		6)
     RELOC_NUMBER (R_AVR_HI8_LDI,		7)
     RELOC_NUMBER (R_AVR_HH8_LDI,		8)
     RELOC_NUMBER (R_AVR_LO8_LDI_NEG,		9)
     RELOC_NUMBER (R_AVR_HI8_LDI_NEG,	       10)
     RELOC_NUMBER (R_AVR_HH8_LDI_NEG,	       11)
     RELOC_NUMBER (R_AVR_LO8_LDI_PM,	       12)
     RELOC_NUMBER (R_AVR_HI8_LDI_PM,	       13)
     RELOC_NUMBER (R_AVR_HH8_LDI_PM,	       14)
     RELOC_NUMBER (R_AVR_LO8_LDI_PM_NEG,       15)
     RELOC_NUMBER (R_AVR_HI8_LDI_PM_NEG,       16)
     RELOC_NUMBER (R_AVR_HH8_LDI_PM_NEG,       17)
     RELOC_NUMBER (R_AVR_CALL,		       18)
     RELOC_NUMBER (R_AVR_LDI,                  19)
     RELOC_NUMBER (R_AVR_6,                    20)
     RELOC_NUMBER (R_AVR_6_ADIW,               21)
     RELOC_NUMBER (R_AVR_MS8_LDI,              22)
     RELOC_NUMBER (R_AVR_MS8_LDI_NEG,          23)
     RELOC_NUMBER (R_AVR_LO8_LDI_GS,	       24)
     RELOC_NUMBER (R_AVR_HI8_LDI_GS,	       25)
     RELOC_NUMBER (R_XPIC_24_PCREL,	       26)
     RELOC_NUMBER (R_XPIC_Y17_PCREL,	       27)
     RELOC_NUMBER (R_XPIC_Z19,      	       28)
     RELOC_NUMBER (R_XPIC_X15_PCREL,	       29)
     RELOC_NUMBER (R_XPIC_Y17,   	           30)
     RELOC_NUMBER (R_XPIC_LONG_JMP,            31)
     RELOC_NUMBER (R_XPIC_X15,                 32)
END_RELOC_NUMBERS (R_XPIC_max)

#endif /* _ELF_AVR_H */
