/*
 *******************************************************************************
 *                                                                             *
 * netx10gs_handlexpic.c                                                       *
 *                                                                             *
 * author: Daniel Kramlinger                                                   *
 *         Hilscher Gesellschaft für Systemautomation mbH                      *
 * date: 15.04.2011                                                            *
 *                                                                             *
 *******************************************************************************
*/


#include "netx10gs_globals.h"
#include "netx10gs_gdbproto.h"
#include "netx10gs_sockets.h"
#include "netx10gs_handlearm.h"
#include "netx10gs_helper.h"


int
	iHardwareBreakpointCounter = 0;

unsigned int
	uiHardwareBreakpoint1 = 0,
	uiHardwareBreakpoint2 = 0;

struct {
	unsigned int uiAddr;
	char acInst[8];
	BOOLEAN bUsed;
} structarraySwBp[GDB_MAX_SWBP+1]; // XXX +1?

char *apcXpicReadRegister[] =
{
	XPIC_READ_REG_R0,
	XPIC_READ_REG_R1,
	XPIC_READ_REG_R2,
	XPIC_READ_REG_R3,
	XPIC_READ_REG_R4,
	XPIC_READ_REG_R5,
	XPIC_READ_REG_R6,
	XPIC_READ_REG_R7,
	XPIC_READ_REG_USR0,
	XPIC_READ_REG_USR1,
	XPIC_READ_REG_USR2,
	XPIC_READ_REG_USR3,
	XPIC_READ_REG_USR4,
	XPIC_READ_REG_PC,
	XPIC_READ_REG_SR,
	XPIC_READ_REG_ZERO
};

char *apcXpicWriteRegister[] =
{
	XPIC_WRITE_REG_R0,
	XPIC_WRITE_REG_R1,
	XPIC_WRITE_REG_R2,
	XPIC_WRITE_REG_R3,
	XPIC_WRITE_REG_R4,
	XPIC_WRITE_REG_R5,
	XPIC_WRITE_REG_R6,
	XPIC_WRITE_REG_R7,
	XPIC_WRITE_REG_USR0,
	XPIC_WRITE_REG_USR1,
	XPIC_WRITE_REG_USR2,
	XPIC_WRITE_REG_USR3,
	XPIC_WRITE_REG_USR4,
	XPIC_WRITE_REG_PC,
	XPIC_WRITE_REG_SR
};

BOOLEAN
	bSwbpAlreadySet;
	



//==============================================================================
// NETX10GS_HANDLEXPIC_HALTXPIC
//==============================================================================
///
/// @brief Stops xpic.
///
//==============================================================================
void netx10gs_handlexpic_HaltXpic( void )
{
	netx10gs_helper_log( LOG_DEBUG, "handlexpic_HaltXpic - start" );
	
	netx10gs_sockets_SendAndReceive( SOCKET_OPENOCD, XPIC_CMD_STOP, buffer, DEFAULT_BUFFER_LEN, GDB_DONT_RETURN_ANSWER, INVALID_SOCKET, GDB_WAIT_FOR_OK, "netx10gs_handlexpic_HaltXpic - could not stop xpic!" );
	
	netx10gs_helper_log( LOG_DEBUG, "handlexpic_HaltXpic - end" );
}
//==============================================================================


//==============================================================================
// NETX10GS_HANDLEXPIC_RESUMEXPIC
//==============================================================================
///
/// @brief Resumes xpic.
///
//==============================================================================
void netx10gs_handlexpic_ResumeXpic( void )
{
	netx10gs_helper_log( LOG_DEBUG, "handlexpic_ResumeXpic - start" );
	
	netx10gs_sockets_SendAndReceive( SOCKET_OPENOCD, XPIC_CMD_START, buffer, DEFAULT_BUFFER_LEN, GDB_DONT_RETURN_ANSWER, INVALID_SOCKET, GDB_WAIT_FOR_OK, "netx10gs_handlexpic_ResumeXpic - could not start xpic!" );
	
	netx10gs_helper_log( LOG_DEBUG, "handlexpic_ResumeXpic - end" );
}
//==============================================================================


// //==============================================================================
// // NETX10GS_HANDLEXPIC_READXPICREGISTERBOTHBANKS
// //==============================================================================
// char netx10gs_handlexpic_ReadXpicRegisterBothBanks( void )
// {
	// #undef TEMP_BUFFER_SIZE
	// #define TEMP_BUFFER_SIZE 20
	
	// char
		// acTempBuffer[TEMP_BUFFER_SIZE] = {0};
	
	// int
		// i = 0x0,
		// iBankSelectTemp = 0x0;
	
	
	// netx10gs_sockets_SendAndReceive( SOCKET_XPICGDB, GDB_MONITOR_MSG_READREGS_BANK0, buffer, DEFAULT_BUFFER_LEN, GDB_DONT_RETURN_ANSWER, INVALID_SOCKET, GDB_DONT_WAIT_FOR_OK, "netx10gs_handlexpic_ReadXpicRegisterBothBanks" );
	
	
	// // get bank value
	// netx10gs_sockets_SendAndReceive( SOCKET_XPICGDB, XPIC_READ_HOLD_PC, buffer, DEFAULT_BUFFER_LEN, GDB_DONT_RETURN_ANSWER, INVALID_SOCKET, GDB_DONT_WAIT_FOR_OK, "netx10gs_handlexpic_ReadXpicRegisterBothBanks" );
	// // iBankSelectTemp = netx10gs_helper_GdbVal2UnsignedInt( buffer );
	
	// // set BANK_SELECT 0 - bit 5, set BANK_CONTROL 1 - bit 6
	// iBankSelectTemp = (iBankSelectTemp & 0xdf) | 0x40;
	
	// // DDD
	// netx10gs_helper_BuildGdbMsg( acTempBuffer, TEMP_BUFFER_SIZE, NULL, NULL, GDB_VALUE_TYPE_GDB, iBankSelectTemp, GDB_CALC_CHECKSUM );
	
	// // read all 15 XPIC registers / register ZERO is not read
	// for ( i = 0; i < 15; i++ )
	// {
		// netx10gs_sockets_SendAndReceive( SOCKET_XPICGDB, apcXpicReadRegister[i], buffer, DEFAULT_BUFFER_LEN, GDB_DONT_RETURN_ANSWER, INVALID_SOCKET, GDB_DONT_WAIT_FOR_OK, "netx10gs_handlexpic_ReadXpicRegisterBothBanks" );
	
	// }

	// // change bank
	// if ( iBankSelectTemp & 0x20 )
	// {
	
	
	// }
	
	// return 0;
