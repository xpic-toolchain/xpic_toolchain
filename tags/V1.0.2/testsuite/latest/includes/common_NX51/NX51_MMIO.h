#ifndef __NX51_MMIO_H
#define __NX51_MMIO_H

#include "netx51_regdef.h"


/*****************************************************************************/
/* Definitions                                                               */
/*****************************************************************************/
#define NX51_MMIO_CONFIG_XM0_IO0            0x00
#define NX51_MMIO_CONFIG_XM0_IO1            0x01
#define NX51_MMIO_CONFIG_XM0_IO2            0x02
#define NX51_MMIO_CONFIG_XM0_IO3            0x03
#define NX51_MMIO_CONFIG_XM0_IO4            0x04
#define NX51_MMIO_CONFIG_XM0_IO5            0x05
#define NX51_MMIO_CONFIG_XM0_RX             0x06
#define NX51_MMIO_CONFIG_XM0_TX_OUT         0x07
#define NX51_MMIO_CONFIG_XM1_IO0            0x08
#define NX51_MMIO_CONFIG_XM1_IO1            0x09
#define NX51_MMIO_CONFIG_XM1_IO2            0x0a
#define NX51_MMIO_CONFIG_XM1_IO3            0x0b
#define NX51_MMIO_CONFIG_XM1_IO4            0x0c
#define NX51_MMIO_CONFIG_XM1_IO5            0x0d
#define NX51_MMIO_CONFIG_XM1_RX             0x0e
#define NX51_MMIO_CONFIG_XM1_TX_OUT         0x0f
#define NX51_MMIO_CONFIG_GPIO0              0x10
#define NX51_MMIO_CONFIG_GPIO1              0x11
#define NX51_MMIO_CONFIG_GPIO2              0x12
#define NX51_MMIO_CONFIG_GPIO3              0x13
#define NX51_MMIO_CONFIG_GPIO4              0x14
#define NX51_MMIO_CONFIG_GPIO5              0x15
#define NX51_MMIO_CONFIG_GPIO6              0x16
#define NX51_MMIO_CONFIG_GPIO7              0x17
#define NX51_MMIO_CONFIG_GPIO8              0x18
#define NX51_MMIO_CONFIG_GPIO9              0x19
#define NX51_MMIO_CONFIG_GPIO10             0x1a
#define NX51_MMIO_CONFIG_GPIO11             0x1b
#define NX51_MMIO_CONFIG_GPIO12             0x1c
#define NX51_MMIO_CONFIG_GPIO13             0x1d
#define NX51_MMIO_CONFIG_GPIO14             0x1e
#define NX51_MMIO_CONFIG_GPIO15             0x1f
#define NX51_MMIO_CONFIG_GPIO16             0x20
#define NX51_MMIO_CONFIG_GPIO17             0x21
#define NX51_MMIO_CONFIG_GPIO18             0x22
#define NX51_MMIO_CONFIG_GPIO19             0x23
#define NX51_MMIO_CONFIG_GPIO20             0x24
#define NX51_MMIO_CONFIG_GPIO21             0x25
#define NX51_MMIO_CONFIG_GPIO22             0x26
#define NX51_MMIO_CONFIG_GPIO23             0x27
#define NX51_MMIO_CONFIG_GPIO24             0x28
#define NX51_MMIO_CONFIG_GPIO25             0x29
#define NX51_MMIO_CONFIG_GPIO26             0x2a
#define NX51_MMIO_CONFIG_GPIO27             0x2b
#define NX51_MMIO_CONFIG_GPIO28             0x2c
#define NX51_MMIO_CONFIG_GPIO29             0x2d
#define NX51_MMIO_CONFIG_GPIO30             0x2e
#define NX51_MMIO_CONFIG_GPIO31             0x2f
#define NX51_MMIO_CONFIG_PHY0_LED0          0x30
#define NX51_MMIO_CONFIG_PHY0_LED1          0x31
#define NX51_MMIO_CONFIG_PHY0_LED2          0x32
#define NX51_MMIO_CONFIG_PHY0_LED3          0x33
#define NX51_MMIO_CONFIG_PHY1_LED0          0x34
#define NX51_MMIO_CONFIG_PHY1_LED1          0x35
#define NX51_MMIO_CONFIG_PHY1_LED2          0x36
#define NX51_MMIO_CONFIG_PHY1_LED3          0x37
#define NX51_MMIO_CONFIG_MII_MDC            0x38
#define NX51_MMIO_CONFIG_MII_MDIO           0x39
#define NX51_MMIO_CONFIG_SPI0_CS2N          0x3a
#define NX51_MMIO_CONFIG_SPI0_SIO2_MMIO     0x3b
#define NX51_MMIO_CONFIG_SPI0_SIO3_MMIO     0x3c
#define NX51_MMIO_CONFIG_SPI1_CLK           0x3d
#define NX51_MMIO_CONFIG_SPI1_CS0N          0x3e
#define NX51_MMIO_CONFIG_SPI1_CS1N          0x3f
#define NX51_MMIO_CONFIG_SPI1_CS2N          0x40
#define NX51_MMIO_CONFIG_SPI1_MISO          0x41
#define NX51_MMIO_CONFIG_SPI1_MOSI          0x42
#define NX51_MMIO_CONFIG_I2C0_SCL_MMIO      0x43
#define NX51_MMIO_CONFIG_I2C0_SDA_MMIO      0x44
#define NX51_MMIO_CONFIG_I2C1_SCL           0x45
#define NX51_MMIO_CONFIG_I2C1_SDA           0x46
#define NX51_MMIO_CONFIG_XC_SAMPLE0         0x47
#define NX51_MMIO_CONFIG_XC_SAMPLE1         0x48
#define NX51_MMIO_CONFIG_XC_TRIGGER0        0x49
#define NX51_MMIO_CONFIG_XC_TRIGGER1        0x4a
#define NX51_MMIO_CONFIG_UART0_CTS          0x4b
#define NX51_MMIO_CONFIG_UART0_RTS          0x4c
#define NX51_MMIO_CONFIG_UART0_RXD          0x4d
#define NX51_MMIO_CONFIG_UART0_TXD          0x4e
#define NX51_MMIO_CONFIG_UART1_CTS          0x4f
#define NX51_MMIO_CONFIG_UART1_RTS          0x50
#define NX51_MMIO_CONFIG_UART1_RXD          0x51
#define NX51_MMIO_CONFIG_UART1_TXD          0x52
#define NX51_MMIO_CONFIG_UART2_CTS          0x53
#define NX51_MMIO_CONFIG_UART2_RTS          0x54
#define NX51_MMIO_CONFIG_UART2_RXD          0x55
#define NX51_MMIO_CONFIG_UART2_TXD          0x56
#define NX51_MMIO_CONFIG_CAN_RX             0x57
#define NX51_MMIO_CONFIG_CAN_TX             0x58
#define NX51_MMIO_CONFIG_MEM_RDY            0x59
#define NX51_MMIO_CONFIG_PIO0               0x5a
#define NX51_MMIO_CONFIG_PIO1               0x5b
#define NX51_MMIO_CONFIG_PIO2               0x5c
#define NX51_MMIO_CONFIG_PIO3               0x5d
#define NX51_MMIO_CONFIG_PIO4               0x5e
#define NX51_MMIO_CONFIG_PIO5               0x5f
#define NX51_MMIO_CONFIG_PIO6               0x60
#define NX51_MMIO_CONFIG_PIO7               0x61
#define NX51_MMIO_CONFIG_PIO                0x7f


/*****************************************************************************/
/* Function prototypes                                                       */
/*****************************************************************************/
void NX51_MMIO_SetCfg( unsigned int uMmioNum, unsigned char bFunction, int fInvertOut, int fInvertIn );
void NX51_MMIO_GetCfg( unsigned int uMmioNum, unsigned char* pbFunction, int* pfInvertOut, int* pfInvertIn );
unsigned long NX51_MMIO_GetLine( unsigned long ulReg01 );
void NX51_MMIO_SetLine( unsigned long ulReg01,  unsigned long ulVal );

#endif /* __NX51_MMIO_H */
