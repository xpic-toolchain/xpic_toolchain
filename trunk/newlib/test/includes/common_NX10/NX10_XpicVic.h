#ifndef __NX10_XPIC_VIC_H
#define __NX10_XPIC_VIC_H

#include "regdef_netx10.h"
#include "defines.h"

#define NX10_XPICVIC_VECTOR_MAX 15


#ifdef __llvm__ /* llvm-gcc -> llc */
#endif

typedef void (*pp_func)(void);


/***********************************************************************************************/
/*! IRQ Sources that can be selected                                                           */
/***********************************************************************************************/
typedef unsigned long NX10_XPICVIC_INTSRC;
#define NX10_XPICVIC_INTSRC_SOFTWARE        0
#define NX10_XPICVIC_INTSRC_TIMER0          1
#define NX10_XPICVIC_INTSRC_TIMER1          2
#define NX10_XPICVIC_INTSRC_GPIO_TIMER      3
#define NX10_XPICVIC_INTSRC_TIMER2          4
#define NX10_XPICVIC_INTSRC_SYSTIME_S       5
#define NX10_XPICVIC_INTSRC_GPIO7           6
#define NX10_XPICVIC_INTSRC_WATCHDOG        7
#define NX10_XPICVIC_INTSRC_UART0           8
#define NX10_XPICVIC_INTSRC_UART1           9
#define NX10_XPICVIC_INTSRC_10             10
#define NX10_XPICVIC_INTSRC_USB            11
#define NX10_XPICVIC_INTSRC_SPI            12
#define NX10_XPICVIC_INTSRC_I2C            13
#define NX10_XPICVIC_INTSRC_14             14
#define NX10_XPICVIC_INTSRC_DPM            15
#define NX10_XPICVIC_INTSRC_GPIO           16
#define NX10_XPICVIC_INTSRC_XPEC_COM0      17
#define NX10_XPICVIC_INTSRC_18             18
#define NX10_XPICVIC_INTSRC_19             19
#define NX10_XPICVIC_INTSRC_20             20
#define NX10_XPICVIC_INTSRC_XPEC_MSYNC0    21
#define NX10_XPICVIC_INTSRC_22             22
#define NX10_XPICVIC_INTSRC_23             23
#define NX10_XPICVIC_INTSRC_ARM            24
#define NX10_XPICVIC_INTSRC_PHY            25
#define NX10_XPICVIC_INTSRC_EXTMEM         26
#define NX10_XPICVIC_INTSRC_DMAC           27
#define NX10_XPICVIC_INTSRC_28             28
#define NX10_XPICVIC_INTSRC_MPWM           29
#define NX10_XPICVIC_INTSRC_MENC           30
#define NX10_XPICVIC_INTSRC_ADC            31
#define NX10_XPICVIC_INTSRC_ADC0           32
#define NX10_XPICVIC_INTSRC_ADC1           33
#define NX10_XPICVIC_INTSRC_ENC0           34
#define NX10_XPICVIC_INTSRC_ENC1           35
#define NX10_XPICVIC_INTSRC_ENC_CAP0       36
#define NX10_XPICVIC_INTSRC_ENC_CAP1       37
#define NX10_XPICVIC_INTSRC_ENC_CAP2       38
#define NX10_XPICVIC_INTSRC_ENC_CAP3       39
#define NX10_XPICVIC_INTSRC_ENC_MP0        40
#define NX10_XPICVIC_INTSRC_ENC_MP1        41
#define NX10_XPICVIC_INTSRC_MPWM0          42
#define NX10_XPICVIC_INTSRC_MPWM1          43
#define NX10_XPICVIC_INTSRC_MPWM_FAILURE   44
#define NX10_XPICVIC_INTSRC_SPI0           45
#define NX10_XPICVIC_INTSRC_SPI1           46
#define NX10_XPICVIC_INTSRC_GPIO_TIMER0    47
#define NX10_XPICVIC_INTSRC_GPIO_TIMER1    48
#define NX10_XPICVIC_INTSRC_GPIO0          49
#define NX10_XPICVIC_INTSRC_GPIO1          50
#define NX10_XPICVIC_INTSRC_GPIO2          51
#define NX10_XPICVIC_INTSRC_GPIO3          52
#define NX10_XPICVIC_INTSRC_GPIO4          53
#define NX10_XPICVIC_INTSRC_GPIO5          54
#define NX10_XPICVIC_INTSRC_GPIO6          55
#define NX10_XPICVIC_INTSRC_56             56
#define NX10_XPICVIC_INTSRC_57             57
#define NX10_XPICVIC_INTSRC_58             58
#define NX10_XPICVIC_INTSRC_59             59
#define NX10_XPICVIC_INTSRC_60             60
#define NX10_XPICVIC_INTSRC_61             61
#define NX10_XPICVIC_INTSRC_62             62
#define NX10_XPICVIC_INTSRC_MISALIGN       63


