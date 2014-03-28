#include "NX10_Adc.h" // function prototypes and defines
#include "defines.h"
#include "regdef_netx10.h"

static NX10_ADC_CTRL_AREA_T* s_ptAdc = (NX10_ADC_CTRL_AREA_T*) Addr_NX10_adc_ctrl;


/*****************************************************************************/
/*! ADC Set Operational/PowerDown
* \description
*   This function sets the ADC into operational or power down mode.
* \class
*   ADC
* \params
*  fOpMode   [in] 1/0: operational/PowerDown
* \return
*   NX10_MOTION_OKAY                                                              */
/*****************************************************************************/
NX10_MOTION_RESULT NX10_Adc_SetOpmode( unsigned int uAdcNum, int fOpMode )
{
  unsigned int uVal=0;

  switch( uAdcNum )
  {
    /* ADC0 */
    case 0:
      if( fOpMode ) {
        // PowerUp
        s_ptAdc->aulAdc_ctrl_mansample_config[0] &= ~MSK_NX10_adc_ctrl_mansample_config0_conv;
        s_ptAdc->aulAdc_ctrl_mansample_config[0] |= MSK_NX10_adc_ctrl_mansample_config0_pdb;
        // delay
        do{ uVal++; } while( uVal<NX10_ADC_POWERUP_DELAY);
        // Dummy Sample to clear X in simu
        s_ptAdc->aulAdc_ctrl_mansample_config[0] |= MSK_NX10_adc_ctrl_mansample_config0_conv;
        s_ptAdc->aulAdc_ctrl_mansample_config[0] &= ~MSK_NX10_adc_ctrl_mansample_config0_conv;

      } else {
        // PowerDown
        s_ptAdc->aulAdc_ctrl_mansample_config[0] &= ~MSK_NX10_adc_ctrl_mansample_config0_pdb;
      }
    break;

    /* ADC1 */
    case 1:
      if( fOpMode ) {
        // PowerUp
        s_ptAdc->aulAdc_ctrl_mansample_config[1] &= ~MSK_NX10_adc_ctrl_mansample_config1_conv;
        s_ptAdc->aulAdc_ctrl_mansample_config[1] |= MSK_NX10_adc_ctrl_mansample_config1_pdb;
        // delay
        do{ uVal++; } while( uVal<NX10_ADC_POWERUP_DELAY);
        // Dummy sample to clear X in simu
        s_ptAdc->aulAdc_ctrl_mansample_config[1] |= MSK_NX10_adc_ctrl_mansample_config1_conv;
        s_ptAdc->aulAdc_ctrl_mansample_config[1] &= ~MSK_NX10_adc_ctrl_mansample_config1_conv;
      } else {
        // PowerDown
        s_ptAdc->aulAdc_ctrl_mansample_config[1] &= ~MSK_NX10_adc_ctrl_mansample_config1_pdb;
      }
    break;

    /* Default */
    default:
      return NX10_MOTION_ERR_INVAL_PARAM;
    break;
  } /* end switch */

  return NX10_MOTION_OKAY;
}

/*****************************************************************************/
/*! ADC Start
* \description
*   This function starts the ADC units for single or dual request.
*   Both unit can be started individually.
* \class
*   ADC
* \params
* \return
*   NX10_MOTION_OKAY                                                              */
/*****************************************************************************/
NX10_MOTION_RESULT NX10_Adc_StartAuto( int fStartAdc0, int fStartAdc1)
{
  s_ptAdc->ulAdc_ctrl_start =  (fStartAdc0<<SRT_NX10_adc_ctrl_start_start_adc0)
                              |(fStartAdc1<<SRT_NX10_adc_ctrl_start_start_adc1);

  return NX10_MOTION_OKAY;
}

/*****************************************************************************/
/*! ADC Config Auto
* \description
*   This function configures an ADC unit for autosample state machine.
* \class
*   ADC
* \params
* \return
*   NX10_MOTION_OKAY
*   NX10_MOTION_ERR_INVAL_PARAM                                                   */
/*****************************************************************************/
NX10_MOTION_RESULT NX10_Adc_ConfigAuto( unsigned int uAdcNum, unsigned int uChannel, unsigned int uPwrDown, unsigned int uInv, NX10_ADC_AUTOCFG_EVENTSEL eEventSel )
{
  switch( uAdcNum )
  {
  /* ADC0 */
    case 0:
    s_ptAdc->aulAdc_ctrl_autosample_config[0] =  (uChannel<<SRT_NX10_adc_ctrl_autosample_config0_sel)
                                               | (uPwrDown<<SRT_NX10_adc_ctrl_autosample_config0_power_down)
                                               | (uInv<<SRT_NX10_adc_ctrl_autosample_config0_event_inv)
                                               | (eEventSel<<SRT_NX10_adc_ctrl_autosample_config0_event_sel);
    break;

  /* ADC1 */
    case 1:
    s_ptAdc->aulAdc_ctrl_autosample_config[1] =  (uChannel<<SRT_NX10_adc_ctrl_autosample_config1_sel)
                                               | (uPwrDown<<SRT_NX10_adc_ctrl_autosample_config1_power_down)
                                               | (uInv<<SRT_NX10_adc_ctrl_autosample_config1_event_inv)
                                               | (eEventSel<<SRT_NX10_adc_ctrl_autosample_config1_event_sel);
    break;

  /* Default */
    default:
      return NX10_MOTION_ERR_INVAL_PARAM;
    break;
  } /* end switch */

  return NX10_MOTION_OKAY;
}


