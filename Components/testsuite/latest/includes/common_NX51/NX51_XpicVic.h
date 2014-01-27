#ifndef __NX51_XPIC_VIC_H
#define __NX51_XPIC_VIC_H

#include "netx51_regdef.h"
#include "defines.h"

#define NX51_XPICVIC_VECTOR_MAX 15


#ifdef __llvm__ /* llvm-gcc -> llc */
#endif

typedef void (*pp_func)(void);


/***********************************************************************************************/
/*! IRQ Sources that can be selected                                                           */
/***********************************************************************************************/
typedef unsigned long NX51_XPICVIC_INTSRC;
#define NX51_XPICVIC_INTSRC_SOFTWARE        0
#define NX51_XPICVIC_INTSRC_TIMER0          1
#define NX51_XPICVIC_INTSRC_TIMER1          2
#define NX51_XPICVIC_INTSRC_GPIO_TIMER      3
#define NX51_XPICVIC_INTSRC_TIMER2          4
#define NX51_XPICVIC_INTSRC_SYSTIME_S       5
#define NX51_XPICVIC_INTSRC_GPIO31          6
#define NX51_XPICVIC_INTSRC_WATCHDOG        7
#define NX51_XPICVIC_INTSRC_UART0           8
#define NX51_XPICVIC_INTSRC_UART1           9
#define NX51_XPICVIC_INTSRC_UART2          10
#define NX51_XPICVIC_INTSRC_USB            11
#define NX51_XPICVIC_INTSRC_SPI            12
#define NX51_XPICVIC_INTSRC_I2C            13
#define NX51_XPICVIC_INTSRC_14             14
#define NX51_XPICVIC_INTSRC_DPM            15
#define NX51_XPICVIC_INTSRC_GPIO           16
#define NX51_XPICVIC_INTSRC_XPEC_COM0      17
#define NX51_XPICVIC_INTSRC_XPEC_COM1      18
#define NX51_XPICVIC_INTSRC_19             19
#define NX51_XPICVIC_INTSRC_20             20
#define NX51_XPICVIC_INTSRC_XPEC_MSYNC0    21
#define NX51_XPICVIC_INTSRC_XPEC_MSYNC1    22
#define NX51_XPICVIC_INTSRC_23             23
#define NX51_XPICVIC_INTSRC_ARM            24
#define NX51_XPICVIC_INTSRC_PHY            25
#define NX51_XPICVIC_INTSRC_EXTMEM         26
#define NX51_XPICVIC_INTSRC_DMAC           27
#define NX51_XPICVIC_INTSRC_TRIGER_LT      28
#define NX51_XPICVIC_INTSRC_CAN            29
#define NX51_XPICVIC_INTSRC_OSAC           30
#define NX51_XPICVIC_INTSRC_31             31
#define NX51_XPICVIC_INTSRC_GPIO7          32
#define NX51_XPICVIC_INTSRC_GPIO8          33
#define NX51_XPICVIC_INTSRC_GPIO9          34
#define NX51_XPICVIC_INTSRC_GPIO10         35
#define NX51_XPICVIC_INTSRC_GPIO11         36
#define NX51_XPICVIC_INTSRC_GPIO12         37
#define NX51_XPICVIC_INTSRC_GPIO13         38
#define NX51_XPICVIC_INTSRC_GPIO14         39
#define NX51_XPICVIC_INTSRC_GPIO15         40
#define NX51_XPICVIC_INTSRC_GPIO16         41
#define NX51_XPICVIC_INTSRC_GPIO_TIMER2    42
#define NX51_XPICVIC_INTSRC_GPIO_TIMER3    43
#define NX51_XPICVIC_INTSRC_GPIO_TIMER4    44
#define NX51_XPICVIC_INTSRC_SPI0           45
#define NX51_XPICVIC_INTSRC_SPI1           46
#define NX51_XPICVIC_INTSRC_GPIO_TIMER0    47
#define NX51_XPICVIC_INTSRC_GPIO_TIMER1    48
#define NX51_XPICVIC_INTSRC_GPIO0          49
#define NX51_XPICVIC_INTSRC_GPIO1          50
#define NX51_XPICVIC_INTSRC_GPIO2          51
#define NX51_XPICVIC_INTSRC_GPIO3          52
#define NX51_XPICVIC_INTSRC_GPIO4          53
#define NX51_XPICVIC_INTSRC_GPIO5          54
#define NX51_XPICVIC_INTSRC_GPIO6          55
#define NX51_XPICVIC_INTSRC_56             56
#define NX51_XPICVIC_INTSRC_57             57
#define NX51_XPICVIC_INTSRC_58             58
#define NX51_XPICVIC_INTSRC_59             59
#define NX51_XPICVIC_INTSRC_ETH            60
#define NX51_XPICVIC_INTSRC_OSAC_NFIFO     61
#define NX51_XPICVIC_INTSRC_OSAC_SCHED     62
#define NX51_XPICVIC_INTSRC_MISALIGN       63

