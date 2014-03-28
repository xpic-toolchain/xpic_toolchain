#include "NX10_Pwm.h" // function prototypes and defines
#include "defines.h"
#include "regdef_netx10.h"

static NX10_MPWM_AREA_T*           s_ptPwm         = (NX10_MPWM_AREA_T*) Addr_NX10_mpwm;

/*****************************************************************************/
/*! PWM Configure counter
* \description
*   This function configures the waveform and the dual edge mode of a PWM counter.
* \class
*   PWM
* \params
*   uInst             [in] Counter instance
*   fWaveForm         [in] 1/0: Sawtooth/Triangle
*   fDualEdge         [in] 1/0: update shadow registers at (mpwm_cnt==0 and mpwm_cnt==max)/(mpwm_cnt==0)
* \return
*   NX10_MOTION_OKAY
*   NX10_MOTION_ERR_INVAL_PARAM                                                   */
/*****************************************************************************/
NX10_MOTION_RESULT NX10_Pwm_ConfigCnt( unsigned int uInst, int fWaveForm, int fDualEdge )
{
  unsigned long ulVal;

  switch( uInst )
  {
    case 0:
      ulVal = s_ptPwm->ulMpwm_config_counter & ~(MSK_NX10_mpwm_config_counter_waveform_cnt0|MSK_NX10_mpwm_config_counter_dual_edge0);
      s_ptPwm->ulMpwm_config_counter = ulVal | (fWaveForm<<SRT_NX10_mpwm_config_counter_waveform_cnt0) |(fDualEdge<<SRT_NX10_mpwm_config_counter_dual_edge0);
    break;

    case 1:
      ulVal = s_ptPwm->ulMpwm_config_counter & ~(MSK_NX10_mpwm_config_counter_waveform_cnt1|MSK_NX10_mpwm_config_counter_dual_edge1);
      s_ptPwm->ulMpwm_config_counter = ulVal | (fWaveForm<<SRT_NX10_mpwm_config_counter_waveform_cnt1) |(fDualEdge<<SRT_NX10_mpwm_config_counter_dual_edge1);
    break;

    default:
      // invalid instance
      return NX10_MOTION_ERR_INVAL_PARAM;
    break;
  } // end switch

  return NX10_MOTION_OKAY;
}

/*****************************************************************************/
/*! PWM Configure failure
* \description
*   This function configures the failure behaviour.
* \class
*   PWM
* \params
*   fEn         [in] 1/0: failure input sets failure bit (all PWM pins set to defined state) / failure input disabled (but might generate an interrupt)
* \return
*   NX10_MOTION_OKAY
*   NX10_MOTION_ERR_INVAL_PARAM                                                   */
/*****************************************************************************/
NX10_MOTION_RESULT NX10_Pwm_ConfigFailure( int fEn )
{
  if( fEn ) {
    s_ptPwm->ulMpwm_config_failure |= MSK_NX10_mpwm_config_failure_enable;
  } else {
    s_ptPwm->ulMpwm_config_failure &= ~MSK_NX10_mpwm_config_failure_enable;
  }

  return NX10_MOTION_OKAY;
}

/*****************************************************************************/
/*! PWM Set SW failure
* \description
*   This function sets the SW failure signal.
* \class
*   PWM
* \params
*   fEn         [in] 1/0: SW failure on/off
* \return
*   NX10_MOTION_OKAY
*   NX10_MOTION_ERR_INVAL_PARAM                                                   */
/*****************************************************************************/
NX10_MOTION_RESULT NX10_Pwm_SetSwFailure( int fEn )
{
  if( fEn ) {
    s_ptPwm->ulMpwm_config_failure |= MSK_NX10_mpwm_config_failure_failure;
  } else {
    s_ptPwm->ulMpwm_config_failure &= ~MSK_NX10_mpwm_config_failure_failure;
  }

  return NX10_MOTION_OKAY;
}

