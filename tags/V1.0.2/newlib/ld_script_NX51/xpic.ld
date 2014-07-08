
MEMORY
{
  INT_PRAM   (rx)   : ORIGIN = 0x10180000, LENGTH = 8K
  INT_DRAM   (rw!x) : ORIGIN = 0x10280000, LENGTH = 6K
  INTRAM3    (rwx)  : ORIGIN = 0x08050000, LENGTH = 0x00010000
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
      /* Avoid a zero-pointer  */
      . += 4;
            
      /* Data                  */
      *(.data* .rodata*)
      
      /* zero-initialized data */
      /* align to 4 byte       */
      . = ALIGN(4);
      __bss_start = . ;      
      *(.bss)
      *(COMMON)
      *(.trampolines)      
      __bss_end = ALIGN(4) ;
    } AT > INT_DRAM

    PROVIDE (__stack_top_std = 0x00001f00);
    PROVIDE (__stack_top_fiq = 0x00002000);
} 



