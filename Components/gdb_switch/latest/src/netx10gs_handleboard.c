/*
 *******************************************************************************
 *                                                                             *
 * netx10gs_handleboard.c                                                      *
 *                                                                             *
 * author: Daniel Kramlinger                                                   *
 *         Hilscher Gesellschaft für Systemautomation mbH                      *
 * date: 21.09.2011                                                            *
 *                                                                             *
 *******************************************************************************
*/


#include "netx10gs_globals.h"
#include "netx10gs_sockets.h"
#include "netx10gs_gdbproto.h"
#include "netx10gs_helper.h"

// XXX
#define TEMP_BUFFER_SIZE 200

char* BOARD_WRITE_PFIFO_BORDER[] =
{
	BOARD_WRITE_PFIFO_BORDER0,
	BOARD_WRITE_PFIFO_BORDER1,
	BOARD_WRITE_PFIFO_BORDER2,
	BOARD_WRITE_PFIFO_BORDER3,
	BOARD_WRITE_PFIFO_BORDER4,
	BOARD_WRITE_PFIFO_BORDER5,
	BOARD_WRITE_PFIFO_BORDER6,
	BOARD_WRITE_PFIFO_BORDER7,
	BOARD_WRITE_PFIFO_BORDER8,
	BOARD_WRITE_PFIFO_BORDER9,
	BOARD_WRITE_PFIFO_BORDER10,
	BOARD_WRITE_PFIFO_BORDER11,
	BOARD_WRITE_PFIFO_BORDER12,
	BOARD_WRITE_PFIFO_BORDER13,
	BOARD_WRITE_PFIFO_BORDER14,
	BOARD_WRITE_PFIFO_BORDER15
};

char* BOARD_WRITE_VIC_VECT_ADDR[] =
{
	BOARD_WRITE_VIC_VECT_ADDR0,
	BOARD_WRITE_VIC_VECT_ADDR1,
	BOARD_WRITE_VIC_VECT_ADDR2,
	BOARD_WRITE_VIC_VECT_ADDR3,
	BOARD_WRITE_VIC_VECT_ADDR4,
	BOARD_WRITE_VIC_VECT_ADDR5,
	BOARD_WRITE_VIC_VECT_ADDR6,
	BOARD_WRITE_VIC_VECT_ADDR7,
	BOARD_WRITE_VIC_VECT_ADDR8,
	BOARD_WRITE_VIC_VECT_ADDR9,
	BOARD_WRITE_VIC_VECT_ADDR10,
	BOARD_WRITE_VIC_VECT_ADDR11,
	BOARD_WRITE_VIC_VECT_ADDR12,
	BOARD_WRITE_VIC_VECT_ADDR13,
	BOARD_WRITE_VIC_VECT_ADDR14,
	BOARD_WRITE_VIC_VECT_ADDR15
};

char* BOARD_WRITE_VIC_VECT_CNTL[] =
{
	BOARD_WRITE_VIC_VECT_CNTL0,
	BOARD_WRITE_VIC_VECT_CNTL1,
	BOARD_WRITE_VIC_VECT_CNTL2,
	BOARD_WRITE_VIC_VECT_CNTL3,
	BOARD_WRITE_VIC_VECT_CNTL4,
	BOARD_WRITE_VIC_VECT_CNTL5,
	BOARD_WRITE_VIC_VECT_CNTL6,
	BOARD_WRITE_VIC_VECT_CNTL7,
	BOARD_WRITE_VIC_VECT_CNTL8,
	BOARD_WRITE_VIC_VECT_CNTL9,
	BOARD_WRITE_VIC_VECT_CNTL10,
	BOARD_WRITE_VIC_VECT_CNTL11,
	BOARD_WRITE_VIC_VECT_CNTL12,
	BOARD_WRITE_VIC_VECT_CNTL13,
	BOARD_WRITE_VIC_VECT_CNTL14,
	BOARD_WRITE_VIC_VECT_CNTL15
};