/*****************************************************************************/
/*! PWM Configure channel
* \description
*   This function configures a PWM output channel.
* \class
*   PWM
* \params
*   uInst             [in] Channel instance
*   ePinCfg           [in] PWM pin configuration
*   fShadowEn         [in] 1/0: shadow enabled/disabled
*   eFailureCfg       [in] Failure configuration
* \return
*   NX10_MOTION_OKAY
*   NX10_MOTION_ERR_INVAL_PARAM                                                   */
/*****************************************************************************/
NX10_MOTION_RESULT NX10_Pwm_ConfigCh( unsigned int uInst, NX10_PWM_PINCFG ePinCfg, int fShadowEn, NX10_PWM_FAILCFG eFailureCfg )
{
  unsigned long ulVal;

  if( uInst>=NX10_PWM_CHANNELS )
    return NX10_MOTION_ERR_INVAL_PARAM;

  switch( uInst )
  {
    case 0:
      ulVal = s_ptPwm->ulMpwm_config_pins & ~(MSK_NX10_mpwm_config_pins_cfg_0|MSK_NX10_mpwm_config_pins_shadow0);
      s_ptPwm->ulMpwm_config_pins = ulVal | (ePinCfg<<SRT_NX10_mpwm_config_pins_cfg_0) | (fShadowEn<<SRT_NX10_mpwm_config_pins_shadow0);

      ulVal = s_ptPwm->ulMpwm_config_failure & (~MSK_NX10_mpwm_config_failure_cfg_0);
      s_ptPwm->ulMpwm_config_failure = ulVal | (eFailureCfg<<SRT_NX10_mpwm_config_failure_cfg_0);
    break;

    case 1:
      ulVal = s_ptPwm->ulMpwm_config_pins & ~(MSK_NX10_mpwm_config_pins_cfg_1|MSK_NX10_mpwm_config_pins_shadow1);
      s_ptPwm->ulMpwm_config_pins = ulVal | (ePinCfg<<SRT_NX10_mpwm_config_pins_cfg_1) | (fShadowEn<<SRT_NX10_mpwm_config_pins_shadow1);

      ulVal = s_ptPwm->ulMpwm_config_failure & ~MSK_NX10_mpwm_config_failure_cfg_1;
      s_ptPwm->ulMpwm_config_failure = ulVal | (eFailureCfg<<SRT_NX10_mpwm_config_failure_cfg_1);
    break;

    case 2:
      ulVal = s_ptPwm->ulMpwm_config_pins & ~(MSK_NX10_mpwm_config_pins_cfg_2|MSK_NX10_mpwm_config_pins_shadow2);
      s_ptPwm->ulMpwm_config_pins = ulVal | (ePinCfg<<SRT_NX10_mpwm_config_pins_cfg_2) | (fShadowEn<<SRT_NX10_mpwm_config_pins_shadow2);

      ulVal = s_ptPwm->ulMpwm_config_failure & (~MSK_NX10_mpwm_config_failure_cfg_2);
      s_ptPwm->ulMpwm_config_failure = ulVal | (eFailureCfg<<SRT_NX10_mpwm_config_failure_cfg_2);
    break;

    case 3:
      ulVal = s_ptPwm->ulMpwm_config_pins & ~(MSK_NX10_mpwm_config_pins_cfg_3|MSK_NX10_mpwm_config_pins_shadow3);
      s_ptPwm->ulMpwm_config_pins = ulVal | (ePinCfg<<SRT_NX10_mpwm_config_pins_cfg_3) | (fShadowEn<<SRT_NX10_mpwm_config_pins_shadow3);

      ulVal = s_ptPwm->ulMpwm_config_failure & (~MSK_NX10_mpwm_config_failure_cfg_3);
      s_ptPwm->ulMpwm_config_failure = ulVal | (eFailureCfg<<SRT_NX10_mpwm_config_failure_cfg_3);
    break;

    case 4:
      ulVal = s_ptPwm->ulMpwm_config_pins & ~(MSK_NX10_mpwm_config_pins_cfg_4|MSK_NX10_mpwm_config_pins_shadow4);
      s_ptPwm->ulMpwm_config_pins = ulVal | (ePinCfg<<SRT_NX10_mpwm_config_pins_cfg_4) | (fShadowEn<<SRT_NX10_mpwm_config_pins_shadow4);

      ulVal = s_ptPwm->ulMpwm_config_failure & (~MSK_NX10_mpwm_config_failure_cfg_4);
      s_ptPwm->ulMpwm_config_failure = ulVal | (eFailureCfg<<SRT_NX10_mpwm_config_failure_cfg_4);
    break;

    case 5:
      ulVal = s_ptPwm->ulMpwm_config_pins & ~(MSK_NX10_mpwm_config_pins_cfg_5|MSK_NX10_mpwm_config_pins_shadow5);
      s_ptPwm->ulMpwm_config_pins = ulVal | (ePinCfg<<SRT_NX10_mpwm_config_pins_cfg_5) | (fShadowEn<<SRT_NX10_mpwm_config_pins_shadow5);

      ulVal = s_ptPwm->ulMpwm_config_failure & (~MSK_NX10_mpwm_config_failure_cfg_5);
      s_ptPwm->ulMpwm_config_failure = ulVal | (eFailureCfg<<SRT_NX10_mpwm_config_failure_cfg_5);
    break;

    case 6:
      ulVal = s_ptPwm->ulMpwm_config_pins & ~(MSK_NX10_mpwm_config_pins_cfg_6|MSK_NX10_mpwm_config_pins_shadow6);
      s_ptPwm->ulMpwm_config_pins = ulVal | (ePinCfg<<SRT_NX10_mpwm_config_pins_cfg_6) | (fShadowEn<<SRT_NX10_mpwm_config_pins_shadow6);

      ulVal = s_ptPwm->ulMpwm_config_failure & (~MSK_NX10_mpwm_config_failure_cfg_6);
      s_ptPwm->ulMpwm_config_failure = ulVal | (eFailureCfg<<SRT_NX10_mpwm_config_failure_cfg_6);
    break;

    case 7:
      ulVal = s_ptPwm->ulMpwm_config_pins & ~(MSK_NX10_mpwm_config_pins_cfg_7|MSK_NX10_mpwm_config_pins_shadow7);
      s_ptPwm->ulMpwm_config_pins = ulVal | (ePinCfg<<SRT_NX10_mpwm_config_pins_cfg_7) | (fShadowEn<<SRT_NX10_mpwm_config_pins_shadow7);

      ulVal = s_ptPwm->ulMpwm_config_failure & (~MSK_NX10_mpwm_config_failure_cfg_7);
      s_ptPwm->ulMpwm_config_failure = ulVal | (eFailureCfg<<SRT_NX10_mpwm_config_failure_cfg_7);
    break;

    default:
      // invalid instance
      return NX10_MOTION_ERR_INVAL_PARAM;
    break;
  } // end switch

  return NX10_MOTION_OKAY;
}

/*****************************************************************************/
/*! PWM Set counter period
* \description
*   This function sets the period of a counter.
*   Note: The counter period must be set after the counter waveform was configured, because it uses the waveform to correct the period value.
* \class
*   PWM
* \params
*   uInst             [in] Counter instance
*   ulPeriod          [in] Period
* \return
*   NX10_MOTION_OKAY
*   NX10_MOTION_ERR_INVAL_PARAM                                                   */
/*****************************************************************************/
NX10_MOTION_RESULT NX10_Pwm_SetCntPeriod( unsigned int uInst, unsigned long ulPeriod )
{
  if( ulPeriod>0x000fffff )
    return NX10_MOTION_ERR_INVAL_PARAM;
  if( uInst>=NX10_PWM_NUM_CNT )
    return NX10_MOTION_ERR_INVAL_PARAM;

  switch( uInst ) {
    case 0:
      if( (s_ptPwm->ulMpwm_config_counter&MSK_NX10_mpwm_config_counter_waveform_cnt0)==0 )
        s_ptPwm->asMpwm_cnt[uInst].ulPeriod = ulPeriod; // triangle
      else
        s_ptPwm->asMpwm_cnt[uInst].ulPeriod = ulPeriod-1; // waveform
    break;

    case 1:
      if( (s_ptPwm->ulMpwm_config_counter&MSK_NX10_mpwm_config_counter_waveform_cnt1)==0 )
        s_ptPwm->asMpwm_cnt[uInst].ulPeriod = ulPeriod; // triangle
      else
        s_ptPwm->asMpwm_cnt[uInst].ulPeriod = ulPeriod-1; // waveform
    break;

  } // end switch

  return NX10_MOTION_OKAY;
}


