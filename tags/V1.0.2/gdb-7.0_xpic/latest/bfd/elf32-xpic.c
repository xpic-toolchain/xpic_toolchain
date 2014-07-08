/* AVR-specific support for 32-bit ELF
   Copyright 1999, 2000, 2001, 2002, 2003, 2004, 2005, 2006, 2007, 2008, 2009
   Free Software Foundation, Inc.
   Contributed by Denis Chertykov <denisc@overta.ru>

   This file is part of BFD, the Binary File Descriptor library.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street - Fifth Floor,
   Boston, MA 02110-1301, USA.  */

#include "sysdep.h"
#include "bfd.h"
#include "libbfd.h"
#include "elf-bfd.h"
#include "elf/xpic.h"
#include "elf32-xpic.h"
#include "bfdlink.h"
//#define SHOW_RELOC_DST
#define SHOW_RELOC_OOR

// BI: Disabled broken relaxing methods
#define NO_Y17_PCREL_TO_ABSOLUTE_RELAX
#define FORCE_Z19_TO_FARLOAD_RELAX

//#define NO_Z19_ABSOLUTE_JUMP_RELAX


/* Enable debugging printout at stdout with this variable.  */
static bfd_boolean debug_relax = FALSE;

/* Enable debugging printout at stdout with this variable.  */
static bfd_boolean debug_stubs = FALSE;

/* Hash table initialization and handling.  Code is taken from the hppa port
   and adapted to the needs of AVR.  */

/* We use two hash tables to hold information for linking xpic objects.

   The first is the elf32_xpic_link_hash_tablse which is derived from the
   stanard ELF linker hash table.  We use this as a place to attach the other
   hash table and some static information.

   The second is the stub hash table which is derived from the base BFD
   hash table.  The stub hash table holds the information on the linker
   stubs.  */

struct elf32_xpic_stub_hash_entry
{
  /* Base hash table entry structure.  */
  struct bfd_hash_entry bh_root;

  /* Offset within stub_sec of the beginning of this stub.  */
  bfd_vma stub_offset;

  /* Given the symbol's value and its section we can determine its final
     value when building the stubs (so the stub knows where to jump).  */
  bfd_vma target_value;

  /* This way we could mark stubs to be no longer necessary.  */
  bfd_boolean is_actually_needed;
};

struct elf32_xpic_link_hash_table
{
  /* The main hash table.  */
  struct elf_link_hash_table etab;

  /* The stub hash table.  */
  struct bfd_hash_table bstab;

  bfd_boolean no_stubs;

  /* Linker stub bfd.  */
  bfd *stub_bfd;

  /* The stub section.  */
  asection *stub_sec;

  /* Usually 0, unless we are generating code for a bootloader.  Will
     be initialized by elf32_xpic_size_stubs to the vma offset of the
     output section associated with the stub section.  */
  bfd_vma vector_base;

  /* Assorted information used by elf32_xpic_size_stubs.  */
  unsigned int        bfd_count;
  int                 top_index;
  asection **         input_list;
  Elf_Internal_Sym ** all_local_syms;

  /* Tables for mapping vma beyond the 128k boundary to the address of the
     corresponding stub.  (AMT)
     "amt_max_entry_cnt" reflects the number of entries that memory is allocated
     for in the "amt_stub_offsets" and "amt_destination_addr" arrays.
     "amt_entry_cnt" informs how many of these entries actually contain
     useful data.  */
  unsigned int amt_entry_cnt;
  unsigned int amt_max_entry_cnt;
  bfd_vma *    amt_stub_offsets;
  bfd_vma *    amt_destination_addr;
};

/* Various hash macros and functions.  */
#define xpic_link_hash_table(p) \
  /* PR 3874: Check that we have an xpic style hash table before using it.  */\
  ((p)->hash->table.newfunc != elf32_xpic_link_hash_newfunc ? NULL : \
   ((struct elf32_xpic_link_hash_table *) ((p)->hash)))

#define xpic_stub_hash_entry(ent) \
  ((struct elf32_xpic_stub_hash_entry *)(ent))

#define xpic_stub_hash_lookup(table, string, create, copy) \
  ((struct elf32_xpic_stub_hash_entry *) \
   bfd_hash_lookup ((table), (string), (create), (copy)))