char* BOARD_READ_MMIO[] =
{
	BOARD_READ_MMIO_CTRL_MMIO0_CFG,
	BOARD_READ_MMIO_CTRL_MMIO1_CFG,
	BOARD_READ_MMIO_CTRL_MMIO2_CFG,
	BOARD_READ_MMIO_CTRL_MMIO3_CFG,
	BOARD_READ_MMIO_CTRL_MMIO4_CFG,
	BOARD_READ_MMIO_CTRL_MMIO5_CFG,
	BOARD_READ_MMIO_CTRL_MMIO6_CFG,
	BOARD_READ_MMIO_CTRL_MMIO7_CFG,
	BOARD_READ_MMIO_CTRL_MMIO8_CFG,
	BOARD_READ_MMIO_CTRL_MMIO9_CFG,
	BOARD_READ_MMIO_CTRL_MMIO10_CFG,
	BOARD_READ_MMIO_CTRL_MMIO11_CFG,
	BOARD_READ_MMIO_CTRL_MMIO12_CFG,
	BOARD_READ_MMIO_CTRL_MMIO13_CFG,
	BOARD_READ_MMIO_CTRL_MMIO14_CFG,
	BOARD_READ_MMIO_CTRL_MMIO15_CFG,
	BOARD_READ_MMIO_CTRL_MMIO16_CFG,
	BOARD_READ_MMIO_CTRL_MMIO17_CFG,
	BOARD_READ_MMIO_CTRL_MMIO18_CFG,
	BOARD_READ_MMIO_CTRL_MMIO19_CFG,
	BOARD_READ_MMIO_CTRL_MMIO20_CFG,
	BOARD_READ_MMIO_CTRL_MMIO21_CFG,
	BOARD_READ_MMIO_CTRL_MMIO22_CFG,
	BOARD_READ_MMIO_CTRL_MMIO23_CFG,
};

char* BOARD_WRITE_MMIO[] =
{
	BOARD_WRITE_MMIO_CTRL_MMIO0_CFG,
	BOARD_WRITE_MMIO_CTRL_MMIO1_CFG,
	BOARD_WRITE_MMIO_CTRL_MMIO2_CFG,
	BOARD_WRITE_MMIO_CTRL_MMIO3_CFG,
	BOARD_WRITE_MMIO_CTRL_MMIO4_CFG,
	BOARD_WRITE_MMIO_CTRL_MMIO5_CFG,
	BOARD_WRITE_MMIO_CTRL_MMIO6_CFG,
	BOARD_WRITE_MMIO_CTRL_MMIO7_CFG,
	BOARD_WRITE_MMIO_CTRL_MMIO8_CFG,
	BOARD_WRITE_MMIO_CTRL_MMIO9_CFG,
	BOARD_WRITE_MMIO_CTRL_MMIO10_CFG,
	BOARD_WRITE_MMIO_CTRL_MMIO11_CFG,
	BOARD_WRITE_MMIO_CTRL_MMIO12_CFG,
	BOARD_WRITE_MMIO_CTRL_MMIO13_CFG,
	BOARD_WRITE_MMIO_CTRL_MMIO14_CFG,
	BOARD_WRITE_MMIO_CTRL_MMIO15_CFG,
	BOARD_WRITE_MMIO_CTRL_MMIO16_CFG,
	BOARD_WRITE_MMIO_CTRL_MMIO17_CFG,
	BOARD_WRITE_MMIO_CTRL_MMIO18_CFG,
	BOARD_WRITE_MMIO_CTRL_MMIO19_CFG,
	BOARD_WRITE_MMIO_CTRL_MMIO20_CFG,
	BOARD_WRITE_MMIO_CTRL_MMIO21_CFG,
	BOARD_WRITE_MMIO_CTRL_MMIO22_CFG,
	BOARD_WRITE_MMIO_CTRL_MMIO23_CFG,
};

