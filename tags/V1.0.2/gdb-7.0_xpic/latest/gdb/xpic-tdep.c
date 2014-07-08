/* Target-dependent code for Hilscher XPIC, for GDB.

   Copyright (C) 1996, 1997, 1998, 1999, 2000, 2001, 2002, 2003, 2004, 2005,
   2006, 2007, 2008, 2009 Free Software Foundation, Inc.

   This file is part of GDB.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* Contributed by Artem Rudskyy, tema13tema@yahoo.de */

#include "defs.h"
#include "frame.h"
#include "frame-unwind.h"
#include "frame-base.h"
#include "trad-frame.h"
#include "gdbcmd.h"
#include "gdbcore.h"
#include "gdbtypes.h"
#include "inferior.h"
#include "symfile.h"
#include "arch-utils.h"
#include "regcache.h"
#include "gdb_string.h"
#include "dis-asm.h"
#include "value.h"

#define XPIC_INT_PRAM		0x1018c000
#define XPIC_INT_PRAM_LEN	0x00002000
//#define XPIC_INT_DRAM		0x1018e000
//#define XPIC_INT_DRAM		0x10280000
extern unsigned int XPIC_INT_DRAM; /* it comes as command line option from gdb/main.c */
#define XPIC_INT_DRAM_LEN	0x00002000


// This function has to correct addresses to access in XPIC_INT_DRAM
// It is called from line 1058 "valops.c"
// Cases from  "gdbtypes.h":
void xpic_value_addr_correction(struct value *val)
{
   CORE_ADDR coreAddr = value_raw_address(val);
   switch (TYPE_CODE (check_typedef (value_type (val))))
    {
    // function addresses are in XPIC_INT_PRAM
    case TYPE_CODE_INTERNAL_FUNCTION:
    case TYPE_CODE_FUNC:
      break;
    default:
	  if( coreAddr < XPIC_INT_DRAM_LEN )
	    set_value_address (val,  coreAddr + XPIC_INT_DRAM );
      break;
    };
}

static void xpic_address_to_pointer (struct gdbarch *gdbarch, struct type *type, gdb_byte *buf, CORE_ADDR addr)
{
  enum bfd_endian byte_order = gdbarch_byte_order (gdbarch);
  // Is it a code address?  
  if (TYPE_CODE (TYPE_TARGET_TYPE (type)) == TYPE_CODE_FUNC || TYPE_CODE (TYPE_TARGET_TYPE (type)) == TYPE_CODE_METHOD)
  {
    store_unsigned_integer (buf, TYPE_LENGTH (type), byte_order,addr);
//  fprintf_unfiltered (gdb_stdlog, ".....xpic_address_to_pointer.....if...%x\n",(unsigned int)addr);
  }
  else
  {
    // correction for XPIC_INT_DRAM
    if( addr>=XPIC_INT_DRAM && addr<(XPIC_INT_DRAM+XPIC_INT_DRAM_LEN) )
    addr -= XPIC_INT_DRAM;

    store_unsigned_integer (buf, TYPE_LENGTH (type), byte_order,addr);
//  fprintf_unfiltered (gdb_stdlog, ".....xpic_address_to_pointer.....else...%x\n",(unsigned int)addr); 
  }
}

static CORE_ADDR xpic_pointer_to_address (struct gdbarch *gdbarch, struct type *type, const gdb_byte *buf)
{
  enum bfd_endian byte_order = gdbarch_byte_order (gdbarch);
  CORE_ADDR addr = extract_unsigned_integer (buf, TYPE_LENGTH (type), byte_order);

  // Is it a code address?  
  if (TYPE_CODE (TYPE_TARGET_TYPE (type)) == TYPE_CODE_FUNC || TYPE_CODE (TYPE_TARGET_TYPE (type)) == TYPE_CODE_METHOD || TYPE_CODE_SPACE (TYPE_TARGET_TYPE (type)))
  {
//    fprintf_unfiltered (gdb_stdlog, ".....xpic_pointer_to_address.....if...%x\n",addr);
    return addr;
  }
  else
  {
    // correction for XPIC_INT_DRAM
    if(addr < XPIC_INT_DRAM_LEN)
      addr += XPIC_INT_DRAM;
//   fprintf_unfiltered (gdb_stdlog, ".....xpic_pointer_to_address.....else...%x\n",addr);
    return addr;
  }
}

#undef XMALLOC
#define XMALLOC(TYPE) ((TYPE*) xmalloc (sizeof (TYPE)))

enum
{
  XPIC_REG_W = 0, //?

  XPIC_REG_X = 0, // in xpic all work regs are 32bit.
  XPIC_REG_Y = 1, // for xpic any r0-r7
  XPIC_REG_Z = 2,

//  XPIC_FP_REGNUM = 7,	 // frame pointer

  XPIC_SREG_REGNUM = 14, // status
  XPIC_SP_REGNUM = 7,    // stack pointer
  XPIC_PC_REGNUM = 13,   // pc register

  XPIC_NUM_REGS = 16,    // r0-r7 + usr0-usr4 + pc + stat + zero + irq( r0-r7, pc2, stat)
  XPIC_NUM_REG_BYTES = XPIC_NUM_REGS*4,

  XPIC_PC_REG_INDEX = 16, /* index into array of registers */


  XPIC_MAX_PROLOGUE_SIZE = 32,	/* bytes */

  /* Count of pushed registers. r0, r1, u0, u1, u2, u3 */
  XPIC_MAX_PUSHES = 6,

  /* Number of the last pushed register. u3 for current llc */
  XPIC_LAST_PUSHED_REGNUM = 11,

  XPIC_ARG1_REGNUM = 0, /* Single byte argument */
  XPIC_ARGN_REGNUM = 0, /* Multi byte argments */
  XPIC_RET1_REGNUM = 0, /* Single byte return value */
  XPIC_RETN_REGNUM = 0, /* Multi byte return value */


  XPIC_IMEM_START = 0x00000000,	/* INSN memory */
  XPIC_SMEM_START = 0x00000000, /* SRAM memory */
#if 1
  /* No eeprom mask defined */
  XPIC_MEM_MASK = 0x00f00000,	/* mask to determine memory space */
#else
  XPIC_EMEM_START = 0x00810000,	/* EEPROM memory */
  XPIC_MEM_MASK = 0x00ff0000,	/* mask to determine memory space */
#endif
};

/* Prologue types:

   NORMAL and CALL are the typical types (the -mcall-prologues gcc option
   causes the generation of the CALL type prologues).  */

