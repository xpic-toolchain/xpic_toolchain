#include "irq_handler.h"
#include "common_func_netx10.h"
#include "xpicgdb.h"
#include <string.h>

/**
 * poll time for xpic gdb server
 * the value is optimized for UART communication:
 * The maximum number of chars to read at 115.2 kbaud (1+8+1 bit/char) is
 * (11520 * XPICGDB_POLLTIME_US / 1000000)
 */
#define XPICGDB_POLLTIME_US 1000
#define XPICGDB_MAX_CHARS (4 + 11520 * XPICGDB_POLLTIME_US / 1000000)

/* UART to use for gdb communication */
#define XPICGDB_UARTNUM 0



static XPICGDB_T s_tXpicGdb;

void gdb_print_uart(void* pvAppCtx, const char* pbData, unsigned long* pulCnt)
{
  unsigned long ulWritten = 0;
  NX10_UART_AREA_T* ptUart = (NX10_UART_AREA_T*)pvAppCtx;

  // Wait until there is space in the FIFO
  while( (ptUart->ulUartfr & MSK_NX10_uartfr_TXFF) == 0 )
  {
    ptUart->ulUartdr = pbData[ulWritten++];
    if( ulWritten == *pulCnt )
      return;
  }

  *pulCnt = ulWritten;
}

void InitIrq(void)
{
  /* init irq vector */
  NX10_VIC_InstallIsrVector( NX10_VIC_IRQVectorHandler_C );

  /* set the vectors for the irqs */
  NX10_VIC_SetIrqVector(0, SRT_NX10_vic_int_enable_timer0 | MSK_NX10_vic_vect_cntl0_ENABLE, VIC_ISR_Timer_XpicGdb);

  /* enable irqs */
  NX10_VIC_EnableIrqSources(MSK_NX10_vic_int_enable_timer0);



  NX10_ArmTimer_Stop(0);
  /* init gdb uart */
  NX10_UART_InitSerial(XPICGDB_UARTNUM);
  while( NX10_UART_Peek(XPICGDB_UARTNUM) ) { NX10_UART_GetCharacter(XPICGDB_UARTNUM); }
  /* init xpic_gdb instance */
  xpicgdb_init(&s_tXpicGdb);
  s_tXpicGdb.pfnOutput = gdb_print_uart;
  s_tXpicGdb.pvAppCtx = (void*)(Addr_NX10_uart0 + 0x40*XPICGDB_UARTNUM);
  /* start cyclic gdb server timer */
  NX10_ArmTimer_EnIrq(1,0,0,0);
  NX10_ArmTimer_Start(0, NX10_ARMTIMER_CFG_MODE_PRELOAD_AT_0, XPICGDB_POLLTIME_US*(NX10_DEV_FREQUENCY/1000000));
}

void VIC_ISR_Timer_XpicGdb(void)
{
  int iChar;
  char abInput[XPICGDB_MAX_CHARS];
  unsigned long ulNumChars;

  /* clear irq */
  NX10_ArmTimer_ConfirmIrq(MSK_NX10_arm_timer_irq_raw_timer0_irq);

  /* check for input from gdb client */
  for(ulNumChars = 0; NX10_UART_Peek(XPICGDB_UARTNUM); ++ulNumChars)
  {
    iChar = NX10_UART_GetCharacter(XPICGDB_UARTNUM);
    if( iChar < 0 )
    {
      NX10_RDYRUN_SetRdyRunLed(NX10_RDYRUN_LED_RED);
      // while(1); //XXX why needed? doesnt work with while activated
    }

    if( ulNumChars >= XPICGDB_MAX_CHARS )
    {
      break;
    }

    abInput[ulNumChars] = 0xff & iChar;
  }

  /* cyclic server process function */
  xpicgdb_process(&s_tXpicGdb, abInput, ulNumChars);
}
