#ifndef __NX10_GPIO_H
#define __NX10_GPIO_H

#include "regdef_netx10.h"
#include "defines.h"


/*****************************************************************************/
/* Definitions                                                               */
/*****************************************************************************/
#define NX10_GPIO_MODE_NOINVERT                   0x0
#define NX10_GPIO_MODE_INVERT                    0x10

// defines the GPIO input or output mode - depends on configuration:
// Input
#define NX10_GPIO_MODE_INPUT_READ                 0x0
#define NX10_GPIO_MODE_CAPT_CONTINUE              0x1
#define NX10_GPIO_MODE_CAPT_ONCE_RISING           0x2
#define NX10_GPIO_MODE_CAPT_ONCE_HILEVEL          0x3
// Output
#define NX10_GPIO_MODE_OUTPUT_SET_TO_0            0x4
#define NX10_GPIO_MODE_OUTPUT_SET_TO_1            0x5
#define NX10_GPIO_MODE_OUTPUT_SET_TO_GPIO_LINE    0x6
/* PWM mode, direct threshold update                                         */
#define NX10_GPIO_MODE_PWM						  0x7
#define NX10_GPIO_MODE_IOLINK                     0x8
/* PWM2 mode with threshold update at counter=0                              */
#define NX10_GPIO_MODE_PWM2						  0xf

// counters
#define NX10_GPIO_CNTR_0                          0x0
#define NX10_GPIO_CNTR_1                          0x1

#define NX10_GPIO_CNTR_RUN_ONCE                   0x1
#define NX10_GPIO_CNTR_RUN_CONTINUE               0x0

#define NX10_GPIO_CNTR_MODE_ASYM                  0x0
#define NX10_GPIO_CNTR_MODE_SYM                   0x1


#define NX10_GPIO_CNTR_IRQ_DIS                    0x0
#define NX10_GPIO_CNTR_IRQ_EN                     0x1

#define NX10_GPIO_CNTR_SEL_EVENT_HI_LEVEL         0x0
#define NX10_GPIO_CNTR_SEL_EVENT_POS_RISING       0x1

#define NX10_GPIO_CNTR_EVENT_COUNT_ALLWAYS        0x0
#define NX10_GPIO_CNTR_EVENT_COUNT_ON_SEL_EVENT   0x1
#define NX10_GPIO_CNTR_EVENT_WATCHDOG             0x2
#define NX10_GPIO_CNTR_EVENT_RUN_ON_SEL_EVENT     0x3
/*****************************************************************************/
/* Function prototypes                                                       */
/*****************************************************************************/
int   NX10_Gpio_GetInput (unsigned long ulGpioNum);
unsigned long NX10_Gpio_GetIn( void );
void  NX10_Gpio_SetupMode (unsigned long ulGpioNum, unsigned long ulMode, unsigned long ulInvert);
void  NX10_Gpio_SetOutput (unsigned long ulGpioNum, int fEnable);
unsigned long NX10_Gpio_GetLine( void );
void  NX10_Gpio_SetLine( unsigned long ulVal );

void  NX10_Gpio_IrqEnable (unsigned long ulGPIOnum);
int   NX10_Gpio_GetIrq    (unsigned long ulGpioNum);
void  NX10_Gpio_IrqReset  (unsigned long ulGpioNum);

void  NX10_Gpio_Sleep (unsigned long ulCounter, unsigned int uiTimeout);


void  NX10_Gpio_SetupModeExt( unsigned long ulGpioNum, unsigned long ulMode, unsigned long ulInvert, unsigned long ulCntrRef);
void  NX10_Gpio_CounterSetup( unsigned int uiGpioNum, unsigned long ulCounterNum, unsigned int uiMode,  unsigned int fIRQ_enable, unsigned int run_once, unsigned int fSel_ext_event, unsigned int ulEvent);
void  NX10_Gpio_CounterStop ( unsigned long ulCounterNum);

void  NX10_Gpio_SetCounterValue    ( unsigned long ulCounterNum, unsigned long ulValue         );
void  NX10_Gpio_SetCounterMaxValue ( unsigned long ulCounterNum, unsigned long ulMaxValue      );
void  NX10_Gpio_SetCounterThresCapt( unsigned long    uiGpioNum, unsigned long ulThresCaptValue);

unsigned long NX10_Gpio_GetCounterValue    ( unsigned long  ulCounterNum);
unsigned long NX10_Gpio_GetCounterMaxValue ( unsigned long  ulCounterNum);
unsigned long NX10_Gpio_GetCounterThresCapt( unsigned long     uiGpioNum);


void NX10_Gpio_Cntr_IrqEnable( unsigned long ulCounterNum );
int  NX10_Gpio_Cntr_GetIrq   ( unsigned long ulCounterNum );
void NX10_Gpio_Cntr_IrqReset ( unsigned long ulCounterNum );

#endif /* __NX10_GPIO_H */
