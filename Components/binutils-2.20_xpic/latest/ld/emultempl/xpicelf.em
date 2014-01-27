# This shell script emits a C file. -*- C -*-
#   Copyright 2006, 2007, 2008, 2009
#   Free Software Foundation, Inc.
#
# This file is part of the GNU Binutils.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street - Fifth Floor, Boston,
# MA 02110-1301, USA.


# This file is sourced from elf32.em, and defines extra xpic-elf specific
# routines.  It is used to generate the trampolines for the xpic6 family
# of devices where one needs to address the issue that it is not possible
# to reach the whole program memory by using 16 bit pointers.

fragment <<EOF

#include "elf32-xpic.h"
#include "ldctor.h"

/* The fake file and it's corresponding section meant to hold
   the linker stubs if needed.  */

static lang_input_statement_type *stub_file;
static asection *xpic_stub_section;

/* Variables set by the command-line parameters and transfered
   to the bfd without use of global shared variables.  */

static bfd_boolean xpic_no_stubs = FALSE;
static bfd_boolean xpic_debug_relax = FALSE;
static bfd_boolean xpic_debug_stubs = FALSE;
static bfd_boolean xpic_replace_call_ret_sequences = TRUE;
static bfd_vma xpic_pc_wrap_around = 0x10000000;

/* Transfers information to the bfd frontend.  */

static void
xpic_elf_set_global_bfd_parameters (void)
{
  elf32_xpic_setup_params (& link_info,
                          stub_file->the_bfd,
                          xpic_stub_section,
                          xpic_no_stubs,
                          xpic_debug_stubs,
                          xpic_debug_relax,
                          xpic_pc_wrap_around,
                          xpic_replace_call_ret_sequences);
}


/* Makes a conservative estimate of the trampoline section size that could
   be corrected later on.  */

static void
xpic_set_options_before_parse (void)
{
  // base options, comes from elf32.em, function: gld${EMULATION_NAME}_before_parse (void)
  ldfile_set_output_arch ("xpic", bfd_arch_xpic);
  config.dynamic_link = TRUE;
  config.has_shared = FALSE;

// XPIC options:
  // turn off page align data (ld option -N)
  config.text_read_only = FALSE;
  config.magic_demand_paged = FALSE;
  config.dynamic_link = FALSE;

  // always relax             (ld option --relax)
  command_line.relax = TRUE;

}

static void
xpic_elf_${EMULATION_NAME}_before_allocation (void)
{
  int ret;

  // XPIC: keep all .data sections for possible LongJump and FarLoad
  asection *sec=link_info.output_bfd->sections;
  while(sec!=0)
  {
    if( (sec->flags & (SEC_ALLOC|SEC_LOAD|SEC_DATA|SEC_HAS_CONTENTS))  == (SEC_ALLOC|SEC_LOAD|SEC_DATA|SEC_HAS_CONTENTS))
    {
      // if this is data section, keep it and align to 4byte
      sec->flags|= SEC_KEEP;
      sec->alignment_power = 2;
    }
    // go to next section
    sec=sec->next;
  }

  gld${EMULATION_NAME}_before_allocation ();

  /* We only need stubs for the xpic6 family.  */
  if (strcmp ("${EMULATION_NAME}","xpic6"))
    xpic_no_stubs = TRUE;

  xpic_elf_set_global_bfd_parameters ();

  /* If generating a relocatable output file, then
     we don't  have to generate the trampolines.  */
  if (link_info.relocatable)
    xpic_no_stubs = TRUE;

  if (xpic_no_stubs)
    return;

  ret = elf32_xpic_setup_section_lists (link_info.output_bfd, &link_info);

  if (ret < 0)
    einfo ("%X%P: can not setup the input section list: %E\n");

  if (ret <= 0)
    return;

  /* Call into the BFD backend to do the real "stub"-work.  */
  if (! elf32_xpic_size_stubs (link_info.output_bfd, &link_info, TRUE))
    einfo ("%X%P: can not size stub section: %E\n");
}

/* This is called before the input files are opened.  We create a new
   fake input file to hold the stub section and generate the section itself.  */

static void
xpic_elf_create_output_section_statements (void)
{
  flagword flags;

  stub_file = lang_add_input_file ("linker stubs",
                                   lang_input_file_is_fake_enum,
                                   NULL);

  stub_file->the_bfd = bfd_create ("linker stubs", link_info.output_bfd);
  if (stub_file->the_bfd == NULL
      || !bfd_set_arch_mach (stub_file->the_bfd,
                             bfd_get_arch (link_info.output_bfd),
                             bfd_get_mach (link_info.output_bfd)))
    {
      einfo ("%X%P: can not create stub BFD %E\n");
      return;
    }

  /* Now we add the stub section.  */

  flags = (SEC_ALLOC | SEC_LOAD | SEC_READONLY | SEC_CODE
           | SEC_HAS_CONTENTS | SEC_RELOC | SEC_IN_MEMORY | SEC_KEEP);
  xpic_stub_section = bfd_make_section_anyway_with_flags (stub_file->the_bfd,
							 ".trampolines",
							 flags);
  if (xpic_stub_section == NULL)
    goto err_ret;

  xpic_stub_section->alignment_power = 1;

  ldlang_add_file (stub_file);

  return;

  err_ret:
   einfo ("%X%P: can not make stub section: %E\n");
   return;
}

