/*
 *******************************************************************************
 *                                                                             *
 * netx10gs_handlegdbproto.c                                                   *
 *                                                                             *
 * author: Daniel Kramlinger                                                   *
 *         Hilscher Gesellschaft für Systemautomation mbH                      *
 * date: 14.04.2011                                                            *
 *                                                                             *
 *******************************************************************************
*/



#include "netx10gs_globals.h"
#include "netx10gs_gdbproto.h"
#include "netx10gs_helper.h"
#include "netx10gs_sockets.h"
#include "netx10gs_handlegdbproto.h"
#include "netx10gs_handlearm.h"
#include "netx10gs_handlexpic.h"



char
	cCleanUpArm  = 0x0,
	cCleanUpXpic = 0x0;
	// cWaitForAnswer = 0x0;

BOOLEAN
	bReceiveAgain = FALSE;

//******************************************************************************
// LOCAL FUNCTIONS                                                             *
//******************************************************************************

//==============================================================================
// ARGSBLA
//==============================================================================
#ifdef GDB_SWITCH_DEBUG
char * argsbla( int value )
{
	
	switch ( value)
	{
		case GDB_CMD_ACK:
			return "GDB_CMD_ACK";
			break;
			
		case GDB_CMD_NEGACK:
			return "GDB_CMD_NEGACK";
			break;
		
		case GDB_CMD_CONTINUE:
			return "GDB_CMD_CONTINUE";
			break;
			
		case GDB_CMD_SINGLESTEP:
			return "GDB_CMD_SINGLESTEP";
			break;
		
		case GDB_CMD_CTRL_C:
			return "GDB_CMD_CTRL_C";
			break;
			
		case GDB_CMD_KILL:
			return "GDB_CMD_KILL";
			break;
		
		case GDB_CMD_READ_SINGLE_REG:
			return "GDB_CMD_READ_SINGLE_REG";
			break;
			
		case GDB_CMD_WRITE_SINGLE_REG:
			return "GDB_CMD_WRITE_SINGLE_REG";
			break;
		
		case GDB_CMD_READ_REGISTERS:
			return "GDB_CMD_READ_REGISTERS";
			break;
			
		case GDB_CMD_WRITE_REGISTERS:
			return "GDB_CMD_WRITE_REGISTERS";
			break;
		
		case GDB_CMD_WRITE_MEMORY_X:
			return "GDB_CMD_WRITE_MEMORY_X";
			break;
			
		case GDB_CMD_WRITE_MEMORY:
			return "GDB_CMD_WRITE_MEMORY";
			break;
		
		case GDB_CMD_READ_MEMORY:
			return "GDB_CMD_READ_MEMORY";
			break;
			
		case GDB_CMD_SET_BREAKPOINT:
			return "GDB_CMD_SET_BREAKPOINT";
			break;
		
		case GDB_CMD_UNSET_BREAKPOINT:
			return "GDB_CMD_UNSET_BREAKPOINT";
			break;
		
		case GDB_CMD_q:
			return "GDB_CMD_q";
			break;
		
		case GDB_CMD_qRcmd:
			return "GDB_CMD_qRcmd";
			break;
		
		case GDB_CMD_Q:
			return "GDB_CMD_Q";
			break;
		
		case GDB_CMD_QStartNoAckMode:
			return "GDB_CMD_QStartNoAckMode";
			break;
	}

	return "XXX";
}
#endif
//==============================================================================



//******************************************************************************
// GLOBAL FUNCTIONS                                                            *
//******************************************************************************