/*****************************************************************************/
/*! PWM Retigger counter watchdog
* \description
*   This function re-triggers the watchdog of a PWM counter.
*   Note: 0 disables the watchdog.
* \class
*   PWM
* \params
*   uInst             [in] Counter instance
*   ulWdgVal          [in] 0: disable 1..15: set number of cycles until watchdog expires
* \return
*   NX10_MOTION_OKAY
*   NX10_MOTION_ERR_INVAL_PARAM                                                   */
/*****************************************************************************/
NX10_MOTION_RESULT NX10_Pwm_RetriggerCnt( unsigned int uInst, unsigned long ulWdgVal )
{
  if( ulWdgVal>15 )
    return NX10_MOTION_ERR_INVAL_PARAM;
  if( uInst>=NX10_PWM_NUM_CNT )
    return NX10_MOTION_ERR_INVAL_PARAM;

  s_ptPwm->asMpwm_cnt[uInst].ulWatchdog = ulWdgVal;

  return NX10_MOTION_OKAY;
}

/*****************************************************************************/
/*! PWM Set channel width
* \description
*   This function sets the width of a PWM channel.
* \class
*   PWM
* \params
*   uInst             [in] Channel instance
*   ulWidth           [in] Width in steps of 2.5ns
* \return
*   NX10_MOTION_OKAY
*   NX10_MOTION_ERR_INVAL_PARAM                                                   */
/*****************************************************************************/
NX10_MOTION_RESULT NX10_Pwm_SetChWidth( unsigned int uInst, unsigned long ulWidth )
{
  if( ulWidth>0x000fffff )
    return NX10_MOTION_ERR_INVAL_PARAM;
  if( uInst>=NX10_PWM_CHANNELS )
    return NX10_MOTION_ERR_INVAL_PARAM;

  s_ptPwm->aulMpwm_t[uInst] = ulWidth;

  return NX10_MOTION_OKAY;
}

/*****************************************************************************/
/*! PWM Configure interrupt source
* \description
*   This function configures the mode and the delay of an interrupt source.
* \class
*   PWM
* \params
*   uInst             [in] Source instance
*   eIrqCfg           [in] Select PWM pin for interrupt
*   uDelay            [in] Delay from event to generation interrupt in steps of 20ns
* \return
*   NX10_MOTION_OKAY
*   NX10_MOTION_ERR_INVAL_PARAM                                                   */
/*****************************************************************************/
NX10_MOTION_RESULT NX10_Pwm_CfgIrqSrc( unsigned int uInst, NX10_PWM_IRQCFG eIrqCfg, unsigned int uDelay )
{
  unsigned long ulVal;

  if( uDelay>0x000003ff )
    return NX10_MOTION_ERR_INVAL_PARAM;
  if( uInst>=NX10_PWM_IRQ_SRC_NUM )
    return NX10_MOTION_ERR_INVAL_PARAM;

  switch( uInst ) {
    case 0:
      ulVal = s_ptPwm->ulMpwm_irq_config & ~(MSK_NX10_mpwm_irq_config_nr0|MSK_NX10_mpwm_irq_config_delay0);
      s_ptPwm->ulMpwm_irq_config = ulVal | (eIrqCfg<<SRT_NX10_mpwm_irq_config_nr0) | (uDelay<<SRT_NX10_mpwm_irq_config_delay0);
    break;
    case 1:
      ulVal = s_ptPwm->ulMpwm_irq_config & ~( MSK_NX10_mpwm_irq_config_nr1|MSK_NX10_mpwm_irq_config_delay1);
      s_ptPwm->ulMpwm_irq_config = ulVal | (eIrqCfg<<SRT_NX10_mpwm_irq_config_nr1) | (uDelay<<SRT_NX10_mpwm_irq_config_delay1);
    break;
    default:
      return NX10_MOTION_ERR_INVAL_PARAM; // invalid instance
    break; // invalid instance
  } // end switch

  return NX10_MOTION_OKAY;
}

/*****************************************************************************/
/*! PWM Get counter status
* \description
*   This function reads the actual counter value and the captures systime at counter==0.
* \class
*   PWM
* \params
*   uInst             [in]  Counter instance
*   ulValue           [out] Actual value
*   ulSystime         [out] Captured systime_ns at counter=0
* \return
*   NX10_MOTION_OKAY
*   NX10_MOTION_ERR_INVAL_PARAM                                                   */
/*****************************************************************************/
NX10_MOTION_RESULT NX10_Pwm_GetCntStat( unsigned int uInst, unsigned long* pulValue, unsigned int* pulSystime )
{
  if( uInst>=NX10_PWM_NUM_CNT )
    return NX10_MOTION_ERR_INVAL_PARAM;

  *pulValue   = s_ptPwm->asMpwm_cnt[uInst].ulCnt;
  *pulSystime = s_ptPwm->asMpwm_cnt[uInst].ulSystime;

  return NX10_MOTION_OKAY;
}

/*****************************************************************************/
/*! PWM Start
* \description
*   This function starts the PWM unit. Both counters can be started individually.
* \class
*   PWM
* \params
* \return
*   NX10_MOTION_OKAY                                                              */
/*****************************************************************************/
NX10_MOTION_RESULT NX10_Pwm_Start( int fCnt0En, int fCnt1En )
{
  unsigned int ulVal;

  /* get current config */
  ulVal = s_ptPwm->ulMpwm_config_counter;

  /* include CntRun if enabled */
  if( fCnt0En ) {
    ulVal |= MSK_NX10_mpwm_config_counter_run_cnt0;
  }
  if( fCnt1En ) {
    ulVal |= MSK_NX10_mpwm_config_counter_run_cnt1;
  }
  /* write config back */
  s_ptPwm->ulMpwm_config_counter = ulVal;

  return NX10_MOTION_OKAY;
}

/*****************************************************************************/
/*! PWM Stop
* \description
*   This function stops the PWM unit. Both counters can be stopped individually.
* \class
*   PWM
* \params
* \return
*   NX10_MOTION_OKAY                                                              */
/*****************************************************************************/
NX10_MOTION_RESULT NX10_Pwm_Stop( int fCnt0Dis, int fCnt1Dis )
{
  unsigned int ulVal;

  /* get current config */
  ulVal = s_ptPwm->ulMpwm_config_counter;

  /* exclude CntRun if disabled */
  if( fCnt0Dis ) {
    ulVal &= ~MSK_NX10_mpwm_config_counter_run_cnt0;
  }
  if( fCnt1Dis ) {
    ulVal &= ~MSK_NX10_mpwm_config_counter_run_cnt1;
  }
  /* write config back */
  s_ptPwm->ulMpwm_config_counter = ulVal;

  return NX10_MOTION_OKAY;
}

