/*
 *******************************************************************************
 *                                                                             *
 * netx10gs_threads.c                                                          *
 *                                                                             *
 * author: Daniel Kramlinger                                                   *
 *         Hilscher Gesellschaft für Systemautomation mbH                      *
 * date: 13.04.2011                                                            *
 *                                                                             *
 *******************************************************************************
*/

#ifdef GDB_SWITCH_LINUX
	#include <unistd.h>
#endif

#include "netx10gs_globals.h"
#include "netx10gs_gdbproto.h"
#include "netx10gs_sockets.h"
#include "netx10gs_handlegdbproto.h"
#include "netx10gs_handlearm.h"
#include "netx10gs_helper.h"


#ifdef GDB_SWITCH_LINUX
void Sleep( int iTime )
{
	usleep( iTime * 1000 );
}
#endif

//==============================================================================
// NETX10GS_THREADS_HANDLECONNECTIONXPICGDB
//==============================================================================
///
///	@brief Thread handles xpic gdb connections.
///
/// @param [in] lpParam not used.
///
/// @return Error code.
///
//==============================================================================
#ifdef GDB_SWITCH_WINDOWS
DWORD WINAPI netx10gs_threads_HandleConnectionXpicgdb( LPVOID lpParam )
#endif
#ifdef GDB_SWITCH_LINUX
void *netx10gs_threads_HandleConnectionXpicgdb( void *lpParam )
#endif
{
	char
		cDummy = 0x0;
	
	lpParam = lpParam;
	
	while ( cDummy == cDummy )
	{
		// check if gdb switch is finished
		EnterCriticalSection( &criticalSectionGdbSwitch );		
			if ( g_bExitGdbSwitch )
			{
				LeaveCriticalSection( &criticalSectionGdbSwitch );
				break;
			}
		LeaveCriticalSection( &criticalSectionGdbSwitch );
		
		// check if xpicgdb is connected, if not wait for connection
		EnterCriticalSection( &criticalSectionXpicgdb );
		
		if ( g_cXpicgdbConnected == GDB_NOT_CONNCECTED )
		{
			LeaveCriticalSection( &criticalSectionXpicgdb );
			
			// try to establish a connection with xpicgdb
			if ( netx10gs_sockets_ConnectToXpicgdb() == GDB_SWITCH_OK )
			{
				EnterCriticalSection( &criticalSectionXpicgdb );
					g_cXpicgdbConnected = GDB_CONNECTED;
				LeaveCriticalSection( &criticalSectionXpicgdb );
			}
			Sleep( 1 );
			// #ifdef GDB_SWITCH_WINDOWS
			// Sleep( 1 );
			// #endif
			// #ifdef GDB_SWITCH_LINUX
			// usleep( 1000 );
			// #endif
		}
		else
		{
			LeaveCriticalSection( &criticalSectionXpicgdb );
			#ifdef GDB_SWITCH_WINDOWS
			Sleep( 1000 );
			#endif
			#ifdef GDB_SWITCH_LINUX
			usleep( 1000000 );
			#endif
		}
	} // end while
	
	#ifdef GDB_SWITCH_WINDOWS
	return 0;
	#endif
	#ifdef GDB_SWITCH_LINUX
	return NULL;
	#endif
}


//==============================================================================
// NETX10GS_THREADS_HANDLECONNECTIONARMGDB
//==============================================================================
///
///	@brief Thread handles arm gdb connections.
///
/// @param [in] lpParam not used.
///
/// @return Error code.
///
//==============================================================================
#ifdef GDB_SWITCH_WINDOWS
DWORD WINAPI netx10gs_threads_HandleConnectionArmgdb( LPVOID lpParam )
#endif
#ifdef GDB_SWITCH_LINUX
void *netx10gs_threads_HandleConnectionArmgdb( void *lpParam )
#endif
{
	char
		cDummy = 0x0;
	
	lpParam = lpParam;
	
	
	while ( cDummy == cDummy )
	{
		// check if gdb switch is finished
		EnterCriticalSection( &criticalSectionGdbSwitch );
			if ( g_bExitGdbSwitch )
			{
				LeaveCriticalSection( &criticalSectionGdbSwitch );
				break;
			}
		LeaveCriticalSection( &criticalSectionGdbSwitch );
		

		// check if armgdb is connected, if not wait for connection
		EnterCriticalSection( &criticalSectionArmgdb );

		if ( g_cArmgdbConnected == GDB_NOT_CONNCECTED )
		{
			LeaveCriticalSection( &criticalSectionArmgdb );
			
			// try to establish a connection with armgdb			
			if ( netx10gs_sockets_ConnectToArmgdb() == GDB_SWITCH_OK )
			{
				EnterCriticalSection( &criticalSectionArmgdb );
					g_cArmgdbConnected = GDB_CONNECTED;
				LeaveCriticalSection( &criticalSectionArmgdb );
			}
		}
		else
		{
			LeaveCriticalSection( &criticalSectionArmgdb );
			#ifdef GDB_SWITCH_WINDOWS
			Sleep( 1000 );
			#endif
			#ifdef GDB_SWITCH_LINUX
			usleep( 1000000 );
			#endif
		}
		#ifdef GDB_SWITCH_WINDOWS
		Sleep( 1000 );
		#endif
		#ifdef GDB_SWITCH_LINUX
			usleep( 1000000 );
		#endif
	} // end while
	
	#ifdef GDB_SWITCH_WINDOWS
	return 0;
	#endif
	#ifdef GDB_SWITCH_LINUX
	return NULL;
	#endif
}