//==============================================================================
// NETX10GS_HANDLEGDBPROTO_GETGDBCMD
//==============================================================================
///
/// @brief Analyzes a given string for gdb commands.
///
/// @param [in] pcBuffer string to be analyzed.
/// @param [in/out] pGdbCommand poitner to gdbcommand struct where the command
///					and some other parameters are stored (e.g. memory address).
///
//==============================================================================
void netx10gs_handlegdbproto_GetGdbCmd( char *pcBuffer, struct gdbCommand *pGdbCommand )
{
	int
		i = 0x0,
		iCnt = 0x0;
	
	
	// set values to default
	pGdbCommand->bMsgError = FALSE;
	pGdbCommand->cCmd = GDB_CMD_DEFAULT;
	pGdbCommand->uiPosition = 0; // XXX remove?
	pGdbCommand->iFirstArg = 0;
	pGdbCommand->acSecondArg[0] = 0;
	pGdbCommand->uiSecondArg = 0;
	
	netx10gs_helper_log( LOG_GETGDBCOMMAND, "handlegdbproto_GetGdbCmd - start" );
	netx10gs_helper_log( LOG_GETGDBCOMMAND, "handlegdbproto_GetGdbCmd - Buffer: %s", pcBuffer );
	
	// '-'
	if ( pcBuffer[0] == GDB_MINUS_CHAR )
	{
		pGdbCommand->cCmd = GDB_CMD_NEGACK;
		return;
	}

	// check for gdb SIGINT
	if ( pcBuffer[0] == GDB_CTRL_C_CHAR )
	{
		pGdbCommand->cCmd = GDB_CMD_CTRL_C;
		return;
	}

	// '+' should not occure in 'NoAckMode'!
	if ( pcBuffer[0] == GDB_PLUS_CHAR && pcBuffer[1] == 0 )
	{
		pGdbCommand->cCmd = GDB_CMD_ACK;
		return;
	}
	
	// remove leading ACKs '+' | XXX more than 3?
	for ( i = 0; i < 3; i++ )
	{
		if ( pcBuffer[i] != GDB_PLUS_CHAR )
		{
			break;
		}
	}
	
	// check for gdb msg error
	if ( pcBuffer[i] != GDB_MSG_START_CHAR )
	{
		pGdbCommand->cCmd = GDB_CMD_ERROR;
		return;
	}
	
	//
		
	// check gdb command
	switch ( pcBuffer[ i + 1 ] )
	{
		case '#':
			pGdbCommand->cCmd = GDB_CMD_NOT_SUPPORTED;
			break;
		//--------
		case 'O':
		
			// check for OK
			if ( strcmp( &pcBuffer[i], GDB_OK ) == 0 )
			{
				pGdbCommand->cCmd = GDB_CMD_OK;
				break;
			}
			
			pGdbCommand->cCmd = GDB_CMD_CONSOLE;
			break;
		//--------
		case 'E':
			pGdbCommand->cCmd = GDB_CMD_GDB_ERROR;
			break;
		
		//--------
		case 'c':
			pGdbCommand->cCmd = GDB_CMD_CONTINUE;
			break;
		
		//--------
		case 's':
			pGdbCommand->cCmd = GDB_CMD_SINGLESTEP;
			break;
		
		//--------
		case 'g':
			pGdbCommand->cCmd = GDB_CMD_READ_REGISTERS;
			break;
		
		//--------
		case 'G':
			pGdbCommand->cCmd = GDB_CMD_WRITE_REGISTERS;
			break;
		
		//--------
		case 'm':
			pGdbCommand->cCmd = GDB_CMD_READ_MEMORY;
			
			// read address
			for ( iCnt = ( i + 2 ); iCnt < ( i + 2 + 8 ); iCnt++ )
			{
				if ( pcBuffer[iCnt] == ',' )
				{
					break;
				}
				pGdbCommand->uiSecondArg = ( pGdbCommand->uiSecondArg << 4 );
				pGdbCommand->uiSecondArg += ( netx10gs_helper_Char2Hex( pcBuffer[ iCnt ] ) );
			}
			
			break;
		
		//--------
		case 'M':
			pGdbCommand->cCmd = GDB_CMD_WRITE_MEMORY;
			
			// read address
			for ( iCnt = ( i + 2 ); iCnt < ( i + 2 + 8 ); iCnt++ )
			{
				if ( pcBuffer[iCnt] == ',' )
				{
					break;
				}
				pGdbCommand->uiSecondArg = ( pGdbCommand->uiSecondArg << 4 );
				pGdbCommand->uiSecondArg += ( netx10gs_helper_Char2Hex( pcBuffer[ iCnt ] ) );
			}
			
			i = iCnt;

			// read value
			for ( iCnt = 0; iCnt < 8 ; iCnt++ )
			{
				pGdbCommand->acSecondArg[ iCnt ] = pcBuffer[ iCnt + i + 3 ];
			}			
			
			pGdbCommand->acSecondArg[ iCnt ] = 0x0;
			
			break;
		
		//--------
		case 'k':
			pGdbCommand->cCmd = GDB_CMD_KILL;
			break;
		
		//--------
		case 'p':
			pGdbCommand->cCmd = GDB_CMD_READ_SINGLE_REG;
			pGdbCommand->iFirstArg = netx10gs_helper_Char2Hex( pcBuffer[ i + 2 ] );
			break;
		
		//--------
		case 'P':
			pGdbCommand->cCmd = GDB_CMD_WRITE_SINGLE_REG;
			pGdbCommand->iFirstArg = netx10gs_helper_Char2Hex( pcBuffer[ i + 2 ] );
			break;
		
		//--------
		case 'X':
			pGdbCommand->cCmd = GDB_CMD_WRITE_MEMORY_X;
			break;

		//--------
		case 'S':
			pGdbCommand->cCmd = GDB_CMD_S;
			break;
		
		//--------
		case 'T':
			pGdbCommand->cCmd = GDB_CMD_T;
			break;
		
		//--------
		case 'q':
			pGdbCommand->cCmd = GDB_CMD_q;
			
			// check for 'qSupported'
			if ( strncmp( &pcBuffer[ i + 2 ], "Supported", strlen( "Supported" ) ) == 0 )
			{
				pGdbCommand->cCmd = GDB_CMD_qSupported;
			}
			
			// check for 'remote command' (Rcmd)
			if ( strncmp( &pcBuffer[ i + 2 ], GDB_MONITOR_STRING, strlen( GDB_MONITOR_STRING ) ) == 0 )
			{
				pGdbCommand->cCmd = GDB_CMD_qRcmd;
				if ( strncmp( &pcBuffer[ i + 7 ], GDB_MONITOR_CMD_RESET_STRING , strlen( GDB_MONITOR_CMD_RESET_STRING ) ) == 0 )
				{
					pGdbCommand->uiSecondArg = GDB_MONITOR_CMD_RESET;
				}
				else if ( strncmp( &pcBuffer[ i + 7 ], GDB_MONITOR_CMD_SHOWREGS_STRING , strlen( GDB_MONITOR_CMD_SHOWREGS_STRING ) ) == 0 )
				{
					pGdbCommand->uiSecondArg = GDB_MONITOR_CMD_SHOWREGS;
				}
				else if ( strncmp( &pcBuffer[ i + 7 ], GDB_MONITOR_CMD_EXITSWITCH_STRING , strlen( GDB_MONITOR_CMD_EXITSWITCH_STRING ) ) == 0 )
				{
					pGdbCommand->uiSecondArg = GDB_MONITOR_CMD_EXITSWITCH;
				}
				else if ( strncmp( &pcBuffer[ i + 7 ], GDB_MONITOR_CMD_INIT_STRING , strlen( GDB_MONITOR_CMD_INIT_STRING ) ) == 0 )
				{
					pGdbCommand->uiSecondArg = GDB_MONITOR_CMD_INIT;
				}
			}
			
			break;
		
		//--------
		case 'Q':
			pGdbCommand->cCmd = GDB_CMD_Q;
			
			// check for 'start no ack mode' (StartNoAckMode)
			if ( strncmp( &pcBuffer[ i + 2 ], "StartNoAckMode", strlen( "StartNoAckMode" ) ) == 0 )
			{
				pGdbCommand->cCmd = GDB_CMD_QStartNoAckMode;
			}
			
			break;
		
		//--------
		case 'z':
			switch ( netx10gs_helper_Char2Hex( pcBuffer[ i + 2 ] ) )
			{
				case 0:
					pGdbCommand->cCmd = GDB_CMD_UNSET_BREAKPOINT;
					break;
				
				case 1:
					pGdbCommand->cCmd = GDB_CMD_UNSET_HW_BREAKPOINT;
					break;
				
				case 2:
					pGdbCommand->cCmd = GDB_CMD_UNSET_WRITE_WATCHPOINT;
					break;

				case 3:
					pGdbCommand->cCmd = GDB_CMD_UNSET_READ_WATCHPOINT;
					break;
					
				case 4:
					pGdbCommand->cCmd = GDB_CMD_UNSET_ACCESS_WATCHPOINT;
					break;
				
				default:
					pGdbCommand->cCmd = GDB_CMD_ERROR;
					break;
			}
			
			for ( iCnt = ( i + 4 ); iCnt < ( i + 4 + 8 ); iCnt++ )
			{
				if ( pcBuffer[iCnt] == ',' )
				{
					break;
				}
				pGdbCommand->uiSecondArg = ( pGdbCommand->uiSecondArg << 4 );
				pGdbCommand->uiSecondArg += ( netx10gs_helper_Char2Hex( pcBuffer[iCnt] ) );
			}
			
			break;
		
		//--------
		case 'Z':
			switch ( netx10gs_helper_Char2Hex( pcBuffer[ i + 2 ] ) )
			{
				case 0:
					pGdbCommand->cCmd = GDB_CMD_SET_BREAKPOINT;
					break;
				
				case 1:
					pGdbCommand->cCmd = GDB_CMD_SET_HW_BREAKPOINT;
					break;
				
				case 2:
					pGdbCommand->cCmd = GDB_CMD_SET_WRITE_WATCHPOINT;
					break;

				case 3:
					pGdbCommand->cCmd = GDB_CMD_SET_READ_WATCHPOINT;
					break;

				case 4:
					pGdbCommand->cCmd = GDB_CMD_SET_ACCESS_WATCHPOINT;
					break;
				
				default:
					pGdbCommand->cCmd = GDB_CMD_ERROR;
					break;
			}
			
			for ( iCnt = ( i + 4 ); iCnt < ( i + 4 + 8 ); iCnt++ )
			{
				if ( pcBuffer[iCnt] == ',' )
				{
					break;
				}
				pGdbCommand->uiSecondArg = ( pGdbCommand->uiSecondArg << 4 );
				pGdbCommand->uiSecondArg += ( netx10gs_helper_Char2Hex( pcBuffer[iCnt] ) );
			}
			
			break;
		
		//--------
		default:
			// XXX what to do?
			// pGdbCommand->cCmd = GDB_CMD_DEFAULT;
			// pcBuffer[ i + 1 ]
			break;
	}
	
	netx10gs_helper_log( LOG_GETGDBCOMMAND, "handlegdbproto_GetGdbCmd - end" );
	return;
}
//==============================================================================