/*****************************************************************************/
/*! PWM Enable Interrupt(s)
* \description
*   This function enables a set of interrupts.
* \class
*   PWM
* \params
*  fSrc0IrqEn     [in] 1: enable src0 interrupt
*  fSrc1IrqEn     [in] 1: enable src1 interrupt
*  fFailureIrqEn  [in] 1: enable failure interrupt
* \return
*   NX10_MOTION_OKAY                                                              */
/*****************************************************************************/
NX10_MOTION_RESULT NX10_Pwm_EnIrq( int fSrc0IrqEn, int fSrc1IrqEn, int fFailureIrqEn )
{
  s_ptPwm->ulMpwm_irq_msk_set =  (fSrc0IrqEn<<SRT_NX10_mpwm_irq_msk_set_interrupt0)
                                |(fSrc1IrqEn<<SRT_NX10_mpwm_irq_msk_set_interrupt1)
                                |(fFailureIrqEn<<SRT_NX10_mpwm_irq_msk_set_failure);
  return NX10_MOTION_OKAY;
}

/*****************************************************************************/
/*! PWM Disable Interrupt(s)
* \description
*   This function disables a set of interrupts.
* \class
*   PWM
* \params
*  fSrc0IrqDis    [in] 1: disable src0 interrupt
*  fSrc1IrqDis    [in] 1: disable src1 interrupt
*  fFailureIrqDis [in] 1: disable failure interrupt
* \return
*   NX10_MOTION_OKAY                                                              */
/*****************************************************************************/
NX10_MOTION_RESULT NX10_Pwm_DisIrq( int fSrc0IrqDis, int fSrc1IrqDis, int fFailureIrqDis )
{
  s_ptPwm->ulMpwm_irq_msk_reset =  (fSrc0IrqDis<<SRT_NX10_mpwm_irq_msk_set_interrupt0)
                                  |(fSrc1IrqDis<<SRT_NX10_mpwm_irq_msk_set_interrupt1)
                                  |(fFailureIrqDis<<SRT_NX10_mpwm_irq_msk_set_failure);
  return NX10_MOTION_OKAY;
}

/*****************************************************************************/
/*! PWM Get Interrupt(s)
* \description
*   This function retrieves the current interrupt masked requests of the PWM unit.
* \class
*   PWM
* \params
   pulIrq     [out] Pointer to interrupt events
* \return
*   NX10_MOTION_OKAY                                                              */
/*****************************************************************************/
NX10_MOTION_RESULT NX10_Pwm_GetIrq( unsigned long *pulIrq )
{
  /* get irq requests */
  *pulIrq = s_ptPwm->ulMpwm_irq_masked;

  return NX10_MOTION_OKAY;
}

/*****************************************************************************/
/*! PWM Get Interrupt(s)
* \description
*   This function retrieves the current interrupt raw requests of the PWM unit.
* \class
*   PWM
* \params
   pulIrq     [out] Pointer to interrupt events
* \return
*   NX10_MOTION_OKAY                                                              */
/*****************************************************************************/
NX10_MOTION_RESULT NX10_Pwm_GetIrqRaw( unsigned long *pulIrq )
{
  /* get irq requests */
  *pulIrq = s_ptPwm->ulMpwm_irq_raw;

  return NX10_MOTION_OKAY;
}

/*****************************************************************************/
/*! PWM Confirm interrupt(s)
* \description
*   This function confirms a set of interrupts that were requested by the PWM unit.
* \class
*   PWM
* \params
*   ulConfirmIrqMask  [in] Mask to confirm interrupt events
* \return
*   NX10_MOTION_OKAY                                                              */
/*****************************************************************************/
NX10_MOTION_RESULT NX10_Pwm_ConfirmIrq( unsigned long ulConfirmIrqMask )
{
  /* confirm the set of irqs */
  s_ptPwm->ulMpwm_irq_raw = ulConfirmIrqMask;

  return NX10_MOTION_OKAY;
}