enum {
    XPIC_PROLOGUE_NONE,              /* No prologue */
    XPIC_PROLOGUE_NORMAL,
    XPIC_PROLOGUE_CALL,              /* -mcall-prologues */
    XPIC_PROLOGUE_MAIN,
    XPIC_PROLOGUE_INTR,              /* interrupt handler */
    XPIC_PROLOGUE_SIG,               /* signal handler */
};

/* Any function with a frame looks like this
   .......    <-SP POINTS HERE
   LOCALS1    <-FP POINTS HERE
   LOCALS0
   SAVED FP
   ...
   SAVED R1
   SAVED R0
   RET PC
   FIRST STACK ARG
   SECOND STACK ARG */

struct xpic_unwind_cache
{
  /* The previous frame's inner most stack address.  Used as this
     frame ID's stack_addr.  */
  CORE_ADDR prev_sp;
  /* The frame's base, optionally used by the high-level debug info.  */
  CORE_ADDR base;
  int size;
  int prologue_type;
  /* Table indicating the location of each and every register.  */
  struct trad_frame_saved_reg *saved_regs;
};

struct gdbarch_tdep
{
  /* Number of bytes stored to the stack by call instructions.
     4 bytes xpic */
  int call_length;
};

/* Lookup the name of a register given it's number. */

static const char *
xpic_register_name (struct gdbarch *gdbarch, int regnum)
{
 static const char * const register_names[] = {
    "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7",
    "u0", "u1", "u2", "u3", "u4", "pc", "stat", "z0"
  };
  if (regnum < 0)
    return NULL;
  if (regnum >= (sizeof (register_names) / sizeof (*register_names)))
    return NULL;
  return register_names[regnum];
}

/* Return the GDB type object for the "standard" data type
   of data in register N.  */

static struct type *
xpic_register_type (struct gdbarch *gdbarch, int reg_nr)
{
  if (reg_nr == XPIC_PC_REGNUM)
    return builtin_type (gdbarch)->builtin_uint32;
  if (reg_nr == XPIC_SP_REGNUM)
    return builtin_type (gdbarch)->builtin_data_ptr;
  else
    return builtin_type (gdbarch)->builtin_uint32; //builtin_uint8;
}

/* Convert from address to pointer and vice-versa. */

static CORE_ADDR xpic_read_pc (struct regcache *regcache)
{
  ULONGEST pc;
  regcache_cooked_read_unsigned (regcache, XPIC_PC_REGNUM, &pc);
  return pc; 
}

static void xpic_write_pc (struct regcache *regcache, CORE_ADDR val)
{
//  regcache_cooked_write_unsigned (regcache, XPIC_PC_REGNUM,xpic_convert_iaddr_to_raw (val));
  regcache_cooked_write_unsigned (regcache, XPIC_PC_REGNUM,val);
}

#if 0
/* Function: xpic_scan_prologue

   This function decodes an XPIC function prologue to determine:
     1) the size of the stack frame
     2) which registers are saved on it
     3) the offsets of saved regs
   This information is stored in the xpic_unwind_cache structure.

   Some devices lack the sbiw instruction, so on those replace this:
        sbiw    r28, XX
   with this:
        subi    r28,lo8(XX)
        sbci    r29,hi8(XX)

   A typical XPIC function prologue with a frame pointer might look like this:
        push    rXX        ; saved regs
        ...
        push    r28
        push    r29
        in      r28,__SP_L__
        in      r29,__SP_H__
        sbiw    r28,<LOCALS_SIZE>
        in      __tmp_reg__,__SREG__
        cli
        out     __SP_H__,r29
        out     __SREG__,__tmp_reg__
        out     __SP_L__,r28

   A typical XPIC function prologue without a frame pointer might look like
   this:
        push    rXX        ; saved regs
        ...

   A main function prologue looks like this:
        ldi     r28,lo8(<RAM_ADDR> - <LOCALS_SIZE>)
        ldi     r29,hi8(<RAM_ADDR> - <LOCALS_SIZE>)
        out     __SP_H__,r29
        out     __SP_L__,r28

   A signal handler prologue looks like this:
        push    __zero_reg__
        push    __tmp_reg__
        in      __tmp_reg__, __SREG__
        push    __tmp_reg__
        clr     __zero_reg__
        push    rXX             ; save registers r18:r27, r30:r31
        ...
        push    r28             ; save frame pointer
        push    r29
        in      r28, __SP_L__
        in      r29, __SP_H__
        sbiw    r28, <LOCALS_SIZE>
        out     __SP_H__, r29
        out     __SP_L__, r28
        
   A interrupt handler prologue looks like this:
        sei
        push    __zero_reg__
        push    __tmp_reg__
        in      __tmp_reg__, __SREG__
        push    __tmp_reg__
        clr     __zero_reg__
        push    rXX             ; save registers r18:r27, r30:r31
        ...
        push    r28             ; save frame pointer
        push    r29
        in      r28, __SP_L__
        in      r29, __SP_H__
        sbiw    r28, <LOCALS_SIZE>
        cli
        out     __SP_H__, r29
        sei     
        out     __SP_L__, r28

   A `-mcall-prologues' prologue looks like this (Note that the megas use a
   jmp instead of a rjmp, thus the prologue is one word larger since jmp is a
   32 bit insn and rjmp is a 16 bit insn):
        ldi     r26,lo8(<LOCALS_SIZE>)
        ldi     r27,hi8(<LOCALS_SIZE>)
        ldi     r30,pm_lo8(.L_foo_body)
        ldi     r31,pm_hi8(.L_foo_body)
        rjmp    __prologue_saves__+RRR
        .L_foo_body:  */

/* Not really part of a prologue, but still need to scan for it, is when a
   function prologue moves values passed via registers as arguments to new
   registers. In this case, all local variables live in registers, so there
   may be some register saves. This is what it looks like:
        movw    rMM, rNN
        ...

   There could be multiple movw's. If the target doesn't have a movw insn, it
   will use two mov insns. This could be done after any of the above prologue
   types.  */

static CORE_ADDR
xpic_scan_prologue (struct gdbarch *gdbarch, CORE_ADDR pc_beg, CORE_ADDR pc_end,
		   struct xpic_unwind_cache *info)
{
  enum bfd_endian byte_order = gdbarch_byte_order (gdbarch);
  int i;
  unsigned short insn;
  int scan_stage = 0;
  struct minimal_symbol *msymbol;
  unsigned char prologue[XPIC_MAX_PROLOGUE_SIZE];
  int vpc = 0;
  int len;

  len = pc_end - pc_beg;
  if (len > XPIC_MAX_PROLOGUE_SIZE)
    len = XPIC_MAX_PROLOGUE_SIZE;

