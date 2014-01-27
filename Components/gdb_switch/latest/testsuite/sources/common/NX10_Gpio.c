#include "NX10_Gpio.h" // function prototypes and defines
#include <string.h>

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
*   ulGpioNum          [in]   Selected GPIO
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
