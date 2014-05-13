
MEMORY
{
  INT_PRAM   (rx)   : ORIGIN = 0x10180000, LENGTH = 8K
  INT_DRAM   (rw!x) : ORIGIN = 0x10280000, LENGTH = 6K
  INTRAM34   (rwx)  : ORIGIN = 0x08050000, LENGTH = 0x00020000
  SDRAM      (rwx)  : ORIGIN = 0x80000000, LENGTH = 0x00800000
}

/* 2k reserved at end of DRAM for stack area */

SECTIONS
{
    .text_local 0x00000000 :
    {
    } AT > INT_PRAM

    .data_local 0x00000000 :
    {
    } AT > INT_DRAM
            
    PROVIDE (__stack_top_std = 0x00001f00);
    PROVIDE (__stack_top_fiq = 0x00002000);
    
    .text :
    {
      *(.text .rodata* .data*)
      *(.constants)
      . = ALIGN(4) ;
    } > SDRAM
    
    
    .bss (NOLOAD) :
    {
      /* zero-initialized data */
      . = ALIGN(4);
      __bss_start = . ;
      
      *(.bss)
      *(COMMON)
      *(.trampolines)
      __bss_end = ALIGN(4) ;
    } > SDRAM

    /* location of the heap for malloc() etc.
     * malloc assumes that heap start is 8 byte aligned */
    PROVIDE (_heap_start = ALIGN(16));
    PROVIDE (_heap_end = ORIGIN(INTRAM34)+LENGTH(INTRAM34));
} 