  /* FIXME: TRoth/2003-06-11: This could be made more efficient by only
     reading in the bytes of the prologue. The problem is that the figuring
     out where the end of the prologue is is a bit difficult. The old code 
     tried to do that, but failed quite often.  */
  read_memory (pc_beg, prologue, len);

  /* Scanning main()'s prologue
     ldi r28,lo8(<RAM_ADDR> - <LOCALS_SIZE>)
     ldi r29,hi8(<RAM_ADDR> - <LOCALS_SIZE>)
     out __SP_H__,r29
     out __SP_L__,r28 */

  if (len >= 4)
    {
      CORE_ADDR locals;
      static const unsigned char img[] = {
	0xde, 0xbf,		/* out __SP_H__,r29 */
	0xcd, 0xbf		/* out __SP_L__,r28 */
      };

      insn = extract_unsigned_integer (&prologue[vpc], 2, byte_order);
      /* ldi r28,lo8(<RAM_ADDR> - <LOCALS_SIZE>) */
      if ((insn & 0xf0f0) == 0xe0c0)
	{
	  locals = (insn & 0xf) | ((insn & 0x0f00) >> 4);
	  insn = extract_unsigned_integer (&prologue[vpc + 2], 2, byte_order);
	  /* ldi r29,hi8(<RAM_ADDR> - <LOCALS_SIZE>) */
	  if ((insn & 0xf0f0) == 0xe0d0)
	    {
	      locals |= ((insn & 0xf) | ((insn & 0x0f00) >> 4)) << 8;
	      if (vpc + 4 + sizeof (img) < len
		  && memcmp (prologue + vpc + 4, img, sizeof (img)) == 0)
		{
                  info->prologue_type = XPIC_PROLOGUE_MAIN;
                  info->base = locals;
                  return pc_beg + 4;
		}
	    }
	}
    }

  /* Scanning `-mcall-prologues' prologue
     Classic prologue is 10 bytes, mega prologue is a 12 bytes long */

  while (1)	/* Using a while to avoid many goto's */
    {
      int loc_size;
      int body_addr;
      unsigned num_pushes;
      int pc_offset = 0;

      /* At least the fifth instruction must have been executed to
	 modify frame shape.  */
      if (len < 10)
	break;

      insn = extract_unsigned_integer (&prologue[vpc], 2, byte_order);
      /* ldi r26,<LOCALS_SIZE> */
      if ((insn & 0xf0f0) != 0xe0a0)
	break;
      loc_size = (insn & 0xf) | ((insn & 0x0f00) >> 4);
      pc_offset += 2;

      insn = extract_unsigned_integer (&prologue[vpc + 2], 2, byte_order);
      /* ldi r27,<LOCALS_SIZE> / 256 */
      if ((insn & 0xf0f0) != 0xe0b0)
	break;
      loc_size |= ((insn & 0xf) | ((insn & 0x0f00) >> 4)) << 8;
      pc_offset += 2;

      insn = extract_unsigned_integer (&prologue[vpc + 4], 2, byte_order);
      /* ldi r30,pm_lo8(.L_foo_body) */
      if ((insn & 0xf0f0) != 0xe0e0)
	break;
      body_addr = (insn & 0xf) | ((insn & 0x0f00) >> 4);
      pc_offset += 2;

      insn = extract_unsigned_integer (&prologue[vpc + 6], 2, byte_order);
      /* ldi r31,pm_hi8(.L_foo_body) */
      if ((insn & 0xf0f0) != 0xe0f0)
	break;
      body_addr |= ((insn & 0xf) | ((insn & 0x0f00) >> 4)) << 8;
      pc_offset += 2;

      msymbol = lookup_minimal_symbol ("__prologue_saves__", NULL, NULL);
      if (!msymbol)
	break;

      insn = extract_unsigned_integer (&prologue[vpc + 8], 2, byte_order);
      /* rjmp __prologue_saves__+RRR */
      if ((insn & 0xf000) == 0xc000)
        {
          /* Extract PC relative offset from RJMP */
          i = (insn & 0xfff) | (insn & 0x800 ? (-1 ^ 0xfff) : 0);
          /* Convert offset to byte addressable mode */
          i *= 2;
          /* Destination address */
          i += pc_beg + 10;

          if (body_addr != (pc_beg + 10)/2)
            break;

          pc_offset += 2;
        }
      else if ((insn & 0xfe0e) == 0x940c)
        {
          /* Extract absolute PC address from JMP */
          i = (((insn & 0x1) | ((insn & 0x1f0) >> 3) << 16)
	       | (extract_unsigned_integer (&prologue[vpc + 10], 2, byte_order)
		  & 0xffff));
          /* Convert address to byte addressable mode */
          i *= 2;

          if (body_addr != (pc_beg + 12)/2)
            break;

          pc_offset += 4;
        }
      else
        break;

      /* Resolve offset (in words) from __prologue_saves__ symbol.
         Which is a pushes count in `-mcall-prologues' mode */
      num_pushes = XPIC_MAX_PUSHES - (i - SYMBOL_VALUE_ADDRESS (msymbol)) / 2;

      if (num_pushes > XPIC_MAX_PUSHES)
        {
          fprintf_unfiltered (gdb_stderr, _("Num pushes too large: %d\n"),
                              num_pushes);
          num_pushes = 0;
        }

      if (num_pushes)
	{
	  int from;

	  info->saved_regs[XPIC_FP_REGNUM + 1].addr = num_pushes;
	  if (num_pushes >= 2)
	    info->saved_regs[XPIC_FP_REGNUM].addr = num_pushes - 1;

	  i = 0;
	  for (from = XPIC_LAST_PUSHED_REGNUM + 1 - (num_pushes - 2);
	       from <= XPIC_LAST_PUSHED_REGNUM; ++from)
	    info->saved_regs [from].addr = ++i;
	}
      info->size = loc_size + num_pushes;
      info->prologue_type = XPIC_PROLOGUE_CALL;

      return pc_beg + pc_offset;
    }

  /* Scan for the beginning of the prologue for an interrupt or signal
     function.  Note that we have to set the prologue type here since the
     third stage of the prologue may not be present (e.g. no saved registered
     or changing of the SP register).  */

