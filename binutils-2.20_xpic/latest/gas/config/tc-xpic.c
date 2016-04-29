/* tc-xpic.c -- Assembler code for the ATMEL AVR

   Copyright 1999, 2000, 2001, 2002, 2004, 2005, 2006, 2007, 2008, 2009
   Free Software Foundation, Inc.
   Contributed by Denis Chertykov <denisc@overta.ru>

   This file is part of GAS, the GNU Assembler.

   GAS is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   GAS is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with GAS; see the file COPYING.  If not, write to
   the Free Software Foundation, 51 Franklin Street - Fifth Floor,
   Boston, MA 02110-1301, USA.  */

#include "as.h"
#include "safe-ctype.h"
#include "subsegs.h"
#include <stdint.h>

struct xpic_opcodes_s
{
  char *        name;
  char *        constraints;
  int           insn_size;		/* In words.  */
  int           isa;
  unsigned int  bin_opcode;
  char *        opcode;
};

#define XPIC_INSN(NAME, CONSTR, OPCODE, SIZE, ISA, BIN) \
{#NAME, CONSTR, SIZE, ISA, BIN, OPCODE},

struct xpic_opcodes_s xpic_opcodes[] =
{
  #include "opcode/xpic.h"
  {NULL, NULL, 0, 0, 0,NULL}
};

const char comment_chars[] = "//";
const char line_comment_chars[] = "//";
const char line_separator_chars[] = "\0";

const char *md_shortopts = "m:";
struct mcu_type_s
{
  char *name;
  int isa;
  int mach;
};

/* XXX - devices that don't seem to exist (renamed, replaced with larger
   ones, or planned but never produced), left here for compatibility.  */

static struct mcu_type_s mcu_types[] =
{
  {"xpic",        XPIC_ISA,     bfd_mach_xpic},
  {"xpic2",       XPIC_ISA2,    bfd_mach_xpic},
  {NULL, 0, 0}
};

/* Current MCU type.  */
static struct mcu_type_s   default_mcu = {"xpic", XPIC_ISA, bfd_mach_xpic};
static struct mcu_type_s * xpic_mcu = & default_mcu;

/* AVR target-specific switches.  */
struct xpic_opt_s
{
  int all_opcodes;  /* -mall-opcodes: accept all known AVR opcodes.  */
  int no_skip_bug;  /* -mno-skip-bug: no warnings for skipping 2-word insns.  */
  int no_wrap;      /* -mno-wrap: reject rjmp/rcall with 8K wrap-around.  */
};

static struct xpic_opt_s xpic_opt = { 0, 0, 0 };

const char EXP_CHARS[] = "eE";
const char FLT_CHARS[] = "dD";

static void xpic_set_arch (int);

/* The target specific pseudo-ops which we support.  */
const pseudo_typeS md_pseudo_table[] =
{
  {"arch", xpic_set_arch,	0},
  { NULL,	NULL,		0}
};

#define LDI_IMMEDIATE(x) (((x) & 0xf) | (((x) << 4) & 0xf00))

#define EXP_MOD_NAME(i)       exp_mod[i].name
#define EXP_MOD_RELOC(i)      exp_mod[i].reloc
#define EXP_MOD_NEG_RELOC(i)  exp_mod[i].neg_reloc
#define HAVE_PM_P(i)          exp_mod[i].have_pm

struct exp_mod_s
{
  char *                    name;
  bfd_reloc_code_real_type  reloc;
  bfd_reloc_code_real_type  neg_reloc;
  int                       have_pm;
};

static struct exp_mod_s exp_mod[] =
{
  {"hh8",    BFD_RELOC_AVR_HH8_LDI,    BFD_RELOC_AVR_HH8_LDI_NEG,    1},
  {"pm_hh8", BFD_RELOC_AVR_HH8_LDI_PM, BFD_RELOC_AVR_HH8_LDI_PM_NEG, 0},
  {"hi8",    BFD_RELOC_AVR_HI8_LDI,    BFD_RELOC_AVR_HI8_LDI_NEG,    1},
  {"pm_hi8", BFD_RELOC_AVR_HI8_LDI_PM, BFD_RELOC_AVR_HI8_LDI_PM_NEG, 0},
  {"lo8",    BFD_RELOC_AVR_LO8_LDI,    BFD_RELOC_AVR_LO8_LDI_NEG,    1},
  {"pm_lo8", BFD_RELOC_AVR_LO8_LDI_PM, BFD_RELOC_AVR_LO8_LDI_PM_NEG, 0},
  {"hlo8",   BFD_RELOC_AVR_HH8_LDI,    BFD_RELOC_AVR_HH8_LDI_NEG,    0},
  {"hhi8",   BFD_RELOC_AVR_MS8_LDI,    BFD_RELOC_AVR_MS8_LDI_NEG,    0},
};

/* A union used to store indicies into the exp_mod[] array
   in a hash table which expects void * data types.  */
typedef union
{
  void * ptr;
  int    index;
} mod_index;

/* Opcode hash table.  */
static struct hash_control *xpic_hash;

/* Reloc modifiers hash control (hh8,hi8,lo8,pm_xx).  */
static struct hash_control *xpic_mod_hash;

/*prints emmited instruction*/
int printinstruction=0;

#define OPTION_MMCU 'm'
enum options
{
  OPTION_ALL_OPCODES = OPTION_MD_BASE + 1,
  OPTION_NO_SKIP_BUG,
  OPTION_NO_WRAP,
  OPTION_XPIC_PRINT_INSTRUCTIONS
};

struct option md_longopts[] =
{
  { "mmcu",   required_argument, NULL, OPTION_MMCU        },
  { "mall-opcodes", no_argument, NULL, OPTION_ALL_OPCODES },
  { "mno-skip-bug", no_argument, NULL, OPTION_NO_SKIP_BUG },
  { "mno-wrap",     no_argument, NULL, OPTION_NO_WRAP     },
  { "print-instr",  required_argument, NULL, OPTION_XPIC_PRINT_INSTRUCTIONS     },
  { NULL, no_argument, NULL, 0 }
};

size_t md_longopts_size = sizeof (md_longopts);

/* Display nicely formatted list of known MCU names.  */
static void show_mcu_list (FILE *stream)
{
  int i, x;

  fprintf (stream, _("Known MCU names:"));
  x = 1000;

  for (i = 0; mcu_types[i].name; i++)
    {
      int len = strlen (mcu_types[i].name);

      x += len + 1;

      if (x < 105)
	fprintf (stream, " %s", mcu_types[i].name);
      else
	{
	  fprintf (stream, "\n  %s", mcu_types[i].name);
	  x = len + 2;
	}
    }

  fprintf (stream, "\n");
}

static inline char *skip_space (char *s)
{
  while (*s == ' ' || *s == '\t' || *s == ',') // || *s == '[' || *s == ']'
    ++s;
  return s;
}

/* Extract one word from FROM and copy it to TO.  */
static char *extract_word (char *from, char *to, int limit)
{
  char *op_start;
  char *op_end;
  int size = 0;

  /* Drop leading whitespace.  */
  from = skip_space (from);
  *to = 0;

  /* Find the op code end.  */
  for (op_start = op_end = from; *op_end != 0 && (is_part_of_name (*op_end)  || *op_end=='-' || *op_end=='+');)
    {
      to[size++] = *op_end++;
      if (size + 1 >= limit)
	break;
    }

  to[size] = 0;
  return op_end;
}

int md_estimate_size_before_relax (fragS *fragp ATTRIBUTE_UNUSED, asection *seg ATTRIBUTE_UNUSED)
{
  abort ();
  return 0;
}

void md_show_usage (FILE *stream)
{
  fprintf (stream,
      _("XPIC options:\n"
	"  -mmcu=[xpic] select microcontroller variant\n"
	"                   [xpic-name] can be:\n"
	"                   xpic  - classic xpic core\n\n"));
  fprintf (stream,
      _("  -print-instr=n   print emmited instructions (n=1-hex, n=2-with source code, n=3 - with binary)\n\n"));

/*
  fprintf (stream,
      _("  -mall-opcodes    accept all AVR opcodes, even if not supported by MCU\n"
	"  -mno-skip-bug    disable warnings for skipping two-word instructions\n"
	"                   (default for a4, a5)\n"
	"  -mno-wrap        reject rjmp/rcall instructions with 8K wrap-around\n"
	"                   (default for a3, a5)\n"
	"  -print-instr=n   print emmited instructions (n=1-hex, n=2-with source code, n=3 - with binary)\n\n"));
*/
  show_mcu_list (stream);
}

static void xpic_set_arch (int dummy ATTRIBUTE_UNUSED)
{
  char str[20];

  input_line_pointer = extract_word (input_line_pointer, str, 20);
  md_parse_option (OPTION_MMCU, str);
  bfd_set_arch_mach (stdoutput, TARGET_ARCH, xpic_mcu->mach);
}

int md_parse_option (int c, char *arg)
{
  switch (c)
    {
    case OPTION_XPIC_PRINT_INSTRUCTIONS:
      {
        printinstruction=*arg-48;
        return 1;
      }

    case OPTION_MMCU:
      {
	int i;
	char *s = alloca (strlen (arg) + 1);

	{
	  char *t = s;
	  char *arg1 = arg;

	  do
	    *t = TOLOWER (*arg1++);
	  while (*t++);
	}

	for (i = 0; mcu_types[i].name; ++i)
	  if (strcmp (mcu_types[i].name, s) == 0)
	    break;

	if (!mcu_types[i].name)
	  {
	    show_mcu_list (stderr);
	    as_fatal (_("unknown MCU: %s\n"), arg);
	  }

	/* It is OK to redefine mcu type within the same XPIC bfd machine
	   type - this for allows passing -mmcu=... via gcc ASM_SPEC as well
	   as .arch ... in the asm output at the same time.  */
	if (xpic_mcu == &default_mcu || xpic_mcu->mach == mcu_types[i].mach)
	  xpic_mcu = &mcu_types[i];
	else

	  as_fatal (_("redefinition of mcu type `%s' to `%s'"),
		    xpic_mcu->name, mcu_types[i].name);
	return 1;
      }
    case OPTION_ALL_OPCODES:
      xpic_opt.all_opcodes = 1;
      return 1;
    case OPTION_NO_SKIP_BUG:
      xpic_opt.no_skip_bug = 1;
      return 1;
    case OPTION_NO_WRAP:
      xpic_opt.no_wrap = 1;
      return 1;
    }

  return 0;
}

symbolS *md_undefined_symbol (char *name ATTRIBUTE_UNUSED)
{
  return NULL;
}

char *md_atof (int type, char *litP, int *sizeP)
{
  return ieee_md_atof (type, litP, sizeP, FALSE);
}

void md_convert_frag (bfd *abfd ATTRIBUTE_UNUSED, asection *sec ATTRIBUTE_UNUSED, fragS *fragP ATTRIBUTE_UNUSED)
{
  abort ();
}

void md_begin (void)
{
  unsigned int i;
  struct xpic_opcodes_s *opcode;

  xpic_hash = hash_new ();

  /* Insert unique names into hash table.  This hash table then provides a
     quick index to the first opcode with a particular name in the opcode
     table.  */
  for (opcode = xpic_opcodes; opcode->name; opcode++)
    hash_insert (xpic_hash, opcode->name, (char *) opcode);

  xpic_mod_hash = hash_new ();

  for (i = 0; i < ARRAY_SIZE (exp_mod); ++i)
    {
      mod_index m;

      m.index = i + 10;
      hash_insert (xpic_mod_hash, EXP_MOD_NAME (i), m.ptr);
    }

  bfd_set_arch_mach (stdoutput, TARGET_ARCH, xpic_mcu->mach);
}

/* Resolve STR as a constant expression and return the result.   If result greater than MAX then error.  */
/*
static unsigned int xpic_get_constant (char *str, int max)
{
  expressionS ex;

  str = skip_space (str);
  input_line_pointer = str;
  expression (& ex);

  if (ex.X_op != O_constant)
    as_bad (_("constant value required"));

  if (ex.X_add_number > max || ex.X_add_number < 0)
    as_bad (_("number must be positive and less than %d"), max + 1);

  return ex.X_add_number;
}
*/
/* Parse for ldd/std offset.  */
/*
static void xpic_offset_expression (expressionS *exp)
{
  char *str = input_line_pointer;
  char *tmp;
  char op[8];

  tmp = str;
  str = extract_word (str, op, sizeof (op));

  input_line_pointer = tmp;
  expression (exp);

  // Warn about expressions that fail to use lo8 ().  
  if (exp->X_op == O_constant)
    {
      int x = exp->X_add_number;

      if (x < -255 || x > 255)
	as_warn (_("constant out of 8-bit range: %d"), x);
    }
}
*/

/* Parse ordinary expression.  */

static char *parse_exp (char *s, expressionS *op)
{
  input_line_pointer = s;
  expression (op);
  if (op->X_op == O_absent)
    as_bad (_("missing operand"));
  return input_line_pointer;
}

/* Parse special expressions (needed for LDI command):
   xx8 (address)
   xx8 (-address)
   pm_xx8 (address)
   pm_xx8 (-address)
   where xx is: hh, hi, lo.  */
/*
static bfd_reloc_code_real_type xpic_ldi_expression (expressionS *exp)
{
  char *str = input_line_pointer;
  char *tmp;
  char op[8];
  int mod;
  int linker_stubs_should_be_generated = 0;

  tmp = str;

  str = extract_word (str, op, sizeof (op));

  if (op[0])
    {
      mod_index m;

      m.ptr = hash_find (xpic_mod_hash, op);
      mod = m.index;

      if (mod)
	{
	  int closes = 0;

	  mod -= 10;
	  str = skip_space (str);

	  if (*str == '(')
	    {
	      bfd_reloc_code_real_type  reloc_to_return;
	      int neg_p = 0;

	      ++str;

	      if (strncmp ("pm(", str, 3) == 0
                  || strncmp ("gs(",str,3) == 0
                  || strncmp ("-(gs(",str,5) == 0
		  || strncmp ("-(pm(", str, 5) == 0)
		{
		  if (HAVE_PM_P (mod))
		    {
		      ++mod;
		      ++closes;
		    }
		  else
		    as_bad (_("illegal expression"));

                  if (str[0] == 'g' || str[2] == 'g')
                    linker_stubs_should_be_generated = 1;

		  if (*str == '-')
		    {
		      neg_p = 1;
		      ++closes;
		      str += 5;
		    }
		  else
		    str += 3;
		}

	      if (*str == '-' && *(str + 1) == '(')
		{
		  neg_p ^= 1;
		  ++closes;
		  str += 2;
		}

	      input_line_pointer = str;
	      expression (exp);

	      do
		{
		  if (*input_line_pointer != ')')
		    {
		      as_bad (_("`)' required"));
		      break;
		    }
		  input_line_pointer++;
		}
	      while (closes--);

	      reloc_to_return =
		neg_p ? EXP_MOD_NEG_RELOC (mod) : EXP_MOD_RELOC (mod);
	      if (linker_stubs_should_be_generated)
		{
		  switch (reloc_to_return)
		    {
		    case BFD_RELOC_AVR_LO8_LDI_PM:
		      reloc_to_return = BFD_RELOC_AVR_LO8_LDI_GS;
		      break;
		    case BFD_RELOC_AVR_HI8_LDI_PM:
		      reloc_to_return = BFD_RELOC_AVR_HI8_LDI_GS;
		      break;

		    default:
		      // PR 5523: Do not generate a warning here, legitimate code can trigger this case.  
		      break;
		    }
		}
	      return reloc_to_return;
	    }
	}
    }

  input_line_pointer = tmp;
  expression (exp);

  // Warn about expressions that fail to use lo8 (). 
  if (exp->X_op == O_constant)
    {
      int x = exp->X_add_number;

      if (x < -255 || x > 255)
	as_warn (_("constant out of 8-bit range: %d"), x);
    }

  return BFD_RELOC_AVR_LDI;
}
*/


/* GAS will call this function for each section at the end of the assembly,
   to permit the CPU backend to adjust the alignment of a section.  */
valueT md_section_align (asection *seg, valueT addr)
{
  int align;
  if( (seg->flags & SEC_ALLOC) || (seg->flags & SEC_LOAD))
  {
    // make all data and code sections 4byte aligned
      align = 2;
  }
  else align = bfd_get_section_alignment (stdoutput, seg);

  return ((addr + (1 << align) - 1) & (-1 << align));
}

/* If you define this macro, it should return the offset between the
   address of a PC relative fixup and the position from which the PC
   relative adjustment should be made.  On many processors, the base
   of a PC relative instruction is the next instruction, so this
   macro would return the length of an instruction.  */
long md_pcrel_from_section (fixS *fixp, segT sec)
{
  if (fixp->fx_addsy != (symbolS *) NULL
      && (!S_IS_DEFINED (fixp->fx_addsy)
	  || (S_GET_SEGMENT (fixp->fx_addsy) != sec)))
    return 0;

  return fixp->fx_frag->fr_address + fixp->fx_where;
}

/* GAS will call this for each fixup.  It should store the correct value in the object file.  */
void md_apply_fix (fixS *fixP, valueT * valP, segT seg)
{
  unsigned char *where;
  unsigned long insn;
  long value = *valP;

  if (fixP->fx_addsy == (symbolS *) NULL)
    fixP->fx_done = 1;

  else if (fixP->fx_pcrel)
    {
      segT s = S_GET_SEGMENT (fixP->fx_addsy);

      if (s == seg || s == absolute_section)
	{
	  value += S_GET_VALUE (fixP->fx_addsy);
	  fixP->fx_done = 1;
	}
    }

  /* We don't actually support subtracting a symbol.  */
  if (fixP->fx_subsy != (symbolS *) NULL)
    as_bad_where (fixP->fx_file, fixP->fx_line, _("expression too complex"));

  switch (fixP->fx_r_type)
    {
    default:
      fixP->fx_no_overflow = 1;
      break;
    case BFD_RELOC_XPIC_24_PCREL:
    as_bad_where (fixP->fx_file, fixP->fx_line, _("internal error! 'BFD_RELOC_XPIC_24_PCREL'"));
break;
    case BFD_RELOC_AVR_7_PCREL:
    case BFD_RELOC_AVR_13_PCREL:
    case BFD_RELOC_32:
    case BFD_RELOC_16:
    case BFD_RELOC_AVR_CALL:
      break;
    }




  if (fixP->fx_done)
    {
      /* Fetch the instruction, insert the fully resolved operand
	 value, and stuff the instruction back again.  */
      where = (unsigned char *) fixP->fx_frag->fr_literal + fixP->fx_where;
//      insn = bfd_getl16 (where);
      insn = bfd_getl32 (where);

      switch (fixP->fx_r_type)
	{
	case BFD_RELOC_AVR_7_PCREL:
	  if (value & 1)
	    as_bad_where (fixP->fx_file, fixP->fx_line,
			  _("odd address operand: %ld"), value);

	  /* Instruction addresses are always right-shifted by 1.  */
	  value >>= 1;
	  --value;			/* Correct PC.  */

	  if (value < -64 || value > 63)
	    as_bad_where (fixP->fx_file, fixP->fx_line,
			  _("operand out of range: %ld"), value);
	  value = (value << 3) & 0x3f8;
	  bfd_putl16 ((bfd_vma) (value | insn), where);
	  break;

	case BFD_RELOC_AVR_13_PCREL:
	  if (value & 1)
	    as_bad_where (fixP->fx_file, fixP->fx_line,
			  _("odd address operand: %ld"), value);

	  /* Instruction addresses are always right-shifted by 1.  */
	  value >>= 1;
	  --value;			/* Correct PC.  */
/*
	  if (value < -2048 || value > 2047)
	    {
//	       No wrap for devices with >8K of program memory.  
	      if ((xpic_mcu->isa & AVR_ISA_MEGA) || xpic_opt.no_wrap)
		as_bad_where (fixP->fx_file, fixP->fx_line,
			      _("operand out of range: %ld"), value);
	    }
*/
	  value &= 0xfff;
	  bfd_putl16 ((bfd_vma) (value | insn), where);
	  break;

      case BFD_RELOC_XPIC_24_PCREL:
as_bad_where (fixP->fx_file, fixP->fx_line,_("Loock at 'tc-xpic.c': case BFD_RELOC_XPIC_24_PCREL:"));
	  if (value & 3)
	    as_bad_where (fixP->fx_file, fixP->fx_line,_("invalid address operand: %ld"), value);

	  if (value < -33554432 || value > 33554431)
	    {
		as_bad_where (fixP->fx_file, fixP->fx_line,_("operand out of range: %ld"), value);
	    }
	  // Instruction addresses are always right-shifted by 2
	  value >>= 2;
	  value &= 0x00ffffff;
	  value <<= 5;
	  bfd_putl32 ((bfd_vma) (value | insn), where);
	  break;

	case BFD_RELOC_32:
	  bfd_putl32 ((bfd_vma) value, where);
	  break;

	case BFD_RELOC_16:
	  bfd_putl16 ((bfd_vma) value, where);
	  break;

	case BFD_RELOC_AVR_16_PM:
	  bfd_putl16 ((bfd_vma) (value >> 1), where);
	  break;

	case BFD_RELOC_AVR_LDI:
	  if (value > 255)
	    as_bad_where (fixP->fx_file, fixP->fx_line,
			  _("operand out of range: %ld"), value);
	  bfd_putl16 ((bfd_vma) insn | LDI_IMMEDIATE (value), where);
	  break;

	case BFD_RELOC_AVR_6:
	  if ((value > 63) || (value < 0))
	    as_bad_where (fixP->fx_file, fixP->fx_line,
			  _("operand out of range: %ld"), value);
	  bfd_putl16 ((bfd_vma) insn | ((value & 7) | ((value & (3 << 3)) << 7) | ((value & (1 << 5)) << 8)), where);
	  break;

	case BFD_RELOC_AVR_6_ADIW:
	  if ((value > 63) || (value < 0))
	    as_bad_where (fixP->fx_file, fixP->fx_line,
			  _("operand out of range: %ld"), value);
	  bfd_putl16 ((bfd_vma) insn | (value & 0xf) | ((value & 0x30) << 2), where);
	  break;

	case BFD_RELOC_AVR_LO8_LDI:
	  bfd_putl16 ((bfd_vma) insn | LDI_IMMEDIATE (value), where);
	  break;

	case BFD_RELOC_AVR_HI8_LDI:
	  bfd_putl16 ((bfd_vma) insn | LDI_IMMEDIATE (value >> 8), where);
	  break;

	case BFD_RELOC_AVR_MS8_LDI:
	  bfd_putl16 ((bfd_vma) insn | LDI_IMMEDIATE (value >> 24), where);
	  break;

	case BFD_RELOC_AVR_HH8_LDI:
	  bfd_putl16 ((bfd_vma) insn | LDI_IMMEDIATE (value >> 16), where);
	  break;

	case BFD_RELOC_AVR_LO8_LDI_NEG:
	  bfd_putl16 ((bfd_vma) insn | LDI_IMMEDIATE (-value), where);
	  break;

	case BFD_RELOC_AVR_HI8_LDI_NEG:
	  bfd_putl16 ((bfd_vma) insn | LDI_IMMEDIATE (-value >> 8), where);
	  break;

	case BFD_RELOC_AVR_MS8_LDI_NEG:
	  bfd_putl16 ((bfd_vma) insn | LDI_IMMEDIATE (-value >> 24), where);
	  break;

	case BFD_RELOC_AVR_HH8_LDI_NEG:
	  bfd_putl16 ((bfd_vma) insn | LDI_IMMEDIATE (-value >> 16), where);
	  break;

	case BFD_RELOC_AVR_LO8_LDI_PM:
	  bfd_putl16 ((bfd_vma) insn | LDI_IMMEDIATE (value >> 1), where);
	  break;

	case BFD_RELOC_AVR_HI8_LDI_PM:
	  bfd_putl16 ((bfd_vma) insn | LDI_IMMEDIATE (value >> 9), where);
	  break;

	case BFD_RELOC_AVR_HH8_LDI_PM:
	  bfd_putl16 ((bfd_vma) insn | LDI_IMMEDIATE (value >> 17), where);
	  break;

	case BFD_RELOC_AVR_LO8_LDI_PM_NEG:
	  bfd_putl16 ((bfd_vma) insn | LDI_IMMEDIATE (-value >> 1), where);
	  break;

	case BFD_RELOC_AVR_HI8_LDI_PM_NEG:
	  bfd_putl16 ((bfd_vma) insn | LDI_IMMEDIATE (-value >> 9), where);
	  break;

	case BFD_RELOC_AVR_HH8_LDI_PM_NEG:
	  bfd_putl16 ((bfd_vma) insn | LDI_IMMEDIATE (-value >> 17), where);
	  break;

	case BFD_RELOC_AVR_CALL:
	  {
	    unsigned long x;

	    x = bfd_getl16 (where);
	    if (value & 1)
	      as_bad_where (fixP->fx_file, fixP->fx_line,
			    _("odd address operand: %ld"), value);
	    value >>= 1;
	    x |= ((value & 0x10000) | ((value << 3) & 0x1f00000)) >> 16;
	    bfd_putl16 ((bfd_vma) x, where);
	    bfd_putl16 ((bfd_vma) (value & 0xffff), where + 2);
	  }
	  break;

	default:
	  as_fatal (_("line %d: unknown relocation type: 0x%x"),
		    fixP->fx_line, fixP->fx_r_type);
	  break;
	}
    }
  else
    {
      switch ((int) fixP->fx_r_type)
	{
	case -BFD_RELOC_AVR_HI8_LDI_NEG:
	case -BFD_RELOC_AVR_HI8_LDI:
	case -BFD_RELOC_AVR_LO8_LDI_NEG:
	case -BFD_RELOC_AVR_LO8_LDI:
	  as_bad_where (fixP->fx_file, fixP->fx_line,
			_("only constant expression allowed"));
	  fixP->fx_done = 1;
	  break;
	default:
	  break;
	}
    }
}

/* GAS will call this to generate a reloc, passing the resulting reloc
   to `bfd_install_relocation'.  This currently works poorly, as
   `bfd_install_relocation' often does the wrong thing, and instances of
   `tc_gen_reloc' have been written to work around the problems, which
   in turns makes it difficult to fix `bfd_install_relocation'.  */

/* If while processing a fixup, a reloc really needs to be created, then it is done here.  */
arelent *tc_gen_reloc (asection *seg ATTRIBUTE_UNUSED, fixS *fixp)
{
  arelent *reloc;

  if (fixp->fx_addsy && fixp->fx_subsy)
    {
      long value = 0;

      if ((S_GET_SEGMENT (fixp->fx_addsy) != S_GET_SEGMENT (fixp->fx_subsy))
          || S_GET_SEGMENT (fixp->fx_addsy) == undefined_section)
        {
          as_bad_where (fixp->fx_file, fixp->fx_line,
              "Difference of symbols in different sections is not supported");
          return NULL;
        }

      /* We are dealing with two symbols defined in the same section.
         Let us fix-up them here.  */
      value += S_GET_VALUE (fixp->fx_addsy);
      value -= S_GET_VALUE (fixp->fx_subsy);

      /* When fx_addsy and fx_subsy both are zero, md_apply_fix
         only takes it's second operands for the fixup value.  */
      fixp->fx_addsy = NULL;
      fixp->fx_subsy = NULL;
      md_apply_fix (fixp, (valueT *) &value, NULL);

      return NULL;
    }

  reloc = xmalloc (sizeof (arelent));

  reloc->sym_ptr_ptr = xmalloc (sizeof (asymbol *));
  *reloc->sym_ptr_ptr = symbol_get_bfdsym (fixp->fx_addsy);

  reloc->address = fixp->fx_frag->fr_address + fixp->fx_where;
  reloc->howto = bfd_reloc_type_lookup (stdoutput, fixp->fx_r_type);
  if (reloc->howto == (reloc_howto_type *) NULL)
    {
      as_bad_where (fixp->fx_file, fixp->fx_line,
		    _("reloc %d not supported by object file format"),
		    (int) fixp->fx_r_type);
      return NULL;
    }

  if (fixp->fx_r_type == BFD_RELOC_VTABLE_INHERIT
      || fixp->fx_r_type == BFD_RELOC_VTABLE_ENTRY)
    reloc->address = fixp->fx_offset;

  reloc->addend = fixp->fx_offset;

  return reloc;
}










/** *********************************************************************************************************
CHanges in files:
                 tc-xpic.h
                 tc-xpic.c
********************************************************************************************************* **/
// conversion from DWORD to string (char[42]). Only for printing
void GetBinary(int iWorkText, char * ResBinary42)
{
  char buf[41];
  char *Binary42=buf;
int i;
  memset(ResBinary42,0,42);
  for (i=0; i<32; i++)
	{
	  *Binary42 = (char)(iWorkText & 1) + 48;
	  iWorkText=iWorkText>>1;
	  Binary42++;
	  if(i==3){*Binary42='_';Binary42++;}
	  if(i==7){*Binary42=' ';Binary42++;}
	  if(i==11){*Binary42='_';Binary42++;}
	  if(i==15){*Binary42=' ';Binary42++;*Binary42='-';Binary42++;*Binary42=' ';Binary42++;}
	  if(i==19){*Binary42='_';Binary42++;}
	  if(i==23){*Binary42=' ';Binary42++;}
	  if(i==27){*Binary42='_';Binary42++;}
	}
  for (i=0; i<41; i++)
  *ResBinary42++ = *--Binary42;
}
// source asm-instruction string. Print on error.
char *__SrcStr__;
// convert chars to lowercase 'A'->'a'
char to_lowercase(char a)
{
  if( a<0x5b && a>0x40)
    return a+0x20;
  return a;
}
/// getXxxxxx() functions - calculate specified codes 
static char *CondCode[] ={/// all strings must be minimal 5 byte
"alwa","z\0\0\0","c\0\0\0","s\0\0\0","v\0\0\0","e\0\0\0","gu\0\0","gs\0\0","geu\0","ges\0","r0z\0","r1z\0","r2z\0","r3z\0","r0123z","res\0","neve","nz\0\0","nc\0\0","ns\0\0","nv\0\0","ne\0\0","leu\0","les\0","lu\0\0","ls\0\0","r0nz","r1nz","r2nz","r3nz","r0123nz", NULL };
// get Condition Code, that is after opcode 
int getConditionCode(char **str)
{
 unsigned int *iTable;
 unsigned char *comma = (unsigned char*)*str;
 int i,ret=-1,fIsCC=0;

  union {
  unsigned int  iStr[3];
  char acStr[12];
  } CCName;
  memset(&CCName,0,12);// clear memory

/// is current operand in string a CC? +
  while(is_end_of_line[*comma]==0)// while not 'null' or not 'newline'
    if(*comma++ ==',') {fIsCC=1;break;}
/// -
  if(fIsCC==0) return 0;//current operand is not a CC, so use a default CC=0 - "always"

/// get the condition code string:
  for(i=0;is_part_of_name(*(*str)) && i<8; i++ )
  {
    CCName.acStr[i] = to_lowercase(*(*str));
    (*str)++;
  }
/// serch for CC:
 for(i=0;CondCode[i];i++)
 {
   iTable = (unsigned int *)CondCode[i];
   if(*iTable == CCName.iStr[0])
   {
     if(CondCode[i][4]!=0)
     {
       iTable++;
       if(*iTable == CCName.iStr[1]) { ret = i; break;}
     }
     else { ret = i; break;}
   }
 }
/// have we found a CC?
 if(CondCode[i]==NULL) as_bad (_("unknown condition code `%s'    Source string: %s"),CCName.acStr,__SrcStr__);

 return ret;
}
// get Condition Code for Condition Execution
int getConditionCodeCE(char *cond)
{
 unsigned int *iCond = (unsigned int *)cond;
 unsigned int *iTable;
 int i,ret=0;
 if(*iCond == 0) return ret;// always CC
 for(i=0;CondCode[i];i++)
 {
   iTable = (unsigned int *)CondCode[i];
   if(*iTable == *iCond)
   {
     if(CondCode[i][4]!=0)
     {
       iTable++;
       iCond++;
       if(*iTable == *iCond) { ret = i; break;}
       else iCond--;
     }
     else { ret = i; break;}
   }
 }
 if(CondCode[i]==NULL) as_bad (_("unknown condition code `%s'    Source string: %s"),cond,__SrcStr__);
  return ret;
}
// get sign flag
int getSign(char **str)
{
  int iRet=-1;
  if(*(*str) == 's') 
  {
    iRet = 1;
    (*str)++;
  }
  else    iRet = 0;
  return  iRet ;
}
// get Memory Acces Size
int getMAS(char **str)
{
  int iRet=MAS_DWORD;//default MAS
  if(is_part_of_name((*str)[0]))
  {
    if(is_part_of_name((*str)[1]) && ( (*str)[1]!='_' ) )
    {/// there are two symbols! But MAS is with one defined! So it is a byte swap, what comes after MAS. 
       /// Default MAS will be returned...
    }
    else
    {/// so, here is our MAS: b,w,d
      switch( *(*str) )
      {
         case 'B': 
         case 'b': iRet=MAS_BYTE;  break;
         case 'W': 
         case 'w': iRet=MAS_WORD;  break;
         case 'D': 
         case 'd': iRet=MAS_DWORD; break;
         default: as_bad (_("Unknown Memory Acces Size: `%c'    Source string: %s"),*(*str),__SrcStr__);
      };
      (*str)++;// skip MAS
      if(*(*str) == '_')
      (*str)++;// and skip symbol '_' if it is
    }
  }
  return iRet;
}
int getSwap(char **str)
{
  int iRet = BS_NO; // default swap
  if(is_part_of_name((*str)[0]) && is_part_of_name((*str)[1]))
  {
    if( to_lowercase((*str)[1] ) == 's')
    {
      char test = to_lowercase((*str)[0] );
      if(test == 'b') iRet = BS_BYTE;
      if(test == 'w') iRet = BS_WORD;
    }
  }
  if(iRet != BS_NO) (*str)+=2;
  return iRet;
}
int getExt(int mas, int sign,int pre)
{
  int mPreinc, mMAS, mSIGN, mExt=0, Code=0;

  mPreinc = pre << 3;
  mMAS    = (2-mas) << 1;
  mSIGN   = (1-sign);
  mExt = mPreinc | mMAS | mSIGN ;
  Code=mExt;
//printf("..ExtCode = %d\n", Code);
  if(Code >  0 && Code < 6) return Code-1;
  if(Code >  8 && Code <14) return Code-4;
  if(Code > 16 && Code <22) return Code-7;
  return -1;
}
static char *Registers[] ={/// all strings must be minimal 4 byte
"r0\0","r1\0","r2\0","r3\0","r4\0","r5\0","r6\0","r7\0","u0\0","u1\0","u2\0","u3\0","u4\0","pc\0","st\0","z0\0","u10","u32",NULL};
int getRegister(char **str, char type)
{
  char acSrcOperand[20];
  int i,iRet=-1;
  unsigned int *iRegisters;
  union {
  unsigned int  iStr;
  char acStr[4];
  } RegisterName;
  RegisterName.iStr = 0;
  // clear array:
  memset(acSrcOperand,0,sizeof (acSrcOperand));
  // get register name:
  extract_word (*str, acSrcOperand, sizeof (acSrcOperand));

  if( !is_part_of_name( *(*str) ) ) return iRet;
/// get the register string:
  for(i=0;is_part_of_name(*(*str)) && i<4; i++ )
  {
    RegisterName.acStr[i] = to_lowercase(*(*str));
    (*str)++;
  }
/// compare strings:
  for(i=0;Registers[i];i++)
  {
    iRegisters = (unsigned int *)Registers[i];
    if(*iRegisters == RegisterName.iStr) 
      break;
  }
/// get result for register code: 
  if(i >= 18) as_bad (_("wrong register operand '%s'!    Source string: %s"),acSrcOperand,__SrcStr__);
  else 
  {
   switch(type){
   case 'q':
   case 'd':
   case 'r':// r..,u..,pc,z0,st registers
             if(i<16)
               iRet = i;
             else
               as_bad (_("wrong register operand '%s'!    Source string: %s"),acSrcOperand,__SrcStr__);
             break;

   case 'w':// r.. registers
             if(i<8)
               iRet = i;
             else
               as_bad (_("Offset register '%s' is not internal work register r0-r7!    Source string: %s"),Registers[i],__SrcStr__);
            break;

  case 'D':// r.., u.., st registers for multiplication with 32bit result
           // u10,u32 as pc,z0 registers for multiplication with 64bit result
            if(i==REG_U10)
            {
              iRet = REG_ZERO;
              break;
            }
            if(i==REG_U32)
            {
              iRet = REG_PC;
              break;
            }
            iRet = i;
            break;

  default:// wrong register opcode passed
            as_bad (_("Error at register encoding!    Source string: %s"),__SrcStr__);
            break;
   };
  }
  return iRet;
}
// constants:
// Resolve STR as a constant expression and return the result./
int giWHERE;
unsigned int getEXPRConstant(char *str,char ConstantType)
{
  expressionS ex;
  symbolS *symbolP;
  str = skip_space (str);
  symbolP=symbol_find(str);
  if(symbolP!=NULL)
  {
    input_line_pointer = str;
    expression (& ex);
    if (ex.X_op == O_constant)
      return ex.X_add_number;
    as_bad (_("constant value required"));
    return 0;
  }

  if(*str=='$')str++;
  else {as_bad (_("constant begin symbol '#' or '$' required")); return 0;}
  if(ConstantType=='Y')
  {
	parse_exp (str, &ex);
	fix_new_exp (frag_now, giWHERE,4, &ex, TRUE,  BFD_RELOC_XPIC_Y17_PCREL);
	return 0x00000004;
/*      in the realocation will be: b0100 - byte  access
				    b0010 - word  access
				    b0001 - dword access */
  }
  if(ConstantType=='Z')
  {
	parse_exp (str, &ex);
	fix_new_exp (frag_now, giWHERE,4, &ex, TRUE,  BFD_RELOC_XPIC_Z19);
	return 0;
  }

  if(ConstantType=='H')
  {
    parse_exp (str, &ex);
    fix_new_exp (frag_now, giWHERE,4, &ex, TRUE,  BFD_RELOC_XPIC_H18);
    return 0;
  }

  if(ConstantType=='L')
  {
    parse_exp (str, &ex);
    fix_new_exp (frag_now, giWHERE,4, &ex, TRUE,  BFD_RELOC_XPIC_L14);
    return 0;
  }

  if(ConstantType=='X')
  {
	parse_exp (str, &ex);
	fix_new_exp (frag_now, giWHERE,4, &ex, TRUE,  BFD_RELOC_XPIC_X15_PCREL);
	return 0;
  }

  as_bad (_("constant estimation failed"));
  return 0;
}
int symblen(char *str)
{
  char *start = str;
  while(is_part_of_name(*str)) str++;
  return (int)(str-start);
}
// this function is used in "expr.c"
int XPIC_getConst(char **str)
{
  int i,iStrLen=0,iTemp=0,sign=1;
  unsigned int uiTemp=0;
  char cChar,*strTemp=*str; 
   if(*(*str)=='h') // h1234abcd
   {
    (*str)++;
    iStrLen=symblen(*str);
    if(iStrLen>8){as_bad (_("Can't translate constant '%s' to decimal, too large!  Source string: %s"),strTemp,__SrcStr__); return 0;}
    for(i=0;i<iStrLen;i++)
    {
       cChar=to_lowercase( *(*str));
       (*str)++;
       if( cChar >= 0x61 && cChar <= 0x66 )
            iTemp = (iTemp<<4) |(unsigned int)(cChar-0x61+10);  //a-f
       else if( cChar >= 0x30 && cChar <= 0x39 )
            iTemp =  (iTemp<<4)|(unsigned int)(cChar-0x30);     //0-9
       else {as_bad (_("Can't translate constant '%s' to decimal!  Source string: %s"),strTemp,__SrcStr__); return 0;}
    }
    return iTemp;
   }
   if((*str)[0]=='0' && (*str)[1]=='x') // 0x1234abcd
   {
    (*str) += 2;
    iStrLen=symblen(*str);
    if(iStrLen>8){as_bad (_("Can't translate constant '%s' to decimal, too large!  Source string: %s"),strTemp,__SrcStr__); return 0;}
    for(i=0;i<iStrLen;i++)
    {
       cChar=to_lowercase( *(*str));
       (*str)++;
       if( cChar >= 0x61 && cChar <= 0x66 )
            iTemp = (iTemp<<4) |(unsigned int)(cChar-0x61+10);  //a-f
       else if( cChar >= 0x30 && cChar <= 0x39 ) 
            iTemp =  (iTemp<<4)|(unsigned int)(cChar-0x30);     //0-9
       else {as_bad (_("Can't translate constant '%s' to decimal!  Source string: %s"),strTemp,__SrcStr__); return 0;}
    }
    return iTemp;
   }
   if(*(*str)=='b')// b10010101
   {
    (*str)++;
    iStrLen=symblen(*str);
    if(iStrLen>32)
      {as_bad (_("Can't translate constant '%s' to decimal, too large!  Source string: %s"),strTemp,__SrcStr__); return 0;}
    for(i=0;i<iStrLen;i++)
    {
     cChar=*(*str);
     (*str)++;
     if(cChar == 49) iTemp = (iTemp<<1) | 1;
     else if(cChar == 48) iTemp = (iTemp<<1);
     else {as_bad (_("Can't translate constant '%s' to decimal!  Source string: %s"),strTemp,__SrcStr__); return 0;}
    }
    return iTemp;
   }
//  +/- dec
   if(*(*str) == '-')   { sign=-1; (*str)++;}
   if(*(*str) == '+')   {          (*str)++;}
    iStrLen=symblen(*str);
    if(iStrLen>10){as_bad (_("Can't translate constant '%s' to decimal, too large!  Source string: %s"),strTemp,__SrcStr__); return 0;}
    for(i=iStrLen-1;i>=0;i--)
    {
       int k,j=1;
       for(k=0;k<i;k++) {j*=10;}
       cChar= *(*str) ;
       (*str)++;
       if( cChar >= 0x30 && cChar <= 0x39 )  //0-9
            uiTemp += (unsigned int)(cChar-0x30)*(j);
       else {as_bad (_("Can't translate constant '%s' to decimal!  Source string: %s"),strTemp,__SrcStr__); return 0;}
    }
    if( (uiTemp>0x7fffffff) && (sign==-1) )
      {as_bad (_("Negative constant '%s' don't fit in 32 bit!  Source string: %s"),strTemp,__SrcStr__); return 0;}
    if(sign==-1) return sign * (int)uiTemp; 
    else         return        (int)uiTemp;
}
int getConstant(char *acConstStr, char ConstantType)
{
  int iRet=0;
  if(*acConstStr == 0) return iRet;
  if(*acConstStr == '#') // immediate constants: hex, bin, dec:
  {
    acConstStr++;
    iRet = XPIC_getConst(&acConstStr);
  }
  else
    iRet = getEXPRConstant(acConstStr, ConstantType);
  return iRet;
}
// 17, 18 or 19 bits sign (MSB is sign) constant depending from MAS 
int getYConst(char *acYConstStr,int iMAS)
{
  int iRet;
  iRet=getConstant(acYConstStr,'Y');
  if(iMAS == MAS_BYTE)
  { 
   if( iRet>65535 || iRet<-65536)
    {as_bad (_("constant '%d' don't fit in 17 bits!  Source string: %s"),iRet,__SrcStr__); return 0;}
  } 
  if(iMAS == MAS_WORD)
  { 
   if(iRet&1)
    {as_bad (_("Relative address offset '%d' is not word aligned (Bit0 != 0)!  Source string: %s"),iRet,__SrcStr__); return 0;}
   if( iRet>131071 || iRet<-131072)
    {as_bad (_("constant '%d' don't fit in 18 bits!  Source string: %s"),iRet,__SrcStr__); return 0;}
   iRet=iRet>>1;
  } 
  if(iMAS == MAS_DWORD)
  { 
   if(iRet&3)
   {as_bad (_("Relative address offset '%d' is not dword aligned (Bit1 or/and Bit0 != 0)!  Source string: %s"),iRet,__SrcStr__); return 0;}
   if( iRet>262143 || iRet<-262144)
    {as_bad (_("constant '%d' don't fit in 19 bits!  Source string: %s"),iRet,__SrcStr__); return 0;}
   iRet=iRet>>2;
  } 
  return (iRet)&0x0001ffff;
}
// 18 bits sign constant (MSB is sign)
int getZConst(char **str)
{
  int iRet;
  char acZConstStr[200];
  *str = extract_word(*str,acZConstStr,sizeof(acZConstStr));
  iRet=getConstant(acZConstStr,'Z');
  if( iRet>262143 || iRet<-262144)
  {as_bad (_("constant '%d' don't fit in 19 bits!  Source string: %s"),iRet,__SrcStr__); return 0;}
  else return (iRet)&0x0007ffff;
}
// 18 bits high constant
int getHConst(char **str)
{
  int iRet;
  char acZConstStr[200];
  *str = extract_word(*str,acZConstStr,sizeof(acZConstStr));
  iRet=getConstant(acZConstStr,'H');
  if( iRet>262143 || iRet<-262144)
  {as_bad (_("constant '%d' don't fit in 19 bits!  Source string: %s"),iRet,__SrcStr__); return 0;}
  else return (iRet)&0x0007ffff;
}
// 15 bit  sign constant, dword aligned
int getXConst(char **str)
{
  int iRet;
  char acConstStr[200];
  *str = extract_word(*str,acConstStr,sizeof(acConstStr));
  iRet=getConstant(acConstStr,'X');
  if(iRet&3)
  {as_bad (_("constant '%d' is not dword aligned (Bit1 or/and Bit0 != 0)!  Source string: %s"),iRet,__SrcStr__); return 0;}
  if( iRet>65535 || iRet<-65536)
  {as_bad (_("constant '%d' don't fit in 17 bits!  Source string: %s"),iRet,__SrcStr__); return 0;}
  else return (iRet>>2)&0x00007fff;
}
// 15 bit  unsigned constant, dword aligned
int getWConst(char **str)
{
  int iRet;
  char acConstStr[200];
  *str = extract_word(*str,acConstStr,sizeof(acConstStr));
  iRet=getConstant(acConstStr,'W');
  if(iRet&3)
  {as_bad (_("constant '%d' is not dword aligned (Bit1 or/and Bit0 != 0)!  Source string: %s"),iRet,__SrcStr__); return 0;}
  if( iRet>131071 )
    {as_bad (_("constant '%d' don't fit in 17 bits!  Source string: %s"),iRet,__SrcStr__); return 0;}
  if(iRet<0)
    {as_bad (_("constant '%d' is negative!   Source string: %s"),iRet,__SrcStr__); return 0;}

  return (iRet>>2)&0x00007fff;
}
// 15 bits sign constant (MSB is sign)
int getVConst(char **str)
{
  int iRet;
  char acVConstStr[200];
  *str = extract_word(*str,acVConstStr,sizeof(acVConstStr));
  iRet=getConstant(acVConstStr,'V');
  if( iRet>16383 || iRet<-16384)
  {as_bad (_("constant '%d' don't fit in 15 bits!  Source string: %s"),iRet,__SrcStr__); return 0;}

  return (iRet)&0x00007fff;
}
// 14 bits low constant
int getLConst(char **str)
{
  int iRet;
  char acVConstStr[200];
  *str = extract_word(*str,acVConstStr,sizeof(acVConstStr));
  iRet=getConstant(acVConstStr,'L');
  if( iRet>16383 || iRet<-16384)
  {as_bad (_("constant '%d' don't fit in 15 bits!  Source string: %s"),iRet,__SrcStr__); return 0;}

  return (iRet)&0x00007fff;
}
// 5 bit  unsigned constant
int getUConst(char **str)
{
  int iRet;
  char acConstStr[200];
  *str = extract_word(*str,acConstStr,sizeof(acConstStr));
  iRet=getConstant(acConstStr,'U');
  if( iRet>31 )
    {as_bad (_("constant '%d' don't fit in 5 bits!  Source string: %s"),iRet,__SrcStr__); return 0;}
  if(iRet<0)
    {as_bad (_("constant '%d' is negative!   Source string: %s"),iRet,__SrcStr__); return 0;}
  else return (iRet&0x0000001f);
}
// 2 bit  unsigned constant
int getIConst(char **str)
{
  int iRet;
  char acConstStr[200];
  *str = extract_word(*str,acConstStr,sizeof(acConstStr));
  iRet=getConstant(acConstStr,'I');
  if( iRet>3 )
    {as_bad (_("constant '%d' don't fit in 2 bits!  Source string: %s"),iRet,__SrcStr__); return 0;}
  if(iRet<0)
    {as_bad (_("constant '%d' is negative!   Source string: %s"),iRet,__SrcStr__); return 0;}

  return iRet;
}
// 2 bit  unsigned constant, but not a NULL
int getMConst(char **str)
{
  int iRet;
  char acConstStr[200];
  *str = extract_word(*str,acConstStr,sizeof(acConstStr));
  iRet=getConstant(acConstStr,'M');
  if( iRet>3 || iRet<1)
    {as_bad (_("wrong mask constant '%d'. Just 1, 2 or 3 is allowed!   Source string: %s"),iRet,__SrcStr__); return 0;}

  return iRet;
}
// 25 bits sign relative jump (MSB is sign)
char *getJump(char **line, int where)
{
  expressionS op_expr;
  char *str = skip_space (*line);
  //if(*str=='$')str++;
  str = parse_exp (str, &op_expr);
  fix_new_exp (frag_now, where,4, &op_expr, TRUE,  BFD_RELOC_XPIC_24_PCREL);
//  fix_new_exp (frag_now, where,4, &op_expr, TRUE,  BFD_RELOC_XPIC_LONG_JMP);
  return str;
}

// shift to specific position
void apply_shifts(int *type, int *shift)
{
  if(*shift== -1) { *type = 0; return; }// no shift, then no operator 
  if(*type == -1) return;               // shift was; but operator corrupt! Pass it for next error...
  *type = *type << *shift;
}
/// Each xpic-asm-instruction has several sets of operands.
/// Get all characteristic (1),analyse it and identify
/// operands type (2), and add at the end specific subnumber
int analyse_code(char *str, char *op, int conditional_execution)
{
  uint32_t *ulOp = (uint32_t *)op;
  int iRet=0,n=0;//iRet=1 is error
// flags:
  int fConst=0,fAddr=0,fPC=0,fComma=0,fPlus=0,fMinus=0,fInc=0,fDec=0,fConstInAddr=0,fCurrentlyInAddr=0;
  // NULL, faled:
  if(str==NULL)return 1;
  // 1 get all characteristics flags:
  while(*str != 0)
  {
    switch(*str)   {
    case ',':
      fComma++;
      break;
    case '$':
    case '#':
      if(fComma<2) {
        fConst|=1;
      } else {
        fConst |= 2;
      }
      if (fCurrentlyInAddr) {
        fConstInAddr++;
      }
      break;
    case '[':
      fAddr = 1;
      fCurrentlyInAddr++;
      break;
    case ']':
      fAddr &= 1;
      if (fCurrentlyInAddr <= 0) {
        // Unbalanced Parentheses
        return 1;
      }
      fCurrentlyInAddr--;
      break;
    case 'p': if( str[-1] == '[' && str[1] == 'c'&& str[2] == '+'&& str[3] != '+' ) fPC = 1;break;
    case '+':
      if (str[1] != '+') {
        fPlus++;
      } else {
        fInc++;
        str++;
      }
      break;
    case '-':
      if (str[1] != '-') {
        fMinus++;
      } else {
        fDec++;
        str++;
      }
      break;
    default: break;};
  str++;
  };

  if (fCurrentlyInAddr != 0) {
    // Unbalanced Parentheses
    return 1;
  }

  // dismiss invalid expressions in address operator []
  if (fAddr && !fPC && fConstInAddr && ((fInc || fDec) && (fPlus || fMinus))) {
    return 1;
  }

// 2 (flags are ready) analyse all opcodes here:
  switch(ulOp[0]){
/// 3.2.5 Standard alu commands

  case 0x32646362:/// bcd2hex
  case 0x32786568:/// hex2bcd
                  n=7; goto construction1;

  case 0x75646461:/// addus / addss <- add_usat / add_ssat
  case 0x73646461:
  case 0x75627573:/// subus / subss <- sub_usat / sub_ssat
  case 0x73627573:
                  n=5; goto construction1;
  case 0x6e61656d:/// mean <- arthm
                  n=4; goto construction1;
  case 0x736d6c63:/// clmsb
                  n=5; goto construction1;
  case 0x736e696d:/// mins <- smax
  case 0x756e696d:/// minu <- umax
  case 0x7378616d:/// maxs <- smin
  case 0x7578616d:/// maxu <- umin
  case 0x63627573:/// subc
  case 0x63646461:/// addc
  case 0x726f6e78:/// xnor 
  case 0x646e616e:/// nand
  case 0x706d696e:/// nimp
                  n=4; goto construction1;

  case 0x00766e69:/// inv <- inv1
  case 0x006f6c63:/// clo
  case 0x007a6c63:/// clz
  case 0x00627573:/// sub
  case 0x00656262:/// bbe
  case 0x00766f6d:/// mov
  case 0x00706d69:/// imp
  case 0x00646e61:/// and
  case 0x00726f78:/// xor 
  case 0x00726f6e:/// nor
  case 0x00646461:/// add
                  n=3; goto construction1;
  case 0x00007362:/// bs
  case 0x0000726f:/// or
		  n=2; goto construction1;
  // !!! 3.2.6 Shift and rotate commands with small constant 0 to 31 will mapped in rol/asr/lsr/lsl
  case 0x006c6f72:/// rol / rol_h / rol_l
  case 0x00727361:/// asr / asr_h / asr_l
  case 0x0072736c:/// lsr / lsr_h / lsr_l
  case 0x006c736c:/// lsl / lsl_h / lsl_l
                  n=3;
		  if(fComma!=2){iRet = 1;break;} // must be 3 operand!
                  if((fConst==3) && fAddr && fPC){if(conditional_execution){iRet = 1;break;}op[n]='7'; break;}//        'opcode' reg, [pc + const], sconst
                  if((fConst==3) && fAddr       ){if(conditional_execution){iRet = 1;break;}op[n]='8'; break;}//        'opcode' reg, [const], sconst
                  if((fConst==2) && fAddr && fPC){                                          op[n]='b'; break;}//{[cond]}'opcode' reg, {s}[pc + wreg{++}]{b}, sconst
                  if((fConst==3)                ){if(conditional_execution){iRet = 1;break;}op[n]='9'; break;}//         'opcode' reg, const, sconst
                  if((fConst==2) && fAddr       ){                                          op[n]='c'; break;}//{[cond]} 'opcode' reg, {s}[wreg{++}]{b}, sconst
                  if (fConst==2)                 {                                          op[n]='a'; break;}//{[cond]} 'opcode' reg, reg, sconst

   construction1: if(fComma!=2){iRet = 1;break;} // must be 3 operand!
                  if((fConst==1)&& fAddr && fPC){if(conditional_execution){iRet = 1;break;}op[n]='1'; break;}//        'opcode' reg, [pc + const], reg
                  if((fConst==1)&& fAddr       ){if(conditional_execution){iRet = 1;break;}op[n]='2'; break;}//        'opcode' reg, [const], reg
                  if(              fAddr && fPC){                                          op[n]='5'; break;}//{[cond]}'opcode' reg, {s}[pc + wreg{++}]{b}, reg
                  if((fConst==1)               ){if(conditional_execution){iRet = 1;break;}op[n]='3'; break;}//         'opcode' reg, const, reg
                  if(              fAddr       ){                                          op[n]='6'; break;}//{[cond]} 'opcode' reg, {s}[wreg{++}]{b}, reg
                                                {                                          op[n]='4'; break;}//{[cond]} 'opcode' reg, reg, reg
    case 0x006c726f:/// orl
      n=3;
      if(fComma!=2){iRet = 1;break;} // must be 3 operand!
      if((fConst==1)               ){op[n]='1'; break;}//         'opcode' reg, const, reg
      break;

/// 3.2.7 Multiplication commands
  case 0x756c756d:/// mulu <- umul
  case 0x736c756d:/// muls <- smul
		  if(fComma!=2){iRet = 1;break;} // must be 3 operand!
                  if(fConst==1                 ){op[4]='1'; break;}//         'opcode' reg, const, reg
				  if(!conditional_execution)	{op[4]='2'; break;}//{[cond]} 'opcode' reg, reg, reg
                  iRet = 1;break;
/// 3.2.8 Increment / Decrement commands
  case 0x00636e69:/// inc
  case 0x00636564:/// dec
                  n=3;
		  if(fComma!=2){iRet = 1;break;} // must be 3 operand!
                  if((fConst==3) && fAddr && fPC){if(conditional_execution){iRet = 1;break;}op[n]='1'; break;}//        'opcode' reg, [pc + const], sconst
                  if((fConst==3) && fAddr       ){if(conditional_execution){iRet = 1;break;}op[n]='2'; break;}//        'opcode' reg, [const], sconst
                  if((fConst==2) && fAddr && fPC){                                          op[n]='5'; break;}//{[cond]}'opcode' reg, {s}[pc + wreg{++}]{b}, sconst
                  if((fConst==3)                ){if(conditional_execution){iRet = 1;break;}op[n]='3'; break;}//         'opcode' reg, const, sconst
                  if((fConst==2) && fAddr       ){                                          op[n]='6'; break;}//{[cond]} 'opcode' reg, {s}[wreg{++}]{b}, sconst
                  if (fConst==2)                 {                                          op[n]='4'; break;}//{[cond]} 'opcode' reg, reg, sconst
/// 3.2.9 Load/Store  commands
  case 0x64616f6c:///load
		  if(fComma!=1){iRet = 1;break;} // must be 2 operand!
                  if((fConst==1)&&fAddr) 	{if(conditional_execution){iRet = 1;break;}op[4]='1';break;}// load1: load reg, s[base_reg + #const]b
                  if((fPlus ==0)&&fAddr) 	{if(conditional_execution){iRet = 1;break;}op[4]='1';break;}// load1: load reg, s[base_reg]b
                  if(fConst==1)	 		{if(conditional_execution){iRet = 1;break;}op[4]='2';break;}// load2: load reg, #const
                  if(fAddr&&(fPlus==1)&&!(fInc&&fDec))  {op[4]='3';break;}// load3: {[cond]} load reg, s[base_reg + wreg++]w_bs
                  iRet = 1;break;
  case 0x726f7473:///store
		  if(fComma!=1){iRet = 1;break;} // must be 2 operand!
                  if((fConst==1)&&fAddr)	{if(conditional_execution){iRet = 1;break;}op[5]='1';break;}// store1: [base_reg + const]b, reg
                  if((fPlus ==0)&&fAddr) 	{if(conditional_execution){iRet = 1;break;}op[5]='1';break;}// store1: [base_reg]b
                  if(fAddr&&(fPlus==1)&&!(fInc&&fDec))  {op[5]='2';break;}// store2: {[cond]} store [base_reg + wreg++]w_bs, reg
                  iRet = 1;break;
    case 0x0069686c: /// lhi load hi 18 bit pseudo command
      if(fComma!=1){iRet = 1;break;} // must be 2 operand!
          if(fConst==1)	 		{op[3]='1';break;}// load2: load reg, #const
          iRet = 1;break;
/// 3.2.10 Jump commands
  case 0x00706d6a:if(conditional_execution){iRet = 1;break;}op[3]='1'; break;///jmp cond, label
/*
if(fComma==1)op[3]='2';///jmp cond, label
		  else         op[3]='1';///jmp label
		  break;
*/
  case 0x64706d6a:if(conditional_execution){iRet = 1;break;}op[6]='1';break;///jmpdec <- jmp_dec

/// 3.2.11 Special commands
  case 0x00656973:                ///sie
                  if(conditional_execution){iRet = 1;break;}
                  if(fComma==1)
                    op[3]='2';    /// sie iem, iev
                  else
                    op[3]='1';    /// sie wreg
                            break;

  case 0x00656967:                ///gie
  case 0x00706f6e:if(conditional_execution){iRet = 1;break;}op[3]='1';break;///nop

  case 0x65697367:                ///gsie
  case 0x66746572:                ///retf <- rfirq
  case 0x69746572:                ///reti <- rirq
  case 0x696c6572:if(conditional_execution){iRet = 1;break;}op[4]='1';break;///reli <- release_irq

  case 0x61657262:if(conditional_execution){iRet = 1;break;}op[5]='1';break;///break

  case 0x726c6572:if(conditional_execution){iRet = 1;break;}op[7]='1';break;///relreti <- release_rirq

  default:        iRet = 1; break;
  };
  return iRet;
}

/// main assembly routine!!!!!!!!!!!!!!!!!!!!!
void md_assemble (char *str) 
{
  struct xpic_opcodes_s *opcode;
  int bin_opcode;
  __SrcStr__=str;
  char op[15]={0};
  char cond[10]={0};

  char *frag; //= frag_more (opcode->insn_size);// x2
  int where; //= frag - frag_now->fr_literal;// ????

//char strchar[] = "write_page \0\1\n";
//unsigned int A=getEPRConstant(strchar);

  if(str[0]=='[')
  {
    str = extract_word (++str, cond, sizeof (cond));
    str = skip_space(str);
    str++;// skip the ']'
  }
  str = skip_space (extract_word (str, op, sizeof (op)));

  if (!op[0])
    as_bad (_("can't find instruction opcode!  Source string: %s"),__SrcStr__);

  if(analyse_code(str,op, cond[0] != 0))
    {as_bad (_("wrong opcode or invalid combination of operands! Source string: %s"),__SrcStr__);return;}
/// serch algorithm +
  opcode = xpic_opcodes;
  while(opcode->insn_size != 0)
 {
   if(strcmp(opcode->name,op)==0)
   {
     break;
   }
   opcode++;
 }
 if(opcode->name == NULL)
 {
      as_bad (_("unknown opcode `%s'.  Source string: '%s'"), op,__SrcStr__);
      return;
 }

 bin_opcode = opcode->bin_opcode;
/// end serch algorithm -

 frag  = frag_more (opcode->insn_size);
 where = frag - frag_now->fr_literal;
 giWHERE = where;

/// Pass current instruktion for ISA?
  if(!xpic_opt.all_opcodes )
  {
//    if((opcode->isa & xpic_mcu->isa) != opcode->isa)
    if(xpic_mcu->isa < opcode->isa )
      as_bad (_("Illegal opcode %s for mcu %s!  Source string: %s"), opcode->name, xpic_mcu->name,__SrcStr__);

  }

  /* We used to set input_line_pointer to the result of get_operands,
     but that is wrong.  Our caller assumes we don't change it.  */
  {
  char *t = input_line_pointer;
/// work with operands +
  char *constrain = opcode->constraints;
  char *strOpcode = opcode->opcode;
  unsigned int bin=0;// = opcode->bin_opcode;
  static int PreviousDreg=REG_ZERO;
  // at first,no operands-opcode and no shifts for operands:
  int iDreg=-1,iR1reg=-1,iW2reg=-1,iQ2reg=-1;
  int sDreg=-1,sR1reg=-1,sW2reg=-1,sQ2reg=-1;
  int iZconst=-1,iYconst=-1,iXconst=-1,iWconst=-1,iVconst=-1,iUconst=-1,iMconst=-1,iIconst=-1;
  int sZconst=-1,sYconst=-1,sXconst=-1,sWconst=-1,sVconst=-1,sUconst=-1,sMconst=-1,sIconst=-1;
  int iCond=-1,iSign=-1,iMAS=-1,iSwap=-1         ,iEmask=0, iPreinc=0;
  int sCond=-1,sSign=-1,sMAS=-1,sSwap=-1,sJump=-1,sEmask=-1;

  char acYConstStr[100];
  int i,j; 
  char iLastOpcodeInFor=0;
/// get current operand types and calculate shifts +
   if(strlen(strOpcode)!=32)as_bad (_("Bad opcode! Don't fit the architecture!   Source string: %s"),__SrcStr__);
  for(i=0,j=31; j>=0; j--,i++)
  {
    if(iLastOpcodeInFor == strOpcode[j]) continue;
    switch(strOpcode[j])
    {
// constants:
      case 'I': sIconst =i;break;
      case 'M': sMconst =i;break;
      case 'T':
      case 'u':
      case 'U': sUconst =i;break;
      case 'V': sVconst =i;break;
      case 'W': sWconst =i;break;
      case 'X': sXconst =i;break;
      case 'Y': sYconst =i;break;
      case 'Z': sZconst =i;break;
// registers:
      case 'q': sQ2reg  =i;break;
      case 'D':
      case 'd': sDreg   =i;break;
      case 'r': sR1reg  =i;break;
      case 'w': sW2reg  =i;break;
// sign, swap, mas, MSP, ...
      case 's': sSign   =i;break;
      case 'm': sMAS    =i;break;
      case 'e': sEmask  =i;break;
      case 'S': sSwap   =i;break;
      case 'j': sJump   =i;break;
      case 'v':
      case 'C':
      case 'c': sCond   =i;break;
// 1, 0, x
      default:  break;
    };
    iLastOpcodeInFor = strOpcode[j];
  };
/// end get current operand types and calculate shifts -

/// evaluate instruction operands +
  while(*constrain)
  {
    str = skip_space (str);
 //   if(*str == ']') constrain++;
    switch(*constrain)
    {
// registers:
      case 'w': iW2reg = getRegister(&str,*constrain);	break;	/// Add reg usr10/usr32!!!!
      case 'q': iQ2reg = getRegister(&str,*constrain);	break;
      case 'r': iR1reg = getRegister(&str,*constrain);	break;
      case 'D':
      case 'd': iDreg  = getRegister(&str,*constrain);	break;
// constants:
      case 'I': iIconst= getIConst(&str);		break;
      case 'M': iMconst= getMConst(&str);		break;
      case 'T': iUconst= getUConst(&str);
                if(iUconst>15)as_bad (_("constant '%d' don't fit in 4 bit!  Source string: %s"),iUconst,__SrcStr__);
		break;
      case 'U': iUconst= getUConst(&str);		break;
      case 'V': iVconst= getVConst(&str);		break;
      case 'L': iVconst= getLConst(&str);       break;
      case 'W': iWconst= getWConst(&str);		break;
      case 'X': iXconst= getXConst(&str);		break;
      case 'Y': str=extract_word(str,acYConstStr,sizeof(acYConstStr)); break;
      case 'Z': iZconst= getZConst(&str);		break;
      case 'H': iZconst= getHConst(&str);       break;
// flags:
      case 'j': str  = getJump(&str,where);		break;
      case 'C': iCond  = getConditionCodeCE(cond);	break;
      case 'c': iCond  = getConditionCode(&str);	break;	/// Not tested!!!!
      case 'v': iCond  = getConditionCode(&str)-26;
		if(iCond<0 || iCond>3)as_bad (_("invalid condition code! Allowed here 'r0nz', 'r1nz', 'r2nz', 'r3nz'    Source string: %s"),__SrcStr__);
	break;	/// Not tested!!!!

      case 's': iSign  = getSign(&str);			break;

      case 'p': if(str[0] == '-' && str[1] == '-')
                { str++;str++; iPreinc = PP_PREDECREMENT;}
                break;
      case 'P': if(str[0] == '+' && str[1] == '+')
                { str++;str++; iPreinc = PP_POSTINCREMENT;}
                break;
      case 'm': iMAS  = getMAS(&str);                  break;
      case 'S': iSwap = getSwap(&str);                 break;
// symbols:
      case ']': if(*str!=']')as_bad (_("wrong opcode!    Source string: %s"),__SrcStr__);
                str++;
                if( (iW2reg == -1) && (sW2reg != -1) )
                {
                  iW2reg = iR1reg; 
                  iR1reg=REG_ZERO;
                  if(iW2reg>7)
                    as_bad (_("offset register is no internal work register r0-r7!    Source string: %s"),__SrcStr__);
                } 
                // data dependence test:
                if( (PreviousDreg!=REG_ZERO  && PreviousDreg!=REG_PC) && ( (PreviousDreg==iR1reg) ||  (PreviousDreg==iW2reg)) )
		  as_bad (_("data dependence!  Source string: %s"),__SrcStr__);
                break;
      case '+': if(*str == '+') str++;                 break;
      case '[': if(*str == '[') str++;                 break;
      case ',':	
                if(str[-1] == ']' && str[1] == ','){
                  if(str[0] == 'd')  str++;
                  else as_bad (_("undefined symbol '%c'. Source string: %s"),*str,__SrcStr__);
                }
				       		       break;
      case 'x': str= str + symblen(str);	       break;
      default:as_bad (_("invalid instruction! Don't fit the constrain!  Source string: %s"),__SrcStr__);
    }
    constrain++;
  };
/// Some constants processing:
// 'Y':
 if(sYconst!=-1)iYconst = getYConst(acYConstStr,iMAS);
// 'U':
 if(iUconst>15) {iUconst -=16;bin_opcode -= 0x0c000000;}//change opcode from 'xxx_l' to 'xxx_h'
/// Some tests here:
// for 'data dependence' test at "switch(*constrain)" in " case ']':"
  if(iDreg==-1 || ((opcode->bin_opcode & 0xa0000000) == 0xa0000000) )
       PreviousDreg=REG_ZERO;
  else PreviousDreg=iDreg;
// byte swap command in combination with byte access option 'b_bs' and 'w_ws'
  if(  (iSwap == BS_BYTE) && (iMAS == MAS_BYTE)  )
    {
      as_bad (_("Byte swap command in combination with byte access option 'b_bs'!  Source string: %s"),__SrcStr__);
      iSwap=-1;
    }
  if(  (iSwap == BS_WORD) && (iMAS == MAS_WORD)  )
    {
      as_bad (_("Word swap command in combination with word access option 'w_ws'!  Source string: %s"),__SrcStr__);
      iSwap=-1;
    }
// sign dword data acceess not allowed:
  if(  (iSign == 1) && (iMAS == MAS_DWORD)  )
    {
      as_bad (_("Sign extention only allowed for byte(8 bit) or word(16 bit) data acccess!  Source string: %s"),__SrcStr__);
      iSign=-1;
    }
/// end evaluate instruction operands -
/// end work with operands -

/// build instruction opcode +
// check all shifts first:
//regs
  apply_shifts(&iDreg, &sDreg);
  apply_shifts(&iR1reg, &sR1reg);
  apply_shifts(&iW2reg, &sW2reg);
  apply_shifts(&iQ2reg, &sQ2reg);
//constants:
  apply_shifts(&iIconst, &sIconst);
  apply_shifts(&iMconst, &sMconst);
  apply_shifts(&iUconst, &sUconst);
  apply_shifts(&iVconst, &sVconst);
  apply_shifts(&iWconst, &sWconst);
  apply_shifts(&iXconst, &sXconst);
  apply_shifts(&iYconst, &sYconst);
  apply_shifts(&iZconst, &sZconst);
//flags:
  apply_shifts(&iSwap, &sSwap);
  apply_shifts(&iCond, &sCond);
 /// apply_shifts(&iJump, &sJump);
// triple-Emask: MASS-SIGN-PREINC/POSTDEC
  if(sEmask==-1)
  {// no triple-Emask
    apply_shifts(&iSign, &sSign);
    apply_shifts(&iMAS,  &sMAS);
  }
  else
  {// triple-Emask must be evalueted:
    // were processed all components? 
    if( (iSign | iMAS) == -1 )
       // something was wrong! opcode in xpic.h or bed asm-instruction, or my bug:)
       as_bad (_("Wrong opcode or asm-instruction!  Source string: %s"),__SrcStr__);
    else
    {  // all OK! create mask:
      iEmask = getExt(iMAS,iSign,iPreinc);
      if(iEmask < 0) as_bad (_("Wrong combination of 'mas', 'sign' and 'preinc/postdec'!  Source string: %s"),__SrcStr__);
    }
    iSign = 0;
    iMAS  = 0;
  }
  apply_shifts(&iEmask, &sEmask);

/// so, build opcode:
  bin = bin_opcode | iDreg  | iR1reg  | iW2reg  | iQ2reg  |\
                    iIconst | iMconst | iWconst | iXconst | iYconst | iZconst | iVconst | iUconst |\
                      iSwap |  iCond  |   iSign |  iMAS   | iEmask  ;
/// end build instruction opcode -

/// test end of line:
    if (*skip_space (str))
      as_bad (_("garbage at end of line!  Source string: %s"),__SrcStr__);
    input_line_pointer = t;

/// print out instruction:
    {
      dwarf2_emit_insn (4);
      bfd_putl32 ((bfd_vma) bin, frag); // write to output!!!!!!!!!!!!

	if(printinstruction==1)
	printf("0x%08x\n",bin) ;
	if(printinstruction==2)
	printf("0x%08x  . . . .  %s\n",bin,__SrcStr__) ;
	if(printinstruction==3)
        { char buf[42];buf[41]=0;
          GetBinary(bin,buf);
	  printf("0x%08x  . . . .  %s  . . . .  %s\n",bin,buf,__SrcStr__) ;
        }

  }
 }
}


/* Flag to pass `pm' mode between `xpic_parse_cons_expression' and
   `xpic_cons_fix_new'.  */
static int exp_mod_pm = 0;

/* Parse special CONS expression: pm (expression)
   or alternatively: gs (expression).
   These are used for addressing program memory.
   Relocation: BFD_RELOC_AVR_16_PM.  */

void xpic_parse_cons_expression (expressionS *exp, int nbytes)
{
  char *tmp;

  exp_mod_pm = 0;

  tmp = input_line_pointer = skip_space (input_line_pointer);

  if (nbytes == 2)
    {
      char *pm_name1 = "pm";
      char *pm_name2 = "gs";
      int len = strlen (pm_name1);
      /* len must be the same for both pm identifiers.  */

      if (strncasecmp (input_line_pointer, pm_name1, len) == 0
          || strncasecmp (input_line_pointer, pm_name2, len) == 0)
	{
	  input_line_pointer = skip_space (input_line_pointer + len);

	  if (*input_line_pointer == '(')
	    {
	      input_line_pointer = skip_space (input_line_pointer + 1);
	      exp_mod_pm = 1;
	      expression (exp);

	      if (*input_line_pointer == ')')
		++input_line_pointer;
	      else
		{
		  as_bad (_("`)' required"));
		  exp_mod_pm = 0;
		}

	      return;
	    }

	  input_line_pointer = tmp;
	}
    }

  expression (exp);
}

void xpic_cons_fix_new (fragS *frag, int where, int nbytes, expressionS *exp)
{
  if (exp_mod_pm == 0)
    {
      if (nbytes == 2)
	fix_new_exp (frag, where, nbytes, exp, FALSE, BFD_RELOC_16);
      else if (nbytes == 4)
	fix_new_exp (frag, where, nbytes, exp, FALSE, BFD_RELOC_32);
      else
	as_bad (_("illegal %srelocation size: %d"), "", nbytes);
    }
  else
    {
      if (nbytes == 2)
	fix_new_exp (frag, where, nbytes, exp, FALSE, BFD_RELOC_AVR_16_PM);
      else
	as_bad (_("illegal %srelocation size: %d"), "`pm' ", nbytes);
      exp_mod_pm = 0;
    }
}

