/*
 *******************************************************************************
 *                                                                             *
 * netx10gs_gdbproto.h                                                         *
 *                                                                             *
 * author: Daniel Kramlinger                                                   *
 *         Hilscher Gesellschaft für Systemautomation mbH                      *
 * date: 13.04.2011                                                            *
 *                                                                             *
 *******************************************************************************
*/


#ifndef NETX10GS_GDBPROTO_H
#define NETX10GS_GDBPROTO_H

//==============================================================================
//
//==============================================================================
#define GDB_STARTNOACKMODE	"$QStartNoAckMode#b0"

#define GDB_MAX_SWBP		99
#define GDB_MSG_START_CHAR	'$'
// #define	GDB_PLUS			-1
// #define	GDB_MINUS			-2
#define	GDB_PLUS			"+"
#define	GDB_MINUS			"-"
#define	GDB_PLUS_CHAR		'+'
#define	GDB_MINUS_CHAR		'-'
#define	GDB_NEGACK			'-'

#define GDB_MSG_ERROR_CHAR	'E'
#define	GDB_CTRL_C		"\x03"
#define	GDB_CTRL_C_CHAR	0x03
#define	GDB_UNKWON		"$#00"
#define	GDB_KEEP_ALIVE	"$#00"
#define	GDB_SIGINT		"$T02#b6"
#define	GDB_SIGTRAP		"$S05#b8"
#define	GDB_OK			"$OK#9a"
#define GDB_OK_CHAR		0x04	// XXX 0x04 already used?
#define	GDB_ERROR_0		"$E00#a5"
#define GDB_QSUPPORTED_ANSWER	"+$PacketSize=fff;qXfer:memory-map:read-;qXfer:features:read-;QStartNoAckMode+#d7"

#define ARM_HALT	0x0
#define ARM_RESUME	0x1
#define	GDB_ARM_RUNNING		"$O7461726765742073746174653a2072756e6e696e670a#4a"
#define	GDB_ARM_HALTED		"$O7461726765742073746174653a2068616c7465640a#76"

//==============================================================================
// SWITCHES
//==============================================================================
// #define GDB_RECV_ANSWER			0x1
// #define GDB_DONT_RECV_ANSWER	0x0

#define GDB_RETURN_ANSWER		0x1
#define GDB_DONT_RETURN_ANSWER	0x0

#define GDB_WAIT_FOR_OK			0x1
#define GDB_DONT_WAIT_FOR_OK	0x0

#define GDB_SEND_SIGTRAP		0x1
#define GDB_DONT_SEND_SIGTRAP	0x0

#define GDB_CALC_CHECKSUM		0x1
#define GDB_DONT_CALC_CHECKSUM	0x0

#define GDB_VALUE_TYPE_NORMAL	0x0
#define GDB_VALUE_TYPE_GDB		0x1
#define GDB_VALUE_TYPE_NO_VALUE	0x2
#define GDB_VALUE_TYPE_MONITOR	0x3

//==============================================================================
//
//==============================================================================
#define GDB_CMD_ERROR_1					-2
#define GDB_CMD_ERROR					-1
#define GDB_CMD_DEFAULT					0
#define GDB_CMD_ACK						1
#define GDB_CMD_NEGACK					2
#define GDB_CMD_CONTINUE				3
#define GDB_CMD_SINGLESTEP				4
#define GDB_CMD_CTRL_C					5
#define GDB_CMD_KILL					6
#define GDB_CMD_READ_SINGLE_REG			7
#define GDB_CMD_WRITE_SINGLE_REG		8
#define GDB_CMD_READ_REGISTERS			9
#define GDB_CMD_WRITE_REGISTERS			10
#define GDB_CMD_WRITE_MEMORY_X			11
#define GDB_CMD_WRITE_MEMORY			12
#define GDB_CMD_READ_MEMORY				13
#define GDB_CMD_SET_BREAKPOINT			14
#define GDB_CMD_UNSET_BREAKPOINT		15
#define GDB_CMD_SET_HW_BREAKPOINT		16
#define GDB_CMD_UNSET_HW_BREAKPOINT		17
#define GDB_CMD_SET_WRITE_WATCHPOINT	18
#define GDB_CMD_UNSET_WRITE_WATCHPOINT	19
#define GDB_CMD_SET_READ_WATCHPOINT		20
#define GDB_CMD_UNSET_READ_WATCHPOINT	21
#define GDB_CMD_SET_ACCESS_WATCHPOINT	22
#define GDB_CMD_UNSET_ACCESS_WATCHPOINT	23
#define GDB_CMD_CONSOLE					24
#define GDB_CMD_OK						25
#define GDB_CMD_S						26
#define GDB_CMD_T						27
#define GDB_CMD_NOT_SUPPORTED			28
#define GDB_CMD_GDB_ERROR				29