/*****************************************************************************/
/*! ADC Start Manual
* \description
*   This function starts an AD-conversion manually.
* \class
*   ADC
* \params
*   uAdcNum          [in]  ADC number (0..1)
*   uChannel         [in]  ADC channel (0..7)
* \return
*   NX10_MOTION_OKAY
*   NX10_MOTION_ERR_INVAL_PARAM
*   NX10_MOTION_ERR_UNIT_NOT_READY                                                */
/*****************************************************************************/
NX10_MOTION_RESULT NX10_Adc_StartMan( unsigned int uAdcNum, unsigned int uChannel )
{
  unsigned int uVal=0;

  if( uChannel>=NX10_ADC_CHANNELS )
    return NX10_MOTION_ERR_INVAL_PARAM;

  switch( uAdcNum )
  {
    // ADC0
    case 0:
      if( (s_ptAdc->aulAdc_ctrl_mansample_config[0] & MSK_NX10_adc_ctrl_mansample_config0_conv)!=0 )
        return NX10_MOTION_ERR_NOT_READY;
      if( (s_ptAdc->aulAdc_ctrl_mansample_config[0] & MSK_NX10_adc_ctrl_mansample_config0_pdb)==0 )
        return NX10_MOTION_ERR_NOT_READY;

      // set SEL incl. PDB=1 and CONV=0
      s_ptAdc->aulAdc_ctrl_mansample_config[0] = (uChannel<<SRT_NX10_adc_ctrl_mansample_config0_sel) | MSK_NX10_adc_ctrl_mansample_config0_pdb;
      // SEL2CONV delay
      do{ uVal++; } while( uVal<NX10_ADC_SEL2CONV_DELAY);
      // start ADC
      s_ptAdc->aulAdc_ctrl_mansample_config[0] = (uChannel<<SRT_NX10_adc_ctrl_mansample_config0_sel) | MSK_NX10_adc_ctrl_mansample_config0_pdb | MSK_NX10_adc_ctrl_mansample_config0_conv;
      s_ptAdc->aulAdc_ctrl_mansample_config[0] = (uChannel<<SRT_NX10_adc_ctrl_mansample_config0_sel) | MSK_NX10_adc_ctrl_mansample_config0_pdb;

    break;

    // ADC1
    case 1:
      if( (s_ptAdc->aulAdc_ctrl_mansample_config[1] & MSK_NX10_adc_ctrl_mansample_config1_conv)!=0 )
        return NX10_MOTION_ERR_NOT_READY;
      if( (s_ptAdc->aulAdc_ctrl_mansample_config[1] & MSK_NX10_adc_ctrl_mansample_config1_pdb)==0 )
        return NX10_MOTION_ERR_NOT_READY;

      // set SEL incl. PDB=1 and CONV=0
      s_ptAdc->aulAdc_ctrl_mansample_config[1] = (uChannel<<SRT_NX10_adc_ctrl_mansample_config1_sel) | MSK_NX10_adc_ctrl_mansample_config1_pdb;
      // SEL2CONV delay
      do{ uVal++; } while( uVal<NX10_ADC_SEL2CONV_DELAY);
      // start ADC
      s_ptAdc->aulAdc_ctrl_mansample_config[1] = (uChannel<<SRT_NX10_adc_ctrl_mansample_config1_sel) | MSK_NX10_adc_ctrl_mansample_config1_pdb | MSK_NX10_adc_ctrl_mansample_config1_conv;
      s_ptAdc->aulAdc_ctrl_mansample_config[1] = (uChannel<<SRT_NX10_adc_ctrl_mansample_config1_sel) | MSK_NX10_adc_ctrl_mansample_config1_pdb;
    break;

  /* Default */
    default:
      return NX10_MOTION_ERR_INVAL_PARAM;
    break;
  } /* end switch */

  return NX10_MOTION_OKAY;
}

