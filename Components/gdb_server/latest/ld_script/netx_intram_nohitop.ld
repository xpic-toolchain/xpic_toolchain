/*---------------------------------------------------------------------------
  Author : Christoph Thelen

           Hilscher GmbH, Copyright (c) 2010, All Rights Reserved

           Redistribution or unauthorized use without expressed written 
           agreement from the Hilscher GmbH is forbidden
---------------------------------------------------------------------------*/

/*
   netX10 has no physical Tightly Coupled Memory (TCM).
   All three ARM channels (system, dTCM and iTCM channels) are mapped to INTRAM.
   dTCM and iTCM MUST point into DIFFERENT INTRAM segments!   
   INTRAM0 is used by ROM loader, so it may not be overwritten during boot process.
   INTRAM1 is used by ROM loader only in DPM boot mode.
   
   INTRAM1 and INTRAM2 is used for ARM program
   INTRAM3 (64k) is reserved for xPIC unit (program/data)
   INTRAM4 (32k) is reserved for XC unit (eddbuffer)
*/


ENTRY(start)

MEMORY
{
  INTRAM0(rwx)       : ORIGIN = 0x08000000, LENGTH = 0x00010000
  INTRAM12(rwx)      : ORIGIN = 0x08010000, LENGTH = 0x00020000
  INTRAM3(rwx)       : ORIGIN = 0x08030000, LENGTH = 0x00010000
  INTRAM4(rwx)       : ORIGIN = 0x08040000, LENGTH = 0x00008000
}


/* Section Definitions */
SECTIONS
{
  .stack (NOLOAD):
  {
    . = . + 0x4000;
    stack_top = . ;
  } > INTRAM0
  
  
  .bss (NOLOAD):
  {
    /* zero-initialized data */
    __bss_start__ = . ;

    *(.bss)
    *(COMMON)

    __bss_end__ = ALIGN(4) ;
  } > INTRAM0
    
    
  PROVIDE(_heap_start = ALIGN(16));
  . = ORIGIN(INTRAM0)+LENGTH(INTRAM0) ;
  PROVIDE(_heap_end = . );
    
    
  .code :
  {
    *(.init_code)
    *(.text)
    *(.glue*)
    *(.vfp11_veneer)
    *(.v4_bx)
  } > INTRAM12
  
  
  .data :
  {
    __data_start__ = . ;

    /* read only data */
    *(.rodata*)

    /* initialized read/write data */
    *(.data)
    SORT(CONSTRUCTORS)
    
    __data_end__ = . ;
  } > INTRAM12
  
  
  . = ALIGN(4);
  _edata = . ;
  PROVIDE (edata = .);

  _end = .;
  PROVIDE (end = .);
  

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
