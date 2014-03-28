#include "NX10_Gpio.h" // function prototypes and defines

static NX10_GPIO_AREA_T* s_ptGpio = (NX10_GPIO_AREA_T*) Addr_NX10_gpio_motion;

/*****************************************************************************/
/*! GPIO Setup Mode
* \description
*   Configure mode for the selected GPIO.
* \class
*   Gpio
* \params
*   ulGpioNum          [in]   Selected GPIO
*   ulMode             [in]   GPIO_MODE_OUTPUT      = Set the selected GPIO to Output mode
*                             GPIO_MODE_INPUT_READ  = Set the selected GPIO to Input mode
*   ulInvert           [in]   GPIO_NOINVERT         = Does not invert the selected GPIO
*                             GPIO_INVERT           = Inverts the selected GPIO
* \return
*                                                                            */
/*****************************************************************************/
void NX10_Gpio_SetupMode( unsigned long ulGpioNum, unsigned long ulMode, unsigned long ulInvert )
{
  s_ptGpio->aulGpio_cfg[ulGpioNum] = (unsigned long) (ulMode | ulInvert);
}

/*****************************************************************************/
/*! GPIO Set Output
* \description
*   Enables/disable output for the selected GPIO.
* \class
*   Gpio
* \params
*   ulGpioNum          [in]   Selected GPIO
*   fEnable            [in]   0 = Disable the Output
*                             1 = Enable the Output
* \return
*                                                                            */
/*****************************************************************************/
void NX10_Gpio_SetOutput( unsigned long ulGpioNum, int fEnable )
{
  if( fEnable )
    s_ptGpio->ulGpio_line |= 1 << ulGpioNum;
  else
    s_ptGpio->ulGpio_line &= ~(1 << ulGpioNum);
}

/*****************************************************************************/
/*! GPIO Get Line
* \description
*  Read the GPIO line register.
* \class
*   Gpio
* \params
* \return
*   Gpio_line value                                                          */
/*****************************************************************************/
unsigned long NX10_Gpio_GetLine( void )
{
  return ( s_ptGpio->ulGpio_line );
}

/*****************************************************************************/
/*! GPIO Set Line
* \description
*   Set GPIO line.
* \class
*   Gpio
* \params
*   ulVal          [in]   Value of GPIO Line
* \return
*                                                                            */
/*****************************************************************************/
void NX10_Gpio_SetLine( unsigned long ulVal )
{
  s_ptGpio->ulGpio_line = ulVal;
}

/*****************************************************************************/
/*! GPIO Get Input
* \description
*   Read selected GPIO.
* \class
*   Gpio
* \params
*   ulGpioNum          [in]   Selected GPIO
* \return
*                                                                            */
/*****************************************************************************/
int NX10_Gpio_GetInput( unsigned long ulGpioNum )
{
  return ( s_ptGpio->ulGpio_in & (1<<ulGpioNum) ) ? 1 : 0;
}

/*****************************************************************************/
/*! GPIO Get In Register
* \description
*  Read the GPIO IN register.
* \class
*   Gpio
* \params
* \return
*   Gpio_in value                                                            */
/*****************************************************************************/
unsigned long NX10_Gpio_GetIn( void )
{
  return ( s_ptGpio->ulGpio_in );
}

/*****************************************************************************/
/*! GPIO Irq Enable
* \description
*   Enable interrupt request for the selected GPIO.
* \class
*   Gpio
* \params
*   ulGpioNum          [in]   Selected GPIO
* \return
*                                                                            */
/*****************************************************************************/
void NX10_Gpio_IrqEnable( unsigned long ulGpioNum )
{
  s_ptGpio->ulGpio_irq_mask_set |= 1 << ulGpioNum;
}

/*****************************************************************************/
/*! GPIO Get Irq
* \description
*   Read interrupt status of the selected GPIO.
* \class
*   Gpio
* \params
*   ulGpioNum          [in]   Selected GPIO
* \return
*  0
*  1                                                                         */
/*****************************************************************************/
int NX10_Gpio_GetIrq( unsigned long ulGpioNum )
{
  return (s_ptGpio->ulGpio_irq_masked & (1 << ulGpioNum)) ? 1 : 0;
}

/*****************************************************************************/
/*! GPIO Irq Reset
* \description
*   Writes 1 to reset the irq of the selected GPIO.
* \class
*   GPIO
* \params
*   ulGpioNum          [in]   Selected GPIO
* \return
*                                                                            */
/*****************************************************************************/
void NX10_Gpio_IrqReset( unsigned long ulGpioNum )
{
  s_ptGpio->ulGpio_irq_raw = 1 << ulGpioNum;
}