  if (1)
    {
      static const unsigned char img[] = {
	0x78, 0x94,		/* sei */
	0x1f, 0x92,		/* push r1 */
	0x0f, 0x92,		/* push r0 */
	0x0f, 0xb6,		/* in r0,0x3f SREG */
	0x0f, 0x92,		/* push r0 */
	0x11, 0x24		/* clr r1 */
      };
      if (len >= sizeof (img)
	  && memcmp (prologue, img, sizeof (img)) == 0)
	{
          info->prologue_type = XPIC_PROLOGUE_INTR;
	  vpc += sizeof (img);
          info->saved_regs[XPIC_SREG_REGNUM].addr = 3;
          info->saved_regs[0].addr = 2;
          info->saved_regs[1].addr = 1;
          info->size += 3;
	}
      else if (len >= sizeof (img) - 2
	       && memcmp (img + 2, prologue, sizeof (img) - 2) == 0)
	{
          info->prologue_type = XPIC_PROLOGUE_SIG;
          vpc += sizeof (img) - 2;
          info->saved_regs[XPIC_SREG_REGNUM].addr = 3;
          info->saved_regs[0].addr = 2;
          info->saved_regs[1].addr = 1;
          info->size += 3;
	}
    }

  /* First stage of the prologue scanning.
     Scan pushes (saved registers) */

  for (; vpc < len; vpc += 2)
    {
      insn = extract_unsigned_integer (&prologue[vpc], 2, byte_order);
      if ((insn & 0xfe0f) == 0x920f)	/* push rXX */
	{
	  /* Bits 4-9 contain a mask for registers R0-R32. */
	  int regno = (insn & 0x1f0) >> 4;
	  info->size++;
	  info->saved_regs[regno].addr = info->size;
	  scan_stage = 1;
	}
      else
	break;
    }

  if (vpc >= XPIC_MAX_PROLOGUE_SIZE)
     fprintf_unfiltered (gdb_stderr,
                         _("Hit end of prologue while scanning pushes\n"));

  /* Second stage of the prologue scanning.
     Scan:
     in r28,__SP_L__
     in r29,__SP_H__ */

  if (scan_stage == 1 && vpc < len)
    {
      static const unsigned char img[] = {
	0xcd, 0xb7,		/* in r28,__SP_L__ */
	0xde, 0xb7		/* in r29,__SP_H__ */
      };
      unsigned short insn1;

      if (vpc + sizeof (img) < len
	  && memcmp (prologue + vpc, img, sizeof (img)) == 0)
	{
	  vpc += 4;
	  scan_stage = 2;
	}
    }

  /* Third stage of the prologue scanning. (Really two stages)
     Scan for:
     sbiw r28,XX or subi r28,lo8(XX)
                    sbci r29,hi8(XX)
     in __tmp_reg__,__SREG__
     cli
     out __SP_H__,r29
     out __SREG__,__tmp_reg__
     out __SP_L__,r28 */

  if (scan_stage == 2 && vpc < len)
    {
      int locals_size = 0;
      static const unsigned char img[] = {
	0x0f, 0xb6,		/* in r0,0x3f */
	0xf8, 0x94,		/* cli */
	0xde, 0xbf,		/* out 0x3e,r29 ; SPH */
	0x0f, 0xbe,		/* out 0x3f,r0  ; SREG */
	0xcd, 0xbf		/* out 0x3d,r28 ; SPL */
      };
      static const unsigned char img_sig[] = {
	0xde, 0xbf,		/* out 0x3e,r29 ; SPH */
	0xcd, 0xbf		/* out 0x3d,r28 ; SPL */
      };
      static const unsigned char img_int[] = {
	0xf8, 0x94,		/* cli */
	0xde, 0xbf,		/* out 0x3e,r29 ; SPH */
	0x78, 0x94,		/* sei */
	0xcd, 0xbf		/* out 0x3d,r28 ; SPL */
      };

      insn = extract_unsigned_integer (&prologue[vpc], 2, byte_order);
      vpc += 2;
      if ((insn & 0xff30) == 0x9720)	/* sbiw r28,XXX */
	locals_size = (insn & 0xf) | ((insn & 0xc0) >> 2);
      else if ((insn & 0xf0f0) == 0x50c0)	/* subi r28,lo8(XX) */
	{
	  locals_size = (insn & 0xf) | ((insn & 0xf00) >> 4);
	  insn = extract_unsigned_integer (&prologue[vpc], 2, byte_order);
	  vpc += 2;
	  locals_size += ((insn & 0xf) | ((insn & 0xf00) >> 4) << 8);
	}
      else
	return pc_beg + vpc;

      /* Scan the last part of the prologue. May not be present for interrupt
         or signal handler functions, which is why we set the prologue type
         when we saw the beginning of the prologue previously.  */

      if (vpc + sizeof (img_sig) < len
	  && memcmp (prologue + vpc, img_sig, sizeof (img_sig)) == 0)
        {
          vpc += sizeof (img_sig);
        }
      else if (vpc + sizeof (img_int) < len 
	       && memcmp (prologue + vpc, img_int, sizeof (img_int)) == 0)
        {
          vpc += sizeof (img_int);
        }
      if (vpc + sizeof (img) < len
	  && memcmp (prologue + vpc, img, sizeof (img)) == 0)
        {
          info->prologue_type = XPIC_PROLOGUE_NORMAL;
          vpc += sizeof (img);
        }

      info->size += locals_size;

      /* Fall through.  */
    }

  /* If we got this far, we could not scan the prologue, so just return the pc
     of the frame plus an adjustment for argument move insns.  */

  for (; vpc < len; vpc += 2)
    {
      insn = extract_unsigned_integer (&prologue[vpc], 2, byte_order);
      if ((insn & 0xff00) == 0x0100)	/* movw rXX, rYY */
        continue;
      else if ((insn & 0xfc00) == 0x2c00) /* mov rXX, rYY */
        continue;
      else
          break;
    }
    
  return pc_beg + vpc;
}
#else
/*
  This function decodes an XPIC function prologue to determine:
  1) the size of the stack frame
  2) which registers are saved on it
  3) the offsets of saved regs
  4) the address of the first instruction after the prologue (return value)
  This information is stored in the xpic_unwind_cache structure.

*/

/* push instruction: store [z0 + --r7]d, wreg */
#define XPIC_OPC_PATTERN_MSK_PUSH        0xe1fe1ff7
#define XPIC_OPC_PATTERN_VAL_PUSH        0xa1f81407
#define XPIC_OPC_TARGET_MSK_PUSH         0x1e000000
#define XPIC_OPC_TARGET_SRT_PUSH                 25
/* stack frame allocation instruction: add r7, #-SIZE, r7 */
#define XPIC_OPC_PATTERN_MSK_STACKALLOC  0xffffc000
#define XPIC_OPC_PATTERN_VAL_STACKALLOC  0x00ef4000
#define XPIC_OPC_SRC1_MSK_PUSH           0x00003fff

static CORE_ADDR
xpic_scan_prologue (struct gdbarch *gdbarch, CORE_ADDR pc_beg, CORE_ADDR pc_end,
                    struct xpic_unwind_cache *info)
{
  enum bfd_endian byte_order = gdbarch_byte_order (gdbarch);
  int i;
  unsigned long instr;
  struct minimal_symbol *msymbol;
  unsigned char prologue[XPIC_MAX_PROLOGUE_SIZE];
  int vpc = 0;
  int len = pc_end - pc_beg;

