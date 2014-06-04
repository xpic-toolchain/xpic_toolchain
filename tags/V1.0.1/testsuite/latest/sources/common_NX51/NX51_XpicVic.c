#include "NX51_XpicVic.h" // function prototypes and defines
///#include <string.h>

static NX51_XPIC_VIC_AREA_T* s_ptXpicVic = (NX51_XPIC_VIC_AREA_T*)Addr_NX51_xpic_vic;

/**
 * This implementation provides a built-in ISR table which resides in a data segment
 * and holds the addresses of all vectored interrupt service routines.
 * This table is compliant with the IRQ handler NX51_XpicVic_IrqVectorHandlerC()
 * The IRQ handler and the ISR table are registered automatically with a call
 * of NX51_XpicVic_Reset().
 *
 * Other implementations might use a static jump table and be slightly faster than
 * this one.
 * If you want to use another IRQ handler you can compile this source with
 * the flag NX51_XPICVIC_NO_DEFAULT_IRQ_HANDLING defined.
 * In this case you have to register your IRQ handler and the ISR table
 * manually with NX51_XpicVic_SetIrqHandler() and NX51_XpicVic_SetIrqTable()
 */
#ifndef NX51_XPICVIC_NO_DEFAULT_IRQ_HANDLING

static volatile pp_func s_apfnIrqTable[NX51_XPICVIC_VECTOR_MAX + 1];

#endif


/*****************************************************************************/
/*! XpicVic Default IRQ Handler
* \description
*   This function calls the appropriate ISR for the most prior active IRQ vector.
*   IRQs are non-nested.
* \class
*   XpicVic
*                                                                            */
/*****************************************************************************/
void IRQ NX51_XpicVic_IrqVectorHandlerC(void) {
  pp_func pfnIsr;

  /* Get the vectored interrupt service routine for the current IRQ */
  pfnIsr = *((pp_func*)s_ptXpicVic->ulXpic_vic_vector_addr);
  /* Execute the ISR */
  pfnIsr();
}


/*****************************************************************************/
/*! XpicVic Reset
* \description
*   This function resets the xPIC Vic. All interrupts are disabled.
* \class
*   XpicVic
*                                                                            */
/*****************************************************************************/
void NX51_XpicVic_Reset(void)
{
  unsigned int uVectorNum;

  NX51_XpicVic_Enable(0);
  NX51_XpicVic_SetFiqSrc(0,0);

  for(uVectorNum = 0; uVectorNum < NX51_XPICVIC_VECTOR_MAX; ++uVectorNum)
  {
    NX51_XpicVic_SetVectorSrc(uVectorNum, 0, 0);
  }

  NX51_XpicVic_SetDefaultSrc(0, 0, 0);
  NX51_XpicVic_SetDefaultFiqSrc(0, 0, 0);

#ifndef NX51_XPICVIC_NO_DEFAULT_IRQ_HANDLING
  s_ptXpicVic->ulXpic_vic_irq_addr = (unsigned long)NX51_XpicVic_IrqVectorHandlerC;
  s_ptXpicVic->ulXpic_vic_table_base_addr = (unsigned long)s_apfnIrqTable;
#endif
}


/*****************************************************************************/
/*! XpicVic Enable
* \description
*   Sets the global enable bit of the xPIC VIC.
*   NX51_XpicVic_Enable(1) enables the VIC.
*   NX51_XpicVic_Enable(0) disables Interrupts completely.
* \class
*   XpicVic
*                                                                            */
/*****************************************************************************/
void NX51_XpicVic_Enable(int fEnable)
{
  unsigned long ulConfig = s_ptXpicVic->ulXpic_vic_config;

  if( fEnable )
    ulConfig |= MSK_NX51_xpic_vic_config_enable;
  else
    ulConfig &= ~MSK_NX51_xpic_vic_config_enable;

  s_ptXpicVic->ulXpic_vic_config = ulConfig;
}


/*****************************************************************************/
/*! XpicVic SetFiqSrc
* \description
*   Select an interrupt wire, i.e. a hardware module that triggers the FIQ.
*   Note that we can 'bundle' different sources to enable them all for FIQ
*   triggering using function NX51_XpicVic_SetDefaultFiqSrc()
* \class
*   XpicVic
*                                                                            */
/*****************************************************************************/
void NX51_XpicVic_SetFiqSrc(NX51_XPICVIC_INTSRC eSrc, int fEnable)
{
  unsigned long ulConfig = eSrc & MSK_NX51_xpic_vic_fiq_vect_config_int_source;

  if( fEnable )
    ulConfig |= MSK_NX51_xpic_vic_fiq_vect_config_enable;

  s_ptXpicVic->ulXpic_vic_fiq_vect_config = ulConfig;
}