/*! xpic2_enable_int
 * \description enables irq and fiq, set interrupt enable flags to:
 *  ief = 1  - enable irq
 *  ief = 2  - enable fiq
 *  ief = 3  - enable irq & fiq
  */
void xpic2_enable_int(int ief);

/*! xpic2_enable_int
 * \description disables irq and fiq, returns interrupt enable flags
   */
unsigned long xpic2_disable_int();

/*! xpic2_disable_irq
 * \description disables irq, returns interrupt enable flags
   */
unsigned long xpic2_disable_irq();

/*! xpic2_int_status
 * \description returns interrupt enable flags
   */
unsigned long  xpic2_int_status();




/*! XpicVic GetRaw0Irq
 * \description Observe raw interrupt requests
 */
#define NX51_XpicVic_GetRaw0Irq()      PEEK(Adr_NX51_xpic_vic_raw_intr0)

/*! XpicVic GetRaw1Irq
 * \description Observe raw interrupt requests
 */
#define NX51_XpicVic_GetRaw1Irq()      PEEK(Adr_NX51_xpic_vic_raw_intr1)

/*! XpicVic SetSw0Irq
 * \description Trigger interrupt by software
 */
#define NX51_XpicVic_SetSw0Irq(mask)   POKE(Adr_NX51_xpic_vic_softint0_set,mask)

/*! XpicVic SetSw1Irq
 * \description Trigger interrupt by software
 */
#define NX51_XpicVic_SetSw1Irq(mask)   POKE(Adr_NX51_xpic_vic_softint1_set,mask)

/*! XpicVic GetSw0Irq
 * \description Observe interrupts triggered by software
 */
#define NX51_XpicVic_GetSw0Irq()       PEEK(Adr_NX51_xpic_vic_softint0_set)

/*! XpicVic GetSw1Irq
 * \description Observe interrupts triggered by software
 */
#define NX51_XpicVic_GetSw1Irq()       PEEK(Adr_NX51_xpic_vic_softint1_set)

/*! XpicVic ClearSw0Irq
 * \description Clear interrupts triggered by software
 */
#define NX51_XpicVic_ClearSw0Irq(mask) POKE(Adr_NX51_xpic_vic_softint0_reset,mask)

/*! XpicVic ClearSw1Irq
 * \description Clear interrupts triggered by software
 */
#define NX51_XpicVic_ClearSw1Irq(mask) POKE(Adr_NX51_xpic_vic_softint1_reset,mask)

void NX51_XpicVic_Reset(void);
void NX51_XpicVic_SetFiqHandler(pp_func pfnFiqHandler);
void NX51_XpicVic_SetFiqSrc(NX51_XPICVIC_INTSRC eSrc, int fEnable);
void NX51_XpicVic_SetVectorSrc(unsigned int uVectorNum, NX51_XPICVIC_INTSRC eSrc, int fEnable);
void NX51_XpicVic_SetDefaultSrc(unsigned long ulSourceSelect0, unsigned long ulSourceSelect1, int fEnable);
void NX51_XpicVic_SetDefaultFiqSrc(unsigned long ulSourceSelect0, unsigned long ulSourceSelect1, int fEnable);
void NX51_XpicVic_Enable(int fEnable);


#ifndef NX51_XPICVIC_NO_DEFAULT_IRQ_HANDLING

void IRQ NX51_XpicVic_IrqVectorHandlerC(void);
void NX51_XpicVic_SetVectorIsr(unsigned int uVectorNum, pp_func pfnIsr);
void NX51_XpicVic_SetDefaultIsr(pp_func pfnIsr);

#else

void NX51_XpicVic_SetIrqHandler(pp_func pfnIrqHandler);
void NX51_XpicVic_SetIrqTable(unsigned long ulTableBase, int fLongJumps);

#endif /* #ifndef NX51_XPICVIC_NO_DEFAULT_IRQ_HANDLING */


#endif /* __NX51_XPIC_VIC_H */