//==============================================================================
// NETX10GS_HANDLEBOARD_IRQFIQLOCK
//==============================================================================
void netx10gs_handleboard_irqFiqLock( void )
{
	netx10gs_helper_log( LOG_IRQFIQLOCK, "handleboard_irqFiqLock - start" );
	
	// send message
	netx10gs_sockets_SendAndReceive( SOCKET_OPENOCD, BOARD_IRQFIQ_LOCK, buffer, DEFAULT_BUFFER_LEN, GDB_DONT_RETURN_ANSWER, INVALID_SOCKET, GDB_DONT_WAIT_FOR_OK, "netx10gs_handleboard_systime_SetBorder - could not read xpic pc!" ); // XXX new msg
	
	netx10gs_helper_log( LOG_IRQFIQLOCK, "handleboard_irqFiqLock - end" );
	
}
//==============================================================================


//==============================================================================
// NETX10GS_HANDLEBOARD_IRQFIQUNLOCK
//==============================================================================
void netx10gs_handleboard_irqFiqUnlock( void )
{
	// send message
	netx10gs_sockets_SendAndReceive( SOCKET_OPENOCD, BOARD_IRQFIQ_UNLOCK, buffer, DEFAULT_BUFFER_LEN, GDB_DONT_RETURN_ANSWER, INVALID_SOCKET, GDB_DONT_WAIT_FOR_OK, "netx10gs_handleboard_systime_SetBorder - could not read xpic pc!" ); // XXX new msg
}
//==============================================================================


//==============================================================================
// NETX10GS_HANDLEBOARD_POKE
//==============================================================================
void netx10gs_handleboard_Poke( char *pcAddr, unsigned long ulValue )
{
	// build message
	netx10gs_helper_BuildGdbMsg( buffer, DEFAULT_BUFFER_LEN, pcAddr, NULL, GDB_VALUE_TYPE_GDB, ulValue, GDB_CALC_CHECKSUM );
	
	netx10gs_helper_log( LOG_POKE, "POKE HIER: %s", buffer );
	
	// send message
	netx10gs_sockets_SendAndReceive( SOCKET_OPENOCD, buffer, buffer, DEFAULT_BUFFER_LEN, GDB_DONT_RETURN_ANSWER, INVALID_SOCKET, GDB_DONT_WAIT_FOR_OK, "netx10gs_handleboard_systime_SetBorder - could not read xpic pc!" ); // XXX new msg

}
//==============================================================================


//==============================================================================
// NETX10GS_HANDLEBOARD_GETANDRETURNACCESSKEY
//==============================================================================
///
/// @brief Gets and returns the access key.
//==============================================================================
void netx10gs_handleboard_GetAndReturnAccessKey( void )
{
	char
		acTempBuffer[TEMP_BUFFER_SIZE] = {0};
	
	
	netx10gs_helper_log( LOG_GETANDRETURNACCESSKEY, "handleboard_GetAndReturnAccessKey - start" );
	netx10gs_helper_log( LOG_VERBOSE, "getting access key ..." );
	
	// read access key
	netx10gs_sockets_SendAndReceive( SOCKET_OPENOCD, BOARD_READ_ACCESS_KEY, buffer, DEFAULT_BUFFER_LEN, GDB_DONT_RETURN_ANSWER, INVALID_SOCKET, GDB_DONT_WAIT_FOR_OK, "netx10gs_handlexpic_GetAndReturnAccessKey - could not read access key!" );
	
	buffer[9] = 0; // XXX kann weg?
	
	//
	if ( netx10gs_helper_BuildGdbMsg( acTempBuffer, TEMP_BUFFER_SIZE, BOARD_WRITE_ACCESS_KEY, &buffer[1], GDB_VALUE_TYPE_NO_VALUE, 0, GDB_CALC_CHECKSUM )
		!= GDB_SWITCH_OK )
	{
		netx10gs_helper_log( LOG_ERROR, "could not write access key!" );
		return;
	}
	
	//
	netx10gs_sockets_SendAndReceive( SOCKET_OPENOCD, acTempBuffer, buffer, DEFAULT_BUFFER_LEN, GDB_DONT_RETURN_ANSWER, INVALID_SOCKET, GDB_DONT_WAIT_FOR_OK, "netx10gs_handlexpic_GetAndReturnAccessKey - could not write access key!" );
	
	netx10gs_helper_log( LOG_VERBOSE, "got and returned access key" );
	netx10gs_helper_log( LOG_GETANDRETURNACCESSKEY, "handleboard_GetAndReturnAccessKey - end" );
	return;
}
//==============================================================================



