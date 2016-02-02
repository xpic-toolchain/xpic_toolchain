/*
  File : core_portme.c
*/
/*
  Author : Shay Gal-On, EEMBC
  Legal : TODO!
*/
#include <stdio.h>
#include <stdlib.h>
#include "coremark.h"

#include "netx51_regdef.h"
#include "mmio_config.h"
static void mmio_config(void);

#if VALIDATION_RUN
  volatile ee_s32 seed1_volatile=0x3415;
  volatile ee_s32 seed2_volatile=0x3415;
  volatile ee_s32 seed3_volatile=0x66;
#endif
#if PERFORMANCE_RUN
  volatile ee_s32 seed1_volatile=0x0;
  volatile ee_s32 seed2_volatile=0x0;
  volatile ee_s32 seed3_volatile=0x66;
#endif
#if PROFILE_RUN
  volatile ee_s32 seed1_volatile=0x8;
  volatile ee_s32 seed2_volatile=0x8;
  volatile ee_s32 seed3_volatile=0x8;
#endif
  volatile ee_s32 seed4_volatile=ITERATIONS;
  volatile ee_s32 seed5_volatile=0;
/* Porting : Timing functions
  How to capture time and convert to seconds must be ported to whatever is supported by the platform.
  e.g. Read value from on board RTC, read value from cpu clock cycles performance counter etc.
  Sample implementation for standard time.h and windows.h definitions included.
*/
/* Define : TIMER_RES_DIVIDER
  Divider to trade off timer resolution and total time that can be measured.

  Use lower values to increase resolution, but make sure that overflow does not occur.
  If there are issues with the return value overflowing, increase this value.
  */
#define NSECS_PER_SEC CLOCKS_PER_SEC
#define GETMYTIME(_t) (*_t=clock())
#define MYTIMEDIFF(fin,ini) ((fin)-(ini))
#define TIMER_RES_DIVIDER 1
#define SAMPLE_TIME_IMPLEMENTATION 1
#define EE_TICKS_PER_SEC           100000000

  //** Define Host specific (POSIX), or target specific global time variables. */
static CORE_TICKS EndTime;


/* Function : start_time
  This function will be called right before starting the timed portion of the benchmark.

  Implementation may be capturing a system timer (as implemented in the example code)
  or zeroing some system parameters - e.g. setting the cpu clocks cycles to 0.
*/
//void start_time(void) {
//  GETMYTIME(&start_time_val );
//}

static NX51_SYSTIME_AREA_T* s_ptSystime = (NX51_SYSTIME_AREA_T*)NX51_NETX_SYS_TIME_AREA;

void start_time(void)
{
  /* Setup SYSTIME to be a 64-Bit tick counter */
  s_ptSystime->ulSystime_count_value = 0x10000000;
  s_ptSystime->ulSystime_border      = 0xFFFFFFFF;
  s_ptSystime->ulSystime_s           = 0;
  s_ptSystime->ulSystime_ns          = 0;
}
/* Function : stop_time
  This function will be called right after ending the timed portion of the benchmark.

  Implementation may be capturing a system timer (as implemented in the example code)
  or other system parameters - e.g. reading the current value of cpu cycles counter.
*/
//void stop_time(void) {
//  GETMYTIME(&stop_time_val );
//}
void stop_time(void) {
  uint32_t secs  = s_ptSystime->ulSystime_s;
  uint32_t nsecs = s_ptSystime->ulSystime_ns;

  EndTime = (((uint64_t)secs) << 32) | nsecs;
}
/* Function : get_time
  Return an abstract "ticks" number that signifies time on the system.

  Actual value returned may be cpu cycles, milliseconds or any other value,
  as long as it can be converted to seconds by <time_in_secs>.
  This methodology is taken to accomodate any hardware or simulated platform.
  The sample implementation returns millisecs by default,
  and the resolution is controlled by <TIMER_RES_DIVIDER>
*/
CORE_TICKS get_time(void) {
  return EndTime;
}