//==============================================================================
// NETX10GS_HANDLEGDBPROTO_HANDLEXPICCOMMAND
//==============================================================================
///
/// @brief Handles a xpic-gdb command.
///
/// @return	Errorcode.
///
//==============================================================================
char netx10gs_handlegdbproto_HandleXpicCommand( void )
{
	#undef TEMP_BUFFER_SIZE
	#define TEMP_BUFFER_SIZE 12
	
	char
		acTempBuffer[TEMP_BUFFER_SIZE];
	
	struct gdbCommand structGdbCommand;
	
	
	netx10gs_handlegdbproto_GetGdbCmd( buffer, &structGdbCommand );
	
	switch ( structGdbCommand.cCmd )
	{
		//----------------------------------------------------------------------
		case GDB_CMD_ACK:
		
			bReceiveAgain = TRUE;			
			break;
		
		//----------------------------------------------------------------------
		case GDB_CMD_CONTINUE:
			
			// resume xpic
			g_bCheckForBreakpoint = TRUE;
			
			if ( g_bBreakpointHappened == TRUE )
			{
				g_bBreakpointHappened = FALSE;
				netx10gs_handlexpic_StepBackPC( 4 );
			}
			
			netx10gs_handlexpic_ResumeXpic();
			break;
		
		//----------------------------------------------------------------------
		case GDB_CMD_SINGLESTEP:
			
			// activate singlestep checking for xpic
			// g_bCheckForSinglestep = TRUE;
			netx10gs_handlexpic_CheckForSinglestep( NULL );
			break;
		
		//----------------------------------------------------------------------
		case GDB_CMD_CTRL_C:
		
			// halt xpic
			netx10gs_handlexpic_HaltXpic();
			
			// send SIGINT to xpicgdb
			netx10gs_sockets_Send( SOCKET_XPICGDB, GDB_SIGINT, "netx10gs_handlegdbproto_HandleXpicCommand - could not send SIGINT!" );
			
			// deactivate singlestep checking for xpic
			g_bCheckForSinglestep = FALSE; // XXX needed?
			
			// deactivate breakpoint checking for xpic
			g_bCheckForBreakpoint = FALSE;
			
			break;
		
		//----------------------------------------------------------------------
		case GDB_CMD_KILL:
		
			// 
			g_bNoAckModeStartedXpic = FALSE;			
			// EnterCriticalSection( &criticalSectionXpicgdb );
				cCleanUpXpic = 0x1;
			// LeaveCriticalSection( &criticalSectionXpicgdb );
			
			break;
		
		//----------------------------------------------------------------------
		case GDB_CMD_q:
			
			// 
			netx10gs_sockets_SendAndReceive( SOCKET_OPENOCD, buffer, buffer, DEFAULT_BUFFER_LEN, GDB_RETURN_ANSWER, SOCKET_XPICGDB, GDB_DONT_WAIT_FOR_OK, "error!" );
				
			break;
		
		// //----------------------------------------------------------------------
		// case GDB_CMD_qSupported:
				// break;
		
		//----------------------------------------------------------------------
		case GDB_CMD_qRcmd:
		
			switch ( structGdbCommand.uiSecondArg )
			{
				case GDB_MONITOR_CMD_INIT:
					
					// sending msg "init xpic"
					if ( netx10gs_helper_BuildGdbMsg( buffer, DEFAULT_BUFFER_LEN, GDB_MONITOR_START_MSG, GDB_MONITOR_MSG_INIT_XPIC, GDB_VALUE_TYPE_MONITOR, 0, GDB_CALC_CHECKSUM )
						!= GDB_SWITCH_OK )
					{
						return GDB_SWITCH_ERROR;
					}
					netx10gs_sockets_Send( SOCKET_XPICGDB, buffer, "netx10gs_handlegdbproto_HandleXpicCommand - could not send msg!" );
					
					// initializing xpic
					if ( netx10gs_handlexpic_InitNetx10() != GDB_SWITCH_OK )
					{
						return GDB_SWITCH_ERROR;
					}
					
					// sending msg "init done"
					if ( netx10gs_helper_BuildGdbMsg( buffer, DEFAULT_BUFFER_LEN, GDB_MONITOR_START_MSG, GDB_MONITOR_MSG_DONE, GDB_VALUE_TYPE_MONITOR, 0, GDB_CALC_CHECKSUM )
						!= GDB_SWITCH_OK )
					{
						return GDB_SWITCH_ERROR;
					}
					netx10gs_sockets_Send( SOCKET_XPICGDB, buffer, "netx10gs_handlegdbproto_HandleXpicCommand - could not send msg!" );
					
					break;
				
				//--------------------------------------
				case GDB_MONITOR_CMD_RESET:
				
					// sending msg "reset xpic"
					if ( netx10gs_helper_BuildGdbMsg( buffer, DEFAULT_BUFFER_LEN, GDB_MONITOR_START_MSG, GDB_MONITOR_MSG_RESETTING_XPIC, GDB_VALUE_TYPE_MONITOR, 0, GDB_CALC_CHECKSUM )
						!= GDB_SWITCH_OK )
					{
						return GDB_SWITCH_ERROR;
					}
					netx10gs_sockets_Send( SOCKET_XPICGDB, buffer, "netx10gs_handlegdbproto_HandleXpicCommand - could not send msg!" );
					
					// reset xpic
					if ( netx10gs_handlexpic_ResetXpic() != GDB_SWITCH_OK )
					{
						return GDB_SWITCH_ERROR;
					}
					
					// sending msg "reset done"
					if ( netx10gs_helper_BuildGdbMsg( buffer, DEFAULT_BUFFER_LEN, GDB_MONITOR_START_MSG, GDB_MONITOR_MSG_DONE, GDB_VALUE_TYPE_MONITOR, 0, GDB_CALC_CHECKSUM )
						!= GDB_SWITCH_OK )
					{
						return GDB_SWITCH_ERROR;
					}
					netx10gs_sockets_Send( SOCKET_XPICGDB, buffer, "netx10gs_handlegdbproto_HandleXpicCommand - could not send msg!" );
					
					break;
				
				//--------------------------------------
				// case GDB_MONITOR_CMD_SHOWREGS:
				
					// //
					// if ( netx10gs_handlexpic_ReadXpicRegisterBothBanks()
						// != GDB_SWITCH_OK ) { return GDB_SWITCH_ERROR; }
					
					// break;
					
				//--------------------------------------
				case GDB_MONITOR_CMD_EXITSWITCH:
					
					// set global bit for leaving switch true
					EnterCriticalSection( &criticalSectionGdbSwitch );
						g_bExitGdbSwitch = TRUE;
					LeaveCriticalSection( &criticalSectionGdbSwitch );
					break;
					
				//--------------------------------------
				default:
					
					// sending msg "invalid command"
					if ( netx10gs_helper_BuildGdbMsg( buffer, DEFAULT_BUFFER_LEN, GDB_MONITOR_START_MSG, GDB_MONITOR_MSG_INVALID_COMMAND, GDB_VALUE_TYPE_MONITOR, 0, GDB_CALC_CHECKSUM )
						!= GDB_SWITCH_OK )
					{
						return GDB_SWITCH_ERROR;
					}
					netx10gs_sockets_Send( SOCKET_XPICGDB, buffer, "netx10gs_handlegdbproto_HandleXpicCommand - could not send msg!" );
					
					break;
			}
			
			// send ok
			netx10gs_sockets_Send( SOCKET_XPICGDB, GDB_OK, "netx10gs_handlegdbproto_HandleXpicCommand - could not send OK!" );
			
			break;
		
		//----------------------------------------------------------------------
		case GDB_CMD_QStartNoAckMode:
		
			//
			netx10gs_sockets_SendAndReceive( SOCKET_OPENOCD, buffer, buffer, DEFAULT_BUFFER_LEN, GDB_RETURN_ANSWER, SOCKET_XPICGDB, GDB_DONT_WAIT_FOR_OK, "error!" );
			
			// check if no ack mode is started
			netx10gs_handlegdbproto_GetGdbCmd( buffer, &structGdbCommand );
			
			// if ( iReturnValue == GDB_SWITCH_OK )
			if ( structGdbCommand.cCmd == GDB_CMD_OK )
			{
				g_bNoAckModeStartedXpic = TRUE;
				g_bNoAckModeStartedOpenocd = TRUE;
			}
			break;
		
		//----------------------------------------------------------------------
		case GDB_CMD_READ_REGISTERS:
			
			// read xpic registers
			if ( netx10gs_handlexpic_ReadXpicRegisters() != GDB_SWITCH_OK )
			{
				return GDB_SWITCH_ERROR;
			}
			break;
		
		//----------------------------------------------------------------------
		case GDB_CMD_WRITE_REGISTERS:
			
			// send command unknown
			netx10gs_sockets_Send( SOCKET_XPICGDB, GDB_UNKWON, "netx10gs_handlegdbproto_HandleXpicCommand - could not send msg!" );
			break;
			
		//----------------------------------------------------------------------
		case GDB_CMD_READ_SINGLE_REG:
		
			// read a single xpic register
			if ( netx10gs_handlexpic_ReadSingleXpicRegister( (unsigned char) structGdbCommand.iFirstArg ) != GDB_SWITCH_OK )
			{
				return GDB_SWITCH_ERROR;
			}
			break;
		
		//----------------------------------------------------------------------
		case GDB_CMD_WRITE_SINGLE_REG:
		
			// write a single xpic register, which one is saved in global variable structGdbCommand
			if ( netx10gs_handlexpic_WriteSingleXpicRegister( (unsigned char) structGdbCommand.iFirstArg ) != GDB_SWITCH_OK )
			{
				return GDB_SWITCH_ERROR;
			}
			break;
		
		//----------------------------------------------------------------------
		case GDB_CMD_WRITE_MEMORY_X:
		
			// send msg
			netx10gs_sockets_Send( SOCKET_XPICGDB, GDB_UNKWON, "netx10gs_handlegdbproto_HandleXpicCommand - could not send msg!" );
			break;
		
		//----------------------------------------------------------------------
		case GDB_CMD_READ_MEMORY:
		
			// workaround: 
			if ( structGdbCommand.uiSecondArg < XPIC_PRAM_SIZE )
			{
				structGdbCommand.uiSecondArg += XPIC_PRAM_STARTADDRESS;
			}
			
			// build msg
			if ( netx10gs_helper_BuildGdbMsg( buffer, DEFAULT_BUFFER_LEN, XPIC_READ_MEMORY_STARTMSG, XPIC_READ_MEMORY_ENDMSG, GDB_VALUE_TYPE_NORMAL, structGdbCommand.uiSecondArg, GDB_CALC_CHECKSUM )
				!= GDB_SWITCH_OK ) { return GDB_SWITCH_ERROR; }
			
			// send msg
			netx10gs_sockets_SendAndReceive( SOCKET_OPENOCD, buffer, buffer, DEFAULT_BUFFER_LEN, GDB_RETURN_ANSWER, SOCKET_XPICGDB, GDB_DONT_WAIT_FOR_OK, "error!" );
			break;
		
		//----------------------------------------------------------------------
		case GDB_CMD_WRITE_MEMORY:
		
			// workaround:
			if ( structGdbCommand.uiSecondArg < XPIC_PRAM_SIZE )
			{
				structGdbCommand.uiSecondArg += XPIC_PRAM_STARTADDRESS;
				strcpy_s( acTempBuffer, TEMP_BUFFER_SIZE, ",4:");
				strcat_s( acTempBuffer, TEMP_BUFFER_SIZE, structGdbCommand.acSecondArg );
			
				// build msg
				if ( netx10gs_helper_BuildGdbMsg( buffer, DEFAULT_BUFFER_LEN, XPIC_WRITE_MEMORY_STARTMSG, acTempBuffer, GDB_VALUE_TYPE_NORMAL, structGdbCommand.uiSecondArg, GDB_CALC_CHECKSUM )
					!= GDB_SWITCH_OK ) { return GDB_SWITCH_ERROR; }
			}
			
			// send msg
			netx10gs_sockets_SendAndReceive( SOCKET_OPENOCD, buffer, buffer, DEFAULT_BUFFER_LEN, GDB_RETURN_ANSWER, SOCKET_XPICGDB, GDB_DONT_WAIT_FOR_OK, "error!" );
			break;
		
		//----------------------------------------------------------------------
		// remove breakpoints - hw and software
		//----------------------------------------------------------------------
		case GDB_CMD_UNSET_BREAKPOINT:
		
			// remove software breakpoint in xpic code
			netx10gs_handlexpic_RemoveSoftwareBreakpoint( structGdbCommand.uiSecondArg );
			break;
		
		//----------------------------------------------------------------------
		case GDB_CMD_UNSET_HW_BREAKPOINT:
		
			// remove hardware breakpoint in xpic code
			if ( netx10gs_handlexpic_RemoveHardwareBreakpoint()
				!= GDB_SWITCH_OK ) { return GDB_SWITCH_ERROR; }
			
			break;
		
		//----------------------------------------------------------------------
		case GDB_CMD_UNSET_WRITE_WATCHPOINT:
		case GDB_CMD_UNSET_READ_WATCHPOINT:
		case GDB_CMD_UNSET_ACCESS_WATCHPOINT:
		
			// send GDB_UNKWON to xpicgdb
			netx10gs_sockets_Send( SOCKET_XPICGDB, GDB_UNKWON, "netx10gs_handlegdbproto_HandleXpicCommand - could not send msg!" );
			break;
		
		//----------------------------------------------------------------------
		// set breakpoints - hw and software
		//----------------------------------------------------------------------
		case GDB_CMD_SET_BREAKPOINT:
			netx10gs_handlexpic_SetSoftwareBreakpoint( structGdbCommand.uiSecondArg );			
			break;
		
		//----------------------------------------------------------------------
		case GDB_CMD_SET_HW_BREAKPOINT:
			netx10gs_handlexpic_SetHardwareBreakpoint( structGdbCommand.uiSecondArg );
			break;
		
		//----------------------------------------------------------------------
		case GDB_CMD_SET_WRITE_WATCHPOINT:
		case GDB_CMD_SET_READ_WATCHPOINT:
		case GDB_CMD_SET_ACCESS_WATCHPOINT:
			
			// send GDB_UNKWON to xpicgdb
			netx10gs_sockets_Send( SOCKET_XPICGDB, GDB_UNKWON, "netx10gs_handlegdbproto_HandleXpicCommand - could not send msg!" );
			break;
		
		//----------------------------------------------------------------------
		default:
		
			// sending msg in buffer to xpicgdb
			netx10gs_sockets_SendAndReceive( SOCKET_OPENOCD, buffer, buffer, DEFAULT_BUFFER_LEN, GDB_RETURN_ANSWER, SOCKET_XPICGDB, GDB_DONT_WAIT_FOR_OK, "error!" );
			break;
	}
	
	bReceiveAgain = FALSE;
	return GDB_SWITCH_OK;
}
//==============================================================================