/*! XpicVic GetRaw0Irq
 * \description Observe raw interrupt requests
 */
#define NX10_XpicVic_GetRaw0Irq()      PEEK(Adr_NX10_xpic_vic_raw_intr0)

/*! XpicVic GetRaw1Irq
 * \description Observe raw interrupt requests
 */
#define NX10_XpicVic_GetRaw1Irq()      PEEK(Adr_NX10_xpic_vic_raw_intr1)

/*! XpicVic SetSw0Irq
 * \description Trigger interrupt by software
 */
#define NX10_XpicVic_SetSw0Irq(mask)   POKE(Adr_NX10_xpic_vic_softint0_set,mask)

/*! XpicVic SetSw1Irq
 * \description Trigger interrupt by software
 */
#define NX10_XpicVic_SetSw1Irq(mask)   POKE(Adr_NX10_xpic_vic_softint1_set,mask)

/*! XpicVic GetSw0Irq
 * \description Observe interrupts triggered by software
 */
#define NX10_XpicVic_GetSw0Irq()       PEEK(Adr_NX10_xpic_vic_softint0_set)

/*! XpicVic GetSw1Irq
 * \description Observe interrupts triggered by software
 */
#define NX10_XpicVic_GetSw1Irq()       PEEK(Adr_NX10_xpic_vic_softint1_set)

/*! XpicVic ClearSw0Irq
 * \description Clear interrupts triggered by software
 */
#define NX10_XpicVic_ClearSw0Irq(mask) POKE(Adr_NX10_xpic_vic_softint0_reset,mask)

/*! XpicVic ClearSw1Irq
 * \description Clear interrupts triggered by software
 */
#define NX10_XpicVic_ClearSw1Irq(mask) POKE(Adr_NX10_xpic_vic_softint1_reset,mask)

void NX10_XpicVic_Reset(void);
void NX10_XpicVic_SetFiqHandler(pp_func pfnFiqHandler);
void NX10_XpicVic_SetFiqSrc(NX10_XPICVIC_INTSRC eSrc, int fEnable);
void NX10_XpicVic_SetVectorSrc(unsigned int uVectorNum, NX10_XPICVIC_INTSRC eSrc, int fEnable);
void NX10_XpicVic_SetDefaultSrc(unsigned long ulSourceSelect0, unsigned long ulSourceSelect1, int fEnable);
void NX10_XpicVic_Enable(int fEnable);


#ifndef NX10_XPICVIC_NO_DEFAULT_IRQ_HANDLING

void IRQ NX10_XpicVic_IrqVectorHandlerC(void);
void NX10_XpicVic_SetVectorIsr(unsigned int uVectorNum, pp_func pfnIsr);
void NX10_XpicVic_SetDefaultIsr(pp_func pfnIsr);

#else

void NX10_XpicVic_SetIrqHandler(pp_func pfnIrqHandler);
void NX10_XpicVic_SetIrqTable(unsigned long ulTableBase, int fLongJumps);

#endif /* #ifndef NX10_XPICVIC_NO_DEFAULT_IRQ_HANDLING */


#endif /* __NX10_XPIC_VIC_H */