  if (len > XPIC_MAX_PROLOGUE_SIZE)
    len = XPIC_MAX_PROLOGUE_SIZE;

  read_memory (pc_beg, prologue, len);

  /* First stage of the prologue scanning.
     Scan pushes (callee register saves) */

  for (; vpc < len; vpc += 4)
    {
      instr = extract_unsigned_integer (&prologue[vpc], 4, byte_order);
      if ((instr & XPIC_OPC_PATTERN_MSK_PUSH) == XPIC_OPC_PATTERN_VAL_PUSH)    /* store [--r7], rX */
        {
          /* Bits 25-28 contain a mask for saved register. */
          int regno = (instr & XPIC_OPC_TARGET_MSK_PUSH) >> XPIC_OPC_TARGET_SRT_PUSH;
          info->size += 4;
          info->saved_regs[regno].addr = info->size;
        }
      else
        {
          break; /* no more pushes */
        }
    }


  /* Second stage of the prologue scanning.
     stack frame allocation */
  if (vpc < len)
    {
      instr = extract_unsigned_integer (&prologue[vpc], 4, byte_order);
      if ((instr & XPIC_OPC_PATTERN_MSK_STACKALLOC) == XPIC_OPC_PATTERN_VAL_STACKALLOC)
        {
          /* Bits 13-0 contain negative size of stack frame */
          int framesize = instr & XPIC_OPC_SRC1_MSK_PUSH;
          framesize |= ~XPIC_OPC_SRC1_MSK_PUSH;
          info->size -= framesize;
          vpc += 4;
        }
      info->prologue_type = XPIC_PROLOGUE_NORMAL;
    }

  return pc_beg + vpc;
}
#endif

static CORE_ADDR
xpic_skip_prologue (struct gdbarch *gdbarch, CORE_ADDR pc)
{
  CORE_ADDR func_addr, func_end;
  CORE_ADDR prologue_end = pc;

  /* See what the symbol table says */
  if (find_pc_partial_function (pc, NULL, &func_addr, &func_end))
    {
      struct symtab_and_line sal;
      struct xpic_unwind_cache info = {0};
      struct trad_frame_saved_reg saved_regs[XPIC_NUM_REGS];

      info.saved_regs = saved_regs;

      /* Need to run the prologue scanner to figure out if the function has a
         prologue and possibly skip over moving arguments passed via registers
         to other registers.  */

      prologue_end = xpic_scan_prologue (gdbarch, func_addr, func_end, &info);

      if (info.prologue_type == XPIC_PROLOGUE_NONE)
        return pc;

    }

  /* Either we didn't find the start of this function (nothing we can do),
     or there's no line info, or the line after the prologue is after
     the end of the function (there probably isn't a prologue). */

  return prologue_end;
}

static const unsigned char *
xpic_breakpoint_from_pc (struct gdbarch *gdbarch, CORE_ADDR * pcptr, int *lenptr)
{
    static const unsigned char xpic_break_insn [] = {0x10,0x00,0x00,0xc4 };/* 0xc4000010 */
    *lenptr = sizeof (xpic_break_insn);
    return xpic_break_insn;
}

/* Given a return value in `regcache' with a type `type', 
   extract and copy its value into `valbuf'.

   Return values are always passed via registers r2, r3  */

static void
xpic_extract_return_value (struct type *type, struct regcache *regcache, gdb_byte *valbuf)
{
  struct gdbarch *gdbarch = get_regcache_arch (regcache);
  enum bfd_endian byte_order = gdbarch_byte_order (gdbarch);

  ULONGEST c;
  int len;
  if (TYPE_LENGTH (type) == 1)
    {
      regcache_cooked_read_unsigned (regcache, 2, &c);
      store_unsigned_integer (valbuf, 1, byte_order, c);
    }
  else
    {
      int i;
      /* The MSB of the return value is always in r3, calculate which
         register holds the LSB.  */
      int lsb_reg = 3 - TYPE_LENGTH (type) + 1;

      for (i=0; i< TYPE_LENGTH (type); i++)
        {
          regcache_cooked_read (regcache, 2 + i,
                                (bfd_byte *) valbuf + i);
        }
    }
}

/* ? Determine, for architecture GDBARCH, how a return value of TYPE
   should be returned.  If it is supposed to be returned in registers,
   and READBUF is non-zero, read the appropriate value from REGCACHE,
   and copy it into READBUF.  If WRITEBUF is non-zero, write the value
   from WRITEBUF into REGCACHE.  */

static enum return_value_convention
xpic_return_value (struct gdbarch *gdbarch, struct type *func_type,
		  struct type *valtype, struct regcache *regcache,
		  gdb_byte *readbuf, const gdb_byte *writebuf)
{
  int struct_return = ((TYPE_CODE (valtype) == TYPE_CODE_STRUCT
			|| TYPE_CODE (valtype) == TYPE_CODE_UNION
			|| TYPE_CODE (valtype) == TYPE_CODE_ARRAY)
		       && !(TYPE_LENGTH (valtype) == 1
			    || TYPE_LENGTH (valtype) == 2
			    || TYPE_LENGTH (valtype) == 4
			    || TYPE_LENGTH (valtype) == 8));

  if (writebuf != NULL)
    {
      gdb_assert (!struct_return);
      error (_("Cannot store return value."));
    }

  if (readbuf != NULL)
    {
      gdb_assert (!struct_return);
      xpic_extract_return_value (valtype, regcache, readbuf);
    }

  if (struct_return)
    return RETURN_VALUE_STRUCT_CONVENTION;
  else
    return RETURN_VALUE_REGISTER_CONVENTION;
}


/* ?! Put here the code to store, into fi->saved_regs, the addresses of
   the saved registers of frame described by FRAME_INFO.  This
   includes special registers such as pc and fp saved in special ways
   in the stack frame.  sp is even more special: the address we return
   for it IS the sp for the next frame. */