//==============================================================================
// netx10gs_handlegdbproto_HandleArmCommand
//==============================================================================
///
/// @brief Handles a arm-gdb command.
///
/// @return	Errorcode.
///
//==============================================================================
char netx10gs_handlegdbproto_HandleArmCommand( void )
{
	struct gdbCommand structGdbCommand;
	
	
	netx10gs_handlegdbproto_GetGdbCmd( buffer, &structGdbCommand );
	// cWaitForAnswer = 0x0;
	
	switch ( structGdbCommand.cCmd )
	{
		case GDB_CMD_ACK:
		
			// send ACK
			netx10gs_sockets_Send( SOCKET_OPENOCD, buffer, "netx10gs_handlegdbproto_HandleArmCommand - could not send msg!" );
			
			//
			bReceiveAgain = TRUE;
			
			break;
		
		//----------------------------------------------------------------------
		case GDB_CMD_CONTINUE:
		
			//
			g_bCheckForArmBreakpoint = TRUE;

			// send CONTINUE
			netx10gs_sockets_Send( SOCKET_OPENOCD, buffer, "netx10gs_handlegdbproto_HandleArmCommand - could not send msg!" );
			// cWaitForAnswer = 0x1;
			break;
		
		//----------------------------------------------------------------------
		case GDB_CMD_CTRL_C:
			
			g_bCheckForArmBreakpoint = FALSE;
			g_bArmRunnig = FALSE;

			netx10gs_sockets_Send( SOCKET_OPENOCD, GDB_CTRL_C, "netx10gs_handlegdbproto_HandleArmCommand - could not send msg!" );
			netx10gs_sockets_Send( SOCKET_ARMGDB, GDB_SIGINT, "netx10gs_handlegdbproto_HandleArmCommand - could not send msg!" );
			
			break;
		
		//----------------------------------------------------------------------
		case GDB_CMD_KILL:
			
			g_bNoAckModeStartedArm = FALSE;
			
			// EnterCriticalSection( &criticalSectionXpicgdb );
				cCleanUpArm = 0x1;
			// LeaveCriticalSection( &criticalSectionXpicgdb );
			
			break;
			
		//----------------------------------------------------------------------
		case GDB_CMD_READ_REGISTERS:
		
			testarm = 0x1;
			
			netx10gs_sockets_SendAndReceive( SOCKET_OPENOCD, buffer, buffer, DEFAULT_BUFFER_LEN, GDB_RETURN_ANSWER, SOCKET_ARMGDB, GDB_DONT_WAIT_FOR_OK, "error!" );
			
			break;
		
		//----------------------------------------------------------------------
		case GDB_CMD_q:
		
			//
			netx10gs_sockets_SendAndReceive( SOCKET_OPENOCD, buffer, buffer, DEFAULT_BUFFER_LEN, GDB_RETURN_ANSWER, SOCKET_ARMGDB, GDB_DONT_WAIT_FOR_OK, "error!" );
			
			break;
		
		//----------------------------------------------------------------------
		case GDB_CMD_qRcmd:
		
			switch ( structGdbCommand.uiSecondArg )
			{
				case GDB_MONITOR_CMD_EXITSWITCH:
					
					EnterCriticalSection( &criticalSectionGdbSwitch );
						g_bExitGdbSwitch = TRUE;
					LeaveCriticalSection( &criticalSectionGdbSwitch );
					break;
					
				default:
				
					netx10gs_sockets_SendAndReceive( SOCKET_OPENOCD, buffer, buffer, DEFAULT_BUFFER_LEN, GDB_RETURN_ANSWER, SOCKET_ARMGDB, GDB_WAIT_FOR_OK, "error!" );
					
					break;
			}
			
			break;
		
		//----------------------------------------------------------------------
		case GDB_CMD_QStartNoAckMode:

			//
			netx10gs_sockets_SendAndReceive( SOCKET_OPENOCD, buffer, buffer, DEFAULT_BUFFER_LEN, GDB_RETURN_ANSWER, SOCKET_ARMGDB, GDB_DONT_WAIT_FOR_OK, "error!" );
			
			// check if no ack mode is started
			netx10gs_handlegdbproto_GetGdbCmd( buffer, &structGdbCommand );
			
			if ( structGdbCommand.cCmd == GDB_CMD_OK )
			{
				g_bNoAckModeStartedArm = TRUE;
				g_bNoAckModeStartedOpenocd = TRUE;
			}
			
			break;
		
		//----------------------------------------------------------------------
		case GDB_CMD_WRITE_MEMORY_X:
		
			// send GDB_UNKWON to armgdb
			netx10gs_sockets_Send( SOCKET_ARMGDB, GDB_UNKWON, "netx10gs_handlegdbproto_HandleArmCommand - could not send msg!" );
			
			break;
		
		//----------------------------------------------------------------------
		default:
		
			netx10gs_sockets_SendAndReceive( SOCKET_OPENOCD, buffer, buffer, DEFAULT_BUFFER_LEN, GDB_RETURN_ANSWER, SOCKET_ARMGDB, GDB_DONT_WAIT_FOR_OK, "error!" );
			
			break;
	}
	
	bReceiveAgain = FALSE;
	return GDB_SWITCH_OK;
}
//==============================================================================


