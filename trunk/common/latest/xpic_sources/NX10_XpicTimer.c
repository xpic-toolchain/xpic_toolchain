#include "NX10_XpicTimer.h" // function prototypes and defines
#include "regdef_netx10.h"


#ifdef DEBUG
#define XPICTIMER_CHK_UINST if( uInst >= NX10_XPICTIMER_CHANNELS ) return NX10_XPICTIMER_ERR_INVAL_PARAM;
#else
#define XPICTIMER_CHK_UINST
#endif



static NX10_XPIC_TIMER_AREA_T*     s_ptXpicTimer   = (NX10_XPIC_TIMER_AREA_T*) Addr_NX10_xpic_timer;
static NX10_SYSTIME_AREA_T*        s_ptSystime     = (NX10_SYSTIME_AREA_T*) Addr_NX10_systime;


/*****************************************************************************/
/*! XpicTimer Start
* \description
*   This function starts an xPIC timer.
* \class 
*   XpicTimer
* \params
*   uInst           [in]  Timer instance ( < NX10_XPICTIMER_CHANNELS)
*   eMode           [in]  Timer mode, see NX10_XPICTIMER_CFG_MODE_...
*   ulVal           [in]  Timer value, meaning depends on eMode
* \return
*   NX10_XPICTIMER_RESULT
*   NX10_XPICTIMER_ERR_INVAL_PARAM                                           */
/*****************************************************************************/
NX10_XPICTIMER_RESULT NX10_XpicTimer_Start( unsigned int uInst, NX10_XPICTIMER_CFG_MODE eMode, unsigned long ulVal )
{
  XPICTIMER_CHK_UINST

  switch( eMode ) {
    
    case NX10_XPICTIMER_CFG_MODE_STOP_AT_0:
      s_ptXpicTimer->aulXpic_timer_config_timer[uInst]  =  eMode<<SRT_NX10_xpic_timer_config_timer0_mode;
      s_ptXpicTimer->aulXpic_timer_timer[uInst]         =  ulVal;
    break;
    
    case NX10_XPICTIMER_CFG_MODE_PRELOAD_AT_0:
      s_ptXpicTimer->aulXpic_timer_config_timer[uInst]  =  eMode<<SRT_NX10_xpic_timer_config_timer0_mode;
      s_ptXpicTimer->aulXpic_timer_preload_timer[uInst] =  ulVal;
      s_ptXpicTimer->aulXpic_timer_timer[uInst]         =  ulVal;
    break;
    
    case NX10_XPICTIMER_CFG_MODE_SYSTIME:
      s_ptXpicTimer->aulXpic_timer_config_timer[uInst]  =  eMode<<SRT_NX10_xpic_timer_config_timer0_mode;
      s_ptXpicTimer->aulXpic_timer_timer[uInst]         =  ulVal;
    break;
 
    default:
      return NX10_XPICTIMER_ERR_INVAL_PARAM;
    break;
  }
    
  return NX10_XPICTIMER_OKAY;  
}

/*****************************************************************************/
/*! XpicTimer Stop
* \description
*   This function stops and resets an XPIC timer.
* \class 
*   XpicTimer
* \params
*   uInst           [in]  Timer Instance ( < NX10_XPICTIMER_CHANNELS)
* \return
*   NX10_XPICTIMER_RESULT
*   NX10_XPICTIMER_ERR_INVAL_PARAM                                           */
/*****************************************************************************/
NX10_XPICTIMER_RESULT NX10_XpicTimer_Stop( unsigned int uInst )
{
  XPICTIMER_CHK_UINST

  /* disable interrupt */
  switch( uInst ) {
    case 0: s_ptXpicTimer->ulXpic_timer_irq_msk_reset =  MSK_NX10_xpic_timer_irq_msk_reset_timer0_irq; break;
    case 1: s_ptXpicTimer->ulXpic_timer_irq_msk_reset =  MSK_NX10_xpic_timer_irq_msk_reset_timer1_irq; break;
    case 2: s_ptXpicTimer->ulXpic_timer_irq_msk_reset =  MSK_NX10_xpic_timer_irq_msk_reset_timer2_irq; break;
    default: return NX10_XPICTIMER_ERR_INVAL_PARAM;
    break;
  }

  /* reset timer registers */
  s_ptXpicTimer->aulXpic_timer_config_timer[uInst]  =  0;
  s_ptXpicTimer->aulXpic_timer_preload_timer[uInst] =  0;
  s_ptXpicTimer->aulXpic_timer_timer[uInst]         =  0;

  /* confirm pending interrupt */
  switch( uInst ) {
    case 0: s_ptXpicTimer->ulXpic_timer_irq_raw =  MSK_NX10_xpic_timer_irq_raw_timer0_irq; break;
    case 1: s_ptXpicTimer->ulXpic_timer_irq_raw =  MSK_NX10_xpic_timer_irq_raw_timer1_irq; break;
    case 2: s_ptXpicTimer->ulXpic_timer_irq_raw =  MSK_NX10_xpic_timer_irq_raw_timer2_irq; break;
    default: return NX10_XPICTIMER_ERR_INVAL_PARAM;
    break;
  }

  return NX10_XPICTIMER_OKAY;  
}