// }
// //==============================================================================


//==============================================================================
// NETX10GS_HANDLEXPIC_STEPBACKPC
//==============================================================================
///
/// @brief Steps back the xpic pc.
///
/// @param [in] uiValue how many steps to go back.
///
//==============================================================================
void netx10gs_handlexpic_StepBackPC( unsigned int uiValue )
{
	unsigned int
		uiOldPc = 0x0;
	
	struct structUiValue_t
		structUiValue;
	
	netx10gs_helper_log( LOG_STEPBACKPC, "handlexpic_StepBackPC - start" );
	netx10gs_helper_log( LOG_STEPBACKPC, "value: %x", uiValue );
	
	// read xpic pc
	netx10gs_sockets_SendAndReceive( SOCKET_OPENOCD, XPIC_READ_REG_PC, buffer, DEFAULT_BUFFER_LEN, GDB_DONT_RETURN_ANSWER, INVALID_SOCKET, GDB_DONT_WAIT_FOR_OK, "netx10gs_handlexpic_StepBackPC - could not read xpic pc!" );
	
	netx10gs_helper_GdbVal2UnsignedInt( &buffer[1], &structUiValue );
	
	// catch error
	if ( structUiValue.cErrorCode == GDB_SWITCH_ERROR )
	{
		return; // XXX
	}
	
	netx10gs_helper_log( LOG_STEPBACKPC, "old pc: %x (%i)", structUiValue.uiRetval, structUiValue.uiRetval );
	uiOldPc = structUiValue.uiRetval - uiValue;
	
	// build message
	netx10gs_helper_BuildGdbMsg( buffer, DEFAULT_BUFFER_LEN, XPIC_WRITE_REG_PC, NULL, GDB_VALUE_TYPE_GDB, uiOldPc, GDB_CALC_CHECKSUM ); // MMM
	
	// send message
	netx10gs_sockets_SendAndReceive( SOCKET_OPENOCD, buffer, buffer, DEFAULT_BUFFER_LEN, GDB_DONT_RETURN_ANSWER, INVALID_SOCKET, GDB_DONT_WAIT_FOR_OK, "netx10gs_handlexpic_StepBackPC - could not read xpic pc!" );
	
	netx10gs_helper_log( LOG_STEPBACKPC, "handlexpic_StepBackPC - end" );
}
//==============================================================================


//==============================================================================
// NETX10GS_HANDLEXPIC_READXPICREGISTERS
//==============================================================================
///
/// @brief Reads the xpic registers.
///
//==============================================================================
void netx10gs_handlexpic_ReadXpicRegisters( void )
{
	#undef TEMP_BUFFER_SIZE
	#define TEMP_BUFFER_SIZE 133
	
	char
		acTempBuffer[TEMP_BUFFER_SIZE] = {0};
	
	int
		i = 0;
	
	
	netx10gs_helper_log( LOG_READXPICREGISTERS, "handlexpic_ReadXpicRegisters - start" );
	
	acTempBuffer[0] = '$';
	
	// read all 15 XPIC registers / register ZERO is not read
	for ( i = 0; i < 15; i++ )
	{
		// getting the register values from OpenOCD
		netx10gs_sockets_SendAndReceive( SOCKET_OPENOCD, apcXpicReadRegister[i], buffer, DEFAULT_BUFFER_LEN, GDB_DONT_RETURN_ANSWER, INVALID_SOCKET, GDB_DONT_WAIT_FOR_OK, "netx10gs_handlexpic_ReadXpicRegisters - could not read xpic registers" );

		// check for error EXX with getgdbcmd?
		if ( buffer[1] == 'E' )
		{
			// sending error EXX to xpicgdb
			netx10gs_sockets_Send( SOCKET_XPICGDB, buffer, "netx10gs_handlexpic_ReadXpicRegisters - could not send msg!" );
			return;
		}

		// concat result
		strncat_s( acTempBuffer, TEMP_BUFFER_SIZE, &buffer[1], 8 );
	}
	
	// build msg
	netx10gs_helper_BuildGdbMsg( buffer, DEFAULT_BUFFER_LEN, acTempBuffer, XPIC_ZERO_VAL, GDB_VALUE_TYPE_NO_VALUE, 0, GDB_CALC_CHECKSUM ); // MMM
	
	// sending msg with register values to xpicgdb
	netx10gs_sockets_Send( SOCKET_XPICGDB, buffer, "netx10gs_handlexpic_ReadXpicRegisters - could not send msg!" );
	
	if ( g_bClockActivated )
	{
		g_bXpicInitFinished	= TRUE;
	}
	
	netx10gs_helper_log( LOG_READXPICREGISTERS, "handlexpic_ReadXpicRegisters - end" );
	return;
}
//==============================================================================