//==============================================================================
// NETX10GS_HANDLEGDBPROTO_XPIC
//==============================================================================
///
/// @brief Handles the xpic-gdb protocol.
///
/// @return	Errorcode.
///
//==============================================================================
char netx10gs_handlegdbproto_Xpic( void )
{
	char
		// cSendSigtrap = 0x0,
		cErrorValue = 0x0;
	
	// struct structUiValue_t
		// structUiValue;
	
	
	testarm = 0x0;
	bReceiveAgain = FALSE;
	
	netx10gs_helper_log( LOG_XPIC, "handlegdbproto_Xpic - start" );
	
	// get xpicGDB command and handle it
	do
	{
		cErrorValue = netx10gs_sockets_Receive( SOCKET_XPICGDB, buffer, DEFAULT_BUFFER_LEN );
		if ( cErrorValue == GDB_SWITCH_NOTHING_RECEIVED )
		{
			// return GDB_SWITCH_OK;
			break;
		}
		if ( cErrorValue != GDB_SWITCH_OK )
		{
			return GDB_SWITCH_ERROR; // dont remove cErrorValue! needed for returning GDB_SWITCH_NOTHING_RECEIVED without quitting program!
		}
		netx10gs_helper_log( LOG_XPIC, "handlegdbproto_Xpic - cmd received: %s", buffer );
		
		bReceiveAgain = netx10gs_handlegdbproto_HandleXpicCommand();

	} while ( bReceiveAgain );
	
	if ( cCleanUpXpic == 0x0 )
	{
		// check for breakpoints (xpic)
		if ( g_bCheckForBreakpoint )
		{
			netx10gs_handlexpic_CheckForBreakpoint();
		}
	}
	else
	{
		netx10gs_sockets_CleanUpXpic();
		cCleanUpXpic = 0x0;
	}
	
	netx10gs_helper_log( LOG_XPIC, "handlegdbproto_Xpic - end" );
	return GDB_SWITCH_OK;
}


//==============================================================================
// NETX10GS_HANDLEGDBPROTO_ARM
//==============================================================================
///
/// @brief Handles the arm-gdb protocol.
///
/// @return	Errorcode.
///
//==============================================================================
char netx10gs_handlegdbproto_Arm( void )
{
	g_bSinglemode = FALSE;
	bReceiveAgain = FALSE;
	
	netx10gs_helper_log( LOG_ARM, "handlegdbproto_Arm - start" );
	
	do
	{
		// if received something
		if ( netx10gs_sockets_Receive( SOCKET_ARMGDB, buffer, DEFAULT_BUFFER_LEN ) == GDB_SWITCH_OK )
		{
			bReceiveAgain = netx10gs_handlegdbproto_HandleArmCommand();
		}
	} while ( bReceiveAgain );
	
	if ( cCleanUpArm == 0x1 )
	{
		netx10gs_sockets_CleanUpArm();
		cCleanUpArm = 0x0;
	}
	netx10gs_helper_log( LOG_ARM, "handlegdbproto_Arm - end" );
	return GDB_SWITCH_OK;
}
//==============================================================================