#define GDB_CMD_q						100
#define GDB_CMD_qRcmd					101
#define GDB_CMD_qSupported				102


#define GDB_CMD_Q						200
#define GDB_CMD_QStartNoAckMode			201


#define GDB_MONITOR_START_MSG					"$O"
#define GDB_MONITOR_STRING						"Rcmd,"
#define GDB_MONITOR_CMD_RESET					0x1
#define GDB_MONITOR_CMD_RESET_STRING			"7265736574#37"
#define GDB_MONITOR_CMD_SHOWREGS				0x2
#define GDB_MONITOR_CMD_SHOWREGS_STRING			"73686f7772656773#b0"
#define GDB_MONITOR_CMD_EXITSWITCH				0x3
#define GDB_MONITOR_CMD_EXITSWITCH_STRING		"65786974#d7"
#define GDB_MONITOR_CMD_INIT					0x4
#define GDB_MONITOR_CMD_INIT_STRING				"696e6974#07"

#define GDB_MONITOR_MSG_INVALID_COMMAND			"ERROR: invalid command\n"
#define GDB_MONITOR_MSG_RESETTING_XPIC			"resetting xpic ..."
#define GDB_MONITOR_MSG_INIT_XPIC				"initialize xpic ..."
#define GDB_MONITOR_MSG_FAILED					"failed\n"
#define GDB_MONITOR_MSG_DONE					"done\n"
#define GDB_MONITOR_MSG_READREGS_BANK0			"reading xpic register - bank 0\n"
#define GDB_MONITOR_MSG_READREGS_BANK1			"reading xpic register - bank 1\n"


//==============================================================================

#define GDB_ARG_RCMD			0x01
#define GDB_ARG_STARTNOACKMODE	0x02


#define OPENOCD_MONITOR_POLL	"$qRcmd,706f6c6c#58"
#define	OPENOCD_MONITOR_HALT	"$qRcmd,68616c74#fc"
#define	OPENOCD_MONITOR_RESUME	"$qRcmd,726573756d65#d2"


//******************************************************************************
// BOARD COMMANDS                                                              *
//******************************************************************************

// from regdef_netx10.h
#define SRT_NX10_hif_io_cfg_hif_mi_cfg                 4
#define MSK_NX10_hif_io_cfg_en_hif_sdram_mi            0x00000040U

// system clock
#define BOARD_ACTIVATE_SYSTEM_CLOCKS	"$M101c0028,4:"

//IRQFIQ
#define BOARD_IRQFIQ_LOCK	"$P19=df000000#e1"
#define BOARD_IRQFIQ_UNLOCK	"$P19=1f000000#ae"


// BORDER
#define BOARD_SET_BORDER	"$M101C1008,4:"


// SPEED
#define BOARD_SET_SPEED		"$M101C100C,4:"


// ACCESS_KEY
#define	BOARD_READ_ACCESS_KEY		"$m101c0070,4#89"
#define	BOARD_WRITE_ACCESS_KEY		"$M101c0070,4:"

// PFIFO
#define BOARD_WRITE_PFIFO_RESET		"$M101a4080,4:"
#define BOARD_WRITE_PFIFO_BORDER0	"$M101a4040,4:"
#define BOARD_WRITE_PFIFO_BORDER1	"$M101a4044,4:"
#define BOARD_WRITE_PFIFO_BORDER2	"$M101a4048,4:"
#define BOARD_WRITE_PFIFO_BORDER3	"$M101a404c,4:"
#define BOARD_WRITE_PFIFO_BORDER4	"$M101a4050,4:"
#define BOARD_WRITE_PFIFO_BORDER5	"$M101a4054,4:"
#define BOARD_WRITE_PFIFO_BORDER6	"$M101a4058,4:"
#define BOARD_WRITE_PFIFO_BORDER7	"$M101a405c,4:"
#define BOARD_WRITE_PFIFO_BORDER8	"$M101a4060,4:"
#define BOARD_WRITE_PFIFO_BORDER9	"$M101a4064,4:"
#define BOARD_WRITE_PFIFO_BORDER10	"$M101a4068,4:"
#define BOARD_WRITE_PFIFO_BORDER11	"$M101a406c,4:"
#define BOARD_WRITE_PFIFO_BORDER12	"$M101a4070,4:"
#define BOARD_WRITE_PFIFO_BORDER13	"$M101a4074,4:"
#define BOARD_WRITE_PFIFO_BORDER14	"$M101a4078,4:"
#define BOARD_WRITE_PFIFO_BORDER15	"$M101a407c,4:"