//==============================================================================
// NETX10GS_THREADS_HANDLECONNECTIONARMGDB
//==============================================================================
///
///	@brief Thread handles the whole gdb session.
///
/// @param [in] lpParam not used.
///
/// @return Error code.
///
//==============================================================================
#ifdef GDB_SWITCH_WINDOWS
DWORD WINAPI netx10gs_threads_HandleGdbSession( LPVOID lpParam )
#endif
#ifdef GDB_SWITCH_LINUX
void *netx10gs_threads_HandleGdbSession( void *lpParam )
#endif
{
	char
		cDummy = 0x0;
	
	lpParam = lpParam;
	
	netx10gs_helper_log( LOG_VERBOSE, "starting gdb session ..." );
	
	while ( cDummy == cDummy )
	{
		// check if gdb switch is finished
		EnterCriticalSection( &criticalSectionGdbSwitch );		
			if ( g_bExitGdbSwitch )
			{
				LeaveCriticalSection( &criticalSectionGdbSwitch );
				break;
			}
		LeaveCriticalSection( &criticalSectionGdbSwitch );
	
		
		//----------------
		// handle xpicgdb
		//----------------
		
		// check if xpicgdb is connected, if not wait for connection
		
		// stop arm if running
		#ifndef GDB_SWITCHTESTER
		if ( !g_bSinglemode)
		{
			netx10gs_handlearm_CheckArmRunning();
			if ( g_bArmRunnig )
			{
				netx10gs_handlearm_ControlArm( ARM_HALT );
			}
		}
		#endif
		
		EnterCriticalSection( &criticalSectionXpicgdb );
		if ( g_cXpicgdbConnected == GDB_CONNECTED )
		{
			LeaveCriticalSection( &criticalSectionXpicgdb );
			
			if ( netx10gs_handlegdbproto_Xpic() != GDB_SWITCH_OK )
			{
				// exit switch only in normal mode!
				#ifndef GDB_SWITCHTESTER
				netx10gs_sockets_ResetSession();
				// EnterCriticalSection( &criticalSectionGdbSwitch );		
					// g_bExitGdbSwitch = TRUE;
				// LeaveCriticalSection( &criticalSectionGdbSwitch );
				
				// #ifdef GDB_SWITCH_WINDOWS
				// return 1;
				// #endif
				// #ifdef GDB_SWITCH_LINUX
				// return NULL;
				// #endif
				#endif
				continue;
			}
		}
		else
		{
			LeaveCriticalSection( &criticalSectionXpicgdb );
			
			#ifndef GDB_SWITCHTESTER
			netx10gs_sockets_SendKeepAlive();
			#endif
		}

		// resume arm if was running
		if ( g_bArmRunnig )
		{	
			netx10gs_handlearm_ControlArm( ARM_RESUME );
		}
		
		//---------------
		// handle armgdb
		//---------------
		
		// check if armgdb is connected, if not wait for connection
		EnterCriticalSection( &criticalSectionArmgdb );
		if ( g_cArmgdbConnected == GDB_CONNECTED )
		{
			LeaveCriticalSection( &criticalSectionArmgdb );
			netx10gs_handlegdbproto_Arm();
		}
		else
		{
			LeaveCriticalSection( &criticalSectionArmgdb );
			g_bSinglemode = TRUE;
		}
		#ifdef GDB_SWITCH_WINDOWS
		Sleep( 1 );
		#endif
		#ifdef GDB_SWITCH_LINUX
			usleep( 1000 );
		#endif
	}
	
	netx10gs_helper_log( LOG_VERBOSE, "gdb session finished" );
	
	#ifdef GDB_SWITCH_WINDOWS
	return 0;
	#endif
	#ifdef GDB_SWITCH_LINUX
	return NULL;
	#endif
}