static struct xpic_unwind_cache *
xpic_frame_unwind_cache (struct frame_info *this_frame,
                        void **this_prologue_cache)
{
  CORE_ADDR start_pc, current_pc;
  ULONGEST prev_sp;
  ULONGEST this_base;
  struct xpic_unwind_cache *info;
  struct gdbarch *gdbarch;
//  struct gdbarch_tdep *tdep;
  int i;

  if (*this_prologue_cache)
    return *this_prologue_cache;

  info = FRAME_OBSTACK_ZALLOC (struct xpic_unwind_cache);
  *this_prologue_cache = info;
  info->saved_regs = trad_frame_alloc_saved_regs (this_frame);

  info->size = 0;
  info->prologue_type = XPIC_PROLOGUE_NONE;

  start_pc = get_frame_func (this_frame);
  current_pc = get_frame_pc (this_frame);
  if ((start_pc > 0) && (start_pc <= current_pc))
    xpic_scan_prologue (get_frame_arch (this_frame),
		       start_pc, current_pc, info);

  /* Assume that the FP is this frame's SP but with that pushed
     stack space added back.  */
  this_base = get_frame_register_unsigned (this_frame, XPIC_SP_REGNUM);

  /* correction for XPIC_INT_DRAM */
  if(this_base < XPIC_INT_DRAM_LEN)
    this_base += XPIC_INT_DRAM;

  prev_sp = this_base + info->size;

  /* Add 1 here to adjust for the pre-decrement nature of the push
     instruction.*/
//  info->prev_sp = xpic_make_saddr (prev_sp + 1);
//  info->base = xpic_make_saddr (this_base);

  info->prev_sp = (prev_sp + 4);
  info->base = this_base;


  gdbarch = get_frame_arch (this_frame);

  /* Adjust all the saved registers so that they contain addresses and not
     offsets.  */
  for (i = 0; i < gdbarch_num_regs (gdbarch) - 1; i++)
    if (info->saved_regs[i].addr > 0)
      info->saved_regs[i].addr = info->prev_sp - info->saved_regs[i].addr;

  /* Except for the main and startup code, the return PC is always saved on
     the stack and is at the base of the frame. */

  if (info->prologue_type != XPIC_PROLOGUE_MAIN)

    info->saved_regs[XPIC_PC_REGNUM].addr = prev_sp;

  /* The previous frame's SP needed to be computed.  Save the computed
     value.  */
//  tdep = gdbarch_tdep (gdbarch);
//  trad_frame_set_value (info->saved_regs, XPIC_SP_REGNUM, info->prev_sp - 4 + tdep->call_length);
  trad_frame_set_value (info->saved_regs, XPIC_SP_REGNUM, info->prev_sp);

  return info;
}

static CORE_ADDR
xpic_unwind_pc (struct gdbarch *gdbarch, struct frame_info *next_frame)
{
  ULONGEST pc;

  pc = frame_unwind_register_unsigned (next_frame, XPIC_PC_REGNUM);

//  return xpic_make_iaddr (pc);
  return pc;
}

static CORE_ADDR
xpic_unwind_sp (struct gdbarch *gdbarch, struct frame_info *next_frame)
{
  ULONGEST sp;

  sp = frame_unwind_register_unsigned (next_frame, XPIC_SP_REGNUM);

//  return xpic_make_saddr (sp);
  return sp;
}

/* Given a GDB frame, determine the address of the calling function's
   frame.  This will be used to create a new GDB frame struct.  */

static void
xpic_frame_this_id (struct frame_info *this_frame,
                   void **this_prologue_cache,
                   struct frame_id *this_id)
{
  struct xpic_unwind_cache *info
    = xpic_frame_unwind_cache (this_frame, this_prologue_cache);
  CORE_ADDR base;
  CORE_ADDR func;
  struct frame_id id;

  /* The FUNC is easy.  */
  func = get_frame_func (this_frame);

  /* Hopefully the prologue analysis either correctly determined the
     frame's base (which is the SP from the previous frame), or set
     that base to "NULL".  */
  base = info->prev_sp;
  if (base == 0)
    return;

  id = frame_id_build (base, func);
  (*this_id) = id;
}

static struct value *
xpic_frame_prev_register (struct frame_info *this_frame,
			 void **this_prologue_cache, int regnum)
{
  struct gdbarch *gdbarch = get_frame_arch (this_frame);
  enum bfd_endian byte_order = gdbarch_byte_order (gdbarch);
  struct xpic_unwind_cache *info
    = xpic_frame_unwind_cache (this_frame, this_prologue_cache);

  if (regnum == XPIC_PC_REGNUM)
    {
      if (trad_frame_addr_p (info->saved_regs, regnum))
        {
	  ULONGEST pc;
	  int i;

          union {
            char buf[4];
            ULONGEST val;
          } unionbuf;
	  struct gdbarch *gdbarch = get_frame_arch (this_frame);
	  struct gdbarch_tdep *tdep = gdbarch_tdep (gdbarch);

          unionbuf.val=0;

	  read_memory (info->saved_regs[regnum].addr, unionbuf.buf, tdep->call_length);
	  /* Extract the PC read from memory */
          pc = unionbuf.val;

          /* saved return address + 8 is real return address */
          pc += 8;

	  return frame_unwind_got_constant (this_frame, regnum, pc );
        }

      return frame_unwind_got_optimized (this_frame, regnum);
    }

  return trad_frame_get_prev_register (this_frame, info->saved_regs, regnum);
}

static const struct frame_unwind xpic_frame_unwind = {
  NORMAL_FRAME,
  xpic_frame_this_id,
  xpic_frame_prev_register,
  NULL,
  default_frame_sniffer
};

static CORE_ADDR
xpic_frame_base_address (struct frame_info *this_frame, void **this_cache)
{
  struct xpic_unwind_cache *info
    = xpic_frame_unwind_cache (this_frame, this_cache);

  return info->base;
}

static const struct frame_base xpic_frame_base = {
  &xpic_frame_unwind,
  xpic_frame_base_address,
  xpic_frame_base_address,
  xpic_frame_base_address
};

/* ? Assuming THIS_FRAME is a dummy, return the frame ID of that dummy
   frame.  The frame ID's base needs to match the TOS value saved by
   save_dummy_frame_tos(), and the PC match the dummy frame's breakpoint.  */

static struct frame_id
xpic_dummy_id (struct gdbarch *gdbarch, struct frame_info *this_frame)
{
  ULONGEST base;

  base = get_frame_register_unsigned (this_frame, XPIC_SP_REGNUM);
//  return frame_id_build (xpic_make_saddr (base), get_frame_pc (this_frame));
  return frame_id_build (base, get_frame_pc (this_frame));
}

/* When arguments must be pushed onto the stack, they go on in reverse
   order.  The below implements a FILO (stack) to do this. */

struct stack_item
{
  int len;
  struct stack_item *prev;
  void *data;
};

static struct stack_item *
push_stack_item (struct stack_item *prev, const bfd_byte *contents, int len)
{
  struct stack_item *si;
  si = xmalloc (sizeof (struct stack_item));
  si->data = xmalloc (len);
  si->len = len;
  si->prev = prev;
  memcpy (si->data, contents, len);
  return si;
}