// HIF
#define BOARD_WRITE_HIF_IO_CFG				"$M101C0C40,4:"

// SDRAM
#define BOARD_WRITE_SDRAM_GENERAL_CTRL		"$M101C0140,4:"
#define BOARD_WRITE_SDRAM_TIMING_CTRL		"$M101C0144,4:"
#define BOARD_WRITE_SDRAM_MR				"$M101C0148,4:"

// VIC
#define BOARD_WRITE_VIC_INT_SELECT			"$M101ff00c,4:"
#define BOARD_WRITE_VIC_INT_ENCLEAR			"$M101ff014,4:"
// VIC ADDR
#define BOARD_WRITE_VIC_VECT_ADDR0			"$M101ff100,4:"
#define BOARD_WRITE_VIC_VECT_ADDR1			"$M101ff104,4:"
#define BOARD_WRITE_VIC_VECT_ADDR2			"$M101ff108,4:"
#define BOARD_WRITE_VIC_VECT_ADDR3			"$M101ff10c,4:"
#define BOARD_WRITE_VIC_VECT_ADDR4			"$M101ff110,4:"
#define BOARD_WRITE_VIC_VECT_ADDR5			"$M101ff114,4:"
#define BOARD_WRITE_VIC_VECT_ADDR6			"$M101ff118,4:"
#define BOARD_WRITE_VIC_VECT_ADDR7			"$M101ff11c,4:"
#define BOARD_WRITE_VIC_VECT_ADDR8			"$M101ff120,4:"
#define BOARD_WRITE_VIC_VECT_ADDR9			"$M101ff124,4:"
#define BOARD_WRITE_VIC_VECT_ADDR10			"$M101ff128,4:"
#define BOARD_WRITE_VIC_VECT_ADDR11			"$M101ff12c,4:"
#define BOARD_WRITE_VIC_VECT_ADDR12			"$M101ff130,4:"
#define BOARD_WRITE_VIC_VECT_ADDR13			"$M101ff134,4:"
#define BOARD_WRITE_VIC_VECT_ADDR14			"$M101ff138,4:"
#define BOARD_WRITE_VIC_VECT_ADDR15			"$M101ff13c,4:"
// VIC CNTL
#define BOARD_WRITE_VIC_VECT_CNTL0			"$M101ff200,4:"
#define BOARD_WRITE_VIC_VECT_CNTL1			"$M101ff204,4:"
#define BOARD_WRITE_VIC_VECT_CNTL2			"$M101ff208,4:"
#define BOARD_WRITE_VIC_VECT_CNTL3			"$M101ff20c,4:"
#define BOARD_WRITE_VIC_VECT_CNTL4			"$M101ff210,4:"
#define BOARD_WRITE_VIC_VECT_CNTL5			"$M101ff214,4:"
#define BOARD_WRITE_VIC_VECT_CNTL6			"$M101ff218,4:"
#define BOARD_WRITE_VIC_VECT_CNTL7			"$M101ff21c,4:"
#define BOARD_WRITE_VIC_VECT_CNTL8			"$M101ff220,4:"
#define BOARD_WRITE_VIC_VECT_CNTL9			"$M101ff224,4:"
#define BOARD_WRITE_VIC_VECT_CNTL10			"$M101ff228,4:"
#define BOARD_WRITE_VIC_VECT_CNTL11			"$M101ff22c,4:"
#define BOARD_WRITE_VIC_VECT_CNTL12			"$M101ff230,4:"
#define BOARD_WRITE_VIC_VECT_CNTL13			"$M101ff234,4:"
#define BOARD_WRITE_VIC_VECT_CNTL14			"$M101ff238,4:"
#define BOARD_WRITE_VIC_VECT_CNTL15			"$M101ff23c,4:"