/*****************************************************************************/
/*! PWM Set Parameter
* \description
*   This function writes parameter.
* \class
*   PWM
* \params
*   ulPrmID          [in]  Parameter ID
*   ulPrmVal         [in]  Parameter value
* \return
*   NX10_MOTION_OKAY
*   NX10_MOTION_ERR_INVAL_PARAM                                                   */
/*****************************************************************************/
NX10_MOTION_RESULT NX10_Pwm_SetPrm( unsigned long ulPrmID, unsigned long ulPrmVal )
{
  switch( ulPrmID )
  {
  /* Config_Counter */
    case NX10_PWM_PRM_WAVEFORM_CNT0:
      s_ptPwm->ulMpwm_config_counter &= ~MSK_NX10_mpwm_config_counter_waveform_cnt0;
      s_ptPwm->ulMpwm_config_counter |= (ulPrmVal<<SRT_NX10_mpwm_config_counter_waveform_cnt0);
    break;
    case NX10_PWM_PRM_DUAL_EDGE0:
      s_ptPwm->ulMpwm_config_counter &= ~MSK_NX10_mpwm_config_counter_dual_edge0;
      s_ptPwm->ulMpwm_config_counter |= (ulPrmVal<<SRT_NX10_mpwm_config_counter_dual_edge0);
    break;
    case NX10_PWM_PRM_WAVEFORM_CNT1:
      s_ptPwm->ulMpwm_config_counter &= ~MSK_NX10_mpwm_config_counter_waveform_cnt1;
      s_ptPwm->ulMpwm_config_counter |= (ulPrmVal<<SRT_NX10_mpwm_config_counter_waveform_cnt1);
    break;
    case NX10_PWM_PRM_DUAL_EDGE1:
      s_ptPwm->ulMpwm_config_counter &= ~MSK_NX10_mpwm_config_counter_dual_edge1;
      s_ptPwm->ulMpwm_config_counter |= (ulPrmVal<<SRT_NX10_mpwm_config_counter_dual_edge1);
    break;

  /* Config_Pins */
    case NX10_PWM_PRM_CFG_PIN0:
      s_ptPwm->ulMpwm_config_pins &= ~MSK_NX10_mpwm_config_pins_cfg_0;
      s_ptPwm->ulMpwm_config_pins |= (ulPrmVal<<SRT_NX10_mpwm_config_pins_cfg_0);
    break;
    case NX10_PWM_PRM_SHADOW0:
      s_ptPwm->ulMpwm_config_pins &= ~MSK_NX10_mpwm_config_pins_shadow0;
      s_ptPwm->ulMpwm_config_pins |= (ulPrmVal<<SRT_NX10_mpwm_config_pins_shadow0);
    break;
    case NX10_PWM_PRM_CFG_PIN1:
      s_ptPwm->ulMpwm_config_pins &= ~MSK_NX10_mpwm_config_pins_cfg_1;
      s_ptPwm->ulMpwm_config_pins |= (ulPrmVal<<SRT_NX10_mpwm_config_pins_cfg_1);
    break;
    case NX10_PWM_PRM_SHADOW1:
      s_ptPwm->ulMpwm_config_pins &= ~MSK_NX10_mpwm_config_pins_shadow1;
      s_ptPwm->ulMpwm_config_pins |= (ulPrmVal<<SRT_NX10_mpwm_config_pins_shadow1);
    break;
    case NX10_PWM_PRM_CFG_PIN2:
      s_ptPwm->ulMpwm_config_pins &= ~MSK_NX10_mpwm_config_pins_cfg_2;
      s_ptPwm->ulMpwm_config_pins |= (ulPrmVal<<SRT_NX10_mpwm_config_pins_cfg_2);
    break;
    case NX10_PWM_PRM_SHADOW2:
      s_ptPwm->ulMpwm_config_pins &= ~MSK_NX10_mpwm_config_pins_shadow2;
      s_ptPwm->ulMpwm_config_pins |= (ulPrmVal<<SRT_NX10_mpwm_config_pins_shadow2);
    break;
    case NX10_PWM_PRM_CFG_PIN3:
      s_ptPwm->ulMpwm_config_pins &= ~MSK_NX10_mpwm_config_pins_cfg_3;
      s_ptPwm->ulMpwm_config_pins |= (ulPrmVal<<SRT_NX10_mpwm_config_pins_cfg_3);
    break;
    case NX10_PWM_PRM_SHADOW3:
      s_ptPwm->ulMpwm_config_pins &= ~MSK_NX10_mpwm_config_pins_shadow3;
      s_ptPwm->ulMpwm_config_pins |= (ulPrmVal<<SRT_NX10_mpwm_config_pins_shadow3);
    break;
    case NX10_PWM_PRM_CFG_PIN4:
      s_ptPwm->ulMpwm_config_pins &= ~MSK_NX10_mpwm_config_pins_cfg_4;
      s_ptPwm->ulMpwm_config_pins |= (ulPrmVal<<SRT_NX10_mpwm_config_pins_cfg_4);
    break;
    case NX10_PWM_PRM_SHADOW4:
      s_ptPwm->ulMpwm_config_pins &= ~MSK_NX10_mpwm_config_pins_shadow4;
      s_ptPwm->ulMpwm_config_pins |= (ulPrmVal<<SRT_NX10_mpwm_config_pins_shadow4);
    break;
    case NX10_PWM_PRM_CFG_PIN5:
      s_ptPwm->ulMpwm_config_pins &= ~MSK_NX10_mpwm_config_pins_cfg_5;
      s_ptPwm->ulMpwm_config_pins |= (ulPrmVal<<SRT_NX10_mpwm_config_pins_cfg_5);
    break;
    case NX10_PWM_PRM_SHADOW5:
      s_ptPwm->ulMpwm_config_pins &= ~MSK_NX10_mpwm_config_pins_shadow5;
      s_ptPwm->ulMpwm_config_pins |= (ulPrmVal<<SRT_NX10_mpwm_config_pins_shadow5);
    break;
    case NX10_PWM_PRM_CFG_PIN6:
      s_ptPwm->ulMpwm_config_pins &= ~MSK_NX10_mpwm_config_pins_cfg_6;
      s_ptPwm->ulMpwm_config_pins |= (ulPrmVal<<SRT_NX10_mpwm_config_pins_cfg_6);
    break;
    case NX10_PWM_PRM_SHADOW6:
      s_ptPwm->ulMpwm_config_pins &= ~MSK_NX10_mpwm_config_pins_shadow6;
      s_ptPwm->ulMpwm_config_pins |= (ulPrmVal<<SRT_NX10_mpwm_config_pins_shadow6);
    break;
    case NX10_PWM_PRM_CFG_PIN7:
      s_ptPwm->ulMpwm_config_pins &= ~MSK_NX10_mpwm_config_pins_cfg_7;
      s_ptPwm->ulMpwm_config_pins |= (ulPrmVal<<SRT_NX10_mpwm_config_pins_cfg_7);
    break;
    case NX10_PWM_PRM_SHADOW7:
      s_ptPwm->ulMpwm_config_pins &= ~MSK_NX10_mpwm_config_pins_shadow7;
      s_ptPwm->ulMpwm_config_pins |= (ulPrmVal<<SRT_NX10_mpwm_config_pins_shadow7);
    break;

  /* Config_Failure */
    case NX10_PWM_PRM_CFG_FAIL0:
      s_ptPwm->ulMpwm_config_failure &= ~MSK_NX10_mpwm_config_failure_cfg_0;
      s_ptPwm->ulMpwm_config_failure |= (ulPrmVal<<SRT_NX10_mpwm_config_failure_cfg_0);
    break;
    case NX10_PWM_PRM_CFG_FAIL1:
      s_ptPwm->ulMpwm_config_failure &= ~MSK_NX10_mpwm_config_failure_cfg_1;
      s_ptPwm->ulMpwm_config_failure |= (ulPrmVal<<SRT_NX10_mpwm_config_failure_cfg_1);
    break;
    case NX10_PWM_PRM_CFG_FAIL2:
      s_ptPwm->ulMpwm_config_failure &= ~MSK_NX10_mpwm_config_failure_cfg_2;
      s_ptPwm->ulMpwm_config_failure |= (ulPrmVal<<SRT_NX10_mpwm_config_failure_cfg_2);
    break;
    case NX10_PWM_PRM_CFG_FAIL3:
      s_ptPwm->ulMpwm_config_failure &= ~MSK_NX10_mpwm_config_failure_cfg_3;
      s_ptPwm->ulMpwm_config_failure |= (ulPrmVal<<SRT_NX10_mpwm_config_failure_cfg_3);
    break;
    case NX10_PWM_PRM_CFG_FAIL4:
      s_ptPwm->ulMpwm_config_failure &= ~MSK_NX10_mpwm_config_failure_cfg_4;
      s_ptPwm->ulMpwm_config_failure |= (ulPrmVal<<SRT_NX10_mpwm_config_failure_cfg_4);
    break;
    case NX10_PWM_PRM_CFG_FAIL5:
      s_ptPwm->ulMpwm_config_failure &= ~MSK_NX10_mpwm_config_failure_cfg_5;
      s_ptPwm->ulMpwm_config_failure |= (ulPrmVal<<SRT_NX10_mpwm_config_failure_cfg_5);
    break;
    case NX10_PWM_PRM_CFG_FAIL6:
      s_ptPwm->ulMpwm_config_failure &= ~MSK_NX10_mpwm_config_failure_cfg_6;
      s_ptPwm->ulMpwm_config_failure |= (ulPrmVal<<SRT_NX10_mpwm_config_failure_cfg_6);
    break;
    case NX10_PWM_PRM_CFG_FAIL7:
      s_ptPwm->ulMpwm_config_failure &= ~MSK_NX10_mpwm_config_failure_cfg_7;
      s_ptPwm->ulMpwm_config_failure |= (ulPrmVal<<SRT_NX10_mpwm_config_failure_cfg_7);
    break;
    case NX10_PWM_PRM_FAIL_INPUT_EN:
      s_ptPwm->ulMpwm_config_failure &= ~MSK_NX10_mpwm_config_failure_enable;
      s_ptPwm->ulMpwm_config_failure |= (ulPrmVal<<SRT_NX10_mpwm_config_failure_enable);
    break;
    case NX10_PWM_PRM_FAIL_FAILURE:
      s_ptPwm->ulMpwm_config_failure &= ~MSK_NX10_mpwm_config_failure_failure;
      s_ptPwm->ulMpwm_config_failure |= (ulPrmVal<<SRT_NX10_mpwm_config_failure_failure);
    break;

    /* Irq_Config */
    case NX10_PWM_PRM_IRQCFG0:
      s_ptPwm->ulMpwm_irq_config &= ~MSK_NX10_mpwm_irq_config_nr0;
      s_ptPwm->ulMpwm_irq_config |= (ulPrmVal<<SRT_NX10_mpwm_irq_config_nr0);
    break;
    case NX10_PWM_PRM_IRQDELAY0:
      s_ptPwm->ulMpwm_irq_config &= ~MSK_NX10_mpwm_irq_config_delay0;
      s_ptPwm->ulMpwm_irq_config |= (ulPrmVal<<SRT_NX10_mpwm_irq_config_delay0);
    break;
    case NX10_PWM_PRM_IRQCFG1:
      s_ptPwm->ulMpwm_irq_config &= ~MSK_NX10_mpwm_irq_config_nr1;
      s_ptPwm->ulMpwm_irq_config |= (ulPrmVal<<SRT_NX10_mpwm_irq_config_nr1);
    break;
    case NX10_PWM_PRM_IRQDELAY1:
      s_ptPwm->ulMpwm_irq_config &= ~MSK_NX10_mpwm_irq_config_delay1;
      s_ptPwm->ulMpwm_irq_config |= (ulPrmVal<<SRT_NX10_mpwm_irq_config_delay1);
    break;

    /* Interrupts */
    case NX10_PWM_PRM_IRQMSKSET: s_ptPwm->ulMpwm_irq_msk_set = ulPrmVal; break;
    case NX10_PWM_PRM_IRQMSKRST: s_ptPwm->ulMpwm_irq_msk_reset = ulPrmVal; break;

    /* Counters */
    case NX10_PWM_PRM_CNT0PERIOD: s_ptPwm->asMpwm_cnt[0].ulPeriod = ulPrmVal; break;
    case NX10_PWM_PRM_CNT1PERIOD: s_ptPwm->asMpwm_cnt[1].ulPeriod = ulPrmVal; break;

    /* T0..T8 */
    case NX10_PWM_PRM_T0: s_ptPwm->aulMpwm_t[0] = ulPrmVal; break;
    case NX10_PWM_PRM_T1: s_ptPwm->aulMpwm_t[1] = ulPrmVal; break;
    case NX10_PWM_PRM_T2: s_ptPwm->aulMpwm_t[2] = ulPrmVal; break;
    case NX10_PWM_PRM_T3: s_ptPwm->aulMpwm_t[3] = ulPrmVal; break;
    case NX10_PWM_PRM_T4: s_ptPwm->aulMpwm_t[4] = ulPrmVal; break;
    case NX10_PWM_PRM_T5: s_ptPwm->aulMpwm_t[5] = ulPrmVal; break;
    case NX10_PWM_PRM_T6: s_ptPwm->aulMpwm_t[6] = ulPrmVal; break;
    case NX10_PWM_PRM_T7: s_ptPwm->aulMpwm_t[7] = ulPrmVal; break;

    /* Default */
    default: return NX10_MOTION_ERR_INVAL_PARAM; break;
  } /* end switch */

  return NX10_MOTION_OKAY;
}


