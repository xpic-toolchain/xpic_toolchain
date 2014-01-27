
#include "common_func_netx10.h"
#include "irq_handler.h"
#include <string.h>

//.##.....##....###....####.##....##
//.###...###...##.##....##..###...##
//.####.####..##...##...##..####..##
//.##.###.##.##.....##..##..##.##.##
//.##.....##.#########..##..##..####
//.##.....##.##.....##..##..##...###
//.##.....##.##.....##.####.##....##

int main()
{
  // ---------------------------------------------------------------------------
  // INITIALIZATION

  // lock irqs during the initialisation
  NX10_IRQFIQ_LOCK

  // initialize systime unit
  NX10_SYSTIME_SetBorder(1000000000, 0);
  NX10_SYSTIME_SetSpeed(0xa0000000, 0);

  // Configure MMIO pin multiplexer
  NX10_MMIO_SetCfg(  0, NX10_MMIO_CONFIG_XM0_RX,                0, 0 );
  NX10_MMIO_SetCfg(  1, NX10_MMIO_CONFIG_PIO_MODE,              0, 0 );
  NX10_MMIO_SetCfg(  2, NX10_MMIO_CONFIG_XM0_IO1,               0, 0 );
  NX10_MMIO_SetCfg(  3, NX10_MMIO_CONFIG_XM0_IO0,               0, 0 );
#ifdef _NXHX10_UART1_ENABLE_
  NX10_MMIO_SetCfg(  4, NX10_MMIO_CONFIG_UART1_TXD,            0, 0 );  /* Disable Dip Switch, Use UART1 */
  NX10_MMIO_SetCfg(  5, NX10_MMIO_CONFIG_UART1_RXD,            0, 0 );  /* Disable Dip Switch, Use UART1 */
  NX10_MMIO_SetCfg(  6, NX10_MMIO_CONFIG_UART1_RTS,            0, 0 );  /* Disable Dip Switch, Use UART1 */
  NX10_MMIO_SetCfg(  7, NX10_MMIO_CONFIG_UART1_CTS,            0, 0 );  /* Disable Dip Switch, Use UART1 */
#else
  NX10_MMIO_SetCfg(  4, NX10_MMIO_CONFIG_PIO_MODE,             0, 0 );
  NX10_MMIO_SetCfg(  5, NX10_MMIO_CONFIG_PIO_MODE,             0, 0 );
  NX10_MMIO_SetCfg(  6, NX10_MMIO_CONFIG_PIO_MODE,             0, 0 );
  NX10_MMIO_SetCfg(  7, NX10_MMIO_CONFIG_PIO_MODE,             0, 0 );
#endif
  NX10_MMIO_SetCfg(  8, NX10_MMIO_CONFIG_PIO_MODE,              0, 0 );
  NX10_MMIO_SetCfg(  9, NX10_MMIO_CONFIG_PIO_MODE,              0, 0 );
  NX10_MMIO_SetCfg( 10, NX10_MMIO_CONFIG_PIO_MODE,              0, 0 );
  NX10_MMIO_SetCfg( 11, NX10_MMIO_CONFIG_PIO_MODE,              0, 0 );
  NX10_MMIO_SetCfg( 12, NX10_MMIO_CONFIG_UART0_CTS,             0, 0 );
  NX10_MMIO_SetCfg( 13, NX10_MMIO_CONFIG_UART0_RTS,             0, 0 );
  NX10_MMIO_SetCfg( 14, NX10_MMIO_CONFIG_PIO_MODE,              0, 0 );
  NX10_MMIO_SetCfg( 15, NX10_MMIO_CONFIG_PIO_MODE,              0, 0 );
  NX10_MMIO_SetCfg( 16, NX10_MMIO_CONFIG_XM0_IO0,               0, 0 );  /* Ethernet LED */
  NX10_MMIO_SetCfg( 17, NX10_MMIO_CONFIG_XM0_IO1,               0, 0 );  /* Ethernet LED */
  NX10_MMIO_SetCfg( 18, NX10_MMIO_CONFIG_GPIO0,                 0, 0 );
  NX10_MMIO_SetCfg( 19, NX10_MMIO_CONFIG_GPIO1,                 0, 0 );
  NX10_MMIO_SetCfg( 20, NX10_MMIO_CONFIG_UART0_RXD,             0, 0 );
  NX10_MMIO_SetCfg( 21, NX10_MMIO_CONFIG_UART0_TXD,             0, 0 );
  NX10_MMIO_SetCfg( 22, NX10_MMIO_CONFIG_GPIO7,                 0, 0 );
  NX10_MMIO_SetCfg( 23, NX10_MMIO_CONFIG_SPI0_CS1N,             0, 0 );

  // init sdram controller
  POKE(Adr_NX10_sdram_general_ctrl, 0);
  POKE(Adr_NX10_sdram_timing_ctrl,  0x00a13262);
  POKE(Adr_NX10_sdram_mr,           0x00000033);
  POKE(Adr_NX10_sdram_general_ctrl, 0x030d0121);

  // init hif interface for sdram usage
  POKE(Adr_NX10_asic_ctrl_access_key, PEEK(Adr_NX10_asic_ctrl_access_key));
  POKE(Adr_NX10_hif_io_cfg, (1 << SRT_NX10_hif_io_cfg_hif_mi_cfg)|MSK_NX10_hif_io_cfg_en_hif_sdram_mi);

  // init diag output uart
  //NX10_UART_InitSerial(0);

  // reset pointer fifo borders
  NX10_PFIFO_Reset();

  // clear all pending irqs, this is important for 'soft restarts' from within the hitop debugger
  NX10_VIC_ClearVic();

  // initialize the irq, set ethernet isr
  InitIrq();

  // unlock irqs
  NX10_IRQFIQ_UNLOCK

  // ---------------------------------------------------------------------------

  while(1)
  {
    /* blink! */
    NX10_RDYRUN_SetRdyRunLed(NX10_RDYRUN_LED_GREEN);
    NX10_GPIO_Sleep(1, 500000);
    NX10_RDYRUN_SetRdyRunLed(NX10_RDYRUN_LED_OFF);
    NX10_GPIO_Sleep(1, 500000);
  }
}