//==============================================================================
// NETX10GS_HANDLEXPIC_SETSOFTWAREBREAKPOINT
//==============================================================================
///
/// @brief Sets a software Breakpoint.
///
/// @param [in] uiAddress address where to set breakpoint.
///
/// @return Errorcode.
///
//==============================================================================
char netx10gs_handlexpic_SetSoftwareBreakpoint( unsigned int uiAddress )
{
	int
		i = 0,
		j = 0;

	// unsigned int uiAddress = g_structGdbCommand.uiSecondArg;
	netx10gs_helper_log( LOG_SETSOFTWAREBREAKPOINT, "handlexpic_SetSoftwareBreakpoint - start" );
	
	// workaround
	if ( uiAddress < 0x00002000 )
	{
		uiAddress += XPIC_PRAM_STARTADDRESS;
	}
	
	// check if breakpoint is already set
	for ( i = 0; i <= GDB_MAX_SWBP; i++ )
	{
		if ( (structarraySwBp[i].uiAddr == uiAddress) && (structarraySwBp[i].bUsed == TRUE) )
		{
			netx10gs_sockets_Send( SOCKET_XPICGDB, GDB_ERROR_0, "handlexpic_SetSoftwareBreakpoint - could not send msg!" );
			return GDB_SWITCH_OK;
		}
	}
	
	// search for free entry in list 'struct_sw_bp'
	i = 0;
	while ( (i <= GDB_MAX_SWBP) && (structarraySwBp[i].bUsed == TRUE) )
	{
		i++;
	}
	
	// if no free entry found send error to GDB
	if ( i > GDB_MAX_SWBP )
	{
		return GDB_SWITCH_ERROR; // XXX ERROR or OK?
	}
	
	structarraySwBp[i].bUsed = TRUE;
	structarraySwBp[i].uiAddr = uiAddress;
	
	// build msg
	netx10gs_helper_BuildGdbMsg( buffer, DEFAULT_BUFFER_LEN, XPIC_READ_MEMORY_STARTMSG, XPIC_READ_MEMORY_ENDMSG, GDB_VALUE_TYPE_NORMAL, uiAddress, GDB_CALC_CHECKSUM );
	
	netx10gs_sockets_SendAndReceive( SOCKET_OPENOCD, buffer, buffer, DEFAULT_BUFFER_LEN, GDB_DONT_RETURN_ANSWER, INVALID_SOCKET, GDB_DONT_WAIT_FOR_OK, "handlexpic_SetSoftwareBreakpoint - could not send/recv msg!" );
	
	// save instruction
	for( j = 0; j < 8; j++ )
	{
		structarraySwBp[i].acInst[j] = buffer[ j + 1 ];
	}
	
	// build msg
	netx10gs_helper_BuildGdbMsg( buffer, DEFAULT_BUFFER_LEN, XPIC_SET_SWBP_BEGINMSG, XPIC_SET_SWBP_ENDMSG, GDB_VALUE_TYPE_NORMAL, uiAddress, GDB_CALC_CHECKSUM );
	// send msg
	netx10gs_sockets_SendAndReceive( SOCKET_OPENOCD, buffer, buffer, DEFAULT_BUFFER_LEN, GDB_DONT_RETURN_ANSWER, INVALID_SOCKET, GDB_DONT_WAIT_FOR_OK, "handlexpic_SetSoftwareBreakpoint - could not send/recv msg!" );
	
	// send OK
	netx10gs_sockets_Send( SOCKET_XPICGDB, GDB_OK, "handlexpic_SetSoftwareBreakpoint - could not send msg!" );
	
	netx10gs_helper_log( LOG_SETSOFTWAREBREAKPOINT, "handlexpic_SetSoftwareBreakpoint - end" );
	return GDB_SWITCH_OK;
}
//==============================================================================


//==============================================================================
// NETX10GS_HANDLEXPIC_REMOVESOFTWAREBREAKPOINT
//==============================================================================
///
/// @brief Removes a software Breakpoint.
///
/// @param [in] uiAddress address where to remove breakpoint.
///
/// @return Errorcode.
///
//==============================================================================
void netx10gs_handlexpic_RemoveSoftwareBreakpoint( unsigned int uiAddress )
{
	#undef TEMP_BUFFER_SIZE
	#define TEMP_BUFFER_SIZE 12
	
	char
		acTempBuffer[TEMP_BUFFER_SIZE];
	
	int
		i = 0;
	
	
	netx10gs_helper_log( LOG_DEBUG, "handlexpic_RemoveSoftwareBreakpoint - start" );
	// workaround
	if ( uiAddress < 0x00002000 )
	{
		uiAddress += XPIC_PRAM_STARTADDRESS;
	}
	
	// search for breakpoint in list
	for ( i = 0; i <= GDB_MAX_SWBP; i++)
	{
		if ( (structarraySwBp[i].uiAddr == uiAddress) && (structarraySwBp[i].bUsed == TRUE) )
			break;
	}
	
	// if breakpoint not found send error to GDB
	if ( i > GDB_MAX_SWBP )
	{
		netx10gs_sockets_Send( SOCKET_XPICGDB, GDB_ERROR_0, "handlexpic_RemoveSoftwareBreakpoint - could not send msg!" );
		return;
	}
	
	structarraySwBp[i].bUsed = FALSE;

	strcpy_s( acTempBuffer, TEMP_BUFFER_SIZE, ",4:");
	strcat_s( acTempBuffer, TEMP_BUFFER_SIZE, structarraySwBp[i].acInst );
	
	// build msg
	netx10gs_helper_BuildGdbMsg( buffer, DEFAULT_BUFFER_LEN, XPIC_SET_SWBP_BEGINMSG, acTempBuffer, GDB_VALUE_TYPE_NORMAL, structarraySwBp[i].uiAddr, GDB_CALC_CHECKSUM );
	
	// send msg
	netx10gs_sockets_SendAndReceive( SOCKET_OPENOCD, buffer, buffer, DEFAULT_BUFFER_LEN, GDB_DONT_RETURN_ANSWER, INVALID_SOCKET, GDB_DONT_WAIT_FOR_OK, "handlexpic_SetSoftwareBreakpoint - could not send/recv msg!" );
	
	// send OK
	netx10gs_sockets_Send( SOCKET_XPICGDB, GDB_OK, "handlexpic_RemoveSoftwareBreakpoint - could not send msg!" );
	
	netx10gs_helper_log( LOG_DEBUG, "handlexpic_RemoveSoftwareBreakpoint - end" );
}
//==============================================================================