//==============================================================================
// NETX10GS_HANDLEBOARD_ACTIVATESYSTEMCLOCKS
//==============================================================================
void netx10gs_handleboard_ActivateSystemClocks( void )
{
	netx10gs_helper_log( LOG_ACTIVATESYSTEMCLOCKS, "netx10gs_handleboard_ActivateSystemClocks - start" );
	
	netx10gs_handleboard_GetAndReturnAccessKey();
	
	// reset pointer fifo borders
	netx10gs_handleboard_Poke( BOARD_ACTIVATE_SYSTEM_CLOCKS, 0x7111 );
	
	netx10gs_helper_log( LOG_ACTIVATESYSTEMCLOCKS, "netx10gs_handleboard_ActivateSystemClocks - end" );
}
//==============================================================================


//==============================================================================
// NETX10GS_HANDLEBOARD_SYSTIME_SETBORDER
//==============================================================================
void netx10gs_handleboard_systime_SetBorder( unsigned long ulBorder )
{
	netx10gs_helper_log( LOG_SETBORDER, "handleboard_systime_SetBorder - start" );
	
	// reset pointer fifo borders
	netx10gs_handleboard_Poke( BOARD_SET_BORDER, ulBorder );
	
	netx10gs_helper_log( LOG_SETBORDER, "handleboard_systime_SetBorder - end" );
}
//==============================================================================


//==============================================================================
// NETX10GS_HANDLEBOARD_SYSTIME_SETSPEED
//==============================================================================
void netx10gs_handleboard_systime_SetSpeed( unsigned long ulSpeed )
{
	netx10gs_helper_log( LOG_SETSPEED, "handleboard_systime_SetSpeed - start" );
	
	netx10gs_handleboard_Poke( BOARD_SET_SPEED, ulSpeed );
	
	netx10gs_helper_log( LOG_SETSPEED, "handleboard_systime_SetSpeed - end" );
}
//==============================================================================


//==============================================================================
// NETX10GS_HANDLEBOARD_MMIO_SETCFG
//==============================================================================
void netx10gs_handleboard_mmio_SetCfg( unsigned int uMmioNum, unsigned char bFunction )
{
	netx10gs_helper_log( LOG_SETCFG, "handleboard_mmio_SetCfg - start" );
	
	// read access key, write back access key
	netx10gs_handleboard_GetAndReturnAccessKey();
	
	netx10gs_handleboard_Poke( BOARD_WRITE_MMIO[uMmioNum], bFunction );
	
	netx10gs_helper_log( LOG_SETCFG, "handleboard_mmio_SetCfg - end" );
}
//==============================================================================





//==============================================================================
// NETX10GS_HANDLEBOARD_PFIFO_RESET
//==============================================================================
void netx10gs_handleboard_pfifo_Reset( void )
{
	unsigned int
		uiCnt;
	

	netx10gs_helper_log( LOG_PFIFO_RESET, "handleboard_pfifo_Reset - start" );
	
	// set reset flag of all fifos
	netx10gs_handleboard_Poke( BOARD_WRITE_PFIFO_RESET, 0x0000ffff );
	
	// reset pointer fifo borders
	for( uiCnt = 0; uiCnt < 16; uiCnt++ )
	{
		netx10gs_handleboard_Poke( BOARD_WRITE_PFIFO_BORDER[uiCnt], ((uiCnt+1)* 64)-1 );
	}
	
	// clear reset flag of all fifos
	netx10gs_handleboard_Poke( BOARD_WRITE_PFIFO_RESET, 0x0 );
	
	netx10gs_helper_log( LOG_PFIFO_RESET, "handleboard_pfifo_Reset - end" );
}
//==============================================================================


