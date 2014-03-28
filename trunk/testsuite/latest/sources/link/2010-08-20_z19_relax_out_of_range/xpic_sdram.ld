/* RUN: xpic-ld -T%s init_xpic.o main.o -L. --start-group -lcodesys_rt -lm -lc -lgcc --end-group -o test.elf */

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
    } AT > INT_PRAM

    .data_local 0x00000000 :
    {
      *init_xpic.o (.constants .data* .rodata* .bss)
    } AT > INT_DRAM
            
    .text :
    {
      *(.constants .text .rodata* .data* .bss)
      *(COMMON)
      
      . = ALIGN(4) ;
    } > SDRAM
    
    . = . + 0x100000 ;
    PROVIDE (__stack_top_std = .);
    . = . + 0x100 ;
    PROVIDE (__stack_top_fiq = .);
    PROVIDE (_heap_start = .);
    PROVIDE (_heap_end = ORIGIN(SDRAM)+LENGTH(SDRAM));
    
    
} 