//==============================================================================
// NETX10GS_HANDLEXPIC_SETHARDWAREBREAKPOINT
//==============================================================================
///
/// @brief Sets a hardware Breakpoint.
///
/// @param [in] uiAddress address where to set hardware breakpoint.
///
/// @return Errorcode.
///
//==============================================================================
char netx10gs_handlexpic_SetHardwareBreakpoint( unsigned int uiAddress )
{
	// check hardware breakpoint counter
	if ( iHardwareBreakpointCounter < 0 || iHardwareBreakpointCounter > 1 )
	{
		netx10gs_sockets_Send( SOCKET_XPICGDB, GDB_ERROR_0, "handlexpic_SetHardwareBreakpoint - could not send msg!" );
		return GDB_SWITCH_OK;
	}
	
	// build msg for first hw breakpoint
	if ( iHardwareBreakpointCounter == 0 )
	{
		netx10gs_helper_BuildGdbMsg( buffer, DEFAULT_BUFFER_LEN, XPIC_SET_HBP0_BEGINMSG, NULL, GDB_VALUE_TYPE_GDB, uiAddress, GDB_CALC_CHECKSUM );
	}
	
	// build msg for second hw breakpoint
	if ( iHardwareBreakpointCounter == 1 )
	{
		netx10gs_helper_BuildGdbMsg( buffer, DEFAULT_BUFFER_LEN, XPIC_SET_HBP1_BEGINMSG, NULL, GDB_VALUE_TYPE_GDB, uiAddress, GDB_CALC_CHECKSUM );
	}
	
	// send breakpoint msg to openocd
	netx10gs_sockets_SendAndReceive( SOCKET_OPENOCD, buffer, buffer, DEFAULT_BUFFER_LEN, GDB_DONT_RETURN_ANSWER, INVALID_SOCKET, GDB_WAIT_FOR_OK, "handlexpic_SetHardwareBreakpoint - could not send/recv msg!" );
	
	// enable hw bp 0
	if ( iHardwareBreakpointCounter == 0 )
	{
		netx10gs_sockets_SendAndReceive( SOCKET_OPENOCD, XPIC_ENABLE_HBP0, buffer, DEFAULT_BUFFER_LEN, GDB_DONT_RETURN_ANSWER, INVALID_SOCKET, GDB_WAIT_FOR_OK, "handlexpic_SetHardwareBreakpoint - could not send/recv msg!" );
		uiHardwareBreakpoint1 = uiAddress;
	}
	
	// enable hw bp 1
	if ( iHardwareBreakpointCounter == 1 )
	{
		netx10gs_sockets_SendAndReceive( SOCKET_OPENOCD, XPIC_ENABLE_HBP1, buffer, DEFAULT_BUFFER_LEN, GDB_DONT_RETURN_ANSWER, INVALID_SOCKET, GDB_WAIT_FOR_OK, "handlexpic_SetHardwareBreakpoint - could not send/recv msg!" );
		uiHardwareBreakpoint2 = uiAddress;
	}
	
	// send OK to xpic gdb
	netx10gs_sockets_Send( SOCKET_XPICGDB, GDB_OK, "handlexpic_SetHardwareBreakpoint - could not send msg!" );
	
	// increment hardware breakpoint counter
	iHardwareBreakpointCounter++;
	
	return GDB_SWITCH_OK;
}
//==============================================================================


//==============================================================================
// NETX10GS_HANDLEXPIC_REMOVEHARDWAREBREAKPOINT
//==============================================================================
///
/// @brief Removes a hardware Breakpoint.
///
/// @param [in] uiAddress address where to remove hardware breakpoint.
///
/// @return Errorcode.
///
//==============================================================================
// char netx10gs_handlexpic_RemoveHardwareBreakpoint( unsigned int uiAddress )
char netx10gs_handlexpic_RemoveHardwareBreakpoint( void )
{
	// check hardware breakpoint counter
	if ( iHardwareBreakpointCounter < 1 || iHardwareBreakpointCounter > 2 )
	{
		netx10gs_sockets_Send( SOCKET_XPICGDB, GDB_ERROR_0, "netx10gs_handlexpic_RemoveHardwareBreakpoint - could not send msg!" );
		return GDB_SWITCH_OK; // vorher -1!?
	}
	
	// disable hw bp 0
	netx10gs_sockets_SendAndReceive( SOCKET_OPENOCD, XPIC_DISABLE_HBP0, buffer, DEFAULT_BUFFER_LEN, GDB_DONT_RETURN_ANSWER, INVALID_SOCKET, GDB_WAIT_FOR_OK, "netx10gs_handlexpic_RemoveHardwareBreakpoint - could not send/recv msg!" );
	
	// disable hw bp 1
	netx10gs_sockets_SendAndReceive( SOCKET_OPENOCD, XPIC_DISABLE_HBP1, buffer, DEFAULT_BUFFER_LEN, GDB_DONT_RETURN_ANSWER, INVALID_SOCKET, GDB_WAIT_FOR_OK, "netx10gs_handlexpic_RemoveHardwareBreakpoint - could not send/recv msg!" );
	
	// send OK to xpic gdb
	netx10gs_sockets_Send( SOCKET_XPICGDB, GDB_OK, "handlexpic_RemoveHardwareBreakpoint - could not send msg!" );
	
	// decrement hardware breakpoint counter
	iHardwareBreakpointCounter--;
	
	return GDB_SWITCH_OK;
}
//==============================================================================




//==============================================================================
// NETX10GS_HANDLEXPIC_READSINGLEXPICREGISTER
//==============================================================================
///
/// @brief Reads a single xpic register.
///
/// @param [in] ucRegister which register.
///
/// @return Errorcode.
///
//==============================================================================
char netx10gs_handlexpic_ReadSingleXpicRegister( unsigned char ucRegister )
{
	netx10gs_helper_log( LOG_READSINGLEXPICREGISTER, "handlexpic_ReadSingleXpicRegister - start" );
	
	// iFirstArg in {0x0;0xf}
	// if ( ( ucRegister > 16 ) || ( ucRegister < 0 ) )
	if ( ucRegister > 16 )
	{
		netx10gs_sockets_Send( SOCKET_XPICGDB, GDB_ERROR_0, "handlexpic_ReadSingleXpicRegister - could not send msg!" );
		return GDB_SWITCH_OK;
	}
	
	// read XPIC memory - addresses are XPIC register block
	netx10gs_sockets_SendAndReceive( SOCKET_OPENOCD, apcXpicReadRegister[ ucRegister ], buffer, DEFAULT_BUFFER_LEN, GDB_RETURN_ANSWER, SOCKET_XPICGDB, GDB_DONT_WAIT_FOR_OK, "netx10gs_handlexpic_ReadSingleXpicRegister - could not send/recv msg!" );
	
	netx10gs_helper_log( LOG_READSINGLEXPICREGISTER, "handlexpic_ReadSingleXpicRegister - end" );
	return GDB_SWITCH_OK;
}
//==============================================================================