//==============================================================================
// NETX10GS_HANDLEBOARD_VIC_CLEARVIC
//==============================================================================
void netx10gs_handleboard_vic_ClearVic( void )
{
	unsigned int
		uiCnt;
	
	netx10gs_helper_log( LOG_CLEARVIC, "handleboard_vic_ClearVic - start" );
	
	// no firqs
	netx10gs_handleboard_Poke( BOARD_WRITE_VIC_INT_SELECT, 0x0 );
	netx10gs_handleboard_Poke( BOARD_WRITE_VIC_INT_ENCLEAR, 0xffffffff );
	
	// clear all vectors
	for( uiCnt = 0; uiCnt < 16; uiCnt++ )
	{
		BOARD_WRITE_VIC_VECT_ADDR[uiCnt] = 0x0;
		BOARD_WRITE_VIC_VECT_CNTL[uiCnt] = 0x0;
	}
	
	netx10gs_helper_log( LOG_CLEARVIC, "handleboard_vic_ClearVic - end" );
}
//==============================================================================


//==============================================================================
// NETX10GS_HANDLEBOARD_INITIRQ
//==============================================================================
void netx10gs_handleboard_InitIrq( void )
{
	// // init irq vector
	// NX10_VIC_InstallIsrVector( NX10_VIC_IRQVectorHandler_C );
	
	// // set the vectors for the irqs
	// NX10_VIC_SetIrqVector(0, SRT_NX10_vic_int_enable_timer0 | MSK_NX10_vic_vect_cntl0_ENABLE, VIC_ISR_Timer_XpicGdb);
	
	// // enable irqs
	// NX10_VIC_EnableIrqSources(MSK_NX10_vic_int_enable_timer0);
	
	// NX10_ArmTimer_Stop(0);
	// // init gdb uart
	// NX10_UART_InitSerial(XPICGDB_UARTNUM);
	// while( NX10_UART_Peek(XPICGDB_UARTNUM) ) { NX10_UART_GetCharacter(XPICGDB_UARTNUM); }
	
	// // init xpic_gdb instance
	// xpicgdb_init(&s_tXpicGdb);
	// s_tXpicGdb.pfnOutput = gdb_print_uart;
	// s_tXpicGdb.pvAppCtx = (void*)(Addr_NX10_uart0 + 0x40*XPICGDB_UARTNUM);
	
	// // start cyclic gdb server timer
	// NX10_ArmTimer_EnIrq(1,0,0,0);
	// NX10_ArmTimer_Start(0, NX10_ARMTIMER_CFG_MODE_PRELOAD_AT_0, XPICGDB_POLLTIME_US*(NX10_DEV_FREQUENCY/1000000));
}
//==============================================================================