static reloc_howto_type elf_xpic_howto_table[] =
{
  HOWTO (R_XPIC_NONE,		/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 32,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_bitfield, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_XPIC_NONE",		/* name */
	 FALSE,			/* partial_inplace */
	 0,			/* src_mask */
	 0,			/* dst_mask */
	 FALSE),		/* pcrel_offset */

  HOWTO (R_XPIC_32,		/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 32,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_bitfield, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_XPIC_32",		/* name */
	 FALSE,			/* partial_inplace */
	 0xffffffff,		/* src_mask */
	 0xffffffff,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* A 7 bit PC relative relocation.  */
  HOWTO (R_AVR_7_PCREL,		/* type */
	 1,			/* rightshift */
	 1,			/* size (0 = byte, 1 = short, 2 = long) */
	 7,			/* bitsize */
	 TRUE,			/* pc_relative */
	 3,			/* bitpos */
	 complain_overflow_bitfield, /* complain_on_overflow */
	 bfd_elf_generic_reloc, /* special_function */
	 "R_AVR_7_PCREL",	/* name */
	 FALSE,			/* partial_inplace */
	 0xffff,		/* src_mask */
	 0xffff,		/* dst_mask */
	 TRUE),			/* pcrel_offset */

  /* A 13 bit PC relative relocation.  */
  HOWTO (R_AVR_13_PCREL,	/* type */
	 1,			/* rightshift */
	 1,			/* size (0 = byte, 1 = short, 2 = long) */
	 13,			/* bitsize */
	 TRUE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_bitfield, /* complain_on_overflow */
	 bfd_elf_generic_reloc, /* special_function */
	 "R_AVR_13_PCREL",	/* name */
	 FALSE,			/* partial_inplace */
	 0xfff,			/* src_mask */
	 0xfff,			/* dst_mask */
	 TRUE),			/* pcrel_offset */

  /* A 16 bit absolute relocation.  */
  HOWTO (R_AVR_16,		/* type */
	 0,			/* rightshift */
	 1,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_AVR_16",		/* name */
	 FALSE,			/* partial_inplace */
	 0xffff,		/* src_mask */
	 0xffff,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* A 16 bit absolute relocation for command address
     Will be changed when linker stubs are needed.  */
  HOWTO (R_AVR_16_PM,		/* type */
	 1,			/* rightshift */
	 1,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_bitfield, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_AVR_16_PM",		/* name */
	 FALSE,			/* partial_inplace */
	 0xffff,		/* src_mask */
	 0xffff,		/* dst_mask */
	 FALSE),		/* pcrel_offset */
  /* A low 8 bit absolute relocation of 16 bit address.
     For LDI command.  */
  HOWTO (R_AVR_LO8_LDI,		/* type */
	 0,			/* rightshift */
	 1,			/* size (0 = byte, 1 = short, 2 = long) */
	 8,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_AVR_LO8_LDI",	/* name */
	 FALSE,			/* partial_inplace */
	 0xffff,		/* src_mask */
	 0xffff,		/* dst_mask */
	 FALSE),		/* pcrel_offset */
  /* A high 8 bit absolute relocation of 16 bit address.
     For LDI command.  */
  HOWTO (R_AVR_HI8_LDI,		/* type */
	 8,			/* rightshift */
	 1,			/* size (0 = byte, 1 = short, 2 = long) */
	 8,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_AVR_HI8_LDI",	/* name */
	 FALSE,			/* partial_inplace */
	 0xffff,		/* src_mask */
	 0xffff,		/* dst_mask */
	 FALSE),		/* pcrel_offset */
  /* A high 6 bit absolute relocation of 22 bit address.
     For LDI command.  As well second most significant 8 bit value of
     a 32 bit link-time constant.  */
  HOWTO (R_AVR_HH8_LDI,		/* type */
	 16,			/* rightshift */
	 1,			/* size (0 = byte, 1 = short, 2 = long) */
	 8,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_AVR_HH8_LDI",	/* name */
	 FALSE,			/* partial_inplace */
	 0xffff,		/* src_mask */
	 0xffff,		/* dst_mask */
	 FALSE),		/* pcrel_offset */
  /* A negative low 8 bit absolute relocation of 16 bit address.
     For LDI command.  */
  HOWTO (R_AVR_LO8_LDI_NEG,	/* type */
	 0,			/* rightshift */
	 1,			/* size (0 = byte, 1 = short, 2 = long) */
	 8,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_AVR_LO8_LDI_NEG",	/* name */
	 FALSE,			/* partial_inplace */
	 0xffff,		/* src_mask */
	 0xffff,		/* dst_mask */
	 FALSE),		/* pcrel_offset */
  /* A negative high 8 bit absolute relocation of 16 bit address.
     For LDI command.  */
  HOWTO (R_AVR_HI8_LDI_NEG,	/* type */
	 8,			/* rightshift */
	 1,			/* size (0 = byte, 1 = short, 2 = long) */
	 8,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_AVR_HI8_LDI_NEG",	/* name */
	 FALSE,			/* partial_inplace */
	 0xffff,		/* src_mask */
	 0xffff,		/* dst_mask */
	 FALSE),		/* pcrel_offset */
  /* A negative high 6 bit absolute relocation of 22 bit address.
     For LDI command.  */
  HOWTO (R_AVR_HH8_LDI_NEG,	/* type */
	 16,			/* rightshift */
	 1,			/* size (0 = byte, 1 = short, 2 = long) */
	 8,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_AVR_HH8_LDI_NEG",	/* name */
	 FALSE,			/* partial_inplace */
	 0xffff,		/* src_mask */
	 0xffff,		/* dst_mask */
	 FALSE),		/* pcrel_offset */
  /* A low 8 bit absolute relocation of 24 bit program memory address.
     For LDI command.  Will not be changed when linker stubs are needed. */
  HOWTO (R_AVR_LO8_LDI_PM,	/* type */
	 1,			/* rightshift */
	 1,			/* size (0 = byte, 1 = short, 2 = long) */
	 8,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_AVR_LO8_LDI_PM",	/* name */
	 FALSE,			/* partial_inplace */
	 0xffff,		/* src_mask */
	 0xffff,		/* dst_mask */
	 FALSE),		/* pcrel_offset */
  /* A low 8 bit absolute relocation of 24 bit program memory address.
     For LDI command.  Will not be changed when linker stubs are needed. */
  HOWTO (R_AVR_HI8_LDI_PM,	/* type */
	 9,			/* rightshift */
	 1,			/* size (0 = byte, 1 = short, 2 = long) */
	 8,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_AVR_HI8_LDI_PM",	/* name */
	 FALSE,			/* partial_inplace */
	 0xffff,		/* src_mask */
	 0xffff,		/* dst_mask */
	 FALSE),		/* pcrel_offset */
  /* A low 8 bit absolute relocation of 24 bit program memory address.
     For LDI command.  Will not be changed when linker stubs are needed. */
  HOWTO (R_AVR_HH8_LDI_PM,	/* type */
	 17,			/* rightshift */
	 1,			/* size (0 = byte, 1 = short, 2 = long) */
	 8,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_AVR_HH8_LDI_PM",	/* name */
	 FALSE,			/* partial_inplace */
	 0xffff,		/* src_mask */
	 0xffff,		/* dst_mask */
	 FALSE),		/* pcrel_offset */
  /* A low 8 bit absolute relocation of 24 bit program memory address.
     For LDI command.  Will not be changed when linker stubs are needed. */
  HOWTO (R_AVR_LO8_LDI_PM_NEG,	/* type */
	 1,			/* rightshift */
	 1,			/* size (0 = byte, 1 = short, 2 = long) */
	 8,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_AVR_LO8_LDI_PM_NEG", /* name */
	 FALSE,			/* partial_inplace */
	 0xffff,		/* src_mask */
	 0xffff,		/* dst_mask */
	 FALSE),		/* pcrel_offset */
  /* A low 8 bit absolute relocation of 24 bit program memory address.
     For LDI command.  Will not be changed when linker stubs are needed. */
  HOWTO (R_AVR_HI8_LDI_PM_NEG,	/* type */
	 9,			/* rightshift */
	 1,			/* size (0 = byte, 1 = short, 2 = long) */
	 8,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_AVR_HI8_LDI_PM_NEG", /* name */
	 FALSE,			/* partial_inplace */
	 0xffff,		/* src_mask */
	 0xffff,		/* dst_mask */
	 FALSE),		/* pcrel_offset */
  /* A low 8 bit absolute relocation of 24 bit program memory address.
     For LDI command.  Will not be changed when linker stubs are needed. */
  HOWTO (R_AVR_HH8_LDI_PM_NEG,	/* type */
	 17,			/* rightshift */
	 1,			/* size (0 = byte, 1 = short, 2 = long) */
	 8,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_AVR_HH8_LDI_PM_NEG", /* name */
	 FALSE,			/* partial_inplace */
	 0xffff,		/* src_mask */
	 0xffff,		/* dst_mask */
	 FALSE),		/* pcrel_offset */
  /* Relocation for CALL command in ATmega.  */
  HOWTO (R_AVR_CALL,		/* type */
	 1,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 23,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont,/* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_AVR_CALL",		/* name */
	 FALSE,			/* partial_inplace */
	 0xffffffff,		/* src_mask */
	 0xffffffff,		/* dst_mask */
	 FALSE),			/* pcrel_offset */
  /* A 16 bit absolute relocation of 16 bit address.
     For LDI command.  */
  HOWTO (R_AVR_LDI,		/* type */
	 0,			/* rightshift */
	 1,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont,/* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_AVR_LDI",		/* name */
	 FALSE,			/* partial_inplace */
	 0xffff,		/* src_mask */
	 0xffff,		/* dst_mask */
	 FALSE),		/* pcrel_offset */
  /* A 6 bit absolute relocation of 6 bit offset.
     For ldd/sdd command.  */
  HOWTO (R_AVR_6,		/* type */
	 0,			/* rightshift */
	 0,			/* size (0 = byte, 1 = short, 2 = long) */
	 6,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont,/* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_AVR_6",		/* name */
	 FALSE,			/* partial_inplace */
	 0xffff,		/* src_mask */
	 0xffff,		/* dst_mask */
	 FALSE),		/* pcrel_offset */
  /* A 6 bit absolute relocation of 6 bit offset.
     For sbiw/adiw command.  */
  HOWTO (R_AVR_6_ADIW,		/* type */
	 0,			/* rightshift */
	 0,			/* size (0 = byte, 1 = short, 2 = long) */
	 6,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont,/* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_AVR_6_ADIW",	/* name */
	 FALSE,			/* partial_inplace */
	 0xffff,		/* src_mask */
	 0xffff,		/* dst_mask */
	 FALSE),		/* pcrel_offset */
  /* Most significant 8 bit value of a 32 bit link-time constant.  */
  HOWTO (R_AVR_MS8_LDI,		/* type */
	 24,			/* rightshift */
	 1,			/* size (0 = byte, 1 = short, 2 = long) */
	 8,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_AVR_MS8_LDI",	/* name */
	 FALSE,			/* partial_inplace */
	 0xffff,		/* src_mask */
	 0xffff,		/* dst_mask */
	 FALSE),		/* pcrel_offset */
  /* Negative most significant 8 bit value of a 32 bit link-time constant.  */
  HOWTO (R_AVR_MS8_LDI_NEG,	/* type */
	 24,			/* rightshift */
	 1,			/* size (0 = byte, 1 = short, 2 = long) */
	 8,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_AVR_MS8_LDI_NEG",	/* name */
	 FALSE,			/* partial_inplace */
	 0xffff,		/* src_mask */
	 0xffff,		/* dst_mask */
	 FALSE), 		/* pcrel_offset */
  /* A low 8 bit absolute relocation of 24 bit program memory address.
     For LDI command.  Will be changed when linker stubs are needed. */
  HOWTO (R_AVR_LO8_LDI_GS,      /* type */
         1,                     /* rightshift */
         1,                     /* size (0 = byte, 1 = short, 2 = long) */
         8,                     /* bitsize */
         FALSE,                 /* pc_relative */
         0,                     /* bitpos */
         complain_overflow_dont, /* complain_on_overflow */
         bfd_elf_generic_reloc, /* special_function */
         "R_AVR_LO8_LDI_GS",    /* name */
         FALSE,                 /* partial_inplace */
         0xffff,                /* src_mask */
         0xffff,                /* dst_mask */
         FALSE),                /* pcrel_offset */
  /* A low 8 bit absolute relocation of 24 bit program memory address.
     For LDI command.  Will be changed when linker stubs are needed. */
  HOWTO (R_AVR_HI8_LDI_GS,      /* type */
         9,                     /* rightshift */
         1,                     /* size (0 = byte, 1 = short, 2 = long) */
         8,                     /* bitsize */
         FALSE,                 /* pc_relative */
         0,                     /* bitpos */
         complain_overflow_dont, /* complain_on_overflow */
         bfd_elf_generic_reloc, /* special_function */
         "R_AVR_HI8_LDI_GS",    /* name */
         FALSE,                 /* partial_inplace */
         0xffff,                /* src_mask */
         0xffff,                /* dst_mask */
         FALSE),                 /* pcrel_offset */

  /* A 24 bit PC relative relocation.  */
  HOWTO (R_XPIC_24_PCREL,	/* type */
	 2,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 24,			/* bitsize */
	 TRUE,			/* pc_relative */
	 5,			/* bitpos */
	 complain_overflow_bitfield, /* complain_on_overflow */
	 bfd_elf_generic_reloc, /* special_function */
	 "R_XPIC_24_PCREL",	/* name */
	 FALSE,			/* partial_inplace */
	 0x00ffffff,		/* src_mask */
	 0x00ffffff,		/* dst_mask */
	 TRUE),			/* pcrel_offset */
  /* A 17 bit PC relative relocation, dependend from memory access  (byte, word, dword aligned)*/
  HOWTO (R_XPIC_Y17_PCREL,	/* type < load rx,[pc + #const] > */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 17,			/* bitsize */
	 TRUE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_bitfield, /* complain_on_overflow */
	 bfd_elf_generic_reloc, /* special_function */
	 "R_XPIC_Y17_PCREL",	/* name */
	 FALSE,			/* partial_inplace */
	 0x0001ffff,		/* src_mask */
	 0x0001ffff,		/* dst_mask */
	 TRUE),			/* pcrel_offset */
  /* A 18 bit absolute relocation of 18 bit data memory address */
  HOWTO (R_XPIC_Z19,	/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 19,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_bitfield, /* complain_on_overflow */
	 bfd_elf_generic_reloc, /* special_function */
	 "R_XPIC_Z19",	/* name */
	 FALSE,			/* partial_inplace */
	 0x0002ffff,		/* src_mask */
	 0x0002ffff,		/* dst_mask */
	 TRUE),			/* pcrel_offset */
  /* A 15 bit PC relative relocation, dword aligned, pass to 17 bit*/
  HOWTO (R_XPIC_X15_PCREL,	/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 15,			/* bitsize */
	 TRUE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_bitfield, /* complain_on_overflow */
	 bfd_elf_generic_reloc, /* special_function */
	 "R_XPIC_X15_PCREL",	/* name */
	 FALSE,			/* partial_inplace */
	 0x00007fff,		/* src_mask */
	 0x00007fff,		/* dst_mask */
	 TRUE),			/* pcrel_offset */
  /* A 17 bit relocation, dependend from memory access  (byte, word, dword aligned)*/
  HOWTO (R_XPIC_Y17,     	/* type < load rx,[z0 + #const] > */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 17,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_bitfield, /* complain_on_overflow */
	 bfd_elf_generic_reloc, /* special_function */
	 "R_XPIC_Y17",  	/* name */
	 FALSE,			/* partial_inplace */
	 0x0001ffff,		/* src_mask */
	 0x0001ffff,		/* dst_mask */
	 TRUE),			/* pcrel_offset */
  /* A 24 bit PC relative relocation.  */
  HOWTO (R_XPIC_LONG_JMP,	/* type */
	 2,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 24,			/* bitsize */
	 TRUE,			/* pc_relative */
	 5,			/* bitpos */
	 complain_overflow_bitfield, /* complain_on_overflow */
	 bfd_elf_generic_reloc, /* special_function */
	 "R_XPIC_LONG_JMP",	/* name */
	 FALSE,			/* partial_inplace */
	 0x00ffffff,		/* src_mask */
	 0x00ffffff,		/* dst_mask */
	 TRUE),			/* pcrel_offset */
};
/* Map BFD reloc types to AVR ELF reloc types.  */

struct xpic_reloc_map
{
  bfd_reloc_code_real_type bfd_reloc_val;
  unsigned int elf_reloc_val;
};

static const struct xpic_reloc_map xpic_reloc_map[] =
{
  { BFD_RELOC_NONE,                 R_XPIC_NONE },
  { BFD_RELOC_32,                   R_XPIC_32 },
  { BFD_RELOC_AVR_7_PCREL,          R_AVR_7_PCREL },
  { BFD_RELOC_AVR_13_PCREL,         R_AVR_13_PCREL },
  { BFD_RELOC_16,                   R_AVR_16 },
  { BFD_RELOC_AVR_16_PM,            R_AVR_16_PM },
  { BFD_RELOC_AVR_LO8_LDI,          R_AVR_LO8_LDI},
  { BFD_RELOC_AVR_HI8_LDI,          R_AVR_HI8_LDI },
  { BFD_RELOC_AVR_HH8_LDI,          R_AVR_HH8_LDI },
  { BFD_RELOC_AVR_MS8_LDI,          R_AVR_MS8_LDI },
  { BFD_RELOC_AVR_LO8_LDI_NEG,      R_AVR_LO8_LDI_NEG },
  { BFD_RELOC_AVR_HI8_LDI_NEG,      R_AVR_HI8_LDI_NEG },
  { BFD_RELOC_AVR_HH8_LDI_NEG,      R_AVR_HH8_LDI_NEG },
  { BFD_RELOC_AVR_MS8_LDI_NEG,      R_AVR_MS8_LDI_NEG },
  { BFD_RELOC_AVR_LO8_LDI_PM,       R_AVR_LO8_LDI_PM },
  { BFD_RELOC_AVR_LO8_LDI_GS,       R_AVR_LO8_LDI_GS },
  { BFD_RELOC_AVR_HI8_LDI_PM,       R_AVR_HI8_LDI_PM },
  { BFD_RELOC_AVR_HI8_LDI_GS,       R_AVR_HI8_LDI_GS },
  { BFD_RELOC_AVR_HH8_LDI_PM,       R_AVR_HH8_LDI_PM },
  { BFD_RELOC_AVR_LO8_LDI_PM_NEG,   R_AVR_LO8_LDI_PM_NEG },
  { BFD_RELOC_AVR_HI8_LDI_PM_NEG,   R_AVR_HI8_LDI_PM_NEG },
  { BFD_RELOC_XPIC_24_PCREL,        R_XPIC_24_PCREL },
  { BFD_RELOC_XPIC_Y17_PCREL,       R_XPIC_Y17_PCREL },
  { BFD_RELOC_XPIC_Y17,	            R_XPIC_Y17 },
  { BFD_RELOC_XPIC_Z19,             R_XPIC_Z19 },
  { BFD_RELOC_XPIC_X15_PCREL,       R_XPIC_X15_PCREL },
  { BFD_RELOC_XPIC_LONG_JMP,        R_XPIC_LONG_JMP },
  { BFD_RELOC_AVR_HH8_LDI_PM_NEG,   R_AVR_HH8_LDI_PM_NEG },
  { BFD_RELOC_AVR_CALL,             R_AVR_CALL },
  { BFD_RELOC_AVR_LDI,              R_AVR_LDI  },
  { BFD_RELOC_AVR_6,                R_AVR_6    },
  { BFD_RELOC_AVR_6_ADIW,           R_AVR_6_ADIW }
};

/* Meant to be filled one day with the wrap around address for the
   specific device.  I.e. should get the value 0x4000 for 16k devices,
   0x8000 for 32k devices and so on.

   We initialize it here with a value of 0x1000000 resulting in
   that we will never suggest a wrap-around jump during relaxation.
   The logic of the source code later on assumes that in
   xpic_pc_wrap_around one single bit is set.  */
static bfd_vma xpic_pc_wrap_around = 0x10000000;

/* If this variable holds a value different from zero, the linker relaxation
   machine will try to optimize call/ret sequences by a single jump
   instruction. This option could be switched off by a linker switch.  */
static int xpic_replace_call_ret_sequences = 1;

/* Initialize an entry in the stub hash table.  */


static struct bfd_hash_entry *
stub_hash_newfunc (struct bfd_hash_entry *entry,
                   struct bfd_hash_table *table,
                   const char *string)
{
  /* Allocate the structure if it has not already been allocated by a
     subclass.  */
  if (entry == NULL)
    {
      entry = bfd_hash_allocate (table,
                                 sizeof (struct elf32_xpic_stub_hash_entry));
      if (entry == NULL)
        return entry;
    }

  /* Call the allocation method of the superclass.  */
  entry = bfd_hash_newfunc (entry, table, string);
  if (entry != NULL)
    {
      struct elf32_xpic_stub_hash_entry *hsh;

      /* Initialize the local fields.  */
      hsh = xpic_stub_hash_entry (entry);
      hsh->stub_offset = 0;
      hsh->target_value = 0;
    }

  return entry;
}

/* This function is just a straight passthrough to the real
   function in linker.c.  Its prupose is so that its address
   can be compared inside the xpic_link_hash_table macro.  */

static struct bfd_hash_entry *
elf32_xpic_link_hash_newfunc (struct bfd_hash_entry * entry,
			     struct bfd_hash_table * table,
			     const char * string)
{
  return _bfd_elf_link_hash_newfunc (entry, table, string);
}

/* Create the derived linker hash table.  The AVR ELF port uses the derived
   hash table to keep information specific to the AVR ELF linker (without
   using static variables).  */

static struct bfd_link_hash_table *
elf32_xpic_link_hash_table_create (bfd *abfd)
{
  struct elf32_xpic_link_hash_table *htab;
  bfd_size_type amt = sizeof (*htab);

  htab = bfd_malloc (amt);
  if (htab == NULL)
    return NULL;

  if (!_bfd_elf_link_hash_table_init (&htab->etab, abfd,
                                      elf32_xpic_link_hash_newfunc,
                                      sizeof (struct elf_link_hash_entry)))
    {
      free (htab);
      return NULL;
    }

  /* Init the stub hash table too.  */
  if (!bfd_hash_table_init (&htab->bstab, stub_hash_newfunc,
                            sizeof (struct elf32_xpic_stub_hash_entry)))
    return NULL;

  htab->stub_bfd = NULL;
  htab->stub_sec = NULL;

  /* Initialize the address mapping table.  */
  htab->amt_stub_offsets = NULL;
  htab->amt_destination_addr = NULL;
  htab->amt_entry_cnt = 0;
  htab->amt_max_entry_cnt = 0;

  return &htab->etab.root;
}

/* Free the derived linker hash table.  */

static void
elf32_xpic_link_hash_table_free (struct bfd_link_hash_table *btab)
{
  struct elf32_xpic_link_hash_table *htab
    = (struct elf32_xpic_link_hash_table *) btab;

  /* Free the address mapping table.  */
  if (htab->amt_stub_offsets != NULL)
    free (htab->amt_stub_offsets);
  if (htab->amt_destination_addr != NULL)
    free (htab->amt_destination_addr);

  bfd_hash_table_free (&htab->bstab);
  _bfd_generic_link_hash_table_free (btab);
}


static reloc_howto_type * bfd_elf32_bfd_reloc_type_lookup (bfd *abfd ATTRIBUTE_UNUSED,bfd_reloc_code_real_type code)
{
  unsigned int i;

  for (i = 0;
       i < sizeof (xpic_reloc_map) / sizeof (struct xpic_reloc_map);
       i++)
    if (xpic_reloc_map[i].bfd_reloc_val == code)
      return &elf_xpic_howto_table[xpic_reloc_map[i].elf_reloc_val];

  return NULL;
}

static reloc_howto_type * bfd_elf32_bfd_reloc_name_lookup (bfd *abfd ATTRIBUTE_UNUSED,const char *r_name)
{
  unsigned int i;

  for (i = 0;
       i < sizeof (elf_xpic_howto_table) / sizeof (elf_xpic_howto_table[0]);
       i++)
    if (elf_xpic_howto_table[i].name != NULL
	&& strcasecmp (elf_xpic_howto_table[i].name, r_name) == 0)
      return &elf_xpic_howto_table[i];

  return NULL;
}

/* Set the howto pointer for an AVR ELF reloc.  */
static void xpic_info_to_howto_rela (bfd *abfd ATTRIBUTE_UNUSED,arelent *cache_ptr,Elf_Internal_Rela *dst)
{
  unsigned int r_type;

  r_type = ELF32_R_TYPE (dst->r_info);
  BFD_ASSERT (r_type < (unsigned int) R_XPIC_max);
  cache_ptr->howto = &elf_xpic_howto_table[r_type];
}

/* Look through the relocs for a section during the first phase.
   Since we don't do .gots or .plts, we just need to consider the
   virtual table relocs for gc.  */

static bfd_boolean elf32_xpic_check_relocs (bfd *abfd,struct bfd_link_info *info,asection *sec, const Elf_Internal_Rela *relocs)
{
  Elf_Internal_Shdr *symtab_hdr;
  struct elf_link_hash_entry **sym_hashes;
  const Elf_Internal_Rela *rel;
  const Elf_Internal_Rela *rel_end;

  if (info->relocatable)
    return TRUE;

  symtab_hdr = &elf_tdata (abfd)->symtab_hdr;
  sym_hashes = elf_sym_hashes (abfd);

  rel_end = relocs + sec->reloc_count;
  for (rel = relocs; rel < rel_end; rel++)
    {
      struct elf_link_hash_entry *h;
      unsigned long r_symndx;

      r_symndx = ELF32_R_SYM (rel->r_info);
      if (r_symndx < symtab_hdr->sh_info)
        h = NULL;
      else
	{
	  h = sym_hashes[r_symndx - symtab_hdr->sh_info];
	  while (h->root.type == bfd_link_hash_indirect
		 || h->root.type == bfd_link_hash_warning)
	    h = (struct elf_link_hash_entry *) h->root.u.i.link;
	}
    }

  return TRUE;
}

static bfd_boolean xpic_stub_is_required_for_16_bit_reloc (bfd_vma relocation)
{
  return (relocation >= 0x020000);
}

#if 0
/* Returns the address of the corresponding stub if there is one.
   Returns otherwise an address above 0x020000.  This function
   could also be used, if there is no knowledge on the section where
   the destination is found.  */
static bfd_vma xpic_get_stub_addr (bfd_vma srel, struct elf32_xpic_link_hash_table *htab)
{
  unsigned int index;
  bfd_vma stub_sec_addr =
              (htab->stub_sec->output_section->vma +
	       htab->stub_sec->output_offset);

  for (index = 0; index < htab->amt_max_entry_cnt; index ++)
    if (htab->amt_destination_addr[index] == srel)
      return htab->amt_stub_offsets[index] + stub_sec_addr;

  /* Return an address that could not be reached by 16 bit relocs.  */
  return 0x020000;
}
#endif
/// All next saturation returns 0 on success!
/// saturation for Y constant
int xpic_saturateYv2(int srel, unsigned  int mas)// mas code from docu
{
    int val;
    // ranges:
    val=srel>>mas;
    if(val< -65536 || val>65535 )
	return -1;// out of range
    return 0;
}
int xpic_saturateY(bfd_signed_vma *srel, unsigned  int mas)
{
    // get align bits
    int sr=(1<<mas)-1;
    // get value
    int val=*srel>>mas;
    unsigned int align;
    if(val< -65536 || val>65535 )
	return -1;// out of range
    // check alignment
    align = (*srel)&0x00000003&sr;
    if (align)
	return -2;// disallignment

    *srel>>=mas;// apply align
    return 0;
}
/// saturation for Z constant
#define xpic_saturateZ(v)   ( (bfd_signed_vma)v <  -262144 || (bfd_signed_vma)v >  262143 )
/// saturation for X constant
#define xpic_saturateX(v)   ( (bfd_signed_vma)v <   -65536 || (bfd_signed_vma)v >   65535 )
/// saturation for Jump
#define xpic_saturateJMP(v) ( (bfd_signed_vma)v <-33554432 || (bfd_signed_vma)v > 33554431 )


/// Perform a single relocation.  By default we use the standard BFD
/// routines, but a few relocs, we have to do them ourselves.  

static bfd_reloc_status_type
xpic_final_link_relocate (reloc_howto_type *                 howto,
			 bfd *                              input_bfd,
			 asection *                         input_section,
			 bfd_byte *                         contents,
			 Elf_Internal_Rela *                rel,
                         bfd_vma                            relocation,
                         struct elf32_xpic_link_hash_table * htab)
{
  bfd_reloc_status_type r = bfd_reloc_ok;
  bfd_vma               x;
  bfd_signed_vma	srel;
  bfd_signed_vma	reloc_addr;
  htab=htab;//unused

  /* Absolute addr of the reloc in the final excecutable.  */
  reloc_addr = rel->r_offset + input_section->output_section->vma + input_section->output_offset;

  switch (howto->type)
    {
    case R_AVR_7_PCREL:
    case R_AVR_13_PCREL:
    case R_AVR_LO8_LDI:
    case R_AVR_LDI:
    case R_AVR_6:
    case R_AVR_6_ADIW:
    case R_AVR_HI8_LDI:
    case R_AVR_HH8_LDI:
    case R_AVR_MS8_LDI:
    case R_AVR_LO8_LDI_NEG:
    case R_AVR_HI8_LDI_NEG:
    case R_AVR_HH8_LDI_NEG:
    case R_AVR_MS8_LDI_NEG:
    case R_AVR_LO8_LDI_GS:
    case R_AVR_HH8_LDI_PM:
    case R_AVR_LO8_LDI_PM_NEG:
    case R_AVR_HI8_LDI_PM_NEG:
    case R_AVR_HH8_LDI_PM_NEG:
    case R_AVR_CALL:
///!!!
    case R_AVR_LO8_LDI_PM:
    case R_AVR_HI8_LDI_GS:
    case R_AVR_HI8_LDI_PM:
    case R_AVR_16_PM:
	return bfd_reloc_outofrange;




case R_XPIC_LONG_JMP:// now it is equal!!!
case R_XPIC_24_PCREL:
      contents   += rel->r_offset;
      srel = (bfd_signed_vma) relocation;// addres of current function
      srel += rel->r_addend;	// addres of instruction jump to
      srel -= rel->r_offset;	// current addres of instruction in object-file
// input_section->output_offset	// current section offset
      srel -= (input_section->output_section->vma + input_section->output_offset);
//printf("rel->r_addend=%d,  rel->r_offset=%d, relocation=%d, input_section->output_section->vma=%d, input_section->output_offset=%d\n",rel->r_addend,rel->r_offset, relocation,input_section->output_section->vma,input_section->output_offset);
      // calculation of reloc done!
      // check it now:
#ifdef SHOW_RELOC_DST
printf("\n ... R_XPIC_24_PCREL = %d (0x%x)\n",srel,srel);
#endif
      if(xpic_saturateJMP(srel))
      {
#ifdef SHOW_RELOC_OOR
        printf("... OUT OF RANGE: R_XPIC_24_PCREL 0x%08x\n",srel);
#endif
        return bfd_reloc_outofrange;
      }
      if (srel & 3) return bfd_reloc_outofrange;// misalligned dword access
      // insert in opcode:
      srel&=0x03ffffff;	// 26 bit mask
      srel>>=2;		// dword: two lsb are 0
      srel<<=5;		// location in 'jmp' instruction bits 5-28
      x = bfd_get_32 (input_bfd, contents);
      bfd_put_32 (input_bfd, x | srel , contents);
//      bfd_put_32 (input_bfd, 0xc0000010 , contents);
      break;

case R_XPIC_Y17:{
      unsigned int mas;
      contents   += rel->r_offset;
      x = bfd_get_32 (input_bfd, contents);
      mas= (x>>18)&3;// mas stored in bits 19-18 of instruction
      x &= ~0x00000007;   	//clear old-mas-information 
      srel = (bfd_signed_vma) relocation;
      srel += rel->r_addend;
      // calculation of reloc done!
#ifdef SHOW_RELOC_DST
printf("\n ... R_XPIC_Y17 = %d (0x%x)\n",srel,srel);
#endif
      if( xpic_saturateY(&srel, mas) )
      {
#ifdef SHOW_RELOC_OOR
        printf("... OUT OF RANGE: R_XPIC_Y17 0x%08x\n",srel);
#endif
        return bfd_reloc_outofrange;
      }
      srel&=0x0001ffff;					// 17 bit mask
      bfd_put_32 (input_bfd, x | srel , contents);
      break;    }
      
case R_XPIC_Y17_PCREL:{
      unsigned int mas;
      contents   += rel->r_offset;
      x = bfd_get_32 (input_bfd, contents);
      mas= (x>>18)&3;// mas stored in bits 19-18 of instruction
      x &= ~0x00000007;   	//clear mas-information 
      srel = (bfd_signed_vma) relocation;
      srel += rel->r_addend;
      srel -= rel->r_offset;
      srel -= (input_section->output_section->vma + input_section->output_offset);
      // calculation of relac done!
#ifdef SHOW_RELOC_DST
printf("\n ... R_XPIC_Y17_PCREL = %d (0x%x)\n",srel,srel);
#endif
      if( xpic_saturateY(&srel, mas) )
      {
#ifdef SHOW_RELOC_OOR
        printf("... OUT OF RANGE: R_XPIC_Y17_PCREL 0x%08x\n",srel);
#endif
        return bfd_reloc_outofrange;
      }
      srel&=0x0001ffff;					// 17 bit mask
      bfd_put_32 (input_bfd, x | srel , contents);
      break;         }

case R_XPIC_Z19:{
      contents += rel->r_offset;
      srel = (bfd_signed_vma) relocation + rel->r_addend;
#ifdef SHOW_RELOC_DST
printf("\n ... R_XPIC_Z19 = %d (0x%x)\n",srel,srel);
#endif
      if(xpic_saturateZ(srel) )
      {
#ifdef SHOW_RELOC_OOR
        printf("... OUT OF RANGE: R_XPIC_Z19 0x%08x\n",srel);
#endif
        return bfd_reloc_outofrange;
      }
      srel&=0x0007ffff;					// 19 bit mask
      x = bfd_get_32 (input_bfd, contents);
      bfd_put_32 (input_bfd, x | srel , contents);
      break;     }

case R_XPIC_X15_PCREL:
      contents   += rel->r_offset;
      x = bfd_get_32 (input_bfd, contents);
      srel = (bfd_signed_vma) relocation;
      srel += rel->r_addend;
      srel -= rel->r_offset;
      srel -= (input_section->output_section->vma + input_section->output_offset);
      // calculation of relac done!
#ifdef SHOW_RELOC_DST
printf("\n ... R_XPIC_X15_PCREL = %d (0x%x)\n",srel,srel);
#endif
      if( xpic_saturateX(srel) )
      {
#ifdef SHOW_RELOC_OOR
        printf("... OUT OF RANGE: R_XPIC_X15_PCREL 0x%08x\n",srel);
#endif
        return bfd_reloc_outofrange;
      }

      srel>>=2;
      srel&=0x00007fff;					// 15 bit mask
      bfd_put_32 (input_bfd, x | srel , contents);
      break; 

    default:
      r = _bfd_final_link_relocate (howto, input_bfd, input_section,contents, rel->r_offset,relocation, rel->r_addend);
    }

  return r;
}

/* Relocate an AVR ELF section.  */
static bfd_boolean
elf32_xpic_relocate_section (bfd *output_bfd ATTRIBUTE_UNUSED,
			    struct bfd_link_info *info,
			    bfd *input_bfd,
			    asection *input_section,
			    bfd_byte *contents,
			    Elf_Internal_Rela *relocs,
			    Elf_Internal_Sym *local_syms,
			    asection **local_sections)
{
  Elf_Internal_Shdr *           symtab_hdr;
  struct elf_link_hash_entry ** sym_hashes;
  Elf_Internal_Rela *           rel;
  Elf_Internal_Rela *           relend;
  struct elf32_xpic_link_hash_table * htab = xpic_link_hash_table (info);

  symtab_hdr = & elf_tdata (input_bfd)->symtab_hdr;
  sym_hashes = elf_sym_hashes (input_bfd);
  relend     = relocs + input_section->reloc_count;

  for (rel = relocs; rel < relend; rel ++)
    {
      reloc_howto_type *           howto;
      unsigned long                r_symndx;
      Elf_Internal_Sym *           sym;
      asection *                   sec;
      struct elf_link_hash_entry * h;
      bfd_vma                      relocation;
      bfd_reloc_status_type        r;
      const char *                 name;
      int                          r_type;

      r_type = ELF32_R_TYPE (rel->r_info);
      r_symndx = ELF32_R_SYM (rel->r_info);
      howto  = elf_xpic_howto_table + ELF32_R_TYPE (rel->r_info);
      h      = NULL;
      sym    = NULL;
      sec    = NULL;

      if (r_symndx < symtab_hdr->sh_info)
	{
	  sym = local_syms + r_symndx;
	  sec = local_sections [r_symndx];
	  relocation = _bfd_elf_rela_local_sym (output_bfd, sym, &sec, rel);

	  name = bfd_elf_string_from_elf_section
	    (input_bfd, symtab_hdr->sh_link, sym->st_name);
	  name = (name == NULL) ? bfd_section_name (input_bfd, sec) : name;
	}
      else
	{
	  bfd_boolean unresolved_reloc, warned;
	  RELOC_FOR_GLOBAL_SYMBOL (info, input_bfd, input_section, rel,
				   r_symndx, symtab_hdr, sym_hashes,
				   h, sec, relocation,
				   unresolved_reloc, warned);

	  name = h->root.root.string;
	}

      if (sec != NULL && elf_discarded_section (sec))
	{
	  /* For relocs against symbols from removed linkonce sections,
	     or sections discarded by a linker script, we just want the
	     section contents zeroed.  Avoid any special processing.  */
	  _bfd_clear_contents (howto, input_bfd, contents + rel->r_offset);
	  rel->r_info = 0;
	  rel->r_addend = 0;
	  continue;
	}

      if (info->relocatable)
	continue;

      r = xpic_final_link_relocate (howto, input_bfd, input_section,
				   contents, rel, relocation, htab);

      if (r != bfd_reloc_ok)
	{
	  const char * msg = (const char *) NULL;
    
	  switch (r)
	    {
	    case bfd_reloc_overflow:
	      r = info->callbacks->reloc_overflow
		(info, (h ? &h->root : NULL),
		 name, howto->name, (bfd_vma) 0,
		 input_bfd, input_section, rel->r_offset);
	      break;

	    case bfd_reloc_undefined:
	      r = info->callbacks->undefined_symbol
		(info, name, input_bfd, input_section, rel->r_offset, TRUE);
	      break;

	    case bfd_reloc_outofrange:
	      msg = _("internal error: out of range error");
	      break;

	    case bfd_reloc_notsupported:
	      msg = _("internal error: unsupported relocation error");
	      break;

	    case bfd_reloc_dangerous:
	      msg = _("internal error: dangerous relocation");
	      break;

	    default:
	      msg = _("internal error: unknown error");
	      break;
	    }

	  if (msg)
	    r = info->callbacks->warning
	      (info, msg, name, input_bfd, input_section, rel->r_offset);

	  if (! r)
	    return FALSE;
	}
    }

  return TRUE;
}

/* The final processing done just before writing out a AVR ELF object
   file.  This gets the AVR architecture right based on the machine
   number.  */
static void bfd_elf_xpic_final_write_processing (bfd *abfd, bfd_boolean linker ATTRIBUTE_UNUSED)
{
  unsigned long val;

  switch (bfd_get_mach (abfd))
    {
    default:

    case bfd_mach_xpic:
      val = E_XPIC_MACH;
      break;
    }

  elf_elfheader (abfd)->e_machine = EM_XPIC;
  elf_elfheader (abfd)->e_flags &= ~ EF_XPIC_MACH;
  elf_elfheader (abfd)->e_flags |= val;
  elf_elfheader (abfd)->e_flags |= EF_AVR_LINKRELAX_PREPARED;
}

/* Set the right machine number.  */
static bfd_boolean elf32_xpic_object_p (bfd *abfd)
{
  unsigned int e_set = bfd_mach_xpic;

  if (elf_elfheader (abfd)->e_machine == EM_XPIC)
    {
      int e_mach = elf_elfheader (abfd)->e_flags & EF_XPIC_MACH;

      switch (e_mach)
	{
	default:
	case E_XPIC_MACH:
	  e_set = bfd_mach_xpic;
	  break;
	}
    }
  return bfd_default_set_arch_mach (abfd, bfd_arch_xpic,e_set);
}

#if 0
/* Delete some bytes from a section while changing the size of an instruction.
   The parameter "addr" denotes the section-relative offset pointing just
   behind the shrinked instruction. "addr+count" point at the first
   byte just behind the original unshrinked instruction.  */

static bfd_boolean
elf32_xpic_relax_delete_bytes (bfd *abfd,
                              asection *sec,
                              bfd_vma addr,
                              int count)
{
  Elf_Internal_Shdr *symtab_hdr;
  unsigned int sec_shndx;
  bfd_byte *contents;
  Elf_Internal_Rela *irel, *irelend;
  Elf_Internal_Rela *irelalign;
  Elf_Internal_Sym *isym;
  Elf_Internal_Sym *isymbuf = NULL;
  bfd_vma toaddr;
  struct elf_link_hash_entry **sym_hashes;
  struct elf_link_hash_entry **end_hashes;
  unsigned int symcount;

  symtab_hdr = &elf_tdata (abfd)->symtab_hdr;
  sec_shndx = _bfd_elf_section_from_bfd_section (abfd, sec);
  contents = elf_section_data (sec)->this_hdr.contents;

  /* The deletion must stop at the next ALIGN reloc for an aligment
     power larger than the number of bytes we are deleting.  */

  irelalign = NULL;
  toaddr = sec->size;

  irel = elf_section_data (sec)->relocs;
  irelend = irel + sec->reloc_count;

  /* Actually delete the bytes.  */
  if (toaddr - addr - count > 0)
    memmove (contents + addr, contents + addr + count,
             (size_t) (toaddr - addr - count));
  sec->size -= count;

  /* Adjust all the reloc addresses.  */
  for (irel = elf_section_data (sec)->relocs; irel < irelend; irel++)
    {
      bfd_vma old_reloc_address;
      bfd_vma shrinked_insn_address;

      old_reloc_address = (sec->output_section->vma
                           + sec->output_offset + irel->r_offset);
      shrinked_insn_address = (sec->output_section->vma
                              + sec->output_offset + addr - count);

      /* Get the new reloc address.  */
      if ((irel->r_offset > addr
           && irel->r_offset < toaddr))
        {
          if (debug_relax)
            printf ("Relocation at address 0x%x needs to be moved.\n"
                    "Old section offset: 0x%x, New section offset: 0x%x \n",
                    (unsigned int) old_reloc_address,
                    (unsigned int) irel->r_offset,
                    (unsigned int) ((irel->r_offset) - count));

          irel->r_offset -= count;
        }

    }

   /* The reloc's own addresses are now ok. However, we need to readjust
      the reloc's addend, i.e. the reloc's value if two conditions are met:
      1.) the reloc is relative to a symbol in this section that
          is located in front of the shrinked instruction
      2.) symbol plus addend end up behind the shrinked instruction.

      The most common case where this happens are relocs relative to
      the section-start symbol.

      This step needs to be done for all of the sections of the bfd.  */

  {
    struct bfd_section *isec;

    for (isec = abfd->sections; isec; isec = isec->next)
     {
       bfd_vma symval;
       bfd_vma shrinked_insn_address;

       shrinked_insn_address = (sec->output_section->vma
                                + sec->output_offset + addr - count);

       irelend = elf_section_data (isec)->relocs + isec->reloc_count;
       for (irel = elf_section_data (isec)->relocs;
            irel < irelend;
            irel++)
         {
           /* Read this BFD's local symbols if we haven't done
              so already.  */
           if (isymbuf == NULL && symtab_hdr->sh_info != 0)
             {
               isymbuf = (Elf_Internal_Sym *) symtab_hdr->contents;
               if (isymbuf == NULL)
                 isymbuf = bfd_elf_get_elf_syms (abfd, symtab_hdr,
                                                 symtab_hdr->sh_info, 0,
                                                 NULL, NULL, NULL);
               if (isymbuf == NULL)
                 return FALSE;
             }

           /* Get the value of the symbol referred to by the reloc.  */
           if (ELF32_R_SYM (irel->r_info) < symtab_hdr->sh_info)
             {
               /* A local symbol.  */
               Elf_Internal_Sym *isym;
               asection *sym_sec;

               isym = isymbuf + ELF32_R_SYM (irel->r_info);
               sym_sec = bfd_section_from_elf_index (abfd, isym->st_shndx);
               symval = isym->st_value;
               /* If the reloc is absolute, it will not have
                  a symbol or section associated with it.  */
               if (sym_sec == sec)
                 {
                   symval += sym_sec->output_section->vma
                             + sym_sec->output_offset;

                   if (debug_relax)
                     printf ("Checking if the relocation's "
                             "addend needs corrections.\n"
                             "Address of anchor symbol: 0x%x \n"
                             "Address of relocation target: 0x%x \n"
                             "Address of relaxed insn: 0x%x \n",
                             (unsigned int) symval,
                             (unsigned int) (symval + irel->r_addend),
                             (unsigned int) shrinked_insn_address);

                   if (symval <= shrinked_insn_address
                       && (symval + irel->r_addend) > shrinked_insn_address)
                     {
                       irel->r_addend -= count;

                       if (debug_relax)
                         printf ("Relocation's addend needed to be fixed \n");
                     }
                 }
	       /* else...Reference symbol is absolute.  No adjustment needed.  */
	     }
	   /* else...Reference symbol is extern.  No need for adjusting
	      the addend.  */
	 }
     }
  }

  /* Adjust the local symbols defined in this section.  */
  isym = (Elf_Internal_Sym *) symtab_hdr->contents;
  /* Fix PR 9841, there may be no local symbols.  */
  if (isym != NULL)
    {
      Elf_Internal_Sym *isymend;

      isymend = isym + symtab_hdr->sh_info;
      for (; isym < isymend; isym++)
	{
	  if (isym->st_shndx == sec_shndx
	      && isym->st_value > addr
	      && isym->st_value < toaddr)
	    isym->st_value -= count;
	}
    }

  /* Now adjust the global symbols defined in this section.  */
  symcount = (symtab_hdr->sh_size / sizeof (Elf32_External_Sym)
              - symtab_hdr->sh_info);
  sym_hashes = elf_sym_hashes (abfd);
  end_hashes = sym_hashes + symcount;
  for (; sym_hashes < end_hashes; sym_hashes++)
    {
      struct elf_link_hash_entry *sym_hash = *sym_hashes;
      if ((sym_hash->root.type == bfd_link_hash_defined
           || sym_hash->root.type == bfd_link_hash_defweak)
          && sym_hash->root.u.def.section == sec
          && sym_hash->root.u.def.value > addr
          && sym_hash->root.u.def.value < toaddr)
        {
          sym_hash->root.u.def.value -= count;
        }
    }

  return TRUE;
}
#endif
/// Construct new string and insert it to the string table
 int InsertStringToStrtbl(bfd *abfd,unsigned int shindex, char *name)
{
 int Name=0;
 Elf_Internal_Shdr *hdr;
 int p;
 char string[32];
 unsigned char *pC;
 static int Counter=0;
 // name:
 int Num = sprintf(string,".ld.addr.to.%s.%d",name,Counter++);
 // get string table header:
 if (elf_elfsections (abfd) == NULL || shindex >= elf_numsections (abfd))
   return -1;
 hdr = elf_elfsections (abfd)[shindex];
 if (hdr->contents == NULL )
    return -1;
 // get size
 p=hdr->sh_size;
 // and hold it to return as entry position in string table for new name 
 Name=p;
 // add one string
 hdr->sh_size+=Num+1;
 // alloc new memory
 pC=bfd_alloc(abfd,hdr->sh_size);
 if(pC==0) return -1;// no memory
 //copy old content
 memcpy(pC,hdr->contents,p);
 // save cache
 hdr->contents=pC;
 // pointer for new name:
 pC=hdr->contents+p;
 // copy new name:
 memcpy(pC,string,Num);
 // null at the end of string:
 pC[Num]=0;
 return Name;
}

/* Inserts some NOP instructions in a section.
   The parameter "addr" denotes the section-relative offset pointing just
   start new NOP instruction. "addr+count" point at the first
   byte just behind the new NOP instruction.  */

static bfd_boolean
elf32_xpic_relax_insert_instruction(bfd *abfd,
                              asection *sec,
                              bfd_vma addr,
                              unsigned int instruction){
  int count = 4;//number of bytes to insert
  Elf_Internal_Shdr *symtab_hdr;
  unsigned int sec_shndx;
  bfd_byte *contents=0;
  Elf_Internal_Rela *irel, *irelend;
  Elf_Internal_Rela *irelalign;
  Elf_Internal_Sym *isym;
  Elf_Internal_Sym *isymbuf = NULL;
  bfd_vma size;
  struct elf_link_hash_entry **sym_hashes;
  struct elf_link_hash_entry **end_hashes;
  unsigned int symcount;
  // Correct size of section
  symtab_hdr = &elf_tdata (abfd)->symtab_hdr;
  sec_shndx = _bfd_elf_section_from_bfd_section (abfd, sec);

  contents = elf_section_data (sec)->this_hdr.contents;
  /* The insrtion must stop at the next ALIGN reloc for an aligment
     power larger than the number of bytes we are deleting.  */

  irelalign = NULL;
  size = sec->size;
  irel = elf_section_data (sec)->relocs;
  irelend = irel + sec->reloc_count;

  /* Actually insert the bytes.  */
  if (size >= addr)
  {
    // correct section size
    sec->size += count;
    // realloc the memory (+4 bytes) 
    contents = (bfd_byte *)bfd_realloc(contents,sec->size);
    // save to header
    elf_section_data (sec)->this_hdr.contents = contents;
    // insert bytes
    memmove (contents + addr +4, contents + addr,(size_t) (size - addr ));
    // write required instruction
    bfd_put_32 (abfd, instruction , contents+addr);
  }
  else 
    return FALSE;

  /* Adjust all the reloc addresses.  */
  for (irel = elf_section_data (sec)->relocs; irel < irelend; irel++)
    {
      bfd_vma old_reloc_address;
      bfd_vma shrinked_insn_address;

      old_reloc_address = (sec->output_section->vma + sec->output_offset + irel->r_offset);
      shrinked_insn_address = (sec->output_section->vma  + sec->output_offset + addr + count);// - +

      /* Get the new reloc address.  */
      if ((irel->r_offset >= (addr) && irel->r_offset <= size))
        {
          if (debug_relax)
            printf ("Relocation at address 0x%x needs to be moved.\n"
                    "Old section offset: 0x%x, New section offset: 0x%x \n",
                    (unsigned int) old_reloc_address,
                    (unsigned int) irel->r_offset,
                    (unsigned int) ((irel->r_offset) + count));
          irel->r_offset += count;
        }
    }

   /* The reloc's own addresses are now ok. However, we need to readjust
      the reloc's addend, i.e. the reloc's value if two conditions are met:
      1.) the reloc is relative to a symbol in this section that
          is located in front of the shrinked instruction
      2.) symbol plus addend end up behind the shrinked instruction.

      The most common case where this happens are relocs relative to
      the section-start symbol.

      This step needs to be done for all of the sections of the bfd.  */

  {
    struct bfd_section *isec;

    for (isec = abfd->sections; isec; isec = isec->next)
     {
       bfd_vma symval;
       bfd_vma shrinked_insn_address;

       shrinked_insn_address = (sec->output_section->vma + sec->output_offset + addr);// + count);//-+

       irelend = elf_section_data (isec)->relocs + isec->reloc_count;
       for (irel = elf_section_data (isec)->relocs;
            irel < irelend;
            irel++)
         {
           /* Read this BFD's local symbols if we haven't done
              so already.  */
           if (isymbuf == NULL && symtab_hdr->sh_info != 0)
             {
               isymbuf = (Elf_Internal_Sym *) symtab_hdr->contents;
               if (isymbuf == NULL)
                 isymbuf = bfd_elf_get_elf_syms (abfd, symtab_hdr,
                                                 symtab_hdr->sh_info, 0,
                                                 NULL, NULL, NULL);
               if (isymbuf == NULL)
                 return FALSE;
             }

           /* Get the value of the symbol referred to by the reloc.  */
           if (ELF32_R_SYM (irel->r_info) < symtab_hdr->sh_info)
             {
               /* A local symbol.  */
               Elf_Internal_Sym *isym;
               asection *sym_sec;

               isym = isymbuf + ELF32_R_SYM (irel->r_info);
               sym_sec = bfd_section_from_elf_index (abfd, isym->st_shndx);
               symval = isym->st_value;
               /* If the reloc is absolute, it will not have
                  a symbol or section associated with it.  */
               if (sym_sec == sec)
                 {
                   symval += sym_sec->output_section->vma
                             + sym_sec->output_offset;

                   if (debug_relax)
                     printf ("Checking if the relocation's "
                             "addend needs corrections.\n"
                             "Address of anchor symbol: 0x%x \n"
                             "Address of relocation target: 0x%x \n"
                             "Address of relaxed insn: 0x%x \n",
                             (unsigned int) symval,
                             (unsigned int) (symval + irel->r_addend),
                             (unsigned int) shrinked_insn_address);

                   if (symval <= shrinked_insn_address && (symval + irel->r_addend) >= shrinked_insn_address)
                     {
                       irel->r_addend += count;//-+

                       if (debug_relax)
                         printf ("Relocation's addend needed to be fixed \n");
                     }
                 }
	       /* else...Reference symbol is absolute.  No adjustment needed.  */
	     }
	   /* else...Reference symbol is extern.  No need for adjusting
	      the addend.  */
	 }
     }
  }

  /* Adjust the local symbols defined in this section.  */
  isym = (Elf_Internal_Sym *) symtab_hdr->contents;
  /* Fix PR 9841, there may be no local symbols.  */
  if (isym != NULL)
    {
      Elf_Internal_Sym *isymend;

      isymend = isym + symtab_hdr->sh_info;
      for (; isym < isymend; isym++)
	{
	  if (isym->st_shndx == sec_shndx
	      && isym->st_value >= addr
	      && isym->st_value <= size)
	    isym->st_value += count;//-+
	}
    }

  /* Now adjust the global symbols defined in this section.  */
  symcount = (symtab_hdr->sh_size / sizeof (Elf32_External_Sym) - symtab_hdr->sh_info);
  sym_hashes = elf_sym_hashes (abfd);
  end_hashes = sym_hashes + symcount;

  for (; sym_hashes < end_hashes; sym_hashes++)
    {
      struct elf_link_hash_entry *sym_hash = *sym_hashes;
      if ((sym_hash->root.type == bfd_link_hash_defined
           || sym_hash->root.type == bfd_link_hash_defweak)
          && sym_hash->root.u.def.section == sec
          && sym_hash->root.u.def.value >= addr
          && sym_hash->root.u.def.value <= size)
        {
          sym_hash->root.u.def.value += count;//-+
        }
    }
  return TRUE;
}

// Get the value of the internal symbol referred to by the reloc.
// Internal symbol must be associated with .data section!
bfd_boolean xpic_get_data_internal_symbol_value(bfd *abfd,asection *data,Elf_Internal_Rela *irel,bfd_vma *symval)
{
  Elf_Internal_Shdr *symtab_hdr = &elf_tdata (abfd)->symtab_hdr;
  Elf_Internal_Sym *isymbuf  = (Elf_Internal_Sym *) symtab_hdr->contents;
  /* Get the value of the symbol referred to by the reloc.  */
  if (ELF32_R_SYM (irel->r_info) < symtab_hdr->sh_info)
  {
    // A local symbol.
    Elf_Internal_Sym *isym;
    // section header
    Elf_Internal_Shdr *hdr;
    // An associated index of .data section:
    unsigned indx_data_sect = _bfd_elf_section_from_bfd_section (abfd, data);
    // current symbol
    isym = isymbuf + ELF32_R_SYM (irel->r_info);
    // refers it to .data section?
    if(isym->st_shndx != indx_data_sect)
      return FALSE;//no, exit
    // info field must be the same,that was inserted in "xpic_insert_symbol_to_symtab()"
    if( isym->st_info != ELF32_ST_INFO(STB_LOCAL,STT_NOTYPE) )
      return FALSE;//not pass, exit

    // Compare first  4 chars of the name:
    // get string table header:
    hdr = elf_elfsections (abfd)[symtab_hdr->sh_link];
    // must be content in string section, and name inside this content
    if( hdr->contents == NULL   &&   isym->st_name < (hdr->sh_size-4) )
      return FALSE;// no content cached
    {// simple locale compare
      bfd_byte name[]=".ld.", res=0;
      bfd_byte *s = hdr->contents+isym->st_name;
      int i;
      for(i=0;i<4;i++)
        res |= s[i] - name[i];
      if(res)
        return FALSE;// not equal
    }
    // calculate symbol value:
    *symval = isym->st_value + data->output_section->vma + data->output_offset;
    // return with success
    return TRUE;
  }
  // External symbol, exit
  return FALSE;
}

// Get the value of the symbol referred to by the reloc.
bfd_boolean xpic_get_symbol_value(bfd *abfd,Elf_Internal_Rela *irel,bfd_vma *symval)
{
  Elf_Internal_Shdr *symtab_hdr = &elf_tdata (abfd)->symtab_hdr;
  Elf_Internal_Sym *isymbuf  = (Elf_Internal_Sym *) symtab_hdr->contents;
      /* Get the value of the symbol referred to by the reloc.  */
      if (ELF32_R_SYM (irel->r_info) < symtab_hdr->sh_info)
        {
          /* A local symbol.  */
          Elf_Internal_Sym *isym;
          asection *sym_sec;

          isym = isymbuf + ELF32_R_SYM (irel->r_info);
          sym_sec = bfd_section_from_elf_index (abfd, isym->st_shndx);
          *symval = isym->st_value;
          /* If the reloc is absolute, it will not have
             a symbol or section associated with it.  */
          if (sym_sec)
            *symval += sym_sec->output_section->vma
              + sym_sec->output_offset;
        }
      else
        {
          unsigned long indx;
          struct elf_link_hash_entry *h;

          /* An external symbol.  */
          indx = ELF32_R_SYM (irel->r_info) - symtab_hdr->sh_info;
          h = elf_sym_hashes (abfd)[indx];
          BFD_ASSERT (h != NULL);
          if (h->root.type != bfd_link_hash_defined
              && h->root.type != bfd_link_hash_defweak)
	    /* This appears to be a reference to an undefined
	       symbol.  Just ignore it--it will be caught by the
	       regular reloc processing.  */
	    return FALSE;

          *symval = (h->root.u.def.value
                    + h->root.u.def.section->output_section->vma
                    + h->root.u.def.section->output_offset);
        }
return TRUE;
}

// Get '.data' section, with cached data
asection *xpic_get_data_section_cached(bfd *abfd,struct bfd_link_info *link_info)
{
  Elf_Internal_Rela *data_internal_relocs;
  asection *data          = bfd_make_section_old_way (abfd,".data");
  bfd_byte *data_contents = elf_section_data (data)->this_hdr.contents;
  data->alignment_power=2;
  // Get the section contents
  if (data_contents == NULL){
   // Get cached copy if it exists
   if(elf_section_data (data)->this_hdr.contents != NULL)
    data_contents = elf_section_data (data)->this_hdr.contents;
   else{ // Go get them off disk.  
    if (! bfd_malloc_and_get_section (abfd, data, &data_contents))
      return NULL;
    // save to header
    elf_section_data (data)->this_hdr.contents = data_contents;
   }
  }
  // Cache relocation data inside this section, if not cached
  // get the section relocs
  if(elf_section_data (data)->relocs==NULL){
    // Get a copy of the native relocations.
    data_internal_relocs = (_bfd_elf_link_read_relocs(abfd, data, NULL, NULL, link_info->keep_memory));
    // save to header
    elf_section_data (data)->relocs = data_internal_relocs;
  }
  return data;
}

// Get internal relocation, that links to 'dst_address_of_cur_irel' as current relocation (cur_irel)
// from current section (cur_sec) but through '.data' section
Elf_Internal_Rela *xpic_get_ireloc_through_data(bfd *abfd,asection *cur_sec, Elf_Internal_Rela *cur_irel,bfd_vma dst_address_of_cur_irel,asection *data)
{
  int d;
  unsigned t;
  bfd_vma target_address,address_in_curfind_rel;
  // get relocs in data section:
  Elf_Internal_Rela *search_irel = elf_section_data (data)->relocs;
  // look in .data relocs
  for(d= data->reloc_count-1; d>=0; d--)
   // for identical with 'cur_irel' shift in target section, and link for the same symbol:
   if(search_irel[d].r_addend == cur_irel->r_addend &&
      ELF32_R_SYM(search_irel[d].r_info) == ELF32_R_SYM(cur_irel->r_info)){
    // found, get the value of this symbol
    if(!xpic_get_symbol_value(abfd,cur_irel,&target_address)) continue;
    // it must be oure target_address
    if(target_address==dst_address_of_cur_irel){
      // get relocation of current section:
      Elf_Internal_Rela *search_irel_curr = elf_section_data (cur_sec)->relocs;
      // and look in this relocs one
      for(t=0; t< cur_sec->reloc_count; t++){
       // that has symbol, wich value is the address, that holds 'target_address' in .data section
       //get value of symbol in cur_sec
       if( !xpic_get_data_internal_symbol_value(abfd,data,search_irel_curr+t, &address_in_curfind_rel) ) 
        continue;
       // compare it with an address, that holds 'target_address' in .data section 
       if(address_in_curfind_rel == (data->output_section->vma + data->output_offset + search_irel[d].r_offset ) )
        // oure allocation with symbol exists and was found
        return (search_irel_curr+t);
      }
    }
   }
 return NULL;// not found
}

// Insert new internal relocation at end of section data, that is links for a AddresS
bfd_boolean xpic_insert_ireloc(bfd *abfd, struct bfd_link_info *link_info, asection *data, bfd_vma r_info, bfd_vma r_addend)
{
  // hold number of relocations in section .data for next use
  int old_reloc_count = data->reloc_count;
  Elf_Internal_Rela *ireld,*data_irel_new;
  // we increase number of relocation
  data->reloc_count += 1;
  // in cache?
  if(link_info->keep_memory){
    data_irel_new = (Elf_Internal_Rela *)bfd_alloc(abfd, data->reloc_count* sizeof(Elf_Internal_Rela));
    if(data_irel_new==NULL){
      (*link_info->callbacks->einfo)(_("%P%F: memory allocation failed.\n"));
      return FALSE;
    }
    if(elf_section_data (data)->relocs!=0) 
      // was reloc-data, copy it
      memcpy(data_irel_new,elf_section_data (data)->relocs,(data->reloc_count-1)*sizeof(Elf_Internal_Rela));
    // save new pointer to reloc data:
    elf_section_data (data)->relocs=data_irel_new;
    // old value of 'elf_section_data (data)->relocs' will be deleted at delete of structure abfd
    // and its field 'memory'
  }
  else {
    // realloc and save new pointer to reloc data:
    elf_section_data (data)->relocs=bfd_realloc(elf_section_data (data)->relocs, data->reloc_count * sizeof(Elf_Internal_Rela));
  }
  // get pointer to relocations:
  ireld = elf_section_data (data)->relocs;
  // and to new relocations structure:
  ireld += old_reloc_count;
  // fill new relocations structure:
  ireld->r_info   = r_info;//ELF32_R_INFO (ELF32_R_SYM(irel->r_info),R_XPIC_32);//(symbol,reloc_type)
  ireld->r_addend = r_addend;//irel->r_addend;// offset in target section
  ireld->r_offset = data->size-4;// offset in actual section
  // now section data is relocable:
  data->flags |=SEC_RELOC;

  return TRUE;//success
}

// Insert to symbol table a new symbol, that associated with section (indx_assoc_section) and has value, and name
int xpic_insert_symbol_to_symtab(bfd *abfd, unsigned int indx_assoc_section, bfd_vma value, char *name)
{
  Elf_Internal_Shdr *symtab_hdr;
  Elf_Internal_Sym * isym;
  int Name;
  unsigned indx_symb;
  // === add a new symbol 
  symtab_hdr = &elf_tdata (abfd)->symtab_hdr;
  indx_symb=symtab_hdr->sh_info; // index of new symbol
  symtab_hdr->sh_info += 1;      // number of symbols one more
  symtab_hdr->sh_size += sizeof(Elf32_External_Sym);
  // cache it:
  symtab_hdr->contents =  bfd_realloc(symtab_hdr->contents,symtab_hdr->sh_info*sizeof(Elf_Internal_Sym));
  isym = (Elf_Internal_Sym *) symtab_hdr->contents;
  // Create new name for symbol:
  Name = InsertStringToStrtbl(abfd,symtab_hdr->sh_link,name);
  if(Name<0) return -1;
  // fill the symbol struct
  isym[indx_symb].st_value=value;		// Value of the symbol 
  isym[indx_symb].st_size=0;			// Associated symbol size 
  isym[indx_symb].st_name=Name;			// Symbol name, index in string tbl 
  isym[indx_symb].st_info=ELF32_ST_INFO(STB_LOCAL,STT_NOTYPE);// Type and binding attributes
  isym[indx_symb].st_other=STV_DEFAULT;		// Visibilty, and target specific 
  isym[indx_symb].st_shndx =indx_assoc_section; // Associated section index 
  // inserted!

  // fix external relocation's:
  Elf_Internal_Rela *irel_cur,*irel_end;
  asection *sect=abfd->sections;
  // walk over all sections, and correct relocations with global symbols:
  while(sect!=0){
    irel_cur = elf_section_data (sect)->relocs;
    irel_end = irel_cur + sect->reloc_count;
    for (; irel_cur < irel_end; irel_cur++){
      if(ELF32_R_SYM (irel_cur->r_info) < (symtab_hdr->sh_info-1))
        continue;// internal symbol
      // correct link to external symbol in reloc info, adding one to symbol part:
      irel_cur->r_info += 1<<8;
    }
    sect=sect->next;// go to next section
  }

  return indx_symb;// success
}

/* This function handles relaxing for the xpic.
   Here we try to replace: jmp   -> LongJmp
                           load RX, [pc + #off]  -> FarLoad
                           load RX, #const       -> FarLoad
                           store [pc + #off], RX -> FarStore
*/
static bfd_boolean
elf32_xpic_relax_section (bfd *abfd,
			 asection *sec,
                         struct bfd_link_info *link_info,
                         bfd_boolean *again)
{
  Elf_Internal_Shdr *symtab_hdr;
  Elf_Internal_Rela *internal_relocs;
  Elf_Internal_Rela *irel, *irelend;
  bfd_byte *contents = NULL;
  Elf_Internal_Sym *isymbuf = NULL;
  static asection *last_input_section = NULL;
  static Elf_Internal_Rela *last_reloc = NULL;
  struct elf32_xpic_link_hash_table *htab;
//debug_relax=TRUE; // serch: xpic_debug_relax

  if (link_info->relocatable)
    (*link_info->callbacks->einfo)
      (_("%P%F: --relax and -r may not be used together\n"));

  htab = xpic_link_hash_table (link_info);
  if (htab == NULL)
    return FALSE;

  /* Assume nothing changes.  */
  *again = FALSE;

  if ((!htab->no_stubs) && (sec == htab->stub_sec))
    {
      /* We are just relaxing the stub section.
	 Let's calculate the size needed again.  */
      bfd_size_type last_estimated_stub_section_size = htab->stub_sec->size;

      if (debug_relax)
        printf ("Relaxing the stub section. Size prior to this pass: %i\n",
                (int) last_estimated_stub_section_size);
      elf32_xpic_size_stubs (htab->stub_sec->output_section->owner,link_info, FALSE);
      /* Check if the number of trampolines changed.  */
      if (last_estimated_stub_section_size != htab->stub_sec->size)
        *again = TRUE;
      if (debug_relax)
        printf ("Size of stub section after this pass: %i\n",(int) htab->stub_sec->size);

      return TRUE;
    }

  /* We don't have to do anything for a relocatable link, if
     this section does not have relocs, or if this is not a
     code section.  */
  if (link_info->relocatable
      || (sec->flags & SEC_RELOC) == 0
      || sec->reloc_count == 0
      || (sec->flags & SEC_CODE) == 0)
    return TRUE;

  /* Check if the object file to relax uses internal symbols so that we
     could fix up the relocations.  */
  if (!(elf_elfheader (abfd)->e_flags & EF_AVR_LINKRELAX_PREPARED))
    return TRUE;

  symtab_hdr = &elf_tdata (abfd)->symtab_hdr;

  /* Get a copy of the native relocations.  */
  internal_relocs = (_bfd_elf_link_read_relocs(abfd, sec, NULL, NULL, link_info->keep_memory));

  if (internal_relocs == NULL)
    goto error_return;
  // save to header
  elf_section_data (sec)->relocs = internal_relocs;

  if (sec != last_input_section)
    last_reloc = NULL;

  last_input_section = sec;


  /* Walk through the relocs looking for relaxing opportunities.  */
  irelend = internal_relocs + sec->reloc_count;
  for (irel = internal_relocs; irel < irelend; irel++)
    {
      bfd_vma symval;

      if (  ELF32_R_TYPE (irel->r_info) != R_XPIC_Y17_PCREL
         && ELF32_R_TYPE (irel->r_info) != R_XPIC_24_PCREL
         && ELF32_R_TYPE (irel->r_info) != R_XPIC_Z19
	)
        continue;

      /* Get the section contents if we haven't done so already.  */
      contents = elf_section_data (sec)->this_hdr.contents;
      if (contents == NULL)
        {
          /* Get cached copy if it exists.  */
          if (elf_section_data (sec)->this_hdr.contents != NULL)
            contents = elf_section_data (sec)->this_hdr.contents;
          else
            {
              /* Go get them off disk.  */
              if (! bfd_malloc_and_get_section (abfd, sec, &contents))
                goto error_return;
              elf_section_data (sec)->this_hdr.contents = contents;
            }
        }

      isymbuf = (Elf_Internal_Sym *) symtab_hdr->contents;
      /* Read this BFD's local symbols if we haven't done so already.  */
      if (isymbuf == NULL && symtab_hdr->sh_info != 0)
        {
          isymbuf = (Elf_Internal_Sym *) symtab_hdr->contents;
          if (isymbuf == NULL)
            isymbuf = bfd_elf_get_elf_syms (abfd, symtab_hdr,
                                            symtab_hdr->sh_info, 0,
                                            NULL, NULL, NULL);
          if (isymbuf == NULL)
            goto error_return;
          // save to header
	  symtab_hdr->contents = (unsigned char *)isymbuf;
        }

     // Get the value of the symbol referred to by the reloc.
      if(!xpic_get_symbol_value(abfd,irel,&symval)) continue;

      /* For simplicity of coding, we are going to modify the section
         contents, the section relocs, and the BFD symbol table.  We
         must tell the rest of the code not to free up this
         information.  It would be possible to instead create a table
         of changes which have to be made, as is done in coff-mips.c;
         that would be more work, but would require less memory when
         the linker is run.  */

      switch (ELF32_R_TYPE (irel->r_info))
	{
	 case R_XPIC_24_PCREL:
	 {
	    //is it CALL instruction sequence?
	    if(bfd_get_32(abfd, contents + irel->r_offset-4)!=XPIC_INS_STORE_dSP_PC)
	       break; // it is not CALL, it's asimple jmp. exit
/**     This is all for CALL instruction sequence!!!
To analyse: JUMP instruction: jmp #offset, where #offset is pc relative in +-32MB
1 If requied address is accessible with 'jmp' inside 32BM, do nothing, return.
2 Else, if target #address_to_jump (where #address_to_jump = pc + #offset) fit in 18 bit, 
  then use direct this #address_to_jump in instruction 'load'. In this case target 
  address is inside INT_PRAM of xPIC. Construct following sequence of instructions:
      add r6, #4, pc
      load pc, #address_to_jump
      store [z0 + --r7], r6
   -> here will be return address
3 Else construct long-jump. 
3.1 If we are not in INT_PRAM (address of current instruction >= 0x2000), then construct 
    following sequence of instructions:
      .text (not INT_PRAM)
      add r6, #8, pc
      load pc, [pc + #8]    -------
      store [z0 + --r7], r6       |
      #address_to_jump      <------
   -> here will be return address
3.2 If we are in INT_PRAM (address of current instruction < 0x2000), then construct 
    following sequence of instructions:
     .text (In INT_PRAM)
      add r6, #4, pc
      load pc, [pc + #offset_to stored_address_to_jump]
      store [z0 + --r7], r6
   -> here will be return address
........................................................
     .data (In INT_DRAM)
stored_address_to_jump:
      $address_to_jump
**/
	    bfd_vma address_to_jump = symval + irel->r_addend;
	    bfd_vma address_of_this_instr;
	    bfd_signed_vma distance;
	    // Get the address of current instruction:
	    address_of_this_instr = (sec->output_section->vma + sec->output_offset + irel->r_offset);
	    // Calculateo the distance (#offset) from this insn to the target address:
	    distance = address_to_jump - address_of_this_instr;

#ifdef SHOW_RELOC_R_XPIC_Z19
        printf("... RELOC_R_XPIC_Z19: addr_my=0x%08x, addr_jmp=0x%08x\n",address_of_this_instr,address_to_jump);
#endif
	    /// 1 Is address is inside +-32BM and accessible with 'jmp'?
	    if( xpic_saturateJMP(distance) == 0 ) 
	      break; // exit switch

	    /// Address is not accessible with 'jmp'
            
#ifndef NO_Z19_ABSOLUTE_JUMP_RELAX
	    /// 2 Is it possible to load addres to jump, direct with instruction 'load pc, #address_to_jump'?
	    if( xpic_saturateZ(address_to_jump) == 0 )
	    {  // yes
	      // set last instruction 'add r6, #4, pc'
	      bfd_put_32 (abfd, XPIC_INS_R6_ADD_R6_4_PC, contents+irel->r_offset-4);
	      // replace current instruction 'jmp' with 'load pc, (relocation R_XPIC_Z19)'
	      bfd_put_32 (abfd, XPIC_INS_LOAD_PC_0, contents+irel->r_offset);
	      // and insert "store [z0 + --r7], r6" instruction
	      if(!elf32_xpic_relax_insert_instruction (abfd, sec, irel->r_offset+4, XPIC_INS_STORE_dSP_R6))
	        goto error_return;// error
	      // fix the relocation's type
	      irel->r_info = ELF32_R_INFO (ELF32_R_SYM (irel->r_info),R_XPIC_Z19);
	      break; // exit switch
	    }
#endif

	    {
	    /// Impossible to load address direct from instruction!
	    /// 3 Construct long jump
	    asection *data=NULL;
	    bfd_vma  addr_of_stored_addr_to_jump;

	    if(address_of_this_instr >= XPIC_MEM_INTPRAM_LEN){
	      /// 3.1 We are not in INT_PRAM
	      // set last instruction 'add r6, #8, pc'
	      bfd_put_32 (abfd, XPIC_INS_R6_ADD_R6_8_PC, contents+irel->r_offset-4);
	      // replace current instruction 'jmp' with 'load pc, [pc + #8]'
	      bfd_put_32 (abfd, XPIC_INS_LOAD_PC_MPC_8, contents+irel->r_offset);
	      // and insert "store [z0 + --r7], r6" instruction
	      if(!elf32_xpic_relax_insert_instruction (abfd, sec, irel->r_offset+4, XPIC_INS_STORE_dSP_R6))
	        goto error_return;// error
	      // insert saved address to jump:
	      if(!elf32_xpic_relax_insert_instruction (abfd, sec, irel->r_offset+8, 0/*address_to_jump*/))
	        goto error_return;// error
	       // fix the relocation's type
	       irel->r_info = ELF32_R_INFO (ELF32_R_SYM (irel->r_info),R_XPIC_32);
	       irel->r_offset = irel->r_offset+8;// offset in actual section
	       break; // exit switch
	    }
	    /// 3.2 We are in INT_PRAM
	    /// set LAST instruction 'add r6, #4, pc'
	    bfd_put_32 (abfd, XPIC_INS_R6_ADD_R6_4_PC, contents+irel->r_offset-4);
	    /// replace CURRENT instruction 'jmp' with 'load pc,  [pc + #offset]d'
	    bfd_put_32 (abfd, XPIC_INS_LOAD_PC_MPC_OFF, contents+irel->r_offset);
	    /// and INSERT "store [z0 + --r7], r6" instruction
	    if(!elf32_xpic_relax_insert_instruction (abfd, sec, irel->r_offset+4, XPIC_INS_STORE_dSP_R6))
	      goto error_return;// error

	    // Get '.data' section
            data= xpic_get_data_section_cached(abfd,link_info);
	    if(data==NULL)
	        goto error_return;// error

	    /// find saved in '.data' section address-to-jump. It can be inserted already by another longjump
	    /// or another instruction
	    Elf_Internal_Rela *search_irel_curr=xpic_get_ireloc_through_data(abfd,sec,irel,address_to_jump,data);
	    if(search_irel_curr != NULL){// it exists!
	      // fix the relocation's type 
	      irel->r_info   = search_irel_curr->r_info;
	      break;// exit switch
	    }

	    addr_of_stored_addr_to_jump = data->size;
	    /// Insertion at the end of '.data' section. It will be AddresS that stores value of callee address
	    if(!elf32_xpic_relax_insert_instruction (abfd, data, data->size, 0))
	      goto error_return;
	    /// Now new data added.
	    /// ADD NEW relocation in data section (a) to link for an AddresS, add new Symbol assigned to AddresS (b)
	    /// and name (c) for it.

	    /// (a) Add new Relocation, that is needed to link for a AddresS 
	    if(!xpic_insert_ireloc(abfd,link_info,data, ELF32_R_INFO (ELF32_R_SYM(irel->r_info),R_XPIC_32), irel->r_addend))
	      goto error_return;

	    /// (b)Assign to new data a symbol
	    // Associated index of .data section:
	    unsigned indx_data_sect = _bfd_elf_section_from_bfd_section (abfd, data);
	    int indx_new_symb=xpic_insert_symbol_to_symtab(abfd, indx_data_sect, addr_of_stored_addr_to_jump,"jump");
	    if( indx_new_symb <0 )
	      goto error_return;

            // fix the relocation's type 
            irel->r_info   = ELF32_R_INFO (indx_new_symb,R_XPIC_Y17_PCREL);
	    irel->r_addend = 0;
	    }
	    break; // exit switch
          }
         case R_XPIC_Y17_PCREL:
          {
/** THIS  RELOCATION  INCLUDES  'LOAD' and 'STORE' instructions!!!
To analyse:
==Far load instruction: load rx, s[pc + #offset]b. Here pc is address of current instruction.
  1: If #offset not more than Y constant (16/17/18bit depend from MAS), do nothing, return.
  2: Else: loadable data can be inside INT_DRAM (starts from 0), and if (#offset+pc) not more 
     than volume of INT_DRAM (8KB), use load rx, s[z0 + (#offset+pc)]b
  3: Loadable data not accessable with standard load. Construct Load via .data section:
      .section .text
      load rx, [pc + #offset_to stored_address_to_load]d
      nop
      load rx, s[rx + #0]b
........................................................
     .section .data
stored_address_to_load:
      $address_to_load

==Far store instruction: store s[pc + #offset]b, rx
  1: the same as in load
  2: the same as in load
  3: Addres to store not accessable with standard store. Construct Store via .data section:
      .section .text
      load r?, [pc + #offset_to stored_address_to_store]d
      nop
      store s[rx + #0]b, r?
........................................................
     .section .data
stored_address_to_store:
      $address_to_store
**/
	    unsigned int code_instr,code_new_instr,mas,opcode;
	    bfd_signed_vma address_to_load = symval + irel->r_addend;
	    bfd_signed_vma distance;
	    // Get the address of current instruction:
	    bfd_vma address_of_this_instr;
	    address_of_this_instr = (sec->output_section->vma + sec->output_offset + irel->r_offset);
	    // Compute the distance (#offset) from this insn to the target address
	    distance = address_to_load - address_of_this_instr;
	    // get current instruction code
	    code_instr = bfd_get_32(abfd, contents + irel->r_offset);
	    // get memory access size from instruction:
	    mas = XPIC_GET_MAS(code_instr);
	    // check alignment, of load address
	    unsigned align;
	    align = address_to_load&0x00000003&((1<<mas)-1);
	    if(align){
	      (*link_info->callbacks->einfo)(_("%P%F: disalligned memory access.\n"));
	      goto error_return;// error, disallignment
	    }
	    /// 1. Is offset for standard load valid?
	    if( xpic_saturateYv2(distance, mas)==0 )
	      break;// yes, use standard load
        
#ifndef NO_Y17_PCREL_TO_ABSOLUTE_RELAX
// FIXME: BI: Y17 relax does not work when target symbol is not yet located
	    /// 2. Is loadable data inside INT_DRAM?
	    if( xpic_saturateYv2(address_to_load, mas)==0 )
            {// YES, offset inside INT_DRAM!
	      // replace with new instruction (simple replace pc with z0)
	      code_new_instr = (code_instr & 0xfe1fffff)|(0x01e00000);
	      bfd_put_32(abfd, code_new_instr , contents + irel->r_offset);
	      // Fix the relocation's type
	      irel->r_info = ELF32_R_INFO (ELF32_R_SYM (irel->r_info),R_XPIC_Y17);
	      break;
	    }
#endif

	    /// 3. Loadable data is to far for load/store, construct FarLoad/FarStore via .data section
	    // construct sequence for load:
	    //  load rx, [pc + #offset_to stored_address_to_load]
	    //  nop
	    //  load rx, s[rx + #0]b
	    // or sequence for store:
	    //  load r?, [pc + #offset_to stored_address_to_load]
	    //  nop
	    //  store s[rx + #0]b, r?
	    // At first (I.) correct instruction and relocation, second (II.) - nop (data dependence) and third (III.) - load data from pointer/store data to pointer
	    // Get opcode load or store:
	    opcode = code_instr&0xe0000000;
	    /// I. replace CURRENT instruction 'load reg, s[pc + #off]b' with 'load pc, [pc + #off_stored_addr_to_load]d'
	    // set: mas=b10(4byte) and sign=0 (unsigned)
	    code_new_instr=(code_instr & (~0x000e0000))|0x00080000;
	    if( opcode == 0xa0000000 ){
	    // store instruction
	    // set more: opcode=100, dst_reg = XPIC_FARSTORE_REG
	      code_new_instr= (code_new_instr & 0x01fffFFF) | 0x80000000 | (XPIC_FARSTORE_REG<<25);
	    }
	    // update CURRENT instruction:
	    bfd_put_32(abfd, code_new_instr , contents + irel->r_offset);
	    //work for relocation: in data section must be placed address to load, and link (relocation) to it
	    asection *data=NULL;
	    bfd_vma  addr_of_stored_addr_to_load;
	    // Get '.data' section
            data= xpic_get_data_section_cached(abfd,link_info);
	    if(data==NULL)
	        goto error_return;// error

	    /// find saved in '.data' section address-to-load. It can be inserted already by another load,
	    ///  longjump or another instruction
	    Elf_Internal_Rela *search_irel_curr=xpic_get_ireloc_through_data(abfd,sec,irel,address_to_load, data);
	    if(search_irel_curr != NULL){// it exists!
	      // fix the relocation's type 
	      irel->r_info   = search_irel_curr->r_info;
	      goto add_nop_load;// exit switch, but add two last instructions
	    }
	    addr_of_stored_addr_to_load = data->size;
	    /// Insertion at the end of '.data' section. It will be AddresS that stores value of callee address
	    if(!elf32_xpic_relax_insert_instruction (abfd, data, data->size, 0))
	      goto error_return;
	    /// Now new data added.
	    /// ADD NEW relocation in data section (a) to link for an AddresS, add new Symbol assigned to AddresS (b)
	    /// and name (c) for it.

	    /// (a) Add new Relocation, that is needed to link for a AddresS 
	    if(!xpic_insert_ireloc(abfd,link_info,data, ELF32_R_INFO (ELF32_R_SYM(irel->r_info),R_XPIC_32), irel->r_addend))
	      goto error_return;

	    /// (b)Assign to new data a symbol
	    // Associated index of .data section:
	    unsigned indx_data_sect = _bfd_elf_section_from_bfd_section (abfd, data);
	    int indx_new_symb=xpic_insert_symbol_to_symtab(abfd, indx_data_sect, addr_of_stored_addr_to_load,"load");
	    if( indx_new_symb <0 )
	      goto error_return;

            // fix the relocation's type 
            irel->r_info   = ELF32_R_INFO (indx_new_symb,R_XPIC_Y17_PCREL);
	    irel->r_addend = 0;

add_nop_load:
	    // II. Insert next, instruction "nop"
	    if(!elf32_xpic_relax_insert_instruction (abfd, sec, irel->r_offset+4, XPIC_INS_NOP))
	        goto error_return;// error
	    // III. Insert after next, instruction to load from pointer rx, in rx: load rx, s[rx + #0]b
	    // In 'code_instr' are saved destination register 'reg', mas, sign. Use it next, but replace 'base_reg' (mask: 0xfe1fffff) with current 'reg' and set offset 'const' null (mask: 0xfffe0000):
	    code_new_instr = code_instr &( 0xfe1fffff & 0xfffe0000 ); // set masks
	    int reg = (code_instr>>4)&0x01e00000;		    // shift 'reg' to 'base_reg'
	    // it can be 'store':
	    if( opcode == 0xa0000000 ){
	    // correction for store instruction
	    // set more: base_reg = XPIC_FARSTORE_REG
	      reg=  (XPIC_FARSTORE_REG<<21);
	    }
	    // set register to new instruction and add this instruction te section:
	    code_new_instr |= reg;
	    if(!elf32_xpic_relax_insert_instruction (abfd, sec, irel->r_offset+8, code_new_instr))
	        goto error_return;// error
	  break;
	  }
         case R_XPIC_Z19:
          {
/**
To analyse: Far load instruction: load rx, #const.
  1: If #const not more than Z constant (18bit + sign bit), do nothing, return.
  2: Else save this #const to .data section, and load with 'load rx, [pc + #off_to stored_address_to_load]':
      .section .text
      load rx, [pc + #offset_to stored_address_to_load]d
.......................................................
     .section .data
stored_address_to_load:
      $address_to_load
**/
	    unsigned int code_instr,code_new_instr;
	    bfd_signed_vma address_to_load = symval + irel->r_addend;

#ifndef FORCE_Z19_TO_FARLOAD_RELAX
// FIXME: BI: 
	    /// 1. Is offset for standard load valid?
	    if(xpic_saturateZ(address_to_load)==0)
	      break;// yes, use standard load
#endif

	    /// 2. Loadable data is to far for load, construct FarLoad via .data section
	    // construct instruction:
	    //   load rx, [pc + #offset_to stored_address_to_load]
	    // from loar rx, #const
	    // Get current instruction code:
	    code_instr  = bfd_get_32(abfd, contents + irel->r_offset);
	    // get destination register rx: 
	    int rx = code_instr & 0x1e000000;
	    code_new_instr = XPIC_INS_LOAD_RX_MPC_OFF | rx;
	    bfd_put_32(abfd, code_new_instr , contents + irel->r_offset);
	    //work for relocation: in data section must be placed address to load, and link (relocation) to it
	    asection *data=NULL;
	    bfd_vma  addr_of_stored_addr_to_load;
	    // Get '.data' section
            data= xpic_get_data_section_cached(abfd,link_info);
	    if(data==NULL)
	        goto error_return;// error

	    /// find saved in '.data' section address-to-load. It can be inserted already by another load,
	    ///  longjump or another instruction
	    Elf_Internal_Rela *search_irel_curr=xpic_get_ireloc_through_data(abfd,sec,irel,address_to_load, data);
	    if(search_irel_curr != NULL){// it exists!
	      // fix the relocation's type 
	      irel->r_info   = search_irel_curr->r_info;
	      break;// exit switch
	    }
	    addr_of_stored_addr_to_load = data->size;
	    /// Insertion at the end of '.data' section. It will be AddresS that stores value of callee address
	    if(!elf32_xpic_relax_insert_instruction (abfd, data, data->size, 0))
	      goto error_return;
	    /// Now new data added.
	    /// ADD NEW relocation in data section (a) to link for an AddresS, add new Symbol assigned to AddresS (b)
	    /// and name (c) for it.

	    /// (a) Add new Relocation, that is needed to link for a AddresS 
	    if(!xpic_insert_ireloc(abfd,link_info,data, ELF32_R_INFO (ELF32_R_SYM(irel->r_info),R_XPIC_32), irel->r_addend))
	      goto error_return;

	    /// (b)Assign to new data a symbol
	    // Associated index of .data section:
	    unsigned indx_data_sect = _bfd_elf_section_from_bfd_section (abfd, data);
	    int indx_new_symb=xpic_insert_symbol_to_symtab(abfd, indx_data_sect, addr_of_stored_addr_to_load,"load");
	    if( indx_new_symb <0 )
	      goto error_return;

            // fix the relocation's type 
            irel->r_info   = ELF32_R_INFO (indx_new_symb,R_XPIC_Y17_PCREL);
	    irel->r_addend = 0;

	 break;
	 }
	default:break;
	}
    }

  if (contents != NULL && elf_section_data (sec)->this_hdr.contents != contents)
      if (! link_info->keep_memory)
        free (contents);

  if (internal_relocs != NULL && elf_section_data (sec)->relocs != internal_relocs)
      if (! link_info->keep_memory)
        free (internal_relocs);

  return TRUE;

 error_return:
  if (isymbuf != NULL
      && symtab_hdr->contents != (unsigned char *) isymbuf)
    free (isymbuf);
  if (contents != NULL
      && elf_section_data (sec)->this_hdr.contents != contents)
    free (contents);
  if (internal_relocs != NULL
      && elf_section_data (sec)->relocs != internal_relocs)
    free (internal_relocs);

  return FALSE;
}

/* This is a version of bfd_generic_get_relocated_section_contents
   which uses elf32_xpic_relocate_section.

   For xpic it's essentially a cut and paste taken from the H8300 port.
   The author of the relaxation support patch for xpic had absolutely no
   clue what is happening here but found out that this part of the code
   seems to be important.  */

static bfd_byte *
elf32_xpic_get_relocated_section_contents (bfd *output_bfd,
                                          struct bfd_link_info *link_info,
                                          struct bfd_link_order *link_order,
                                          bfd_byte *data,
                                          bfd_boolean relocatable,
                                          asymbol **symbols)
{
  Elf_Internal_Shdr *symtab_hdr;
  asection *input_section = link_order->u.indirect.section;
  bfd *input_bfd = input_section->owner;
  asection **sections = NULL;
  Elf_Internal_Rela *internal_relocs = NULL;
  Elf_Internal_Sym *isymbuf = NULL;

  /* We only need to handle the case of relaxing, or of having a
     particular set of section contents, specially.  */
  if (relocatable
      || elf_section_data (input_section)->this_hdr.contents == NULL)
    return bfd_generic_get_relocated_section_contents (output_bfd, link_info,
                                                       link_order, data,
                                                       relocatable,
                                                       symbols);
  symtab_hdr = &elf_tdata (input_bfd)->symtab_hdr;

  memcpy (data, elf_section_data (input_section)->this_hdr.contents,
          (size_t) input_section->size);

  if ((input_section->flags & SEC_RELOC) != 0
      && input_section->reloc_count > 0)
    {
      asection **secpp;
      Elf_Internal_Sym *isym, *isymend;
      bfd_size_type amt;

      internal_relocs = (_bfd_elf_link_read_relocs
                         (input_bfd, input_section, NULL, NULL, FALSE));
      if (internal_relocs == NULL)
        goto error_return;

      if (symtab_hdr->sh_info != 0)
        {
          isymbuf = (Elf_Internal_Sym *) symtab_hdr->contents;
          if (isymbuf == NULL)
            isymbuf = bfd_elf_get_elf_syms (input_bfd, symtab_hdr,
                                            symtab_hdr->sh_info, 0,
                                            NULL, NULL, NULL);
          if (isymbuf == NULL)
            goto error_return;
        }

      amt = symtab_hdr->sh_info;
      amt *= sizeof (asection *);
      sections = bfd_malloc (amt);
      if (sections == NULL && amt != 0)
        goto error_return;

      isymend = isymbuf + symtab_hdr->sh_info;
      for (isym = isymbuf, secpp = sections; isym < isymend; ++isym, ++secpp)
        {
          asection *isec;

          if (isym->st_shndx == SHN_UNDEF)
            isec = bfd_und_section_ptr;
          else if (isym->st_shndx == SHN_ABS)
            isec = bfd_abs_section_ptr;
          else if (isym->st_shndx == SHN_COMMON)
            isec = bfd_com_section_ptr;
          else
            isec = bfd_section_from_elf_index (input_bfd, isym->st_shndx);

          *secpp = isec;
        }

      if (! elf32_xpic_relocate_section (output_bfd, link_info, input_bfd,
                                        input_section, data, internal_relocs,
                                        isymbuf, sections))
        goto error_return;

      if (sections != NULL)
        free (sections);
      if (isymbuf != NULL
          && symtab_hdr->contents != (unsigned char *) isymbuf)
        free (isymbuf);
      if (elf_section_data (input_section)->relocs != internal_relocs)
        free (internal_relocs);
    }

  return data;

 error_return:
  if (sections != NULL)
    free (sections);
  if (isymbuf != NULL
      && symtab_hdr->contents != (unsigned char *) isymbuf)
    free (isymbuf);
  if (internal_relocs != NULL
      && elf_section_data (input_section)->relocs != internal_relocs)
    free (internal_relocs);
  return NULL;
}


/* Determines the hash entry name for a particular reloc. It consists of
   the identifier of the symbol section and the added reloc addend and
   symbol offset relative to the section the symbol is attached to.  */

static char *
xpic_stub_name (const asection *symbol_section,
               const bfd_vma symbol_offset,
               const Elf_Internal_Rela *rela)
{
  char *stub_name;
  bfd_size_type len;

  len = 8 + 1 + 8 + 1 + 1;
  stub_name = bfd_malloc (len);

  sprintf (stub_name, "%08x+%08x",
           symbol_section->id & 0xffffffff,
           (unsigned int) ((rela->r_addend & 0xffffffff) + symbol_offset));

  return stub_name;
}


/* Add a new stub entry to the stub hash.  Not all fields of the new
   stub entry are initialised.  */

static struct elf32_xpic_stub_hash_entry *
xpic_add_stub (const char *stub_name,
              struct elf32_xpic_link_hash_table *htab)
{
  struct elf32_xpic_stub_hash_entry *hsh;

  /* Enter this entry into the linker stub hash table.  */
  hsh = xpic_stub_hash_lookup (&htab->bstab, stub_name, TRUE, FALSE);

  if (hsh == NULL)
    {
      (*_bfd_error_handler) (_("%B: cannot create stub entry %s"),
                             NULL, stub_name);
      return NULL;
    }

  hsh->stub_offset = 0;
  return hsh;
}

/* We assume that there is already space allocated for the stub section
   contents and that before building the stubs the section size is
   initialized to 0.  We assume that within the stub hash table entry,
   the absolute position of the jmp target has been written in the
   target_value field.  We write here the offset of the generated jmp insn
   relative to the trampoline section start to the stub_offset entry in
   the stub hash table entry.  */

static  bfd_boolean
xpic_build_one_stub (struct bfd_hash_entry *bh, void *in_arg)
{
  struct elf32_xpic_stub_hash_entry *hsh;
  struct bfd_link_info *info;
  struct elf32_xpic_link_hash_table *htab;
  bfd *stub_bfd;
  bfd_byte *loc;
  bfd_vma target;
  bfd_vma starget;

  /* Basic opcode */
  bfd_vma jmp_insn = 0x0000940c;

  /* Massage our args to the form they really have.  */
  hsh = xpic_stub_hash_entry (bh);

  if (!hsh->is_actually_needed)
    return TRUE;

  info = (struct bfd_link_info *) in_arg;

  htab = xpic_link_hash_table (info);
  if (htab == NULL)
    return FALSE;

  target = hsh->target_value;

  /* Make a note of the offset within the stubs for this entry.  */
  hsh->stub_offset = htab->stub_sec->size;
  loc = htab->stub_sec->contents + hsh->stub_offset;

  stub_bfd = htab->stub_sec->owner;

  if (debug_stubs)
    printf ("Building one Stub. Address: 0x%x, Offset: 0x%x\n",
             (unsigned int) target,
             (unsigned int) hsh->stub_offset);

  /* We now have to add the information on the jump target to the bare
     opcode bits already set in jmp_insn.  */

  /* Check for the alignment of the address.  */
  if (target & 1)
     return FALSE;

  starget = target >> 1;
  jmp_insn |= ((starget & 0x10000) | ((starget << 3) & 0x1f00000)) >> 16;
  bfd_put_16 (stub_bfd, jmp_insn, loc);
  bfd_put_16 (stub_bfd, (bfd_vma) starget & 0xffff, loc + 2);

  htab->stub_sec->size += 4;

  /* Now add the entries in the address mapping table if there is still
     space left.  */
  {
    unsigned int nr;

    nr = htab->amt_entry_cnt + 1;
    if (nr <= htab->amt_max_entry_cnt)
      {
        htab->amt_entry_cnt = nr;

        htab->amt_stub_offsets[nr - 1] = hsh->stub_offset;
        htab->amt_destination_addr[nr - 1] = target;
      }
  }

  return TRUE;
}

static bfd_boolean
xpic_mark_stub_not_to_be_necessary (struct bfd_hash_entry *bh,
                                   void *in_arg)
{
  struct elf32_xpic_stub_hash_entry *hsh;
  struct elf32_xpic_link_hash_table *htab;

  htab = in_arg;
  hsh = xpic_stub_hash_entry (bh);
  hsh->is_actually_needed = FALSE;

  return TRUE;
}

static bfd_boolean
xpic_size_one_stub (struct bfd_hash_entry *bh, void *in_arg)
{
  struct elf32_xpic_stub_hash_entry *hsh;
  struct elf32_xpic_link_hash_table *htab;
  int size;

  /* Massage our args to the form they really have.  */
  hsh = xpic_stub_hash_entry (bh);
  htab = in_arg;

  if (hsh->is_actually_needed)
    size = 4;
  else
    size = 0;

  htab->stub_sec->size += size;
  return TRUE;
}

void
elf32_xpic_setup_params (struct bfd_link_info *info,
                        bfd *xpic_stub_bfd,
                        asection *xpic_stub_section,
                        bfd_boolean no_stubs,
                        bfd_boolean deb_stubs,
                        bfd_boolean deb_relax,
                        bfd_vma pc_wrap_around,
                        bfd_boolean call_ret_replacement)
{
  struct elf32_xpic_link_hash_table *htab = xpic_link_hash_table (info);

  if (htab == NULL)
    return;
  htab->stub_sec = xpic_stub_section;
  htab->stub_bfd = xpic_stub_bfd;
  htab->no_stubs = no_stubs;

  debug_relax = deb_relax;
  debug_stubs = deb_stubs;
  xpic_pc_wrap_around = pc_wrap_around;
  xpic_replace_call_ret_sequences = call_ret_replacement;
}


/* Set up various things so that we can make a list of input sections
   for each output section included in the link.  Returns -1 on error,
   0 when no stubs will be needed, and 1 on success.  It also sets
   information on the stubs bfd and the stub section in the info
   struct.  */

int
elf32_xpic_setup_section_lists (bfd *output_bfd,
                               struct bfd_link_info *info)
{
  bfd *input_bfd;
  unsigned int bfd_count;
  int top_id, top_index;
  asection *section;
  asection **input_list, **list;
  bfd_size_type amt;
  struct elf32_xpic_link_hash_table *htab = xpic_link_hash_table(info);

  if (htab == NULL || htab->no_stubs)
    return 0;

  /* Count the number of input BFDs and find the top input section id.  */
  for (input_bfd = info->input_bfds, bfd_count = 0, top_id = 0;
       input_bfd != NULL;
       input_bfd = input_bfd->link_next)
    {
      bfd_count += 1;
      for (section = input_bfd->sections;
           section != NULL;
           section = section->next)
	if (top_id < section->id)
	  top_id = section->id;
    }

  htab->bfd_count = bfd_count;

  /* We can't use output_bfd->section_count here to find the top output
     section index as some sections may have been removed, and
     strip_excluded_output_sections doesn't renumber the indices.  */
  for (section = output_bfd->sections, top_index = 0;
       section != NULL;
       section = section->next)
    if (top_index < section->index)
      top_index = section->index;

  htab->top_index = top_index;
  amt = sizeof (asection *) * (top_index + 1);
  input_list = bfd_malloc (amt);
  htab->input_list = input_list;
  if (input_list == NULL)
    return -1;

  /* For sections we aren't interested in, mark their entries with a
     value we can check later.  */
  list = input_list + top_index;
  do
    *list = bfd_abs_section_ptr;
  while (list-- != input_list);

  for (section = output_bfd->sections;
       section != NULL;
       section = section->next)
    if ((section->flags & SEC_CODE) != 0)
      input_list[section->index] = NULL;

  return 1;
}


/* Read in all local syms for all input bfds, and create hash entries
   for export stubs if we are building a multi-subspace shared lib.
   Returns -1 on error, 0 otherwise.  */

static int
get_local_syms (bfd *input_bfd, struct bfd_link_info *info)
{
  unsigned int bfd_indx;
  Elf_Internal_Sym *local_syms, **all_local_syms;
  struct elf32_xpic_link_hash_table *htab = xpic_link_hash_table (info);
  bfd_size_type amt;

  if (htab == NULL)
    return -1;

  /* We want to read in symbol extension records only once.  To do this
     we need to read in the local symbols in parallel and save them for
     later use; so hold pointers to the local symbols in an array.  */
  amt = sizeof (Elf_Internal_Sym *) * htab->bfd_count;
  all_local_syms = bfd_zmalloc (amt);
  htab->all_local_syms = all_local_syms;
  if (all_local_syms == NULL)
    return -1;

  /* Walk over all the input BFDs, swapping in local symbols.
     If we are creating a shared library, create hash entries for the
     export stubs.  */
  for (bfd_indx = 0;
       input_bfd != NULL;
       input_bfd = input_bfd->link_next, bfd_indx++)
    {
      Elf_Internal_Shdr *symtab_hdr;

      /* We'll need the symbol table in a second.  */
      symtab_hdr = &elf_tdata (input_bfd)->symtab_hdr;
      if (symtab_hdr->sh_info == 0)
	continue;

      /* We need an array of the local symbols attached to the input bfd.  */
      local_syms = (Elf_Internal_Sym *) symtab_hdr->contents;
      if (local_syms == NULL)
	{
	  local_syms = bfd_elf_get_elf_syms (input_bfd, symtab_hdr,
					     symtab_hdr->sh_info, 0,
					     NULL, NULL, NULL);
	  /* Cache them for elf_link_input_bfd.  */
	  symtab_hdr->contents = (unsigned char *) local_syms;
	}
      if (local_syms == NULL)
	return -1;

      all_local_syms[bfd_indx] = local_syms;
    }

  return 0;
}

#define ADD_DUMMY_STUBS_FOR_DEBUGGING 0

bfd_boolean
elf32_xpic_size_stubs (bfd *output_bfd,
                      struct bfd_link_info *info,
                      bfd_boolean is_prealloc_run)
{
  struct elf32_xpic_link_hash_table *htab;
  int stub_changed = 0;

  htab = xpic_link_hash_table (info);
  if (htab == NULL)
    return FALSE;

  /* At this point we initialize htab->vector_base
     To the start of the text output section.  */
  htab->vector_base = htab->stub_sec->output_section->vma;

  if (get_local_syms (info->input_bfds, info))
    {
      if (htab->all_local_syms)
	goto error_ret_free_local;
      return FALSE;
    }

  if (ADD_DUMMY_STUBS_FOR_DEBUGGING)
    {
      struct elf32_xpic_stub_hash_entry *test;

      test = xpic_add_stub ("Hugo",htab);
      test->target_value = 0x123456;
      test->stub_offset = 13;

      test = xpic_add_stub ("Hugo2",htab);
      test->target_value = 0x84210;
      test->stub_offset = 14;
    }

  while (1)
    {
      bfd *input_bfd;
      unsigned int bfd_indx;

      /* We will have to re-generate the stub hash table each time anything
         in memory has changed.  */

      bfd_hash_traverse (&htab->bstab, xpic_mark_stub_not_to_be_necessary, htab);
      for (input_bfd = info->input_bfds, bfd_indx = 0;
           input_bfd != NULL;
           input_bfd = input_bfd->link_next, bfd_indx++)
        {
          Elf_Internal_Shdr *symtab_hdr;
          asection *section;
          Elf_Internal_Sym *local_syms;

          /* We'll need the symbol table in a second.  */
          symtab_hdr = &elf_tdata (input_bfd)->symtab_hdr;
          if (symtab_hdr->sh_info == 0)
            continue;

          local_syms = htab->all_local_syms[bfd_indx];

          /* Walk over each section attached to the input bfd.  */
          for (section = input_bfd->sections;
               section != NULL;
               section = section->next)
            {
              Elf_Internal_Rela *internal_relocs, *irelaend, *irela;

              /* If there aren't any relocs, then there's nothing more
                 to do.  */
              if ((section->flags & SEC_RELOC) == 0
                  || section->reloc_count == 0)
                continue;

              /* If this section is a link-once section that will be
                 discarded, then don't create any stubs.  */
              if (section->output_section == NULL
                  || section->output_section->owner != output_bfd)
                continue;

              /* Get the relocs.  */
              internal_relocs
                = _bfd_elf_link_read_relocs (input_bfd, section, NULL, NULL,
                                             info->keep_memory);
              if (internal_relocs == NULL)
                goto error_ret_free_local;

              /* Now examine each relocation.  */
              irela = internal_relocs;
              irelaend = irela + section->reloc_count;
              for (; irela < irelaend; irela++)
                {
                  unsigned int r_type, r_indx;
                  struct elf32_xpic_stub_hash_entry *hsh;
                  asection *sym_sec;
                  bfd_vma sym_value;
                  bfd_vma destination;
                  struct elf_link_hash_entry *hh;
                  char *stub_name;

                  r_type = ELF32_R_TYPE (irela->r_info);
                  r_indx = ELF32_R_SYM (irela->r_info);

                  /* Only look for 16 bit GS relocs. No other reloc will need a
                     stub.  */
                  if (!((r_type == R_AVR_16_PM)
                        || (r_type == R_AVR_LO8_LDI_GS)
                        || (r_type == R_AVR_HI8_LDI_GS)))
                    continue;

                  /* Now determine the call target, its name, value,
                     section.  */
                  sym_sec = NULL;
                  sym_value = 0;
                  destination = 0;
                  hh = NULL;
                  if (r_indx < symtab_hdr->sh_info)
                    {
                      /* It's a local symbol.  */
                      Elf_Internal_Sym *sym;
                      Elf_Internal_Shdr *hdr;
		      unsigned int shndx;

                      sym = local_syms + r_indx;
                      if (ELF_ST_TYPE (sym->st_info) != STT_SECTION)
                        sym_value = sym->st_value;
		      shndx = sym->st_shndx;
		      if (shndx < elf_numsections (input_bfd))
			{
			  hdr = elf_elfsections (input_bfd)[shndx];
			  sym_sec = hdr->bfd_section;
			  destination = (sym_value + irela->r_addend
					 + sym_sec->output_offset
					 + sym_sec->output_section->vma);
			}
                    }
                  else
                    {
                      /* It's an external symbol.  */
                      int e_indx;

                      e_indx = r_indx - symtab_hdr->sh_info;
                      hh = elf_sym_hashes (input_bfd)[e_indx];

                      while (hh->root.type == bfd_link_hash_indirect
                             || hh->root.type == bfd_link_hash_warning)
                        hh = (struct elf_link_hash_entry *)
                              (hh->root.u.i.link);

                      if (hh->root.type == bfd_link_hash_defined
                          || hh->root.type == bfd_link_hash_defweak)
                        {
                          sym_sec = hh->root.u.def.section;
                          sym_value = hh->root.u.def.value;
                          if (sym_sec->output_section != NULL)
                          destination = (sym_value + irela->r_addend
                                         + sym_sec->output_offset
                                         + sym_sec->output_section->vma);
                        }
                      else if (hh->root.type == bfd_link_hash_undefweak)
                        {
                          if (! info->shared)
                            continue;
                        }
                      else if (hh->root.type == bfd_link_hash_undefined)
                        {
                          if (! (info->unresolved_syms_in_objects == RM_IGNORE
                                 && (ELF_ST_VISIBILITY (hh->other)
                                     == STV_DEFAULT)))
                             continue;
                        }
                      else
                        {
                          bfd_set_error (bfd_error_bad_value);

                          error_ret_free_internal:
                          if (elf_section_data (section)->relocs == NULL)
                            free (internal_relocs);
                          goto error_ret_free_local;
                        }
                    }

                  if (! xpic_stub_is_required_for_16_bit_reloc
		      (destination - htab->vector_base))
                    {
                      if (!is_prealloc_run)
			/* We are having a reloc that does't need a stub.  */
			continue;

		      /* We don't right now know if a stub will be needed.
			 Let's rather be on the safe side.  */
                    }

                  /* Get the name of this stub.  */
                  stub_name = xpic_stub_name (sym_sec, sym_value, irela);

                  if (!stub_name)
                    goto error_ret_free_internal;


                  hsh = xpic_stub_hash_lookup (&htab->bstab,
                                              stub_name,
                                              FALSE, FALSE);
                  if (hsh != NULL)
                    {
                      /* The proper stub has already been created.  Mark it
                         to be used and write the possibly changed destination
                         value.  */
                      hsh->is_actually_needed = TRUE;
                      hsh->target_value = destination;
                      free (stub_name);
                      continue;
                    }

                  hsh = xpic_add_stub (stub_name, htab);
                  if (hsh == NULL)
                    {
                      free (stub_name);
                      goto error_ret_free_internal;
                    }

                  hsh->is_actually_needed = TRUE;
                  hsh->target_value = destination;

                  if (debug_stubs)
                    printf ("Adding stub with destination 0x%x to the"
                            " hash table.\n", (unsigned int) destination);
                  if (debug_stubs)
                    printf ("(Pre-Alloc run: %i)\n", is_prealloc_run);

                  stub_changed = TRUE;
                }

              /* We're done with the internal relocs, free them.  */
              if (elf_section_data (section)->relocs == NULL)
                free (internal_relocs);
            }
        }

      /* Re-Calculate the number of needed stubs.  */
      htab->stub_sec->size = 0;
      bfd_hash_traverse (&htab->bstab, xpic_size_one_stub, htab);

      if (!stub_changed)
        break;

      stub_changed = FALSE;
    }

  free (htab->all_local_syms);
  return TRUE;

 error_ret_free_local:
  free (htab->all_local_syms);
  return FALSE;
}


/* Build all the stubs associated with the current output file.  The
   stubs are kept in a hash table attached to the main linker hash
   table.  We also set up the .plt entries for statically linked PIC
   functions here.  This function is called via hppaelf_finish in the
   linker.  */

bfd_boolean
elf32_xpic_build_stubs (struct bfd_link_info *info)
{
  asection *stub_sec;
  struct bfd_hash_table *table;
  struct elf32_xpic_link_hash_table *htab;
  bfd_size_type total_size = 0;

  htab = xpic_link_hash_table (info);
  if (htab == NULL)
    return FALSE;

  /* In case that there were several stub sections:  */
  for (stub_sec = htab->stub_bfd->sections;
       stub_sec != NULL;
       stub_sec = stub_sec->next)
    {
      bfd_size_type size;

      /* Allocate memory to hold the linker stubs.  */
      size = stub_sec->size;
      total_size += size;

      stub_sec->contents = bfd_zalloc (htab->stub_bfd, size);
      if (stub_sec->contents == NULL && size != 0)
	return FALSE;
      stub_sec->size = 0;
    }

  /* Allocate memory for the adress mapping table.  */
  htab->amt_entry_cnt = 0;
  htab->amt_max_entry_cnt = total_size / 4;
  htab->amt_stub_offsets = bfd_malloc (sizeof (bfd_vma)
                                       * htab->amt_max_entry_cnt);
  htab->amt_destination_addr = bfd_malloc (sizeof (bfd_vma)
					   * htab->amt_max_entry_cnt );

  if (debug_stubs)
    printf ("Allocating %i entries in the AMT\n", htab->amt_max_entry_cnt);

  /* Build the stubs as directed by the stub hash table.  */
  table = &htab->bstab;
  bfd_hash_traverse (table, xpic_build_one_stub, info);

  if (debug_stubs)
    printf ("Final Stub section Size: %i\n", (int) htab->stub_sec->size);

  return TRUE;
}

#define ELF_ARCH		bfd_arch_xpic
#define ELF_MACHINE_CODE	EM_XPIC
#define ELF_MACHINE_ALT1	EM_XPIC 
//EM_AVR_OLD
#define ELF_MAXPAGESIZE		1

#define TARGET_LITTLE_SYM       bfd_elf32_xpic_vec
#define TARGET_LITTLE_NAME	"elf32-xpic"

#define bfd_elf32_bfd_link_hash_table_create elf32_xpic_link_hash_table_create
#define bfd_elf32_bfd_link_hash_table_free   elf32_xpic_link_hash_table_free

#define elf_info_to_howto	             xpic_info_to_howto_rela
#define elf_info_to_howto_rel	             NULL
#define elf_backend_relocate_section         elf32_xpic_relocate_section
#define elf_backend_check_relocs             elf32_xpic_check_relocs
#define elf_backend_can_gc_sections          1
#define elf_backend_rela_normal		     1
#define elf_backend_final_write_processing \
					bfd_elf_xpic_final_write_processing
#define elf_backend_object_p		elf32_xpic_object_p

#define bfd_elf32_bfd_relax_section elf32_xpic_relax_section
#define bfd_elf32_bfd_get_relocated_section_contents \
                                        elf32_xpic_get_relocated_section_contents

#include "elf32-target.h"