//==============================================================================
// NETX10GS_HANDLEXPIC_WRITESINGLEXPICREGISTER
//==============================================================================
///
/// @brief Writes a single xpic register.
///
/// @param [in] ucRegister which register.
///
/// @return Errorcode.
///
//==============================================================================
char netx10gs_handlexpic_WriteSingleXpicRegister( unsigned char ucRegister )
{

#undef TEMP_BUFFER_SIZE
// XXX 26?
#define	TEMP_BUFFER_SIZE	26

	char
		acTempBuffer[TEMP_BUFFER_SIZE] = {0};
	
	
	// "> 12" : don't write ZERO and STAT register
	// if ( ( ucRegister > 0xd ) || ( ucRegister < 0 ) )
	if ( ucRegister > 0xd )
	{
		netx10gs_sockets_Send( SOCKET_XPICGDB, GDB_ERROR_0, "handlexpic_WriteSingleXpicRegister - could not send msg!" );
		return GDB_SWITCH_OK;
	}

	buffer[12] = 0;
	if ( netx10gs_helper_BuildGdbMsg( acTempBuffer, TEMP_BUFFER_SIZE, apcXpicWriteRegister[ ucRegister ], &buffer[ 4 ], GDB_VALUE_TYPE_NO_VALUE, 0, GDB_CALC_CHECKSUM )
		!= GDB_SWITCH_OK )
	{
		netx10gs_helper_log( LOG_ERROR, "handlexpic_WriteSingleXpicRegister - could not build msg!" );
		return GDB_SWITCH_ERROR;
	}
	
	// read XPIC memory - addresses are XPIC register block
	netx10gs_sockets_SendAndReceive( SOCKET_OPENOCD, acTempBuffer, buffer, DEFAULT_BUFFER_LEN, GDB_RETURN_ANSWER, SOCKET_XPICGDB, GDB_DONT_WAIT_FOR_OK, "netx10gs_handlexpic_WriteSingleXpicRegister - could not send/recv msg!" );
	
	return GDB_SWITCH_OK;
}
//==============================================================================


//==============================================================================
// NETX10GS_HANDLEXPIC_SINGLESTEPXPIC
//==============================================================================
///
/// @brief Does one singlestep.
///
//==============================================================================
void netx10gs_handlexpic_SinglestepXpic( void )
{
	char
		cDummy = 0;
	
	struct structUiValue_t
		structUiValue;
	
	netx10gs_helper_log( LOG_SINGLESTEPXPIC, "handlexpic_SinglestepXpic - start" );
	
	// set singlestep mode on and start xPIC
	netx10gs_sockets_SendAndReceive( SOCKET_OPENOCD, XPIC_SINGLESTEP, buffer, DEFAULT_BUFFER_LEN, GDB_DONT_RETURN_ANSWER, INVALID_SOCKET, GDB_DONT_WAIT_FOR_OK, "netx10gs_handlexpic_SinglestepXpic - could not send/recv msg!" );
	
	// wait for singlestep be done
	while ( cDummy == cDummy )
	{
		// get break status
		netx10gs_sockets_SendAndReceive( SOCKET_OPENOCD, XPIC_READ_BREAK_STATUS, buffer, DEFAULT_BUFFER_LEN, GDB_DONT_RETURN_ANSWER, INVALID_SOCKET, GDB_DONT_WAIT_FOR_OK, "netx10gs_handlexpic_CheckForSinglestep - could not send/recv msg!" );

		// convert gdb value 2 int
		netx10gs_helper_GdbVal2UnsignedInt( &buffer[1], &structUiValue );
	
		// catch error
		if ( structUiValue.cErrorCode != GDB_SWITCH_OK )
		{
			netx10gs_helper_log( LOG_ERROR, "ERROR:" ); // XXX new msg!
			// return GDB_SWITCH_ERROR; // XXX errorhandling
			break;
		}
		
		netx10gs_helper_log( LOG_SINGLESTEPXPIC, "handlexpic_SinglestepXpic - uiRetval: %x (%i)", structUiValue.uiRetval, structUiValue.uiRetval );
		
		// if singlestep occured break from while loop
		if ( structUiValue.uiRetval & MSK_SW_SINGLESTEP )
		{
			// halt xpic
			netx10gs_handlexpic_HaltXpic();
	
			// reset xpic singlestep irq	
			netx10gs_sockets_SendAndReceive( SOCKET_OPENOCD, XPIC_RESET_IRQ_SSTEP, buffer, DEFAULT_BUFFER_LEN, GDB_DONT_RETURN_ANSWER, INVALID_SOCKET, GDB_DONT_WAIT_FOR_OK, "netx10gs_handlexpic_CheckForSinglestep - could not send/recv msg!" );
			
			break;
		}
	}
	
	netx10gs_helper_log( LOG_SINGLESTEPXPIC, "handlexpic_SinglestepXpic - end" );
}
//==============================================================================


