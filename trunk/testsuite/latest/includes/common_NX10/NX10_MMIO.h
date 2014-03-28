#ifndef __NX10_MMIO_H
#define __NX10_MMIO_H

#include "regdef_netx10.h"


/*****************************************************************************/
/* Definitions                                                               */
/*****************************************************************************/
#define NX10_MMIO_CONFIG_XM0_IO0            0x00
#define NX10_MMIO_CONFIG_XM0_IO1            0x01
#define NX10_MMIO_CONFIG_XM0_IO2            0x02
#define NX10_MMIO_CONFIG_XM0_IO3            0x03
#define NX10_MMIO_CONFIG_XM0_IO4            0x04
#define NX10_MMIO_CONFIG_XM0_IO5            0x05
#define NX10_MMIO_CONFIG_XM0_RX             0x06
#define NX10_MMIO_CONFIG_GPIO0              0x07
#define NX10_MMIO_CONFIG_GPIO1              0x08
#define NX10_MMIO_CONFIG_GPIO2              0x09
#define NX10_MMIO_CONFIG_GPIO3              0x0A
#define NX10_MMIO_CONFIG_GPIO4              0x0B
#define NX10_MMIO_CONFIG_GPIO5              0x0C
#define NX10_MMIO_CONFIG_GPIO6              0x0D
#define NX10_MMIO_CONFIG_GPIO7              0x0E
#define NX10_MMIO_CONFIG_PHY0_LED0          0x0F
#define NX10_MMIO_CONFIG_PHY0_LED1          0x10
#define NX10_MMIO_CONFIG_PHY0_LED2          0x11
#define NX10_MMIO_CONFIG_PHY0_LED3          0x12
#define NX10_MMIO_CONFIG_SPI0_CS1N          0x13
#define NX10_MMIO_CONFIG_SPI0_CS2N          0x14
#define NX10_MMIO_CONFIG_SPI1_CLK           0x15
#define NX10_MMIO_CONFIG_SPI1_CS0N          0x16
#define NX10_MMIO_CONFIG_SPI1_CS1N          0x17
#define NX10_MMIO_CONFIG_SPI1_CS2N          0x18
#define NX10_MMIO_CONFIG_SPI1_MISO          0x19
#define NX10_MMIO_CONFIG_SPI1_MOSI          0x1A
#define NX10_MMIO_CONFIG_I2C_SCL_MMIO       0x1B
#define NX10_MMIO_CONFIG_I2C_SDA_MMIO       0x1C
#define NX10_MMIO_CONFIG_UART0_CTS          0x1D
#define NX10_MMIO_CONFIG_UART0_RTS          0x1E
#define NX10_MMIO_CONFIG_UART0_RXD          0x1F
#define NX10_MMIO_CONFIG_UART0_TXD          0x20
#define NX10_MMIO_CONFIG_UART1_CTS          0x21
#define NX10_MMIO_CONFIG_UART1_RTS          0x22
#define NX10_MMIO_CONFIG_UART1_RXD          0x23
#define NX10_MMIO_CONFIG_UART1_TXD          0x24
#define NX10_MMIO_CONFIG_PWM_FAILURE_N      0x25
#define NX10_MMIO_CONFIG_POS_ENC0_A         0x26
#define NX10_MMIO_CONFIG_POS_ENC0_B         0x27
#define NX10_MMIO_CONFIG_POS_ENC0_N         0x28
#define NX10_MMIO_CONFIG_POS_ENC1_A         0x29
#define NX10_MMIO_CONFIG_POS_ENC1_B         0x2A
#define NX10_MMIO_CONFIG_POS_ENC1_N         0x2B
#define NX10_MMIO_CONFIG_POS_MP0            0x2C
#define NX10_MMIO_CONFIG_POS_MP1            0x2D
#define NX10_MMIO_CONFIG_IO_LINK0_IN        0x2E
#define NX10_MMIO_CONFIG_IO_LINK0_OUT       0x2F
#define NX10_MMIO_CONFIG_IO_LINK0_OE        0x30
#define NX10_MMIO_CONFIG_IO_LINK1_IN        0x31
#define NX10_MMIO_CONFIG_IO_LINK1_OUT       0x32
#define NX10_MMIO_CONFIG_IO_LINK1_OE        0x33
#define NX10_MMIO_CONFIG_IO_LINK2_IN        0x34
#define NX10_MMIO_CONFIG_IO_LINK2_OUT       0x35
#define NX10_MMIO_CONFIG_IO_LINK2_OE        0x36
#define NX10_MMIO_CONFIG_IO_LINK3_IN        0x37
#define NX10_MMIO_CONFIG_IO_LINK3_OUT       0x38
#define NX10_MMIO_CONFIG_IO_LINK3_OE        0x39
#define NX10_MMIO_CONFIG_PIO_MODE           0x3F


/*****************************************************************************/
/* Function prototypes                                                       */
/*****************************************************************************/
void NX10_MMIO_SetCfg( unsigned int uMmioNum, unsigned char bFunction, int fInvertOut, int fInvertIn );
void NX10_MMIO_GetCfg( unsigned int uMmioNum, unsigned char* pbFunction, int* pfInvertOut, int* pfInvertIn );
unsigned long NX10_MMIO_GetLine( void );
void NX10_MMIO_SetLine( unsigned long ulVal );


#endif /* __NX10_MMIO_H */