static struct stack_item *pop_stack_item (struct stack_item *si);
static struct stack_item *
pop_stack_item (struct stack_item *si)
{
  struct stack_item *dead = si;
  si = si->prev;
  xfree (dead->data);
  xfree (dead);
  return si;
}


// ???
/* Setup the function arguments for calling a function in the inferior.

   On the XPIC architecture, there are 18 registers (R25 to R8) which are
   dedicated for passing function arguments.  Up to the first 18 arguments
   (depending on size) may go into these registers.  The rest go on the stack.

   All arguments are aligned to start in even-numbered registers (odd-sized
   arguments, including char, have one free register above them). For example,
   an int in arg1 and a char in arg2 would be passed as such:

      arg1 -> r25:r24
      arg2 -> r22

   Arguments that are larger than 2 bytes will be split between two or more
   registers as available, but will NOT be split between a register and the
   stack. Arguments that go onto the stack are pushed last arg first (this is
   similar to the d10v).  */

/* NOTE: TRoth/2003-06-17: The rest of this comment is old looks to be
   inaccurate.

   An exceptional case exists for struct arguments (and possibly other
   aggregates such as arrays) -- if the size is larger than WORDSIZE bytes but
   not a multiple of WORDSIZE bytes.  In this case the argument is never split
   between the registers and the stack, but instead is copied in its entirety
   onto the stack, AND also copied into as many registers as there is room
   for.  In other words, space in registers permitting, two copies of the same
   argument are passed in.  As far as I can tell, only the one on the stack is
   used, although that may be a function of the level of compiler
   optimization.  I suspect this is a compiler bug.  Arguments of these odd
   sizes are left-justified within the word (as opposed to arguments smaller
   than WORDSIZE bytes, which are right-justified).
 
   If the function is to return an aggregate type such as a struct, the caller
   must allocate space into which the callee will copy the return value.  In
   this case, a pointer to the return value location is passed into the callee
   in register R0, which displaces one of the other arguments passed in via
   registers R0 to R2. */

static CORE_ADDR
xpic_push_dummy_call (struct gdbarch *gdbarch, struct value *function,
                     struct regcache *regcache, CORE_ADDR bp_addr,
                     int nargs, struct value **args, CORE_ADDR sp,
                     int struct_return, CORE_ADDR struct_addr)
{
  enum bfd_endian byte_order = gdbarch_byte_order (gdbarch);
  int i;
  unsigned char buf[2];
//  CORE_ADDR return_pc = xpic_convert_iaddr_to_raw (bp_addr);
  CORE_ADDR return_pc = bp_addr;
  int regnum = XPIC_ARGN_REGNUM;
  struct stack_item *si = NULL;

#if 0
  /* FIXME: TRoth/2003-06-18: Not sure what to do when returning a struct. */
  if (struct_return)
    {
      fprintf_unfiltered (gdb_stderr, "struct_return: 0x%lx\n", struct_addr);
      regcache_cooked_write_unsigned (regcache, argreg--, struct_addr & 0xff);
      regcache_cooked_write_unsigned (regcache, argreg--, (struct_addr >>8) & 0xff);
    }
#endif

  for (i = 0; i < nargs; i++)
    {
      int last_regnum;
      int j;
      struct value *arg = args[i];
      struct type *type = check_typedef (value_type (arg));
      const bfd_byte *contents = value_contents (arg);
      int len = TYPE_LENGTH (type);

      /* Calculate the potential last register needed. */
      last_regnum = regnum - (len + (len & 1));

      /* If there are registers available, use them. Once we start putting
         stuff on the stack, all subsequent args go on stack. */
      if ((si == NULL) && (last_regnum >= 8))
        {
          ULONGEST val;

          /* Skip a register for odd length args. */
          if (len & 1)
            regnum--;

          val = extract_unsigned_integer (contents, len, byte_order);
          for (j=0; j<len; j++)
            {
              regcache_cooked_write_unsigned (regcache, regnum--,
                                              val >> (8*(len-j-1)));
            }
        }
      /* No registers available, push the args onto the stack. */
      else
        {
          /* From here on, we don't care about regnum. */
          si = push_stack_item (si, contents, len);
        }
    }

  /* Push args onto the stack. */
  while (si)
    {
      sp -= si->len;
      /* Add 1 to sp here to account for post decr nature of pushes. */
      write_memory (sp + 1, si->data, si->len);
      si = pop_stack_item (si);
    }

  /* Set the return address.  For the xpic, the return address is the BP_ADDR.
     Need to push the return address onto the stack noting that it needs to be
     in big-endian order on the stack.  */
  buf[0] = (return_pc >> 8) & 0xff;
  buf[1] = return_pc & 0xff;

  sp -= 2;
  write_memory (sp + 1, buf, 2);  /* Add one since pushes are post decr ops. */

  /* Finally, update the SP register. */
//  regcache_cooked_write_unsigned (regcache, XPIC_SP_REGNUM,xpic_convert_saddr_to_raw (sp));
  regcache_cooked_write_unsigned (regcache, XPIC_SP_REGNUM,sp);
  return sp;
}


static int xpic_memory_insert_breakpoint (struct gdbarch *gdbarch,struct bp_target_info *bp_tgt)
{
  CORE_ADDR addr = bp_tgt->placed_address;
  int val;
  gdb_byte buf[4]= {0x10,0x00,0x00,0xc4 };/* 0xc4000010 */
  gdb_byte *contents_cache = bp_tgt->shadow_contents;
  /* Save the memory contents.  */
  val = target_read_memory (addr & 0xfffffffc, contents_cache, 4);
  if (val != 0)
    return val;			/* return error */

  bp_tgt->placed_size = bp_tgt->shadow_len = 4;

  /* Write the breakpoint.  */
  val = target_write_memory (addr & 0xfffffffc, buf, 4);
  return val;
}


static int xpic_memory_remove_breakpoint (struct gdbarch *gdbarch,struct bp_target_info *bp_tgt)
{
  CORE_ADDR addr = bp_tgt->placed_address;
  int val;
//  gdb_byte buf[4];
  gdb_byte *contents_cache = bp_tgt->shadow_contents;

  /* Write contents.  */
  val = target_write_memory (addr & 0xfffffffc, contents_cache, 4);
  return val;
}
/* Initialize the gdbarch structure for the XPIC's. */


