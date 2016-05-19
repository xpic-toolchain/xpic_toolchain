
MEMORY
{
  INT_PRAM   (rx)   : ORIGIN = 0x10180000, LENGTH = 8K
  INT_DRAM   (rw!x) : ORIGIN = 0x10280000, LENGTH = 6K
  INTRAM3    (rx)   : ORIGIN = 0x08040000, LENGTH = 128K
  INTRAM4    (rw!x) : ORIGIN = 0x08070000, LENGTH = 64K
}

/* 2k reserved at end of DRAM for stack area */

SECTIONS
{
    .text_local 0x00000000 :
    {
      *init_xpic.o (.text)
      *core_state.o (.text)
     /*  *core_main.o (.text) */
      *core_list_join.o (.text)
      *core_util.o (.text)
      *core_matrix.o (.text)
	/*  *syscalls_file.o (.text) */
    /* *core_portme.o (.text) */
    } AT > INT_PRAM

    .data_local 0x00000000 :
    {
      *init_xpic.o (.data* .rodata* .bss)
	  *init_netX51_xpic.o (.data* .rodata* .bss)
      *core_state.o (.data* .rodata* .bss)
      *core_main.o (.data* .rodata* .bss COMMON)
      *core_list_join.o (.data* .rodata* .bss)
      *core_util.o (.data* .rodata* .bss)
      *core_matrix.o (.data* .rodata* .bss)
	  *syscalls_file.o (.data* .rodata* .bss)
      *core_portme.o (.data* .rodata* .bss)
      *(.trampolines)
    } AT > INT_DRAM
    
    PROVIDE (__stack_top_std = 0x00001f00);
    PROVIDE (__stack_top_fiq = 0x00002000);
    
    .text :
    {
      *(.text)      
      . = ALIGN(4) ;
    } > INTRAM3
    
    .data :
    {
      *(.rodata* .data*)      
      . = ALIGN(4) ;
    } > INTRAM4
    
    .bss (NOLOAD) :
    {
      /* zero-initialized data */
      . = ALIGN(4);
      __bss_start = . ;
      
      *(.bss)
      *(COMMON)
      
      __bss_end = ALIGN(4) ;
    } > INTRAM4

    /* location of the heap for malloc() etc.
     * malloc assumes that heap start is 8 byte aligned */
    PROVIDE (_heap_start = ALIGN(16));
    PROVIDE (_heap_end = ORIGIN(INTRAM4)+LENGTH(INTRAM4));
} 