/*****************************************************************************/
/*! ADC Get status
* \description
*   This function reads the status of an ADC unit.
* \class
*   ADC
*   uAdcNum          [in]   ADC number (0..1)
*   pfFin            [out] Pointer to Fin Flag
*   pfEocb           [out] Pointer to "End Of Conversion" Flag
* \params
* \return
*   NX10_MOTION_OKAY
*   NX10_MOTION_ERR_INVAL_PARAM                                                   */
/*****************************************************************************/
NX10_MOTION_RESULT NX10_Adc_GetStatus( unsigned int uAdcNum, int* pfFin, int* pfEocb )
{
  if( uAdcNum>=NX10_ADC_NUM )
    return NX10_MOTION_ERR_INVAL_PARAM;

  switch( uAdcNum )
  {
  /* ADC0 */
    case 0:
      *pfFin  = (s_ptAdc->ulAdc_ctrl_status & MSK_NX10_adc_ctrl_status_adc0_finish) ? 1 : 0;
      *pfEocb = (s_ptAdc->ulAdc_ctrl_status & MSK_NX10_adc_ctrl_status_adc0_eocb)   ? 1 : 0;
    break;

  /* ADC1 */
    case 1:
      *pfFin  = (s_ptAdc->ulAdc_ctrl_status & MSK_NX10_adc_ctrl_status_adc1_finish) ? 1 : 0;
      *pfEocb = (s_ptAdc->ulAdc_ctrl_status & MSK_NX10_adc_ctrl_status_adc1_eocb)   ? 1 : 0;
    break;

  /* Default */
    default:
      return NX10_MOTION_ERR_INVAL_PARAM;
    break;
  } /* end switch */

  return NX10_MOTION_OKAY;
}
/*****************************************************************************/
/*! ADC Get result
* \description
*   This function reads an AD-conversion result.
* \class
*   ADC
* \params
*   uAdcNum          [in]   ADC number (0..1)
*   pusVal           [out] Pointer to sampled value
* \return
*   NX10_MOTION_OKAY
*   NX10_MOTION_ERR_INVAL_PARAM                                                   */
/*****************************************************************************/
NX10_MOTION_RESULT NX10_Adc_GetAdcVal( unsigned int uAdcNum, unsigned short* pusVal )
{
  if( uAdcNum>=NX10_ADC_NUM )
    return NX10_MOTION_ERR_INVAL_PARAM;

  *pusVal = s_ptAdc->aulAdc_ctrl_adc_val[uAdcNum];

  return NX10_MOTION_OKAY;
}

/*****************************************************************************/
/*! ADC Enable Interrupt(s)
* \description
*   This function enables interrupts of the ADC.
* \class
*   ADC
* \params
   ulIrqMask     [in] Interrupt enable mask
* \return
*   NX10_MOTION_OKAY                                                              */
/*****************************************************************************/
NX10_MOTION_RESULT NX10_Adc_EnableIrq( unsigned long ulIrqMask )
{
  /* enable interrupts */
  s_ptAdc->ulAdc_ctrl_irq_msk_set = ulIrqMask;

  return NX10_MOTION_OKAY;
}

/*****************************************************************************/
/*! ADC Disable Interrupt(s)
* \description
*   This function disables interrupts of the ADC.
* \class
*   ADC
* \params
   ulIrqMask     [in] Interrupt disable mask
* \return
*   NX10_MOTION_OKAY                                                              */
/*****************************************************************************/
NX10_MOTION_RESULT NX10_Adc_DisableIrq( unsigned long ulIrqMask )
{
  /* disable interrupts */
  s_ptAdc->ulAdc_ctrl_irq_msk_reset = ulIrqMask;

  return NX10_MOTION_OKAY;
}

/*****************************************************************************/
/*! ADC Get masked Interrupt(s)
* \description
*   This function retrieves the current interrupt masked requests of the ADC.
* \class
*   ADC
* \params
   pulIrq     [out] Pointer to interrupt events
* \return
*   NX10_MOTION_OKAY                                                              */
/*****************************************************************************/
NX10_MOTION_RESULT NX10_Adc_GetIrq( unsigned long *pulIrq )
{
  /* get irq requests */
  *pulIrq = s_ptAdc->ulAdc_ctrl_irq_masked;

  return NX10_MOTION_OKAY;
}

/*****************************************************************************/
/*! ADC Get raw Interrupt(s)
* \description
*   This function retrieves the current interrupt raw requests of the ADC.
* \class
*   ADC
* \params
   pulIrq     [out] Pointer to interrupt events
* \return
*   NX10_MOTION_OKAY                                                              */
/*****************************************************************************/
NX10_MOTION_RESULT NX10_Adc_GetIrqRaw( unsigned long *pulIrq )
{
  /* get irq requests */
  *pulIrq = s_ptAdc->ulAdc_ctrl_irq_raw;

  return NX10_MOTION_OKAY;
}

/*****************************************************************************/
/*! ADC Confirm interrupt(s)
* \description
*   This function confirms a set of interrupts that were requested by the ADCoder.
* \class
*   ADC
* \params
*   ulConfirmIrqMask  [in] Mask to confirm interrupt events
* \return
*   NX10_MOTION_OKAY                                                              */
/*****************************************************************************/
NX10_MOTION_RESULT NX10_Adc_ConfirmIrq( unsigned long ulConfirmIrqMask )
{

  /* confirm the set of irqs */
  s_ptAdc->ulAdc_ctrl_irq_raw = ulConfirmIrqMask;

  return NX10_MOTION_OKAY;
}
