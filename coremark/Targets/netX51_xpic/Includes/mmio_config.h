/******************************************************************************

  Copyright (C) 2010 [Hilscher Gesellschaft für Systemautomation mbH]

  This program can be used by everyone according to the netX Open Source
  Software license. The license agreement can be downloaded from
  http://www.industrialNETworX.com

*******************************************************************************

  Last Modification:
    @version $Id: mmio_config.h 1538 2013-07-01 10:35:58Z MichaelT $

  Description:
    netX MMIO definitions

  Changes:
    Date        Author        Description
  ---------------------------------------------------------------------------
    2010-03-24  NC            File created.

******************************************************************************/


#ifndef __MMIO_CONFIG__H
#define __MMIO_CONFIG__H

#define MMIO_CONFIG_XM0_IO0            0x00
#define MMIO_CONFIG_XM0_IO1            0x01
#define MMIO_CONFIG_XM0_IO2            0x02
#define MMIO_CONFIG_XM0_IO3            0x03
#define MMIO_CONFIG_XM0_IO4            0x04
#define MMIO_CONFIG_XM0_IO5            0x05
#define MMIO_CONFIG_XM0_RX             0x06
#define MMIO_CONFIG_XM0_TX_OUT         0x07
#define MMIO_CONFIG_XM1_IO0            0x08
#define MMIO_CONFIG_XM1_IO1            0x09
#define MMIO_CONFIG_XM1_IO2            0x0a
#define MMIO_CONFIG_XM1_IO3            0x0b
#define MMIO_CONFIG_XM1_IO4            0x0c
#define MMIO_CONFIG_XM1_IO5            0x0d
#define MMIO_CONFIG_XM1_RX             0x0e
#define MMIO_CONFIG_XM1_TX_OUT         0x0f
#define MMIO_CONFIG_GPIO0              0x10
#define MMIO_CONFIG_GPIO1              0x11
#define MMIO_CONFIG_GPIO2              0x12
#define MMIO_CONFIG_GPIO3              0x13
#define MMIO_CONFIG_GPIO4              0x14
#define MMIO_CONFIG_GPIO5              0x15
#define MMIO_CONFIG_GPIO6              0x16
#define MMIO_CONFIG_GPIO7              0x17
#define MMIO_CONFIG_GPIO8              0x18
#define MMIO_CONFIG_GPIO9              0x19
#define MMIO_CONFIG_GPIO10             0x1a
#define MMIO_CONFIG_GPIO11             0x1b
#define MMIO_CONFIG_GPIO12             0x1c
#define MMIO_CONFIG_GPIO13             0x1d
#define MMIO_CONFIG_GPIO14             0x1e
#define MMIO_CONFIG_GPIO15             0x1f
#define MMIO_CONFIG_GPIO16             0x20
#define MMIO_CONFIG_GPIO17             0x21
#define MMIO_CONFIG_GPIO18             0x22
#define MMIO_CONFIG_GPIO19             0x23
#define MMIO_CONFIG_GPIO20             0x24
#define MMIO_CONFIG_GPIO21             0x25
#define MMIO_CONFIG_GPIO22             0x26
#define MMIO_CONFIG_GPIO23             0x27
#define MMIO_CONFIG_GPIO24             0x28
#define MMIO_CONFIG_GPIO25             0x29
#define MMIO_CONFIG_GPIO26             0x2a
#define MMIO_CONFIG_GPIO27             0x2b
#define MMIO_CONFIG_GPIO28             0x2c
#define MMIO_CONFIG_GPIO29             0x2d
#define MMIO_CONFIG_GPIO30             0x2e
#define MMIO_CONFIG_GPIO31             0x2f
#define MMIO_CONFIG_PHY0_LED0          0x30
#define MMIO_CONFIG_PHY0_LED1          0x31
#define MMIO_CONFIG_PHY0_LED2          0x32
#define MMIO_CONFIG_PHY0_LED3          0x33
#define MMIO_CONFIG_PHY1_LED0          0x34
#define MMIO_CONFIG_PHY1_LED1          0x35
#define MMIO_CONFIG_PHY1_LED2          0x36
#define MMIO_CONFIG_PHY1_LED3          0x37
#define MMIO_CONFIG_MII_MDC            0x38
#define MMIO_CONFIG_MII_MDIO           0x39
#define MMIO_CONFIG_SPI0_CS2N          0x3a
#define MMIO_CONFIG_SPI0_SIO2_MMIO     0x3b
#define MMIO_CONFIG_SPI0_SIO3_MMIO     0x3c
#define MMIO_CONFIG_SPI1_CLK           0x3d
#define MMIO_CONFIG_SPI1_CS0N          0x3e
#define MMIO_CONFIG_SPI1_CS1N          0x3f
#define MMIO_CONFIG_SPI1_CS2N          0x40
#define MMIO_CONFIG_SPI1_MISO          0x41
#define MMIO_CONFIG_SPI1_MOSI          0x42
#define MMIO_CONFIG_I2C0_SCL_MMIO      0x43
#define MMIO_CONFIG_I2C0_SDA_MMIO      0x44
#define MMIO_CONFIG_I2C1_SCL           0x45
#define MMIO_CONFIG_I2C1_SDA           0x46
#define MMIO_CONFIG_XC_SAMPLE0         0x47
#define MMIO_CONFIG_XC_SAMPLE1         0x48
#define MMIO_CONFIG_XC_TRIGGER0        0x49
#define MMIO_CONFIG_XC_TRIGGER1        0x4a
#define MMIO_CONFIG_UART0_CTS          0x4b
#define MMIO_CONFIG_UART0_RTS          0x4c
#define MMIO_CONFIG_UART0_RXD          0x4d
#define MMIO_CONFIG_UART0_TXD          0x4e
#define MMIO_CONFIG_UART1_CTS          0x4f
#define MMIO_CONFIG_UART1_RTS          0x50
#define MMIO_CONFIG_UART1_RXD          0x51
#define MMIO_CONFIG_UART1_TXD          0x52
#define MMIO_CONFIG_UART2_CTS          0x53
#define MMIO_CONFIG_UART2_RTS          0x54
#define MMIO_CONFIG_UART2_RXD          0x55
#define MMIO_CONFIG_UART2_TXD          0x56
#define MMIO_CONFIG_CAN_RX             0x57
#define MMIO_CONFIG_CAN_TX             0x58
#define MMIO_CONFIG_MEM_RDY            0x59
#define MMIO_CONFIG_PIO0               0x5a
#define MMIO_CONFIG_PIO1               0x5b
#define MMIO_CONFIG_PIO2               0x5c
#define MMIO_CONFIG_PIO3               0x5d
#define MMIO_CONFIG_PIO4               0x5e
#define MMIO_CONFIG_PIO5               0x5f
#define MMIO_CONFIG_PIO6               0x60
#define MMIO_CONFIG_PIO7               0x61
#define MMIO_CONFIG_PIO                0x7f

typedef struct MMIO_CONFIG_Ttag
{
  unsigned char bMmio;
  unsigned char bFunction;
  unsigned char fInvertIn;
  unsigned char fInvertOut;
} MMIO_CONFIG_T;

#endif /* __MMIO_CONFIG__H */