//==============================================================================
// NETX10GS_HANDLEXPIC_CHECKFORSINGLESTEP
//==============================================================================
///
/// @brief Checks if a singlestep occured.
///
/// @param [in] bSendSigtrap if bSendSigTrap is set, SIGTRAP is sended to XpicGDB.
///
/// @return Errorcode.
///
//==============================================================================
char netx10gs_handlexpic_CheckForSinglestep( char *pcSendSigtrap )
{
	// unsigned int
		// uiValue = 0x0;
	
	struct structUiValue_t
		structUiValue;
	
	
	netx10gs_helper_log( LOG_CHECKFORSINGLESTEP, "handlexpic_CheckForSinglestep - start" );
	
	pcSendSigtrap = pcSendSigtrap;
	
	if ( g_bBreakpointHappened == TRUE )
	{
		g_bBreakpointHappened = FALSE;
		netx10gs_handlexpic_StepBackPC( 4 );
	
		netx10gs_handlexpic_SinglestepXpic();
		
		// debug start
		// read xpic pc
		netx10gs_sockets_SendAndReceive( SOCKET_OPENOCD, XPIC_READ_REG_PC, buffer, DEFAULT_BUFFER_LEN, GDB_DONT_RETURN_ANSWER, INVALID_SOCKET, GDB_DONT_WAIT_FOR_OK, "netx10gs_handlexpic_StepBackPC - could not read xpic pc!" );
		netx10gs_helper_GdbVal2UnsignedInt( &buffer[1], &structUiValue );
		netx10gs_helper_log( LOG_CHECKFORSINGLESTEP, "cfss - pc: %x (%i)", structUiValue.uiRetval, structUiValue.uiRetval );
		// debug end
	}

	netx10gs_handlexpic_SinglestepXpic();

	//
	// read xpic pc
	netx10gs_sockets_SendAndReceive( SOCKET_OPENOCD, XPIC_READ_REG_PC, buffer, DEFAULT_BUFFER_LEN, GDB_DONT_RETURN_ANSWER, INVALID_SOCKET, GDB_DONT_WAIT_FOR_OK, "netx10gs_handlexpic_StepBackPC - could not read xpic pc!" );
	netx10gs_helper_GdbVal2UnsignedInt( &buffer[1], &structUiValue );
	netx10gs_helper_log( LOG_STEPBACKPC, "cfss - pc: %x (%i)", structUiValue.uiRetval, structUiValue.uiRetval );
	//	

	// netx10gs_handlexpic_StepBackPC( 0 ); // XXX debug
	
	// HACK stepping over break instruction
	// the gdb removes the breakpoint after it was fetched! We have to rerun this command.
	// This should be corrected in xpic-gdb
	
	// read break status
	netx10gs_sockets_Send( SOCKET_OPENOCD, XPIC_READ_BREAK_STATUS, "netx10gs_handlexpic_CheckForBreakpoint - could not send msg!" );
	netx10gs_sockets_Receive( SOCKET_OPENOCD, buffer, xpicgdb_buflen );
	netx10gs_helper_GdbVal2UnsignedInt( &buffer[1], &structUiValue );
	// catch error
	if ( structUiValue.cErrorCode == GDB_SWITCH_ERROR )
	{
		netx10gs_helper_log( LOG_CHECKFORSINGLESTEP, "handlexpic_CheckForSinglestep - end" );
		return GDB_SWITCH_ERROR;
	}
	// uiValue = structUiValue.uiRetval;
	
	netx10gs_helper_log( LOG_CHECKFORSINGLESTEP, "handlexpic_CheckForSinglestep - uiValue: %x (%i)", structUiValue.uiRetval, structUiValue.uiRetval );
	
	if ( structUiValue.uiRetval & MSK_SW_BREAKPOINT )
	{
		netx10gs_handlexpic_StepBackPC( 8 );
		netx10gs_handlexpic_SinglestepXpic();
		netx10gs_handlexpic_SinglestepXpic();
	}
	
	// // halt xpic
	// netx10gs_handlexpic_HaltXpic();
	
	// // reset xpic singlestep irq	
	// netx10gs_sockets_SendAndReceive( SOCKET_OPENOCD, XPIC_RESET_IRQ_SSTEP, buffer, DEFAULT_BUFFER_LEN, GDB_DONT_RETURN_ANSWER, INVALID_SOCKET, GDB_DONT_WAIT_FOR_OK, "netx10gs_handlexpic_CheckForSinglestep - could not send/recv msg!" );
	
	// send SIGTRAP to GDB
	netx10gs_sockets_Send( SOCKET_XPICGDB, GDB_SIGTRAP, "netx10gs_handlexpic_CheckForSinglestep - could not send msg!" );
	
	// // send SIGTRAP to GDB if bSendSigTrap is TRUE
	// if ( pcSendSigtrap != NULL )
	// {
		// *pcSendSigtrap = TRUE;
	// }
	
	netx10gs_helper_log( LOG_CHECKFORSINGLESTEP, "handlexpic_CheckForSinglestep - end" );
	
	return GDB_SWITCH_OK;
//------------------------------------------------------------------------------------

}
//==============================================================================


//==============================================================================
// NETX10GS_HANDLEXPIC_CHECKFORBREAKPOINT
//==============================================================================
///
/// @brief Checks if a breakpoint occured.
///
/// @return Errorcode.
///
//==============================================================================
char netx10gs_handlexpic_CheckForBreakpoint( void )
{
	char
		cDummy = 0x0;
	
	unsigned int
		uiValue = 0x0;
	
	struct structUiValue_t
		structUiValue;
	
	netx10gs_helper_log( LOG_CHECKFORBREAKPOINT, "handlexpic_CheckForBreakpoint - start" );
	
	// read break status
	netx10gs_sockets_Send( SOCKET_OPENOCD, XPIC_READ_BREAK_STATUS, "netx10gs_handlexpic_CheckForBreakpoint - could not send msg!" );
	
	cDummy = cDummy;
	netx10gs_sockets_Receive( SOCKET_OPENOCD, buffer, xpicgdb_buflen );
	netx10gs_helper_GdbVal2UnsignedInt( &buffer[1], &structUiValue );
	// catch error
	if ( structUiValue.cErrorCode == GDB_SWITCH_ERROR )
	{
		netx10gs_helper_log( LOG_CHECKFORBREAKPOINT, "handlexpic_CheckForBreakpoint - end" );
		return GDB_SWITCH_ERROR;
	}
	uiValue = structUiValue.uiRetval;
	netx10gs_helper_log( LOG_CHECKFORBREAKPOINT, "handlexpic_CheckForBreakpoint - value: %i", uiValue );
	netx10gs_helper_log( LOG_CHECKFORBREAKPOINT, "handlexpic_CheckForBreakpoint - value2: %i",  uiValue & MSK_BREAKPOINTS );
	
	if ( uiValue & MSK_BREAKPOINTS )
	{
		netx10gs_helper_log( LOG_CHECKFORBREAKPOINT, "found breakpoint" );
		
		// halt xpic
		netx10gs_handlexpic_HaltXpic();
		
		if ( uiValue & MSK_SW_BREAKPOINT )
		{
			// reset xpic software break irq
			netx10gs_sockets_SendAndReceive( SOCKET_OPENOCD, XPIC_RESET_IRQ_SBREAK, buffer, DEFAULT_BUFFER_LEN, GDB_DONT_RETURN_ANSWER, INVALID_SOCKET, GDB_DONT_WAIT_FOR_OK, "netx10gs_handlexpic_CheckForBreakpoint - could not send/recv msg!" );
			
			// set pc := pc - 4
			netx10gs_handlexpic_StepBackPC( 4 );
			g_bDoSingleStepAfterBreakpoint = TRUE;
			g_bBreakpointHappened = TRUE;
		}
		
		else
		{
			//
			netx10gs_sockets_SendAndReceive( SOCKET_OPENOCD, XPIC_RESET_IRQ_HBREAK0, buffer, DEFAULT_BUFFER_LEN, GDB_DONT_RETURN_ANSWER, INVALID_SOCKET, GDB_WAIT_FOR_OK, "netx10gs_handlexpic_CheckForBreakpoint - could not send/recv msg!" );

			//
			netx10gs_sockets_SendAndReceive( SOCKET_OPENOCD, XPIC_RESET_IRQ_HBREAK1, buffer, DEFAULT_BUFFER_LEN, GDB_DONT_RETURN_ANSWER, INVALID_SOCKET, GDB_WAIT_FOR_OK, "netx10gs_handlexpic_CheckForBreakpoint - could not send/recv msg!" );
		}
		
		// send GDB SIGTRAP
		netx10gs_sockets_Send( SOCKET_XPICGDB, GDB_SIGTRAP, "netx10gs_handlexpic_CheckForBreakpoint - could not send msg!" );
		
		g_bCheckForBreakpoint = FALSE;
	}
	netx10gs_helper_log( LOG_CHECKFORBREAKPOINT, "handlexpic_CheckForBreakpoint - end" );
	return GDB_SWITCH_OK;
}
//==============================================================================



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// 
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