static struct gdbarch *
xpic_gdbarch_init (struct gdbarch_info info, struct gdbarch_list *arches)
{
  struct gdbarch *gdbarch;
  struct gdbarch_tdep *tdep;
  struct gdbarch_list *best_arch;
  int call_length;

  /* Avr-6 call instructions save 3 bytes.  */
  switch (info.bfd_arch_info->mach)
    {
    default:
    case bfd_mach_xpic:
      call_length = 4;
      break;
    }

  /* If there is already a candidate, use it.  */
  for (best_arch = gdbarch_list_lookup_by_info (arches, &info);
       best_arch != NULL;
       best_arch = gdbarch_list_lookup_by_info (best_arch->next, &info))
    {
      if (gdbarch_tdep (best_arch->gdbarch)->call_length == call_length)
	return best_arch->gdbarch;
    }

  /* None found, create a new architecture from the information provided. */
  tdep = XMALLOC (struct gdbarch_tdep);
  gdbarch = gdbarch_alloc (&info, tdep);
  
  tdep->call_length = call_length;

  set_gdbarch_short_bit (gdbarch, 2 * TARGET_CHAR_BIT);
  set_gdbarch_int_bit (gdbarch, 4 * TARGET_CHAR_BIT);
  set_gdbarch_long_bit (gdbarch, 4 * TARGET_CHAR_BIT);
  set_gdbarch_long_long_bit (gdbarch, 8 * TARGET_CHAR_BIT);
  set_gdbarch_ptr_bit (gdbarch, 4 * TARGET_CHAR_BIT);
  set_gdbarch_addr_bit (gdbarch, 32);

  set_gdbarch_float_bit (gdbarch, 4 * TARGET_CHAR_BIT);
  set_gdbarch_double_bit (gdbarch, 4 * TARGET_CHAR_BIT);
  set_gdbarch_long_double_bit (gdbarch, 4 * TARGET_CHAR_BIT);

  set_gdbarch_float_format (gdbarch, floatformats_ieee_single);
  set_gdbarch_double_format (gdbarch, floatformats_ieee_single);
  set_gdbarch_long_double_format (gdbarch, floatformats_ieee_single);

  set_gdbarch_read_pc (gdbarch, xpic_read_pc);
  set_gdbarch_write_pc (gdbarch, xpic_write_pc);

  set_gdbarch_num_regs (gdbarch, XPIC_NUM_REGS);

  set_gdbarch_sp_regnum (gdbarch, XPIC_SP_REGNUM);
  set_gdbarch_pc_regnum (gdbarch, XPIC_PC_REGNUM);

  set_gdbarch_register_name (gdbarch, xpic_register_name);
  set_gdbarch_register_type (gdbarch, xpic_register_type);

  set_gdbarch_return_value (gdbarch, xpic_return_value);
  set_gdbarch_print_insn (gdbarch, print_insn_xpic);

  set_gdbarch_push_dummy_call (gdbarch, xpic_push_dummy_call);

  set_gdbarch_address_to_pointer (gdbarch, xpic_address_to_pointer);
  set_gdbarch_pointer_to_address (gdbarch, xpic_pointer_to_address);

  set_gdbarch_skip_prologue (gdbarch, xpic_skip_prologue);
  set_gdbarch_inner_than (gdbarch, core_addr_lessthan);

  set_gdbarch_memory_insert_breakpoint (gdbarch,xpic_memory_insert_breakpoint);
  set_gdbarch_memory_remove_breakpoint (gdbarch,xpic_memory_remove_breakpoint);
  set_gdbarch_breakpoint_from_pc       (gdbarch,xpic_breakpoint_from_pc      );

  frame_unwind_append_unwinder (gdbarch, &xpic_frame_unwind);
  frame_base_set_default (gdbarch, &xpic_frame_base);

  set_gdbarch_dummy_id (gdbarch, xpic_dummy_id);

  set_gdbarch_unwind_pc (gdbarch, xpic_unwind_pc);
  set_gdbarch_unwind_sp (gdbarch, xpic_unwind_sp);

  return gdbarch;
}

/* Send a query request to the xpic remote target asking for values of the io
   registers. If args parameter is not NULL, then the user has requested info
   on a specific io register [This still needs implemented and is ignored for
   now]. The query string should be one of these forms:

   "Rxpic.io_reg" -> reply is "NN" number of io registers

   "Rxpic.io_reg:addr,len" where addr is first register and len is number of
   registers to be read. The reply should be "<NAME>,VV;" for each io register
   where, <NAME> is a string, and VV is the hex value of the register.

   All io registers are 8-bit. */

static void
xpic_io_reg_read_command (char *args, int from_tty)
{
  LONGEST bufsiz = 0;
  gdb_byte *buf;
  char query[400];
  char *p;
  unsigned int nreg = 0;
  unsigned int val;
  int i, j, k, step;

  /* Find out how many io registers the target has. */
  bufsiz = target_read_alloc (&current_target, TARGET_OBJECT_XPIC,
			      "xpic.io_reg", &buf);

  if (bufsiz <= 0)
    {
      fprintf_unfiltered (gdb_stderr,
			  _("ERR: info io_registers NOT supported "
			    "by current target\n"));
      return;
    }

  if (sscanf (buf, "%x", &nreg) != 1)
    {
      fprintf_unfiltered (gdb_stderr,
			  _("Error fetching number of io registers\n"));
      xfree (buf);
      return;
    }

  xfree (buf);

  reinitialize_more_filter ();

  printf_unfiltered (_("Target has %u io registers:\n\n"), nreg);

  /* only fetch up to 8 registers at a time to keep the buffer small */
  step = 8;

  for (i = 0; i < nreg; i += step)
    {
      /* how many registers this round? */
      j = step;
      if ((i+j) >= nreg)
        j = nreg - i;           /* last block is less than 8 registers */

      snprintf (query, sizeof (query) - 1, "xpic.io_reg:%x,%x", i, j);
      bufsiz = target_read_alloc (&current_target, TARGET_OBJECT_XPIC,
				  query, &buf);

      p = buf;
      for (k = i; k < (i + j); k++)
	{
	  if (sscanf (p, "%[^,],%x;", query, &val) == 2)
	    {
	      printf_filtered ("[%02x] %-15s : %02x\n", k, query, val);
	      while ((*p != ';') && (*p != '\0'))
		p++;
	      p++;		/* skip over ';' */
	      if (*p == '\0')
		break;
	    }
	}

      xfree (buf);
    }
}

extern initialize_file_ftype _initialize_xpic_tdep; /* -Wmissing-prototypes */

void
_initialize_xpic_tdep (void)
{
  register_gdbarch_init (bfd_arch_xpic, xpic_gdbarch_init);

  /* Add a new command to allow the user to query the xpic remote target for
     the values of the io space registers in a saner way than just using
     `x/NNNb ADDR`. */

  add_cmd ("io_registers", class_info, xpic_io_reg_read_command,
	   _("query remote xpic target for io space register values"),
	   &infolist);
}