/*****************************************************************************/
/*! GPIO Sleep
* \description
*   Sleep a while using a GPIO timer.
* \class
*   Gpio
* \params
*   ulCounter          [in]   Selected counter
*   uiTimeout          [out]  The Time to wait in µs
* \return
*                                                                            */
/*****************************************************************************/
void NX10_Gpio_Sleep( unsigned long ulCounter, unsigned int uiTimeout )
{
  unsigned int uiVal;

  /* Convert counter value from µs to ns */
  uiTimeout = uiTimeout * (NX10_DEV_FREQUENCY/1000000);

  s_ptGpio->aulGpio_counter_ctrl[ulCounter]  = 0;          /* Clear the timer register         */
  s_ptGpio->aulGpio_counter_cnt[ulCounter]   = 0;          /* Clear the current counter value  */
  s_ptGpio->aulGpio_counter_max[ulCounter]   = uiTimeout;  /* Set the counter value            */
  s_ptGpio->aulGpio_counter_ctrl[ulCounter] |= (MSK_NX10_gpio_counter0_ctrl_run | MSK_NX10_gpio_counter0_ctrl_once  ); /* Enable the timer to one shot */

  /* poll timer ctrl for 'run' bit */
  do {
    uiVal  = s_ptGpio->aulGpio_counter_ctrl[ulCounter];
    uiVal &= MSK_NX10_gpio_counter0_ctrl_run;
  } while ( uiVal!=0 );
}



/*****************************************************************************/
/*! GPIO Setup Mode Ext
* \description
*   Configure mode for the selected GPIO.
* \class
*   Gpio
* \params
*   ulGpioNum          [in]   Selected GPIO
*   ulMode             [in]   GPIO_MODE_OUTPUT      = Set the selected GPIO to Output mode
*                             GPIO_MODE_INPUT_READ  = Set the selected GPIO to Input mode
*   ulInvert           [in]   GPIO_NOINVERT         = Does not invert the selected GPIO
*                             GPIO_INVERT           = Inverts the selected GPIO
*   ulCntrRef          [in]   Counter reference: 0 or 1
* \return
*                                                                            */
/*****************************************************************************/
void  NX10_Gpio_SetupModeExt (unsigned long ulGpioNum, unsigned long ulMode, unsigned long ulInvert, unsigned long ulCntrRef)
{
	  s_ptGpio->aulGpio_cfg[ulGpioNum] = (unsigned long) (ulMode | ulInvert | (ulCntrRef<<5) );
}

/*****************************************************************************/
/*! GPIO Counter Setup
* \description
*   Setup a corresponding GPIO timer.
* \class
*   Gpio
* \params
*   uiGpioNum          [in]   Selected GPIO: 0 - 7
*   ulCounterNum       [in]   Selected counter: 0 or 1
*   uiMode             [in]   Counter mode:  0: asymmetric mode (sawtooth) / 1: symmetric mode (triangle)
*   fIRQ_enable        [in]   Interrupt request  on selected event: 0: disable / 1: enable
*   fSel_ext_event     [in]   Enable external event: 0 -???????????????????????
*   ulEvent            [in]   Event:-???????????????????????
* \return
*                                                                            */
/*****************************************************************************/
void NX10_Gpio_CounterSetup( unsigned int uiGpioNum, unsigned long ulCounterNum, unsigned int uiMode,  unsigned int fIRQ_enable, unsigned int run_once, unsigned int fSelEvent, unsigned int ulEvent)
{
	NX10_GPIO_COUNTER0_CTRL_T Cntr;
	Cntr.bf.run       = 1;
	Cntr.bf.reserved1 = 0;
	Cntr.bf.once      = run_once;
	Cntr.bf.gpio_ref  = uiGpioNum;
	Cntr.bf.sym_nasym = uiMode;
	Cntr.bf.irq_en    = fIRQ_enable;
	Cntr.bf.sel_event = fSelEvent;
	Cntr.bf.event_act = ulEvent;

	s_ptGpio->aulGpio_counter_cnt[ulCounterNum]   = 0;          /* Clear the current counter value  */
	s_ptGpio->aulGpio_counter_ctrl[ulCounterNum]  =Cntr.val;    /* Setup counter */
}

/*****************************************************************************/
/*! GPIO Counter Stop
* \description
*   Stop a corresponding GPIO timer.
* \class
*   Gpio
* \params
*   ulCounterNum       [in]   Selected counter: 0 or 1
* \return
*                                                                            */
/*****************************************************************************/
void NX10_Gpio_CounterStop(unsigned long ulCounterNum)
{
	s_ptGpio->aulGpio_counter_ctrl[ulCounterNum]  = 0;          /* Stop timer */
}

