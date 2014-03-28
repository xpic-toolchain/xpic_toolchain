
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
      *main.o (.text)
    } AT > INT_PRAM

    .data_local 0x00000000 :
    {
      *init_xpic.o (.data .constants .rodata*)
      *main.o (.data .constants .rodata* .bss)
      
    } AT > INT_DRAM
        
    PROVIDE (__stack_top_std = 0x00001f00);
    PROVIDE (__stack_top_fiq = 0x00002000);
    
    .text :
    {
      *(.text .constants .rodata* .data .bss)
      *(COMMON)
    } > SDRAM
    
    PROVIDE (_heap_start = .);
    PROVIDE (_heap_end = ORIGIN(SDRAM)+LENGTH(SDRAM));
} 