//==============================================================================
// NETX10GS_HANDLEBOARD_INITBOARD
//==============================================================================
void netx10gs_handleboard_InitBoard( void )
{
	netx10gs_helper_log( LOG_INITBOARD, "handleboard_InitBoard - start" );
	
	netx10gs_handleboard_ActivateSystemClocks();
	
	// lock irqs during the initialisation
	netx10gs_handleboard_irqFiqLock();
	
	// initialize systime unit
	netx10gs_handleboard_systime_SetBorder( 1000000000 );
	netx10gs_handleboard_systime_SetSpeed ( 0xa0000000 );
	
	// Configure MMIO pin multiplexer
	netx10gs_handleboard_mmio_SetCfg(  0, NX10_MMIO_CONFIG_XM0_RX    );
	netx10gs_handleboard_mmio_SetCfg(  1, NX10_MMIO_CONFIG_PIO_MODE  );
	netx10gs_handleboard_mmio_SetCfg(  2, NX10_MMIO_CONFIG_XM0_IO1   );
	netx10gs_handleboard_mmio_SetCfg(  3, NX10_MMIO_CONFIG_XM0_IO0   );
#ifdef _NXHX10_UART1_ENABLE_
	netx10gs_handleboard_mmio_SetCfg(  4, NX10_MMIO_CONFIG_UART1_TXD );  // Disable Dip Switch, Use UART1
	netx10gs_handleboard_mmio_SetCfg(  5, NX10_MMIO_CONFIG_UART1_RXD );  // Disable Dip Switch, Use UART1
	netx10gs_handleboard_mmio_SetCfg(  6, NX10_MMIO_CONFIG_UART1_RTS );  // Disable Dip Switch, Use UART1
	netx10gs_handleboard_mmio_SetCfg(  7, NX10_MMIO_CONFIG_UART1_CTS );  // Disable Dip Switch, Use UART1
#else
	netx10gs_handleboard_mmio_SetCfg(  4, NX10_MMIO_CONFIG_PIO_MODE  );
	netx10gs_handleboard_mmio_SetCfg(  5, NX10_MMIO_CONFIG_PIO_MODE  );
	netx10gs_handleboard_mmio_SetCfg(  6, NX10_MMIO_CONFIG_PIO_MODE  );
	netx10gs_handleboard_mmio_SetCfg(  7, NX10_MMIO_CONFIG_PIO_MODE  );
#endif
	netx10gs_handleboard_mmio_SetCfg(  8, NX10_MMIO_CONFIG_PIO_MODE  );
	netx10gs_handleboard_mmio_SetCfg(  9, NX10_MMIO_CONFIG_PIO_MODE  );
	netx10gs_handleboard_mmio_SetCfg( 10, NX10_MMIO_CONFIG_PIO_MODE  );
	netx10gs_handleboard_mmio_SetCfg( 11, NX10_MMIO_CONFIG_PIO_MODE  );
	netx10gs_handleboard_mmio_SetCfg( 12, NX10_MMIO_CONFIG_UART0_CTS );
	netx10gs_handleboard_mmio_SetCfg( 13, NX10_MMIO_CONFIG_UART0_RTS );
	netx10gs_handleboard_mmio_SetCfg( 14, NX10_MMIO_CONFIG_PIO_MODE  );
	netx10gs_handleboard_mmio_SetCfg( 15, NX10_MMIO_CONFIG_PIO_MODE  );
	netx10gs_handleboard_mmio_SetCfg( 16, NX10_MMIO_CONFIG_XM0_IO0   );  // Ethernet LED
	netx10gs_handleboard_mmio_SetCfg( 17, NX10_MMIO_CONFIG_XM0_IO1   );  // Ethernet LED
	netx10gs_handleboard_mmio_SetCfg( 18, NX10_MMIO_CONFIG_GPIO0     );
	netx10gs_handleboard_mmio_SetCfg( 19, NX10_MMIO_CONFIG_GPIO1     );
	netx10gs_handleboard_mmio_SetCfg( 20, NX10_MMIO_CONFIG_UART0_RXD );
	netx10gs_handleboard_mmio_SetCfg( 21, NX10_MMIO_CONFIG_UART0_TXD );
	netx10gs_handleboard_mmio_SetCfg( 22, NX10_MMIO_CONFIG_GPIO7     );
	netx10gs_handleboard_mmio_SetCfg( 23, NX10_MMIO_CONFIG_SPI0_CS1N );
	
	// init sdram controller
	netx10gs_handleboard_Poke( BOARD_WRITE_SDRAM_GENERAL_CTRL, 0x0 );
	netx10gs_handleboard_Poke( BOARD_WRITE_SDRAM_TIMING_CTRL,  0x00a13262 );
	netx10gs_handleboard_Poke( BOARD_WRITE_SDRAM_MR,           0x00000033 );
	netx10gs_handleboard_Poke( BOARD_WRITE_SDRAM_GENERAL_CTRL, 0x030d0121 );
	
	// init hif interface for sdram usage	
	netx10gs_handleboard_GetAndReturnAccessKey();
	netx10gs_handleboard_Poke( BOARD_WRITE_HIF_IO_CFG, (1 << SRT_NX10_hif_io_cfg_hif_mi_cfg) | MSK_NX10_hif_io_cfg_en_hif_sdram_mi );
	
	// // reset pointer fifo borders
	// netx10gs_handleboard_pfifo_Reset();
	
	// // clear all pending irqs, this is important for 'soft restarts' from within the hitop debugger
	// netx10gs_handleboard_vic_ClearVic();
	
	// // initialize the irq, set ethernet isr
	// InitIrq();
	
	// unlock irqs
	netx10gs_handleboard_irqFiqUnlock();
	
	netx10gs_helper_log( LOG_INITBOARD, "handleboard_InitBoard - end" );
}