/* Function : time_in_secs
  Convert the value returned by get_time to seconds.

  The <secs_ret> type is used to accomodate systems with no support for floating point.
  Default implementation implemented by the EE_TICKS_PER_SEC macro above.
*/
//secs_ret time_in_secs(CORE_TICKS ticks) {
//  secs_ret retval=((secs_ret)ticks) / (secs_ret)EE_TICKS_PER_SEC;
//  return retval;
//}

secs_ret time_in_secs(CORE_TICKS ticks) {
  secs_ret retval=((secs_ret)ticks) / EE_TICKS_PER_SEC;
  return retval;
}

ee_u32 default_num_contexts=1;

/* Function : portable_init
  Target specific initialization code
  Test for some common mistakes.
*/
void portable_init(core_portable *p, int *argc, char *argv[])
{
  NX51_UART_AREA_T* ptUart = (NX51_UART_AREA_T*)Addr_NX51_uart0;

  /* First of all disable everything */
  ptUart->ulUartcr     = 0;

  /* Set the bit for the second baud rate mode */
  ptUart->ulUartcr_2   = MSK_NX51_uartcr_2_Baud_Rate_Mode;

  /* Adjust the baud rate register */
  ptUart->ulUartlcr_l  = 0xB7;
  ptUart->ulUartlcr_m  = 0x04;

  /* set UART to 8 bits, 1 stop bit, no parity, FIFO enabled */
  ptUart->ulUartlcr_h  = (MSK_NX51_uartlcr_h_WLEN | MSK_NX51_uartlcr_h_FEN);
  /* Set TX-Driver to enabled */
  ptUart->ulUartdrvout = MSK_NX51_uartdrvout_DRVTX;
  /* Finally enable the UART */
  ptUart->ulUartcr     = MSK_NX51_uartcr_uartEN;

  /* Setup MMIO */
  mmio_config();

  if (sizeof(ee_ptr_int) != sizeof(ee_u8 *)) {
    ee_printf("ERROR! Please define ee_ptr_int to a type that holds a pointer!\r\n");
  }
  if (sizeof(ee_u32) != 4) {
    ee_printf("ERROR! Please define ee_u32 to a 32b unsigned type!\r\n");
  }

  ee_printf("Starting coremark\r\n");

#if defined(ARM_ENABLE_TCM)
  __asm__ __volatile__(
    "mrc     p15, 0, r0, c1, c0, 0\n" \
    "orr     r0, r0, #0x1000\n"       \
    "orr     r0, r0, #0x0004\n"       \
    "mcr     p15, 0, r0, c1, c0, 0\n" \
    : : : "r0"
  );
#endif

  p->portable_id=1;
}
/* Function : portable_fini
  Target specific final code
*/
void portable_fini(core_portable *p)
{
#ifdef INTRAM
#define TEST_DESCRIPTION "netX51 (xPIC) - Running from INTRAM"
#endif

#ifdef TCM
#define TEST_DESCRIPTION "netX51 (xPIC) - Running from xPIC PRAM"
#endif

#ifdef SDRAM
#define TEST_DESCRIPTION "netX51 (xPIC) - Running from SDRAM"
#endif

#ifndef TEST_DESCRIPTION
#define TEST_DESCRIPTION "TEST_DESCRIPTION not defined"
#endif

  ee_printf("Test description: %s\n", TEST_DESCRIPTION);

  p->portable_id=0;

  /* make sure we never return, as ARM will return to ROM loader or crash */
  while(1) ;
}

extern char _heap_start, _heap_end;

char*        g_pchHeapStart      = &_heap_start;  /*!< Start of heap area   */
char*        g_pchHeapMax        = &_heap_end;    /*!< End of heap area     */
static char* s_pchHeapCurrentEnd = &_heap_start;  /*!< Current end of heap  */

/*****************************************************************************/
/*! Non-Reentrant version of increase program data space
*   \param iIncr     Size to increase heap pointer
*   \return previous heap pointer (NULL on error)                            */
/*****************************************************************************/
caddr_t _sbrk(size_t iIncr)
{
  caddr_t pvBase = (caddr_t)-1;

  /* TODO: Insert lock */

  if( (s_pchHeapCurrentEnd + iIncr) <= g_pchHeapMax)
  {
    pvBase = s_pchHeapCurrentEnd;
    s_pchHeapCurrentEnd += iIncr;
  }

  return pvBase;
}

