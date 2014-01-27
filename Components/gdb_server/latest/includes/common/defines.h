/*
************************************************************
*   Macros Definitions
************************************************************
*/
#ifndef __defines_h
#define __defines_h

#define POKE(addr, val) (*(volatile unsigned int *)(addr) = (val))
#define PEEK(addr) (*(volatile unsigned int *)(addr))

#define ARRAY_LEN(x) (sizeof(x)/sizeof(x[0]))

#endif  /* ...__defines_h */