/*****************************************************************************/
/*! GPIO Set Counter Threshold or Capture
* \description
*   Set value from the corresponding threshold or capture register
* \class
*   Gpio
* \params
*   uiGpioNum          [in]   Selected threshold or capture register: 0 - 7
*   ulThresCaptValue   [in]   Value for threshold
* \return
*                                                                            */
/*****************************************************************************/
void NX10_Gpio_SetCounterThresCapt( unsigned long uiGpioNum, unsigned long ulThresCaptValue)
{
	s_ptGpio ->aulGpio_tc[uiGpioNum] = ulThresCaptValue;
}

/*****************************************************************************/
/*! GPIO Get Counter Threshold or Capture
* \description
*   Read value to the corresponding threshold or capture register
* \class
*   Gpio
* \params
*   uiGpioNum          [in]   Selected threshold or capture register: 0 - 7
*   ulValue            [in]   Value for threshold
* \return
*   Threshold or capture value                                               */
/*****************************************************************************/
unsigned long NX10_Gpio_GetCounterThresCapt( unsigned long uiGpioNum)
{
	return s_ptGpio ->aulGpio_tc[uiGpioNum];
}

/*****************************************************************************/
/*! GPIO Set Counter Value
* \description
*   Set value to the corresponding counter
* \class
*   Gpio
* \params
*   ulCounterNum       [in]   Selected counter: 0 or 1
*   ulValue            [in]   Value for counter
* \return
*                                                                            */
/*****************************************************************************/
void NX10_Gpio_SetCounterValue(unsigned long ulCounterNum, unsigned long ulValue)
{
	s_ptGpio->aulGpio_counter_cnt[ulCounterNum] = ulValue;    /* Set value to counter */
}

/*****************************************************************************/
/*! GPIO Get Counter Value
* \description
*   Get value from the corresponding counter
* \class
*   Gpio
* \params
*   ulCounterNum       [in]   Selected counter: 0 or 1
* \return
*   Counter value                                                                         */
/*****************************************************************************/
unsigned long NX10_Gpio_GetCounterValue(unsigned long ulCounterNum)
{
	return s_ptGpio->aulGpio_counter_cnt[ulCounterNum];    /* Get value from counter */
}

/*****************************************************************************/
/*! GPIO Set Counter Max Value
* \description
*   Set value to the corresponding counter
* \class
*   Gpio
* \params
*   ulCounterNum       [in]   Selected counter: 0 or 1
*   ulMaxValue         [in]   Maximal value for counter
* \return
*                                                                            */
/*****************************************************************************/
void NX10_Gpio_SetCounterMaxValue(unsigned long ulCounterNum, unsigned long ulMaxValue)
{
	s_ptGpio->aulGpio_counter_max[ulCounterNum] = ulMaxValue;    /* Set maximal counter value */
}

/*****************************************************************************/
/*! GPIO Get Counter Max Value
* \description
*   Get value from the corresponding counter
* \class
*   Gpio
* \params
*   ulCounterNum       [in]   Selected counter: 0 or 1
* \return
*   Counter maximal value                                                                         */
/*****************************************************************************/
unsigned long NX10_Gpio_GetCounterMaxValue(unsigned long ulCounterNum)
{
	return s_ptGpio->aulGpio_counter_max[ulCounterNum];    /* Get maximal counter value */
}
/*****************************************************************************/
/*! GPIO Counter Irq Enable
* \description
*   Enable interrupt request for the selected Counter.
* \class
*   Gpio
* \params
*   ulCounterNum          [in]   Selected Counter
* \return
*                                                                            */
/*****************************************************************************/
void NX10_Gpio_Cntr_IrqEnable( unsigned long ulCounterNum )
{
	s_ptGpio->ulCnt_irq_raw  |= 1 << ulCounterNum;
  s_ptGpio->ulCnt_irq_mask_set  |= 1 << ulCounterNum;
}

/*****************************************************************************/
/*! GPIO Counter Get Irq
* \description
*   Read interrupt status of the selected GPIO Counter.
* \class
*   Gpio
* \params
*   ulCounterNum          [in]   Selected Counter
* \return
*  0
*  1                                                                         */
/*****************************************************************************/
int NX10_Gpio_Cntr_GetIrq( unsigned long ulCounterNum )
{
  return (s_ptGpio->ulCnt_irq_masked & (1 << ulCounterNum)) ? 1 : 0;
}

/*****************************************************************************/
/*! GPIO Counter Irq Reset
* \description
*   Writes 1 to reset the irq of the selected Counter.
* \class
*   GPIO
* \params
*   ulCounterNum          [in]   Selected Counter
* \return
*                                                                            */
/*****************************************************************************/
void NX10_Gpio_Cntr_IrqReset( unsigned long ulCounterNum )
{
  s_ptGpio->ulCnt_irq_raw = 1 << ulCounterNum;
}
