/*
 *******************************************************************************
 *                                                                             *
 * netx10gs_handlearm.c                                                        *
 *                                                                             *
 * author: Daniel Kramlinger                                                   *
 *         Hilscher Gesellschaft für Systemautomation mbH                      *
 * date: 14.04.2011                                                            *
 *                                                                             *
 *******************************************************************************
*/


#include "netx10gs_globals.h"
#include "netx10gs_gdbproto.h"
#include "netx10gs_sockets.h"
#include "netx10gs_helper.h"


//==============================================================================
// NETX10GS_HANDLEARM_CONTROLARM
//==============================================================================
///
/// @brief Starts and stops the arm.
///
/// @param [in] cCommand the command.
///
//==============================================================================
void netx10gs_handlearm_ControlArm( char cCommand )
{
	// halt arm
	if ( cCommand == ARM_HALT )
	{
		netx10gs_sockets_SendAndReceive( SOCKET_OPENOCD, OPENOCD_MONITOR_HALT, temp_buffer, DEFAULT_BUFFER_LEN, GDB_DONT_RETURN_ANSWER, INVALID_SOCKET, GDB_WAIT_FOR_OK, "netx10gs_handlearm_ControlArm - could not start/stop arm!" );
	}
	// resume arm
	else if ( cCommand == ARM_RESUME )
	{
		netx10gs_sockets_SendAndReceive( SOCKET_OPENOCD, OPENOCD_MONITOR_RESUME, temp_buffer, DEFAULT_BUFFER_LEN, GDB_DONT_RETURN_ANSWER, INVALID_SOCKET, GDB_WAIT_FOR_OK, "netx10gs_handlearm_ControlArm - could not start/stop arm!" );
	}
}
//==============================================================================


//==============================================================================
// NETX10GS_HANDLEARM_CHECKARMRUNNING
//==============================================================================
///
/// @brief	Checks if arm is running or stopped
///
/// @return	Errorcode.
///
//==============================================================================
char netx10gs_handlearm_CheckArmRunning( void )
{
	int
		i = 0;

	
	// send "monitor poll" to openocd
	netx10gs_sockets_Send( SOCKET_OPENOCD, OPENOCD_MONITOR_POLL, "netx10gs_handlearm_CheckArmRunning - could not send msg!" );
	
	while ( i == i )
	{
		// receive answer
		if ( netx10gs_sockets_Receive( SOCKET_OPENOCD, temp_buffer, DEFAULT_BUFFER_LEN ) != GDB_SWITCH_OK )
		{
			return GDB_SWITCH_ERROR;
		}
		netx10gs_helper_log( LOG_CHECKARMRUNNING, "handlearm_CheckArmRunning - buffer: %s", temp_buffer );
		
		// check for "target state: halted"
		if ( netx10gs_helper_GdbCmp( temp_buffer, GDB_ARM_HALTED ) )
		{
			g_bArmRunnig = FALSE;
		}
		
		// check for "target state: running"
		if ( netx10gs_helper_GdbCmp( temp_buffer, GDB_ARM_RUNNING ) )
		{
			g_bArmRunnig = TRUE;
		}

		// check for OK
		if ( netx10gs_helper_GdbCmp( temp_buffer, GDB_OK ) )
		{
			break;
		}
		
		// if no GDB_OK was found return ERROR
		i++;
		if ( i > 9 )
		{
			return GDB_SWITCH_ERROR;
		}
	}
		
	return GDB_SWITCH_OK;
}
//==============================================================================