/*****************************************************************************/
/*! XpicTimer Get Parameter
* \description
*   This function reads parameter.
* \class 
*   XpicTimer
* \params
*   ulPrmID          [in]  Parameter ID
*   pulPrmVal        [out] Pointer to parameter value
* \return
*   NX10_XPICTIMER_OKAY                                                             
*   NX10_XPICTIMER_ERR_INVAL_PARAM                                                 */
/*****************************************************************************/
NX10_XPICTIMER_RESULT NX10_XpicTimer_GetPrm( unsigned long ulPrmID, unsigned long* pulPrmVal )
{
  switch( ulPrmID )
  { 
    case NX10_XPICTIMER_PRM_CONFIG_TIMER0:      *pulPrmVal = s_ptXpicTimer->aulXpic_timer_config_timer[0];   break;
    case NX10_XPICTIMER_PRM_CONFIG_TIMER1:      *pulPrmVal = s_ptXpicTimer->aulXpic_timer_config_timer[1];   break;
    case NX10_XPICTIMER_PRM_CONFIG_TIMER2:      *pulPrmVal = s_ptXpicTimer->aulXpic_timer_config_timer[2];   break;
    case NX10_XPICTIMER_PRM_PRELOAD_TIMER0:     *pulPrmVal = s_ptXpicTimer->aulXpic_timer_preload_timer[0];  break;
    case NX10_XPICTIMER_PRM_PRELOAD_TIMER1:     *pulPrmVal = s_ptXpicTimer->aulXpic_timer_preload_timer[1];  break;
    case NX10_XPICTIMER_PRM_PRELOAD_TIMER2:     *pulPrmVal = s_ptXpicTimer->aulXpic_timer_preload_timer[2];  break;
    case NX10_XPICTIMER_PRM_TIMER0:             *pulPrmVal = s_ptXpicTimer->aulXpic_timer_timer[0];          break;
    case NX10_XPICTIMER_PRM_TIMER1:             *pulPrmVal = s_ptXpicTimer->aulXpic_timer_timer[1];          break;
    case NX10_XPICTIMER_PRM_TIMER2:             *pulPrmVal = s_ptXpicTimer->aulXpic_timer_timer[2];          break;
    case NX10_XPICTIMER_PRM_IRQMSKSET:          *pulPrmVal = s_ptXpicTimer->ulXpic_timer_irq_msk_set;        break;
    case NX10_XPICTIMER_PRM_IRQMSKRST:          *pulPrmVal = s_ptXpicTimer->ulXpic_timer_irq_msk_reset;      break;

    /* Default */
    default:
      return NX10_XPICTIMER_ERR_INVAL_PARAM;
    break;
  } /* end switch */

  return NX10_XPICTIMER_OKAY;
}

/*****************************************************************************/
/*! XpicTimer Start Systime S compare
* \description
*   This function starts the systime_s compare machine.
* \class
*   XpicTimer
* \params
*   ulVal              [in]  compare value
* \return
*   NX10_XPICTIMER_RESULT                                                    */
/*****************************************************************************/
void NX10_XpicTimer_StartSystimeSecCmp( unsigned long ulVal )
{
  // write value
  s_ptXpicTimer->ulXpic_timer_compare_systime_s_value = ulVal;

  // reset irq_raw.systime_s_irq to activate compare machine
  s_ptXpicTimer->ulXpic_timer_irq_raw = MSK_NX10_xpic_timer_irq_raw_systime_s_irq;
}

/*****************************************************************************/
/*! XpicTimer Set Systime
* \description
*   Sets the systime.
* \class
*   XpicTimer
* \params
*   ulSystime_s           [in]  Systime Value in Seconds
*   ulSystime_ns          [in]  Systime Value in Nanoseconds
* \return
*                                                                            */
/*****************************************************************************/
void NX10_XpicTimer_SetSystime( unsigned long ulSystime_s, unsigned long ulSystime_ns )
{
  /* The xPIC mirror of these registers are read only, so we have to access
   * systime module directly for writing.
   */
  s_ptSystime->ulSystime_s = ulSystime_s;
  s_ptSystime->ulSystime_ns = ulSystime_ns;
}

/*****************************************************************************/
/*! XpicTimer Get Systime
* \description
*   Read the systime.
* \class
*   XpicTimer
* \params
*   pulSystime_s        [out]  Pointer of Systime Value in Seconds
*   pulSystime_ns       [out]  Pointer of Systime Value in Nanoseconds
* \return
*                                                                            */
/*****************************************************************************/
void NX10_XpicTimer_GetSystime( unsigned long* pulSystime_s, unsigned long* pulSystime_ns )
{
  *pulSystime_s  = s_ptXpicTimer->ulXpic_timer_systime_s;
  *pulSystime_ns = s_ptXpicTimer->ulXpic_timer_systime_ns;
}

/*****************************************************************************/
/*! XpicTimer Get Systime Diff
* \description
*   Get the difference of a given timestamp to the current systime.
* \class
*   XpicTimer
* \params
*   pulTime_s   [in]   Pointer to previous systime seconds value
*               [out]  Pointer to systime difference in seconds
*   pulTime_ns  [in]   Pointer to previous systime nanoseconds value
*               [out]  Pointer to systime difference in nanoseconds
* \return
*                                                                            */
/*****************************************************************************/
void NX10_XpicTimer_GetSystimeDiff( unsigned long* pulTime_s, unsigned long* pulTime_ns )
{
  unsigned long ulDiff_s = s_ptXpicTimer->ulXpic_timer_systime_s - *pulTime_s;
  unsigned long ulDiff_ns = s_ptXpicTimer->ulXpic_timer_systime_ns;

  if( ulDiff_ns < *pulTime_ns )
  {
    --ulDiff_s;
    ulDiff_ns += (s_ptSystime->ulSystime_border - *pulTime_ns);
  }
  else
  {
    ulDiff_ns -= *pulTime_ns;
  }
  *pulTime_s = ulDiff_s;
  *pulTime_ns = ulDiff_ns;
}