// MMIO CTRL
#define BOARD_READ_MMIO_CTRL_MMIO0_CFG		"$m101c0a00,4#b3"
#define BOARD_WRITE_MMIO_CTRL_MMIO0_CFG		"$M101c0a00,4:"

#define BOARD_READ_MMIO_CTRL_MMIO1_CFG		"$m101c0a04,4#b7"
#define BOARD_WRITE_MMIO_CTRL_MMIO1_CFG		"$M101c0a04,4:"
	
#define BOARD_READ_MMIO_CTRL_MMIO2_CFG		"$m101c0a08,4#bb"
#define BOARD_WRITE_MMIO_CTRL_MMIO2_CFG		"$M101c0a08,4:"
	
#define BOARD_READ_MMIO_CTRL_MMIO3_CFG		"$m101c0a0c,4#e6"
#define BOARD_WRITE_MMIO_CTRL_MMIO3_CFG		"$M101c0a0c,4:"
	
#define BOARD_READ_MMIO_CTRL_MMIO4_CFG		"$m101c0a10,4#b4"
#define BOARD_WRITE_MMIO_CTRL_MMIO4_CFG		"$M101c0a10,4:"
	
#define BOARD_READ_MMIO_CTRL_MMIO5_CFG		"$m101c0a14,4#b8"
#define BOARD_WRITE_MMIO_CTRL_MMIO5_CFG		"$M101c0a14,4:"
	
#define BOARD_READ_MMIO_CTRL_MMIO6_CFG		"$m101c0a18,4#bc"
#define BOARD_WRITE_MMIO_CTRL_MMIO6_CFG		"$M101c0a18,4:"
	
#define BOARD_READ_MMIO_CTRL_MMIO7_CFG		"$m101c0a1c,4#e7"
#define BOARD_WRITE_MMIO_CTRL_MMIO7_CFG		"$M101c0a1c,4:"

#define BOARD_READ_MMIO_CTRL_MMIO8_CFG		"$m101c0a20,4#b5"
#define BOARD_WRITE_MMIO_CTRL_MMIO8_CFG		"$M101c0a20,4:"

#define BOARD_READ_MMIO_CTRL_MMIO9_CFG		"$m101c0a24,4#b9"
#define BOARD_WRITE_MMIO_CTRL_MMIO9_CFG		"$M101c0a24,4:"

#define BOARD_READ_MMIO_CTRL_MMIO10_CFG		"$m101c0a28,4#bd"
#define BOARD_WRITE_MMIO_CTRL_MMIO10_CFG	"$M101c0a28,4:"

#define BOARD_READ_MMIO_CTRL_MMIO11_CFG		"$m101c0a2c,4#e8"
#define BOARD_WRITE_MMIO_CTRL_MMIO11_CFG	"$M101c0a2c,4:"

#define BOARD_READ_MMIO_CTRL_MMIO12_CFG		"$m101c0a30,4#b6"
#define BOARD_WRITE_MMIO_CTRL_MMIO12_CFG	"$M101c0a30,4:"

#define BOARD_READ_MMIO_CTRL_MMIO13_CFG		"$m101c0a34,4#ba"
#define BOARD_WRITE_MMIO_CTRL_MMIO13_CFG	"$M101c0a34,4:"

#define BOARD_READ_MMIO_CTRL_MMIO14_CFG		"$m101c0a38,4#be"
#define BOARD_WRITE_MMIO_CTRL_MMIO14_CFG	"$M101c0a38,4:"

#define BOARD_READ_MMIO_CTRL_MMIO15_CFG		"$m101c0a3c,4#e9"
#define BOARD_WRITE_MMIO_CTRL_MMIO15_CFG	"$M101c0a3c,4:"

#define BOARD_READ_MMIO_CTRL_MMIO16_CFG		"$m101c0a40,4#b7"
#define BOARD_WRITE_MMIO_CTRL_MMIO16_CFG	"$M101c0a40,4:"

#define BOARD_READ_MMIO_CTRL_MMIO17_CFG		"$m101c0a44,4#bb"
#define BOARD_WRITE_MMIO_CTRL_MMIO17_CFG	"$M101c0a44,4:"

#define BOARD_READ_MMIO_CTRL_MMIO18_CFG		"$m101c0a48,4#bf"
#define BOARD_WRITE_MMIO_CTRL_MMIO18_CFG	"$M101c0a48,4:"

