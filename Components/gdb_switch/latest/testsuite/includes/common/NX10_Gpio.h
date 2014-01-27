#ifndef __NX10_GPIO_H
#define __NX10_GPIO_H

#include "regdef_netx10.h"
#include "defines.h"


/*****************************************************************************/
/* Definitions                                                               */
/*****************************************************************************/
#define NX10_GPIO_MODE_NOINVERT                   0x0
#define NX10_GPIO_MODE_INVERT                    0x10
#define NX10_GPIO_MODE_INPUT_READ                 0x0
#define NX10_GPIO_MODE_OUTPUT_SET_TO_0            0x4
#define NX10_GPIO_MODE_OUTPUT_SET_TO_1            0x5
#define NX10_GPIO_MODE_OUTPUT_SET_TO_GPIO_LINE    0x6

/*****************************************************************************/
/* Function prototypes                                                       */
/*****************************************************************************/
int   NX10_Gpio_GetIrq (unsigned long ulGpioNum);
int   NX10_Gpio_GetInput (unsigned long ulGpioNum);
unsigned long NX10_Gpio_GetIn( void );
void  NX10_Gpio_SetupMode (unsigned long ulGpioNum, unsigned long ulMode, unsigned long ulInvert);
void  NX10_Gpio_SetOutput (unsigned long ulGpioNum, int fEnable);
unsigned long NX10_Gpio_GetLine( void );
void  NX10_Gpio_SetLine( unsigned long ulVal );
void  NX10_Gpio_IrqEnable (unsigned long ulGPIOnum);
void  NX10_Gpio_IrqReset (unsigned long ulGpioNum);
void  NX10_Gpio_Sleep (unsigned long ulCounter, unsigned int uiTimeout);

#endif /* __NX10_GPIO_H */
