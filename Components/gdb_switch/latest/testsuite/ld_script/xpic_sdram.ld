
MEMORY
{
  INT_PRAM   (rx)   : ORIGIN = 0x1018c000, LENGTH = 8K
  INT_DRAM   (rw!x) : ORIGIN = 0x1018e000, LENGTH = 6K
  SDRAM      (rwx)  : ORIGIN = 0x80000000, LENGTH = 0x00800000
}

/* 2k reserved at end of DRAM for stack area */

SECTIONS
{
    .text_local 0x00000000 :
    {
      *init_xpic.o (.text)
      . = ALIGN(4) ;
    } AT > INT_PRAM

    .data_local 0x00000000 :
    {
      *init_xpic.o (.rodata* .data .bss)
      . = ALIGN(4) ;
    } AT > INT_DRAM
            
    .text :
    {
      *(.text .rodata* .data*)
      
      . = ALIGN(4) ;
    } > SDRAM
    
    .bss (NOLOAD) :
    {
      /* zero-initialized data */
      __bss_start = . ;
      
      *(.bss)
      *(COMMON)
      *(.trampolines)
      
      __bss_end = ALIGN(4) ;
    } > SDRAM
    
    . = ALIGN(16) + 0x100000 ;
    PROVIDE (__stack_top_std = .);
    . = . + 0x1000 ;
    PROVIDE (__stack_top_fiq = .);
    PROVIDE (_heap_start = ALIGN(16));
    PROVIDE (_heap_end = ORIGIN(SDRAM)+LENGTH(SDRAM));
    
  /* Stabs debugging sections.  */
  .stab          0 : { *(.stab) }
  .stabstr       0 : { *(.stabstr) }
  .stab.excl     0 : { *(.stab.excl) }
  .stab.exclstr  0 : { *(.stab.exclstr) }
  .stab.index    0 : { *(.stab.index) }
  .stab.indexstr 0 : { *(.stab.indexstr) }
  .comment       0 : { *(.comment) }
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
  .debug_info     0 : { *(.debug_info .gnu.linkonce.wi.*) }
  .debug_abbrev   0 : { *(.debug_abbrev) }
  .debug_line     0 : { *(.debug_line) }
  .debug_frame    0 : { *(.debug_frame) }
  .debug_str      0 : { *(.debug_str) }
  .debug_loc      0 : { *(.debug_loc) }
  .debug_macinfo  0 : { *(.debug_macinfo) }
  /* SGI/MIPS DWARF 2 extensions */
  .debug_weaknames 0 : { *(.debug_weaknames) }
  .debug_funcnames 0 : { *(.debug_funcnames) }
  .debug_typenames 0 : { *(.debug_typenames) }
  .debug_varnames  0 : { *(.debug_varnames) }
} 