/*****************************************************************************/
/*! PWM Get Parameter
* \description
*   This function reads parameter.
* \class
*   PWM
* \params
*   ulPrmID          [in]  Parameter ID
*   pulPrmVal        [in]  Pointer to parameter value
* \return
*   NX10_MOTION_OKAY
*   NX10_MOTION_ERR_INVAL_PARAM                                                   */
/*****************************************************************************/
NX10_MOTION_RESULT Pwm_GetPrm( unsigned long ulPrmID, unsigned long* pulPrmVal )
{
  switch( ulPrmID )
  {
  /* Config_Counter */
    case NX10_PWM_PRM_RUN_CNT0:
      *pulPrmVal = (s_ptPwm->ulMpwm_config_counter & MSK_NX10_mpwm_config_counter_run_cnt0)>>SRT_NX10_mpwm_config_counter_run_cnt0;
    break;
    case NX10_PWM_PRM_WAVEFORM_CNT0:
      *pulPrmVal = (s_ptPwm->ulMpwm_config_counter & MSK_NX10_mpwm_config_counter_waveform_cnt0)>>SRT_NX10_mpwm_config_counter_waveform_cnt0;
    break;
    case NX10_PWM_PRM_DUAL_EDGE0:
      *pulPrmVal = (s_ptPwm->ulMpwm_config_counter & MSK_NX10_mpwm_config_counter_dual_edge0)>>SRT_NX10_mpwm_config_counter_dual_edge0;
    break;
    case NX10_PWM_PRM_RUN_CNT1:
      *pulPrmVal = (s_ptPwm->ulMpwm_config_counter & MSK_NX10_mpwm_config_counter_run_cnt1)>>SRT_NX10_mpwm_config_counter_run_cnt1;
    break;
    case NX10_PWM_PRM_WAVEFORM_CNT1:
      *pulPrmVal = (s_ptPwm->ulMpwm_config_counter & MSK_NX10_mpwm_config_counter_waveform_cnt1)>>SRT_NX10_mpwm_config_counter_waveform_cnt1;
    break;
    case NX10_PWM_PRM_DUAL_EDGE1:
      *pulPrmVal = (s_ptPwm->ulMpwm_config_counter & MSK_NX10_mpwm_config_counter_dual_edge1)>>SRT_NX10_mpwm_config_counter_dual_edge1;
    break;

  /* Config_Pins */
    case NX10_PWM_PRM_CFG_PIN0:
      *pulPrmVal = (s_ptPwm->ulMpwm_config_pins & MSK_NX10_mpwm_config_pins_cfg_0)>>SRT_NX10_mpwm_config_pins_cfg_0;
    break;
    case NX10_PWM_PRM_SHADOW0:
      *pulPrmVal = (s_ptPwm->ulMpwm_config_pins & MSK_NX10_mpwm_config_pins_shadow0)>>SRT_NX10_mpwm_config_pins_shadow0;
    break;
    case NX10_PWM_PRM_CFG_PIN1:
      *pulPrmVal = (s_ptPwm->ulMpwm_config_pins & MSK_NX10_mpwm_config_pins_cfg_1)>>SRT_NX10_mpwm_config_pins_cfg_1;
    break;
    case NX10_PWM_PRM_SHADOW1:
      *pulPrmVal = (s_ptPwm->ulMpwm_config_pins & MSK_NX10_mpwm_config_pins_shadow1)>>SRT_NX10_mpwm_config_pins_shadow1;
    break;
    case NX10_PWM_PRM_CFG_PIN2:
      *pulPrmVal = (s_ptPwm->ulMpwm_config_pins & MSK_NX10_mpwm_config_pins_cfg_2)>>SRT_NX10_mpwm_config_pins_cfg_2;
    break;
    case NX10_PWM_PRM_SHADOW2:
      *pulPrmVal = (s_ptPwm->ulMpwm_config_pins & MSK_NX10_mpwm_config_pins_shadow2)>>SRT_NX10_mpwm_config_pins_shadow2;
    break;
    case NX10_PWM_PRM_CFG_PIN3:
      *pulPrmVal = (s_ptPwm->ulMpwm_config_pins & MSK_NX10_mpwm_config_pins_cfg_3)>>SRT_NX10_mpwm_config_pins_cfg_3;
    break;
    case NX10_PWM_PRM_SHADOW3:
      *pulPrmVal = (s_ptPwm->ulMpwm_config_pins & MSK_NX10_mpwm_config_pins_shadow3)>>SRT_NX10_mpwm_config_pins_shadow3;
    break;
    case NX10_PWM_PRM_CFG_PIN4:
      *pulPrmVal = (s_ptPwm->ulMpwm_config_pins & MSK_NX10_mpwm_config_pins_cfg_4)>>SRT_NX10_mpwm_config_pins_cfg_4;
    break;
    case NX10_PWM_PRM_SHADOW4:
      *pulPrmVal = (s_ptPwm->ulMpwm_config_pins & MSK_NX10_mpwm_config_pins_shadow4)>>SRT_NX10_mpwm_config_pins_shadow4;
    break;
    case NX10_PWM_PRM_CFG_PIN5:
      *pulPrmVal = (s_ptPwm->ulMpwm_config_pins & MSK_NX10_mpwm_config_pins_cfg_5)>>SRT_NX10_mpwm_config_pins_cfg_5;
    break;
    case NX10_PWM_PRM_SHADOW5:
      *pulPrmVal = (s_ptPwm->ulMpwm_config_pins & MSK_NX10_mpwm_config_pins_shadow5)>>SRT_NX10_mpwm_config_pins_shadow5;
    break;
    case NX10_PWM_PRM_CFG_PIN6:
      *pulPrmVal = (s_ptPwm->ulMpwm_config_pins & MSK_NX10_mpwm_config_pins_cfg_6)>>SRT_NX10_mpwm_config_pins_cfg_6;
    break;
    case NX10_PWM_PRM_SHADOW6:
      *pulPrmVal = (s_ptPwm->ulMpwm_config_pins & MSK_NX10_mpwm_config_pins_shadow6)>>SRT_NX10_mpwm_config_pins_shadow6;
    break;
    case NX10_PWM_PRM_CFG_PIN7:
      *pulPrmVal = (s_ptPwm->ulMpwm_config_pins & MSK_NX10_mpwm_config_pins_cfg_7)>>SRT_NX10_mpwm_config_pins_cfg_7;
    break;
    case NX10_PWM_PRM_SHADOW7:
      *pulPrmVal = (s_ptPwm->ulMpwm_config_pins & MSK_NX10_mpwm_config_pins_shadow7)>>SRT_NX10_mpwm_config_pins_shadow7;
    break;

  /* Config_Failure */
    case NX10_PWM_PRM_CFG_FAIL0:
      *pulPrmVal = (s_ptPwm->ulMpwm_config_failure & MSK_NX10_mpwm_config_failure_cfg_0)>>SRT_NX10_mpwm_config_failure_cfg_0;
    break;
    case NX10_PWM_PRM_CFG_FAIL1:
      *pulPrmVal = (s_ptPwm->ulMpwm_config_failure & MSK_NX10_mpwm_config_failure_cfg_1)>>SRT_NX10_mpwm_config_failure_cfg_1;
    break;
    case NX10_PWM_PRM_CFG_FAIL2:
      *pulPrmVal = (s_ptPwm->ulMpwm_config_failure & MSK_NX10_mpwm_config_failure_cfg_2)>>SRT_NX10_mpwm_config_failure_cfg_2;
    break;
    case NX10_PWM_PRM_CFG_FAIL3:
      *pulPrmVal = (s_ptPwm->ulMpwm_config_failure & MSK_NX10_mpwm_config_failure_cfg_3)>>SRT_NX10_mpwm_config_failure_cfg_3;
    break;
    case NX10_PWM_PRM_CFG_FAIL4:
      *pulPrmVal = (s_ptPwm->ulMpwm_config_failure & MSK_NX10_mpwm_config_failure_cfg_4)>>SRT_NX10_mpwm_config_failure_cfg_4;
    break;
    case NX10_PWM_PRM_CFG_FAIL5:
      *pulPrmVal = (s_ptPwm->ulMpwm_config_failure & MSK_NX10_mpwm_config_failure_cfg_5)>>SRT_NX10_mpwm_config_failure_cfg_5;
    break;
    case NX10_PWM_PRM_CFG_FAIL6:
      *pulPrmVal = (s_ptPwm->ulMpwm_config_failure & MSK_NX10_mpwm_config_failure_cfg_6)>>SRT_NX10_mpwm_config_failure_cfg_6;
    break;
    case NX10_PWM_PRM_CFG_FAIL7:
      *pulPrmVal = (s_ptPwm->ulMpwm_config_failure & MSK_NX10_mpwm_config_failure_cfg_7)>>SRT_NX10_mpwm_config_failure_cfg_7;
    break;
    case NX10_PWM_PRM_FAIL_INPUT_EN:
      *pulPrmVal = (s_ptPwm->ulMpwm_config_failure & MSK_NX10_mpwm_config_failure_enable)>>SRT_NX10_mpwm_config_failure_enable;
    break;
    case NX10_PWM_PRM_FAIL_FAILURE:
      *pulPrmVal = (s_ptPwm->ulMpwm_config_failure & MSK_NX10_mpwm_config_failure_failure)>>SRT_NX10_mpwm_config_failure_failure;
    break;

  /* Irq_Config */
    case NX10_PWM_PRM_IRQCFG0:
      *pulPrmVal = (s_ptPwm->ulMpwm_irq_config & MSK_NX10_mpwm_irq_config_nr0)>>SRT_NX10_mpwm_irq_config_nr0;
    break;
    case NX10_PWM_PRM_IRQDELAY0:
      *pulPrmVal = (s_ptPwm->ulMpwm_irq_config & MSK_NX10_mpwm_irq_config_delay0)>>SRT_NX10_mpwm_irq_config_delay0;
    break;
    case NX10_PWM_PRM_IRQCFG1:
      *pulPrmVal = (s_ptPwm->ulMpwm_irq_config & MSK_NX10_mpwm_irq_config_nr1)>>SRT_NX10_mpwm_irq_config_nr1;
    break;
    case NX10_PWM_PRM_IRQDELAY1:
      *pulPrmVal = (s_ptPwm->ulMpwm_irq_config & MSK_NX10_mpwm_irq_config_delay1)>>SRT_NX10_mpwm_irq_config_delay1;
    break;

    /* Interrupt */
    case NX10_PWM_PRM_IRQMSKSET: *pulPrmVal = s_ptPwm->ulMpwm_irq_msk_set; break;
    case NX10_PWM_PRM_IRQMSKRST: *pulPrmVal = s_ptPwm->ulMpwm_irq_msk_reset; break;

    /* Counters */
    case NX10_PWM_PRM_CNT0PERIOD:  *pulPrmVal = s_ptPwm->asMpwm_cnt[0].ulPeriod; break;
    case NX10_PWM_PRM_CNT0:        *pulPrmVal = s_ptPwm->asMpwm_cnt[0].ulCnt; break;
    case NX10_PWM_PRM_CNT0SYSTIME: *pulPrmVal = s_ptPwm->asMpwm_cnt[0].ulSystime; break;
    case NX10_PWM_PRM_CNT0WDG:     *pulPrmVal = s_ptPwm->asMpwm_cnt[0].ulWatchdog; break;
    case NX10_PWM_PRM_CNT1PERIOD:  *pulPrmVal = s_ptPwm->asMpwm_cnt[1].ulPeriod; break;
    case NX10_PWM_PRM_CNT1:        *pulPrmVal = s_ptPwm->asMpwm_cnt[1].ulCnt; break;
    case NX10_PWM_PRM_CNT1SYSTIME: *pulPrmVal = s_ptPwm->asMpwm_cnt[1].ulSystime; break;
    case NX10_PWM_PRM_CNT1WDG:     *pulPrmVal = s_ptPwm->asMpwm_cnt[1].ulWatchdog; break;

    /* T0..T8 */
    case NX10_PWM_PRM_T0: *pulPrmVal = s_ptPwm->aulMpwm_t[0]; break;
    case NX10_PWM_PRM_T1: *pulPrmVal = s_ptPwm->aulMpwm_t[1];  break;
    case NX10_PWM_PRM_T2: *pulPrmVal = s_ptPwm->aulMpwm_t[2];  break;
    case NX10_PWM_PRM_T3: *pulPrmVal = s_ptPwm->aulMpwm_t[3];  break;
    case NX10_PWM_PRM_T4: *pulPrmVal = s_ptPwm->aulMpwm_t[4];  break;
    case NX10_PWM_PRM_T5: *pulPrmVal = s_ptPwm->aulMpwm_t[5];  break;
    case NX10_PWM_PRM_T6: *pulPrmVal = s_ptPwm->aulMpwm_t[6];  break;
    case NX10_PWM_PRM_T7: *pulPrmVal = s_ptPwm->aulMpwm_t[7];  break;

  /* T0_Shadow..T8_Shadow */
    case NX10_PWM_PRM_T0_SHADOW: *pulPrmVal = s_ptPwm->aulMpwm_t_shadow[0]; break;
    case NX10_PWM_PRM_T1_SHADOW: *pulPrmVal = s_ptPwm->aulMpwm_t_shadow[1];  break;
    case NX10_PWM_PRM_T2_SHADOW: *pulPrmVal = s_ptPwm->aulMpwm_t_shadow[2];  break;
    case NX10_PWM_PRM_T3_SHADOW: *pulPrmVal = s_ptPwm->aulMpwm_t_shadow[3];  break;
    case NX10_PWM_PRM_T4_SHADOW: *pulPrmVal = s_ptPwm->aulMpwm_t_shadow[4];  break;
    case NX10_PWM_PRM_T5_SHADOW: *pulPrmVal = s_ptPwm->aulMpwm_t_shadow[5];  break;
    case NX10_PWM_PRM_T6_SHADOW: *pulPrmVal = s_ptPwm->aulMpwm_t_shadow[6];  break;
    case NX10_PWM_PRM_T7_SHADOW: *pulPrmVal = s_ptPwm->aulMpwm_t_shadow[7];  break;

  /* Default */
    default:
      return NX10_MOTION_ERR_INVAL_PARAM;
    break;
  } /* end switch */

  return NX10_MOTION_OKAY;
}
