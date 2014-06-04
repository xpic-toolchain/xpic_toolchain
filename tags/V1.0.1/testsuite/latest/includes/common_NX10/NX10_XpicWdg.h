#ifndef __NX10_XPIC_WDG_H
#define __NX10_XPIC_WDG_H

#include "regdef_netx10.h"
#include "defines.h"

/***********************************************************************************************/
/*! IRQ Sources that can be selected                                                           */
/***********************************************************************************************/
typedef unsigned long NX10_XPICWDG_IRQ;
#define NX10_XPICWDG_IRQ_ARM   MSK_NX10_xpic_wdg_irq_raw_wdg_arm_irq

/**
 * The following defines should not be used by the xPIC because they control the IRQs to the ARM only!
 * They are just here for completeness.
 * The xPIC IRQ cannot be disabled and is confirmed with the function NX10_XpicWdg_ConfirmOwnIrq()
 */
#define NX10_XpicWdg_EnIrq(mask)       POKE(Adr_NX10_xpic_wdg_irq_msk_set, mask)
#define NX10_XpicWdg_DisIrq(mask)      POKE(Adr_NX10_xpic_wdg_irq_msk_reset, mask)
#define NX10_XpicWdg_ConfirmIrq(mask)  POKE(Adr_NX10_xpic_wdg_irq_raw, mask)
#define NX10_XpicWdg_GetIrq()          PEEK(Adr_NX10_xpic_wdg_irq_masked)
#define NX10_XpicWdg_GetIrqRaw()       PEEK(Adr_NX10_xpic_wdg_irq_raw)


void NX10_XpicWdg_Start(unsigned long ulXpicTimeout, unsigned long ulArmTimeout);
void NX10_XpicWdg_Trigger(void);
void NX10_XpicWdg_ConfirmOwnIrq(void);
int NX10_XpicWdg_GetOwnIrq(void);

#endif /* __NX10_XPIC_WDG_H */