static const MMIO_CONFIG_T s_atMMIOConfig[] =
{
  {.bMmio = 0,  .bFunction = MMIO_CONFIG_PIO,         .fInvertIn = 0, .fInvertOut = 0},
  {.bMmio = 1,  .bFunction = MMIO_CONFIG_PIO,         .fInvertIn = 0, .fInvertOut = 0},
  {.bMmio = 2,  .bFunction = MMIO_CONFIG_PIO,         .fInvertIn = 0, .fInvertOut = 0},
  {.bMmio = 3,  .bFunction = MMIO_CONFIG_PIO,         .fInvertIn = 0, .fInvertOut = 0},
  {.bMmio = 4,  .bFunction = MMIO_CONFIG_PIO,         .fInvertIn = 0, .fInvertOut = 0},
  {.bMmio = 5,  .bFunction = MMIO_CONFIG_PIO,         .fInvertIn = 0, .fInvertOut = 0},
  {.bMmio = 6,  .bFunction = MMIO_CONFIG_PIO,         .fInvertIn = 0, .fInvertOut = 0},
  {.bMmio = 7,  .bFunction = MMIO_CONFIG_PIO,         .fInvertIn = 0, .fInvertOut = 0},
  {.bMmio = 8,  .bFunction = MMIO_CONFIG_PIO,         .fInvertIn = 0, .fInvertOut = 0},
  {.bMmio = 9,  .bFunction = MMIO_CONFIG_PIO,         .fInvertIn = 0, .fInvertOut = 0},
  {.bMmio = 10, .bFunction = MMIO_CONFIG_PIO,         .fInvertIn = 0, .fInvertOut = 0},
  {.bMmio = 11, .bFunction = MMIO_CONFIG_PIO,         .fInvertIn = 0, .fInvertOut = 0},
  {.bMmio = 12, .bFunction = MMIO_CONFIG_PIO,         .fInvertIn = 0, .fInvertOut = 0},
  {.bMmio = 13, .bFunction = MMIO_CONFIG_PIO,         .fInvertIn = 0, .fInvertOut = 0},
  {.bMmio = 14, .bFunction = MMIO_CONFIG_PIO,         .fInvertIn = 0, .fInvertOut = 0},
  {.bMmio = 15, .bFunction = MMIO_CONFIG_PIO,         .fInvertIn = 0, .fInvertOut = 0},
  {.bMmio = 16, .bFunction = MMIO_CONFIG_PIO,         .fInvertIn = 0, .fInvertOut = 0},
  {.bMmio = 17, .bFunction = MMIO_CONFIG_PIO,         .fInvertIn = 0, .fInvertOut = 0},
  {.bMmio = 18, .bFunction = MMIO_CONFIG_PIO,         .fInvertIn = 0, .fInvertOut = 0},
  {.bMmio = 19, .bFunction = MMIO_CONFIG_PIO,         .fInvertIn = 0, .fInvertOut = 0},
  {.bMmio = 20, .bFunction = MMIO_CONFIG_PIO,         .fInvertIn = 0, .fInvertOut = 0},
  {.bMmio = 21, .bFunction = MMIO_CONFIG_PIO,         .fInvertIn = 0, .fInvertOut = 0},
  {.bMmio = 22, .bFunction = MMIO_CONFIG_PIO,         .fInvertIn = 0, .fInvertOut = 0},
  {.bMmio = 23, .bFunction = MMIO_CONFIG_PIO,         .fInvertIn = 0, .fInvertOut = 0},
  {.bMmio = 24, .bFunction = MMIO_CONFIG_PIO,         .fInvertIn = 0, .fInvertOut = 0},
  {.bMmio = 25, .bFunction = MMIO_CONFIG_PIO,         .fInvertIn = 0, .fInvertOut = 0},
  {.bMmio = 26, .bFunction = MMIO_CONFIG_PIO,         .fInvertIn = 0, .fInvertOut = 0},
  {.bMmio = 27, .bFunction = MMIO_CONFIG_PIO,         .fInvertIn = 0, .fInvertOut = 0},
  {.bMmio = 28, .bFunction = MMIO_CONFIG_PIO,         .fInvertIn = 0, .fInvertOut = 0},
  {.bMmio = 29, .bFunction = MMIO_CONFIG_PIO,         .fInvertIn = 0, .fInvertOut = 0},
  {.bMmio = 30, .bFunction = MMIO_CONFIG_PIO,         .fInvertIn = 0, .fInvertOut = 0},
  {.bMmio = 31, .bFunction = MMIO_CONFIG_PIO,         .fInvertIn = 0, .fInvertOut = 0},
  {.bMmio = 32, .bFunction = MMIO_CONFIG_PIO,         .fInvertIn = 0, .fInvertOut = 0},
  {.bMmio = 33, .bFunction = MMIO_CONFIG_PIO,         .fInvertIn = 0, .fInvertOut = 0},
  {.bMmio = 34, .bFunction = MMIO_CONFIG_UART0_RXD,   .fInvertIn = 0, .fInvertOut = 0},
  {.bMmio = 35, .bFunction = MMIO_CONFIG_UART0_TXD,   .fInvertIn = 0, .fInvertOut = 0},
  {.bMmio = 36, .bFunction = MMIO_CONFIG_PIO,         .fInvertIn = 0, .fInvertOut = 0},
  {.bMmio = 37, .bFunction = MMIO_CONFIG_PIO,         .fInvertIn = 0, .fInvertOut = 0},
  {.bMmio = 38, .bFunction = MMIO_CONFIG_PIO,         .fInvertIn = 0, .fInvertOut = 0},
  {.bMmio = 39, .bFunction = MMIO_CONFIG_PIO,         .fInvertIn = 0, .fInvertOut = 0},
};

