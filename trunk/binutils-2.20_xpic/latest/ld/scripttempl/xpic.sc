cat <<EOF
OUTPUT_FORMAT("${OUTPUT_FORMAT}","${OUTPUT_FORMAT}","${OUTPUT_FORMAT}")
OUTPUT_ARCH(${ARCH})

MEMORY
{
  INT_PRAM   (rx)   : ORIGIN = 0x1018c000, LENGTH = 8K
  INT_DRAM   (rw!x) : ORIGIN = 0x1018e000, LENGTH = 6K
}

/* 2k reserved at end of DRAM for stack area */

SECTIONS
{
    .text 0x00000000:
    {
      *init_xpic.o (.text)
      *(.text)
    } AT > INT_PRAM

    .data 0x00000000:
    {
      *(.data)
      *(.constants)
      *(.rodata*)
      *(COMMON)
      *(.bss)
    } AT > INT_DRAM

    PROVIDE (__stack_top_std = 0x00001f00);
    PROVIDE (__stack_top_fiq = 0x00002000);


 
  /* DWARF debug sections.
     Symbols in the DWARF debugging sections are relative to the beginning
     of the section so we begin them at 0.  */

  /* DWARF 1 */
  .debug          0 : { *(.debug) }
  .line           0 : { *(.line) }

  /* GNU DWARF 1 extensions */
  .debug_srcinfo  0 : { *(.debug_srcinfo) }
  .debug_sfnames  0 : { *(.debug_sfnames) }

  /* DWARF 1.1 and DWARF 2 */
  .debug_aranges  0 : { *(.debug_aranges) }
  .debug_pubnames 0 : { *(.debug_pubnames) }

  /* DWARF 2 */
  .debug_info     0 : { *(.debug_info) *(.gnu.linkonce.wi.*) }
  .debug_abbrev   0 : { *(.debug_abbrev) }
  .debug_line     0 : { *(.debug_line) }
  .debug_frame    0 : { *(.debug_frame) }
  .debug_str      0 : { *(.debug_str) }
  .debug_loc      0 : { *(.debug_loc) }
  .debug_macinfo  0 : { *(.debug_macinfo) }
}
EOF