#define BOARD_READ_MMIO_CTRL_MMIO19_CFG		"$m101c0a4c,4#ea"
#define BOARD_WRITE_MMIO_CTRL_MMIO19_CFG	"$M101c0a4c,4:"

#define BOARD_READ_MMIO_CTRL_MMIO20_CFG		"$m101c0a50,4#b8"
#define BOARD_WRITE_MMIO_CTRL_MMIO20_CFG	"$M101c0a50,4:"

#define BOARD_READ_MMIO_CTRL_MMIO21_CFG		"$m101c0a54,4#bc"
#define BOARD_WRITE_MMIO_CTRL_MMIO21_CFG	"$M101c0a54,4:"

#define BOARD_READ_MMIO_CTRL_MMIO22_CFG		"$m101c0a58,4#c0"
#define BOARD_WRITE_MMIO_CTRL_MMIO22_CFG	"$M101c0a58,4:"

#define BOARD_READ_MMIO_CTRL_MMIO23_CFG		"$m101c0a5c,4#eb"
#define BOARD_WRITE_MMIO_CTRL_MMIO23_CFG	"$M101c0a5c,4:"

//******************************************************************************
// MMIO DEFINITIONS from common_func_netx10.h                                  *
//******************************************************************************
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

#define NX10_MMIO_MODE_IN  0
#define NX10_MMIO_MODE_OUT 1



//******************************************************************************
// XPIC REGISTER AND COMMANDS                                                  *
//******************************************************************************

#define XPIC_READ_MEMORY_STARTMSG	"$m"
#define XPIC_READ_MEMORY_ENDMSG		",4"
#define XPIC_WRITE_MEMORY_STARTMSG	"$M"
#define XPIC_WRITE_MEMORY_ENDMSG	",4"
#define XPIC_MSK_CLOCK				0x2000
#define XPIC_ZERO_VAL				"00000000"

//==============================================================================
// XPIC REGISTER r0 - r7, usr0 - usr4, pc, sr
//==============================================================================

// WRITE
#define XPIC_WRITE_REG_R0		"$M10188000,4:"
#define XPIC_WRITE_REG_R1		"$M10188004,4:"
#define XPIC_WRITE_REG_R2		"$M10188008,4:"
#define XPIC_WRITE_REG_R3		"$M1018800c,4:"
#define XPIC_WRITE_REG_R4		"$M10188010,4:"
#define XPIC_WRITE_REG_R5		"$M10188014,4:"
#define XPIC_WRITE_REG_R6		"$M10188018,4:"
#define XPIC_WRITE_REG_R7		"$M1018801c,4:"
#define XPIC_WRITE_REG_USR0		"$M10188020,4:"
#define XPIC_WRITE_REG_USR1		"$M10188024,4:"
#define XPIC_WRITE_REG_USR2		"$M10188028,4:"
#define XPIC_WRITE_REG_USR3		"$M1018802c,4:"
#define XPIC_WRITE_REG_USR4		"$M10188030,4:"
#define XPIC_WRITE_REG_PC		"$M10188034,4:"
// XXX needed?
#define XPIC_WRITE_REG_SR		"$M10188038,4:"
// XXX needed?
#define XPIC_WRITE_REG_ZERO		"$M1018803c,4:"

// READ
#define XPIC_READ_REG_R0		"$m10188000,4#5f"
#define XPIC_READ_REG_R1		"$m10188004,4#63"
#define XPIC_READ_REG_R2		"$m10188008,4#67"
#define XPIC_READ_REG_R3		"$m1018800c,4#92"
#define XPIC_READ_REG_R4		"$m10188010,4#60"
#define XPIC_READ_REG_R5		"$m10188014,4#64"
#define XPIC_READ_REG_R6		"$m10188018,4#68"
#define XPIC_READ_REG_R7		"$m1018801c,4#93"
#define XPIC_READ_REG_USR0		"$m10188020,4#61"
#define XPIC_READ_REG_USR1		"$m10188024,4#65"
#define XPIC_READ_REG_USR2		"$m10188028,4#69"
#define XPIC_READ_REG_USR3		"$m1018802c,4#94"
#define XPIC_READ_REG_USR4		"$m10188030,4#62"
#define XPIC_READ_REG_PC		"$m10188034,4#66"
#define	XPIC_READ_REG_PC2		"$m10188134,4#66"
#define XPIC_READ_REG_SR		"$m10188038,4#6a"
#define XPIC_READ_REG_ZERO		"$m1018803c,4#95"


