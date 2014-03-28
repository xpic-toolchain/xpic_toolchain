#ifndef __defines_h
#define __defines_h


/*****************************************************************************/
/* Standard C Macros                                                         */
/*****************************************************************************/

/* read/write at address */
#define POKE(addr, val) (*(volatile unsigned int *)(addr) = (val))
#define PEEK(addr) (*(volatile unsigned int *)(addr))

/* array length */
#define ARRAY_LEN(x) (sizeof(x)/sizeof(x[0]))

/* math */
#ifndef MAX
#define MAX(x,y)  ( (x) > (y) ? (x) : (y) )
#endif

#ifndef MIN
#define MIN(x,y)  ( (x) < (y) ? (x) : (y) )
#endif

#ifndef ABS
#define ABS(z)    ( (z & 0x800000000) ? (-z) : (z) )
#endif

/*****************************************************************************/
/* Compiler Specific Macros                                                  */
/*****************************************************************************/

#ifdef __llvm__ /* llvm-gcc or clang */

/* Fast interrupt handler, recognized by xpic-llc */
#define FIQ  __attribute__((annotate("fiq")))

/**
 * Non-nested interrupt handler, recognized by xpic-llc
 * This kind of interrupt handler switches from irq to user mode when the function returns */
#define IRQ  __attribute__((annotate("irq")))

/**
 * Nested (interruptable) interrupt handler, recognized by xpic-llc
 * This kind of interrupt handler does not switch from irq to user mode automatically.
 * All ISRs have to switch the mode explicitly with the "reli" assembler instruction.
 */
#define IRQN __attribute__((annotate("irqn")))
#define RELEASE_IRQ  __asm__("reli");

/* packed structures/members */
#define PACKED __attribute__ ((packed))

#else
#warning Unknown compiler
#endif


/*****************************************************************************/
/* netX10 Specific Macros                                                    */
/*****************************************************************************/

/* clk frequency */
#define NX10_DEV_FREQUENCY 100000000L // 100 Mhz

#endif  /* ...__defines_h */
