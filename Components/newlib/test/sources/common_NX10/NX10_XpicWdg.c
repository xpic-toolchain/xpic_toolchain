#include "NX10_XpicWdg.h" // function prototypes and defines
//#include <string.h>

static NX10_XPIC_WDG_AREA_T* s_ptXpicWdg = (NX10_XPIC_WDG_AREA_T*)Addr_NX10_xpic_wdg;


/*****************************************************************************/
/*! XpicWdg Start
* \description
*   This function starts or stops the xPIC watchdog.
*   If one of the parameters are non-zero, the watchdoch starts counting
*   and generates an interrupt to the xPIC and/or ARM after the according
*   timeout.
*   If both parameters are zero, the watchdog is stopped and no more
*   interrupts are generated.
* \class
*   XpicWdg
* \params
*   ulXpicTimeout      [in]  Irq timeout for xPIC in ms
*   ulArmTimeout       [in]  Irq timeout for ARM in ms
*                                                                            */
/*****************************************************************************/
void NX10_XpicWdg_Start(unsigned long ulXpicTimeout, unsigned long ulArmTimeout)
{
  unsigned long ulTrigger;

  /* ms to 1/10000 clock cycles */
  ulXpicTimeout = ulXpicTimeout * (NX10_DEV_FREQUENCY/10000000);
  ulArmTimeout  = ulArmTimeout  * (NX10_DEV_FREQUENCY/10000000);

  /* unlock timeout registers */
  ulTrigger = s_ptXpicWdg->ulXpic_wdg_trig;
  s_ptXpicWdg->ulXpic_wdg_trig = ulTrigger | MSK_NX10_xpic_wdg_trig_write_enable;

  /* set timeout registers */
  s_ptXpicWdg->ulXpic_wdg_xpic_irq_timeout = ulXpicTimeout;
  s_ptXpicWdg->ulXpic_wdg_arm_irq_timeout  = ulArmTimeout;

  /* lock timeout register and start watchdog by triggering */
  ulTrigger = s_ptXpicWdg->ulXpic_wdg_trig;
  s_ptXpicWdg->ulXpic_wdg_trig = ulTrigger & ~MSK_NX10_xpic_wdg_trig_write_enable | MSK_NX10_xpic_wdg_trig_wdg_counter_trigger_w;
}


/*****************************************************************************/
/*! XpicWdg Trigger
* \description
*   This function resets the internal counter of the xPIC watchdog.
*   This averts the interrupts for another timeout cycle.
* \class
*   XpicWdg
*                                                                            */
/*****************************************************************************/
void NX10_XpicWdg_Trigger(void)
{
  unsigned long ulTrigger = s_ptXpicWdg->ulXpic_wdg_trig;
  ulTrigger |= MSK_NX10_xpic_wdg_trig_wdg_counter_trigger_w;
  s_ptXpicWdg->ulXpic_wdg_trig = ulTrigger;
}


/*****************************************************************************/
/*! XpicWdg ConfirmOwnIrq
* \description
*   Reset the interrupt from the watchdog to the xPIC.
* \class
*   XpicWdg
*                                                                            */
/*****************************************************************************/
void NX10_XpicWdg_ConfirmOwnIrq(void)
{
  unsigned long ulTrigger = s_ptXpicWdg->ulXpic_wdg_trig;
  s_ptXpicWdg->ulXpic_wdg_trig = ulTrigger | MSK_NX10_xpic_wdg_trig_irq_req_watchdog;
}


/*****************************************************************************/
/*! XpicWdg GetOwnIrq
* \description
*   Observe the interrupt from the watchdog to the xPIC.
* \class
*   XpicWdg
* \return
*   1 = interrupt is active
*   0 = interrupt is inactive
*                                                                            */
/*****************************************************************************/
int NX10_XpicWdg_GetOwnIrq(void)
{
  return 1 & (s_ptXpicWdg->ulXpic_wdg_trig >> SRT_NX10_xpic_wdg_trig_irq_req_watchdog);
}