//==============================================================================
// NETX10GS_HANDLEXPIC_GETANDRETURNACCESSKEY
//==============================================================================
///
/// @brief Gets and returns the access key.
///
/// @return Errorcode.
///
//==============================================================================
char netx10gs_handlexpic_GetAndReturnAccessKey( void )
{
	char
		acTempBuffer[TEMP_BUFFER_SIZE] = {0};
	
	
	netx10gs_helper_log( LOG_DEBUG, "handlexpic_GetAndReturnAccessKey - start" );
	netx10gs_helper_log( LOG_VERBOSE, "getting access key ..." );
	
	// read access key
	netx10gs_sockets_SendAndReceive( SOCKET_OPENOCD, XPIC_READ_ACCESS_KEY, buffer, DEFAULT_BUFFER_LEN, GDB_DONT_RETURN_ANSWER, INVALID_SOCKET, GDB_DONT_WAIT_FOR_OK, "netx10gs_handlexpic_GetAndReturnAccessKey - could not read access key!" );
	
	buffer[9] = 0; // XXX kann weg?
	
	//
	if ( netx10gs_helper_BuildGdbMsg( acTempBuffer, TEMP_BUFFER_SIZE, XPIC_WRITE_ACCESS_KEY, &buffer[1], GDB_VALUE_TYPE_NO_VALUE, 0, GDB_CALC_CHECKSUM )
		!= GDB_SWITCH_OK )
	{
		netx10gs_helper_log( LOG_ERROR, "could not write access key!" );
		return GDB_SWITCH_ERROR;
	}
	
	//
	netx10gs_sockets_SendAndReceive( SOCKET_OPENOCD, acTempBuffer, buffer, DEFAULT_BUFFER_LEN, GDB_DONT_RETURN_ANSWER, INVALID_SOCKET, GDB_DONT_WAIT_FOR_OK, "netx10gs_handlexpic_GetAndReturnAccessKey - could not write access key!" );
	
	netx10gs_helper_log( LOG_VERBOSE, "got and returned access key" );
	netx10gs_helper_log( LOG_DEBUG, "handlexpic_GetAndReturnAccessKey - end" );
	return GDB_SWITCH_OK;
}
//==============================================================================


//==============================================================================
// NETX10GS_HANDLEXPIC_ACTIVATEXPICCLOCK
//==============================================================================
///
/// @brief Activates xpic clock.
///
/// @return Errorcode.
///
//==============================================================================
char netx10gs_handlexpic_ActivateXpicClock( void )
{
	netx10gs_helper_log( LOG_DEBUG, "handlexpic_ActivateXpicClock - start" );
	netx10gs_helper_log( LOG_VERBOSE, "activating xpic clock ..." );
	
	// check XPIC clock
	netx10gs_sockets_SendAndReceive( SOCKET_OPENOCD, XPIC_READ_CLOCK_ENABLE, buffer, DEFAULT_BUFFER_LEN, GDB_DONT_RETURN_ANSWER, INVALID_SOCKET, GDB_DONT_WAIT_FOR_OK, "netx10gs_handlexpic_ActivateXpicClock - could not get xpic clock status!" );
	
	//
	if ( netx10gs_handlexpic_GetAndReturnAccessKey() != GDB_SWITCH_OK )
	{
		return GDB_SWITCH_ERROR;
	}
	
	//
	netx10gs_sockets_SendAndReceive( SOCKET_OPENOCD, XPIC_ENABLE_CLOCK, buffer, DEFAULT_BUFFER_LEN, GDB_DONT_RETURN_ANSWER, INVALID_SOCKET, GDB_DONT_WAIT_FOR_OK, "netx10gs_handlexpic_ActivateXpicClock - could not activate xpic clock!" );
	
	netx10gs_helper_log( LOG_VERBOSE, "xpic clock activated" );
	netx10gs_helper_log( LOG_DEBUG, "handlexpic_ActivateXpicClock - end" );
	return GDB_SWITCH_OK;
}
//==============================================================================