//==============================================================================
// XPIC DEBUG REEGISTER
//==============================================================================

// WRITE
#define	XPIC_WRITE_HOLD_PC					"$M10188100,4:"
#define	XPIC_WRITE_BREAK0_ADDR				"$M10188104,4:"
#define	XPIC_WRITE_BREAK0_ADDR_MSK			"$M10188108,4:"
#define	XPIC_WRITE_BREAK0_DATA				"$M1018810c,4:"
#define	XPIC_WRITE_BREAK0_DATA_MSK			"$M10188110,4:"
#define	XPIC_WRITE_BREAK0_CONTR				"$M10188114,4:"
#define	XPIC_WRITE_BREAK0_CONTR_MSK			"$M10188118,4:"
#define	XPIC_WRITE_BREAK1_ADDR				"$M1018811c,4:"
#define	XPIC_WRITE_BREAK1_ADDR_MSK			"$M10188120,4:"
#define	XPIC_WRITE_BREAK1_DATA				"$M10188124,4:"
#define	XPIC_WRITE_BREAK1_DATA_MSK			"$M10188128,4:"
#define	XPIC_WRITE_BREAK1_CONTR				"$M1018812c,4:"
#define	XPIC_WRITE_BREAK1_CONTR_MSK			"$M10188130,4:"
#define	XPIC_WRITE_BREAK_LAST_PC			"$M10188134,4:"
#define	XPIC_WRITE_BREAK_STATUS				"$M10188138,4:"
#define	XPIC_WRITE_BREAK_IRQ_RAW			"$M1018813c,4:"
#define	XPIC_WRITE_BREAK_IRQ_MASKED			"$M10188140,4:"
#define	XPIC_WRITE_BREAK_IRQ_MSK_SET		"$M10188144,4:"
#define	XPIC_WRITE_BREAK_IRQ_MSK_RESET		"$M10188148,4:"
#define	XPIC_WRITE_BREAK_OWN_IRQ_MASKED		"$M1018814c,4:"
#define	XPIC_WRITE_BREAK_OWN_IRQ_MSK_SET	"$M10188150,4:"
#define	XPIC_WRITE_BREAK_OWN_IRQ_MSK_RESET	"$M10188154,4:"
#define	XPIC_WRITE_BREAK_RETURN_FIQ_PC		"$M10188158,4:"
#define	XPIC_WRITE_BREAK_RETURN_IRQ_PC		"$M1018815c,4:"

// READ
#define	XPIC_READ_HOLD_PC					"$m10188100,4#60"
#define	XPIC_READ_BREAK0_ADDR				"$m10188104,4#64"
#define	XPIC_READ_BREAK0_ADDR_MSK			"$m10188108,4#68"
#define	XPIC_READ_BREAK0_DATA				"$m1018810c,4#93"
#define	XPIC_READ_BREAK0_DATA_MSK			"$m10188110,4#61"
#define	XPIC_READ_BREAK0_CONTR				"$m10188114,4#65"
#define	XPIC_READ_BREAK0_CONTR_MSK			"$m10188118,4#69"
#define	XPIC_READ_BREAK1_ADDR				"$m1018811c,4#94"
#define	XPIC_READ_BREAK1_ADDR_MSK			"$m10188120,4#62"
#define	XPIC_READ_BREAK1_DATA				"$m10188124,4#66"
#define	XPIC_READ_BREAK1_DATA_MSK			"$m10188128,4#6a"
#define	XPIC_READ_BREAK1_CONTR				"$m1018812c,4#95"
#define	XPIC_READ_BREAK1_CONTR_MSK			"$m10188130,4#63"
#define	XPIC_READ_BREAK_LAST_PC				"$m10188134,4#67"
#define	XPIC_READ_BREAK_STATUS				"$m10188138,4#6b"
#define	XPIC_READ_BREAK_IRQ_RAW				"$m1018813c,4#96"
#define	XPIC_READ_BREAK_IRQ_MASKED			"$m10188140,4#64"
#define	XPIC_READ_BREAK_IRQ_MSK_SET			"$m10188144,4#68"
#define	XPIC_READ_BREAK_IRQ_MSK_RESET		"$m10188148,4#6c"
#define	XPIC_READ_BREAK_OWN_IRQ_MASKED		"$m1018814c,4#97"
#define	XPIC_READ_BREAK_OWN_IRQ_MSK_SET		"$m10188150,4#65"
#define	XPIC_READ_BREAK_OWN_IRQ_MSK_RESET	"$m10188154,4#69"
#define	XPIC_READ_BREAK_RETURN_FIQ_PC		"$m10188158,4#6d"
#define	XPIC_READ_BREAK_RETURN_IRQ_PC		"$m1018815c,4#98"