#define PEEK(x) (*((volatile unsigned int*)x))
#define POKE(x, y) ((*((volatile unsigned int*)x)) = y)

void mmio_config(void)
{
  unsigned long          ulIdx;
  NX51_MMIO_CTRL_AREA_T* ptMMioCtrl = (NX51_MMIO_CTRL_AREA_T*)NX51_NETX_MMIO_CTRL_AREA;

  /*Initialize MMIO Matrix */
  ptMMioCtrl->aulMmio_pio_out_line_cfg[0] = 0;
  ptMMioCtrl->aulMmio_pio_oe_line_cfg[0]  = 0;
  ptMMioCtrl->aulMmio_pio_out_line_cfg[1] = 0;
  ptMMioCtrl->aulMmio_pio_oe_line_cfg[1]  = 0;

  POKE(Adr_NX51_asic_ctrl_asic_ctrl_access_key, PEEK(Adr_NX51_asic_ctrl_asic_ctrl_access_key));
  POKE(Adr_NX51_asic_ctrl_io_config, 0);

  for(ulIdx = 0; ulIdx < sizeof(s_atMMIOConfig) / sizeof(s_atMMIOConfig[0]); ++ulIdx)
  {
    MMIO_CONFIG_T* ptMMio = (MMIO_CONFIG_T*)&s_atMMIOConfig[ulIdx];

    POKE(Adr_NX51_asic_ctrl_asic_ctrl_access_key,
         PEEK(Adr_NX51_asic_ctrl_asic_ctrl_access_key));

    ptMMioCtrl->aulMmio_cfg[ptMMio->bMmio] = (ptMMio->bFunction  << SRT_NX51_mmio0_cfg_mmio_sel)    |
                                             (ptMMio->fInvertIn  << SRT_NX51_mmio0_cfg_mmio_in_inv) |
                                             (ptMMio->fInvertOut << SRT_NX51_mmio0_cfg_mmio_out_inv);
  }
}

void UART_PutChar(char cOut)
{
  NX51_UART_AREA_T* ptUart = (NX51_UART_AREA_T*)Addr_NX51_uart0;
  // Write to UART

  while(ptUart->ulUartfr & MSK_NX51_uartfr_TXFF) ;

  ptUart->ulUartdr = cOut;
}