//==============================================================================
// NETX10GS_HANDLEXPIC_RESETXPIC
//==============================================================================
///
/// @brief Resets xpic.
///
/// @return Errorcode.
///
//==============================================================================
char netx10gs_handlexpic_ResetXpic( void )
{
	struct structUiValue_t
		structUiValue;
	
	netx10gs_helper_log( LOG_DEBUG, "handlexpic_ResetXpic - start" );
	
	// halt xpic
	netx10gs_handlexpic_HaltXpic();
	
	// disable xpic VIC
	netx10gs_sockets_SendAndReceive( SOCKET_OPENOCD, XPIC_RESET_VIC_CONFIG, buffer, DEFAULT_BUFFER_LEN, GDB_DONT_RETURN_ANSWER, INVALID_SOCKET, GDB_DONT_WAIT_FOR_OK, "netx10gs_handlexpic_ResetXpic - could not reset vic!" );
	
	// clear all hold reasons
	netx10gs_sockets_SendAndReceive( SOCKET_OPENOCD, XPIC_RESET_IRQ_RAW, buffer, DEFAULT_BUFFER_LEN, GDB_DONT_RETURN_ANSWER, INVALID_SOCKET, GDB_DONT_WAIT_FOR_OK, "netx10gs_handlexpic_ResetXpic - could not reset irq raw!" );
	
	// hold xPIC and request reset
	netx10gs_sockets_SendAndReceive( SOCKET_OPENOCD, XPIC_RESET_HOLD_PC, buffer, DEFAULT_BUFFER_LEN, GDB_DONT_RETURN_ANSWER, INVALID_SOCKET, GDB_DONT_WAIT_FOR_OK, "netx10gs_handlexpic_ResetXpic - could not reset hold pc!" );
	
	// wait for reset to be activated
	do
	{
		netx10gs_sockets_SendAndReceive( SOCKET_OPENOCD, XPIC_READ_BREAK_STATUS, buffer, DEFAULT_BUFFER_LEN, GDB_DONT_RETURN_ANSWER, INVALID_SOCKET, GDB_DONT_WAIT_FOR_OK, "netx10gs_handlexpic_ResetXpic - could not read break status!" );
		
		netx10gs_helper_GdbVal2UnsignedInt( &buffer[1], &structUiValue );
		
		// catch error
		if ( structUiValue.cErrorCode == GDB_SWITCH_ERROR )
		{
			return GDB_SWITCH_ERROR;
		}
		
	} while( ( structUiValue.uiRetval & 0x200 ) == 0 );
	
	// release reset request, engage bank control, select bank 1
	netx10gs_sockets_SendAndReceive( SOCKET_OPENOCD, XPIC_SELECT_BANK1, buffer, DEFAULT_BUFFER_LEN, GDB_DONT_RETURN_ANSWER, INVALID_SOCKET, GDB_DONT_WAIT_FOR_OK, "netx10gs_handlexpic_ResetXpic - could not release reset request!" );
	
	// reset xPIC user registers
	netx10gs_sockets_SendAndReceive( SOCKET_OPENOCD, XPIC_RESET_USER_REGISTER, buffer, DEFAULT_BUFFER_LEN, GDB_DONT_RETURN_ANSWER, INVALID_SOCKET, GDB_DONT_WAIT_FOR_OK, "netx10gs_handlexpic_ResetXpic - could not reset xpic user register!" );
	
	// reset xPIC work registers (bank 1)
	netx10gs_sockets_SendAndReceive( SOCKET_OPENOCD, XPIC_RESET_WORK_REGISTER, buffer, DEFAULT_BUFFER_LEN, GDB_DONT_RETURN_ANSWER, INVALID_SOCKET, GDB_DONT_WAIT_FOR_OK, "netx10gs_handlexpic_ResetXpic - could not reset xpic work register (bank1)!" );
	
	// select bank 0
	netx10gs_sockets_SendAndReceive( SOCKET_OPENOCD, XPIC_SELECT_BANK0, buffer, DEFAULT_BUFFER_LEN, GDB_DONT_RETURN_ANSWER, INVALID_SOCKET, GDB_DONT_WAIT_FOR_OK, "netx10gs_handlexpic_ResetXpic - could not select bank 0!" );
	
	// reset xPIC work registers (bank 0)
	netx10gs_sockets_SendAndReceive( SOCKET_OPENOCD, XPIC_RESET_WORK_REGISTER, buffer, DEFAULT_BUFFER_LEN, GDB_DONT_RETURN_ANSWER, INVALID_SOCKET, GDB_DONT_WAIT_FOR_OK, "netx10gs_handlexpic_ResetXpic - could not reset xpic work register (bank 0)!!" );

	// release bank control
	netx10gs_sockets_SendAndReceive( SOCKET_OPENOCD, XPIC_CMD_STOP, buffer, DEFAULT_BUFFER_LEN, GDB_DONT_RETURN_ANSWER, INVALID_SOCKET, GDB_DONT_WAIT_FOR_OK, "netx10gs_handlexpic_ResetXpic - could not release bank control!" );
	
	netx10gs_helper_log( LOG_DEBUG, "handlexpic_ResetXpic - end" );
	return GDB_SWITCH_OK;
}
//==============================================================================


//==============================================================================
// NETX10GS_HANDLEXPIC_INITNETX10
//==============================================================================
///
/// @brief Initializes xpic.
///
/// @return Errorcode.
///
//==============================================================================
char netx10gs_handlexpic_InitNetx10( void )
{
	netx10gs_helper_log( LOG_INITNETX10, "handlexpic_InitNetx10 - start" );
	netx10gs_helper_log( LOG_VERBOSE, "initialize system ..." );
	
	// halt arm
	netx10gs_handlearm_ControlArm( ARM_HALT );
	
	// // activate xpic clock
	// if ( netx10gs_handlexpic_ActivateXpicClock()
		// != GDB_SWITCH_OK ) { return GDB_SWITCH_ERROR; }
	
	// init stuff //XXX wohin? in reset?
	netx10gs_sockets_SendAndReceive( SOCKET_OPENOCD, XPIC_BREAK0_CONTR_DONTCARE_RANGE_CHAIN, buffer, DEFAULT_BUFFER_LEN, GDB_DONT_RETURN_ANSWER, INVALID_SOCKET, GDB_DONT_WAIT_FOR_OK, "netx10gs_handlexpic_InitNetx10 - could not release bank control!" );
	
	netx10gs_sockets_SendAndReceive( SOCKET_OPENOCD, XPIC_BREAK1_CONTR_DONTCARE_RANGE_CHAIN, buffer, DEFAULT_BUFFER_LEN, GDB_DONT_RETURN_ANSWER, INVALID_SOCKET, GDB_DONT_WAIT_FOR_OK, "netx10gs_handlexpic_InitNetx10 - could not release bank control!" );
	
	// reset xpic
	if ( netx10gs_handlexpic_ResetXpic()
		!= GDB_SWITCH_OK ) { return GDB_SWITCH_ERROR; }
	
	netx10gs_helper_log( LOG_VERBOSE, "system initialized" );
	
	netx10gs_helper_log( LOG_INITNETX10, "handlexpic_InitNetx10 - end" );
	return GDB_SWITCH_OK;
}
//==============================================================================