/*****************************************************************************/
/*! XpicVic SetVectorSrc
* \description
*   Select an interrupt wire, i.e. a hardware module that triggers the
*   specified IRQ vector.
*   Note that only one source can be selected, we can 'bundle'
*   different sources for the 'default irq vector' only
*   (see NX51_XpicVic_SetDefaultSrc).
* \class
*   XpicVic
*                                                                            */
/*****************************************************************************/
void NX51_XpicVic_SetVectorSrc(unsigned int uVectorNum, NX51_XPICVIC_INTSRC eSrc, int fEnable)
{
  unsigned long ulConfig = eSrc & MSK_NX51_xpic_vic_vect_config0_int_source;

  if( fEnable )
    ulConfig |= MSK_NX51_xpic_vic_vect_config0_enable;

  s_ptXpicVic->aulXpic_vic_vect_config[uVectorNum] = ulConfig;
}


/*****************************************************************************/
/*! XpicVic SetDefaultSrc
* \description
*   Select one or more interrupt wires, i.e. hardware modules that trigger the
*   default IRQ vector.
*   The different sources are combined with a logical OR to determine the
*   trigger wire for the default vector.
* \class
*   XpicVic
*                                                                            */
/*****************************************************************************/
void NX51_XpicVic_SetDefaultSrc(unsigned long ulSourceSelect0, unsigned long ulSourceSelect1, int fEnable)
{
  unsigned long ulConfig = 0;

  if( fEnable )
    ulConfig = MSK_NX51_xpic_vic_vect_config15_enable;

  s_ptXpicVic->aulXpic_vic_default[0] = ulSourceSelect0;
  s_ptXpicVic->aulXpic_vic_default[1] = ulSourceSelect1;

  s_ptXpicVic->aulXpic_vic_vect_config[NX51_XPICVIC_VECTOR_MAX] = ulConfig;

}


/*****************************************************************************/
/*! XpicVic SetDefaultFiqSrc
* \description
*   Select one or more interrupt wires, i.e. hardware modules that trigger the
*   FIQ.
*   The different sources are combined with a logical OR to determine the
*   trigger wire for the default FIQ.
* \class
*   XpicVic
*                                                                            */
/*****************************************************************************/
void NX51_XpicVic_SetDefaultFiqSrc(unsigned long ulSourceSelect0, unsigned long ulSourceSelect1, int fEnable)
{
  unsigned long ulConfig = 0;

  if( fEnable )
    ulConfig = MSK_NX51_xpic_vic_fiq_vect_config_enable | MSK_NX51_xpic_vic_fiq_vect_config_select_fiq_default;

  s_ptXpicVic->aulXpic_vic_fiq_default[0] = ulSourceSelect0;
  s_ptXpicVic->aulXpic_vic_fiq_default[1] = ulSourceSelect1;

  s_ptXpicVic->ulXpic_vic_fiq_vect_config = ulConfig;
}

/*****************************************************************************/
/*! XpicVic SetFiqHandler
* \description
*   This function registers a FIQ handler.
* \class
*   XpicVic
*                                                                            */
/*****************************************************************************/
void NX51_XpicVic_SetFiqHandler(pp_func pfnFiqHandler)
{
  s_ptXpicVic->ulXpic_vic_fiq_addr = (unsigned long)pfnFiqHandler;
}


#ifndef NX51_XPICVIC_NO_DEFAULT_IRQ_HANDLING

/*****************************************************************************/
/*! XpicVic SetVectorIsr
* \description
*   Register a C-function as service routine for a specified IRQ vector.
* \class
*   XpicVic
*                                                                            */
/*****************************************************************************/
void NX51_XpicVic_SetVectorIsr(unsigned int uVectorNum, pp_func pfnIsr) {
  s_apfnIrqTable[uVectorNum] = pfnIsr;
}


/*****************************************************************************/
/*! XpicVic SetVectorIsr
* \description
*   Register a C-function as default service routine for combined interrupt
*   sources.
* \class
*   XpicVic
*                                                                            */
/*****************************************************************************/
void NX51_XpicVic_SetDefaultIsr(pp_func pfnIsr) {
  s_apfnIrqTable[NX51_XPICVIC_VECTOR_MAX] = pfnIsr;
}


#else

/*****************************************************************************/
/*! XpicVic SetIrqHandler
* \description
*   This function registers a IRQ handler.
* \class
*   XpicVic
*                                                                            */
/*****************************************************************************/
void NX51_XpicVic_SetIrqHandler(pp_func pfnIrqHandler)
{
  s_ptXpicVic->ulXpic_vic_irq_addr = (unsigned long)pfnIrqHandler;
}

void NX51_XpicVic_SetIrqTable(unsigned long ulTableBase, int fLongJumps)
{
  unsigned long ulConfig = s_ptXpicVic->ulXpic_vic_config & MSK_NX51_xpic_vic_config_enable;

  if( fLongJumps )
    ulConfig |= MSK_NX51_xpic_vic_config_table;

  s_ptXpicVic->ulXpic_vic_table_base_addr = ulTableBase;
  s_ptXpicVic->ulXpic_vic_config = ulConfig;
}

#endif