/* Re-calculates the size of the stubs so that we won't waste space.  */

static void
xpic_elf_after_allocation (void)
{
  if (!xpic_no_stubs && !command_line.relax)
    {
      /* If relaxing, elf32_xpic_size_stubs will be called from
	 elf32_xpic_relax_section.  */
      if (!elf32_xpic_size_stubs (link_info.output_bfd, &link_info, FALSE))
	einfo ("%X%P: can not size stub section: %E\n");
    }

  gld${EMULATION_NAME}_after_allocation ();

  /* Now build the linker stubs.  */
  if (!xpic_no_stubs)
    {
      if (!elf32_xpic_build_stubs (&link_info))
	einfo ("%X%P: can not build stubs: %E\n");
    }
}


EOF


PARSE_AND_LIST_PROLOGUE='

#define OPTION_NO_CALL_RET_REPLACEMENT 301
#define OPTION_PMEM_WRAP_AROUND        302
#define OPTION_NO_STUBS                303
#define OPTION_DEBUG_STUBS             304
#define OPTION_DEBUG_RELAX             305
'

PARSE_AND_LIST_LONGOPTS='
  { "no-call-ret-replacement", no_argument,
     NULL, OPTION_NO_CALL_RET_REPLACEMENT},
  { "pmem-wrap-around", required_argument,
    NULL, OPTION_PMEM_WRAP_AROUND},
  { "no-stubs", no_argument,
    NULL, OPTION_NO_STUBS},
  { "debug-stubs", no_argument,
    NULL, OPTION_DEBUG_STUBS},
  { "debug-relax", no_argument,
    NULL, OPTION_DEBUG_RELAX},
'

PARSE_AND_LIST_OPTIONS='
  fprintf (file, _("  --pmem-wrap-around=<val>    "
		   "Make the linker relaxation machine assume that a\n"
		   "                              "
		   "  program counter wrap-around occures at address\n"
		   "                              "
		   "  <val>.  Supported values: 8k, 16k, 32k and 64k.\n"));
  fprintf (file, _("  --no-call-ret-replacement   "
		   "The relaxation machine normally will\n"
		   "                              "
		   "  substitute two immediately following call/ret\n"
		   "                              "
		   "  instructions by a single jump instruction.\n"
		   "                              "
		   "  This option disables this optimization.\n"));
  fprintf (file, _("  --no-stubs                  "
		   "If the linker detects to attempt to access\n"
		   "                              "
		   "  an instruction beyond 128k by a reloc that\n"
		   "                              "
		   "  is limited to 128k max, it inserts a jump\n"
		   "                              "
		   "  stub. You can de-active this with this switch.\n"));
  fprintf (file, _("  --debug-stubs               "
		   "Used for debugging xpic-ld.\n"));
  fprintf (file, _("  --debug-relax               "
		   "Used for debugging xpic-ld.\n"));
'

PARSE_AND_LIST_ARGS_CASES='

    case OPTION_PMEM_WRAP_AROUND:
      {
        /* This variable is defined in the bfd library.  */
        if ((!strcmp (optarg,"32k"))      || (!strcmp (optarg,"32K")))
          xpic_pc_wrap_around = 32768;
        else if ((!strcmp (optarg,"8k")) || (!strcmp (optarg,"8K")))
          xpic_pc_wrap_around = 8192;
        else if ((!strcmp (optarg,"16k")) || (!strcmp (optarg,"16K")))
          xpic_pc_wrap_around = 16384;
        else if ((!strcmp (optarg,"64k")) || (!strcmp (optarg,"64K")))
          xpic_pc_wrap_around = 0x10000;
        else
          return FALSE;
      }
      break;

    case OPTION_DEBUG_STUBS:
      xpic_debug_stubs = TRUE;
      break;

    case OPTION_DEBUG_RELAX:
      xpic_debug_relax = TRUE;
      break;

    case OPTION_NO_STUBS:
      xpic_no_stubs = TRUE;
      break;

    case OPTION_NO_CALL_RET_REPLACEMENT:
      {
        /* This variable is defined in the bfd library.  */
        xpic_replace_call_ret_sequences = FALSE;
      }
      break;
'

#
# Put these extra xpic-elf routines in ld_${EMULATION_NAME}_emulation
#
LDEMUL_BEFORE_ALLOCATION=xpic_elf_${EMULATION_NAME}_before_allocation
LDEMUL_AFTER_ALLOCATION=xpic_elf_after_allocation
LDEMUL_CREATE_OUTPUT_SECTION_STATEMENTS=xpic_elf_create_output_section_statements