//==============================================================================
// XPIC VIC REEGISTER
//==============================================================================



//==============================================================================
// ASIC CTRL REGISTER
//==============================================================================

// WRITE
#define	XPIC_WRITE_CLOCK_ENABLE		"$M101c0028,4:"
#define	XPIC_WRITE_ACCESS_KEY		"$M101c0070,4:"

// READ
#define XPIC_READ_CLOCK_ENABLE		"$m101c0028,4#8c"
#define	XPIC_READ_ACCESS_KEY		"$m101c0070,4#89"






//==============================================================================
// XPIC CONTROL
//==============================================================================

#define	XPIC_CMD_STOP				"$M10188100,4:01000000#fb"
#define	XPIC_CMD_START				"$M10188100,4:00000000#fa"

#define XPIC_RESET_HOLD_PC			"$M10188100,4:81000000#03"
#define XPIC_SELECT_BANK0			"$M10188100,4:41000000#ff"
#define XPIC_SELECT_BANK1			"$M10188100,4:61000000#01"
#define XPIC_RESET_IRQ_RAW			"$M1018813c,4:1f000000#67"

#define XPIC_RESET_WORK_REGISTER	"$M10188000,20:0000000000000000000000000000000000000000000000000000000000000000#a7"
#define XPIC_RESET_USER_REGISTER	"$M10188020,14:0000000000000000000000000000000000000000#2c"
// #define XPIC_SETECT_BANK0 // XXX was das?
// #define XPIC_SETECT_BANK1 // XXX was das?

#define	XPIC_ENABLE_CLOCK			"$M101c0028,4:ffffffff#d6" // XXX exact (not all clocks) value?

//==============================================================================
// BREAKPOINTS & SINGLESTEP
//==============================================================================
#define XPIC_BREAK0_CONTR_DONTCARE_RANGE_CHAIN	"$M10188118,4:c0000000#36"
#define XPIC_BREAK1_CONTR_DONTCARE_RANGE_CHAIN	"$M1018812c,4:c0000000#62"

// software breakpoint
#define	XPIC_SET_SWBP_BEGINMSG	"$M"
#define	XPIC_SET_SWBP_ENDMSG	",4:100000c4"

// hardware breakpoint 0
#define	XPIC_SET_HBP0_BEGINMSG	"$M10188104,4:"
#define	XPIC_SET_HBP0_ENDMSG	"#42" // XXX calc checksum
#define XPIC_ENABLE_HBP0		"$M10188114,4:00010000#00"
#define XPIC_DISABLE_HBP0		"$M10188114,4:00000000#ff"

// hardware breakpoint 1
#define	XPIC_SET_HBP1_BEGINMSG	"$M1018811c,4:"
#define	XPIC_SET_HBP1_ENDMSG	"0000#42" // XXX calc checksum
#define XPIC_ENABLE_HBP1		"$M1018812c,4:00010000#30"
#define XPIC_DISABLE_HBP1		"$M1018812c,4:00000000#2f"

// singlestep
#define	XPIC_SINGLESTEP			"$M10188100,4:02000000#fc"

// reset irqs
#define XPIC_RESET_IRQ_HBREAK0	"$M1018813c,4:01000000#31"
#define XPIC_RESET_IRQ_HBREAK1	"$M1018813c,4:02000000#32"
#define XPIC_RESET_IRQ_SBREAK	"$M1018813c,4:04000000#34"
#define XPIC_RESET_IRQ_SSTEP	"$M1018813c,4:08000000#38"


// VIC
#define XPIC_RESET_VIC_CONFIG	"$M10140800,4:00000000#f5"
#define XPIC_RESET_VIC_SOFTINT0	"$M10140814,4:01000000#fb"







#define	MSK_HW_BREAKPOINT0		0x4
#define	MSK_HW_BREAKPOINT1		0x8
#define MSK_HW_BREAKPOINTS		0xc
#define	MSK_SW_BREAKPOINT		0x10
#define MSK_BREAKPOINTS			0x1d
#define	MSK_SW_SINGLESTEP		0x20


#endif
