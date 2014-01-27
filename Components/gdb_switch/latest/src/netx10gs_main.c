/*
 *******************************************************************************
 *                                                                             *
 * netx10gs_main.c                                                             *
 *                                                                             *
 * author: Daniel Kramlinger                                                   *
 * Hilscher Gesellschaft für Systemautomation mbH                              *
 * date: 08.04.2011                                                            *
 *                                                                             *
 *******************************************************************************
*/

#ifdef GDB_SWITCH_WINDOWS
#include <windows.h>
#endif

#ifdef GDB_SWITCH_LINUX
#include <pthread.h>
#endif

#include "netx10gs_globals.h"
#include "netx10gs_threads.h"
#include "netx10gs_helper.h"
#include "netx10gs_sockets.h"
#include "netx10gs_handleboard.h"
#include "netx10gs_handlexpic.h"





//==============================================================================
// MAIN
//==============================================================================
///
/// @brief	Main program.
///
///			Handles given program run arguments. Initializes Winsock.
///
/// @param	argc	argument counter
/// @param	argv	argument vector
///
/// @return			exitcode
///
//==============================================================================
#ifdef GDB_SWITCH_WINDOWS
int __cdecl main( int argc, char **argv )
#endif
#ifdef GDB_SWITCH_LINUX
int main( int argc, char **argv )
#endif
{	
	char
		i,
		cErrorValue = GDB_SWITCH_ERROR;

	
	#define THREADS_COUNT 0x3
	
	#ifdef GDB_SWITCH_WINDOWS
	HANDLE ahandleThreads[ THREADS_COUNT ];
	DWORD  adwThreadId[ THREADS_COUNT ];
	#endif
	
	g_pcProgname = argv[0];
	
	// main program begins here
	
	netx10gs_helper_log( LOG_DEBUG, "netx10gs_main - start" );
	netx10gs_helper_log( LOG_VERBOSE, "starting programm ..." );
	netx10gs_helper_InitGlobals();
	netx10gs_helper_CheckArguments( argc, argv );
	
	// init basic socket stuff
	netx10gs_sockets_InitSockets();

	// connect to openocd
	cErrorValue = netx10gs_sockets_ConnectToOpenocd();
	
	netx10gs_handleboard_InitBoard();
	netx10gs_handlexpic_InitNetx10();
	
	// init armgdb connection
	if ( cErrorValue == GDB_SWITCH_OK )
	{
		cErrorValue = netx10gs_sockets_InitConnectionArmgdb();
	}
	
	// init xpicgdb connection
	if ( cErrorValue == GDB_SWITCH_OK )
	{
		cErrorValue = netx10gs_sockets_InitConnectionXpicgdb();
	}
	
	// #ifndef GDB_SWITCHTESTER
	// // init netx10
	// if ( cErrorValue == GDB_SWITCH_OK )
	// {
		// cErrorValue = netx10gs_handlexpic_InitNetx10();
	// }
	// #endif
	
	// check for errors
	if ( cErrorValue != GDB_SWITCH_OK )
	{
		netx10gs_sockets_CleanUp();
		return GDB_SWITCH_ERROR;
	}

	
//----------------------------------------- WINDOWS -----------------------------------------	
// create threads WINDWOS

	#ifdef GDB_SWITCH_WINDOWS
	ahandleThreads[0] = CreateThread
		(
			NULL,
			0,
			netx10gs_threads_HandleGdbSession,
			NULL,
			0,
			&adwThreadId[0]
		);
	
	ahandleThreads[1] = CreateThread
		(
			NULL,
			0,
			netx10gs_threads_HandleConnectionXpicgdb,
			NULL,
			0,
			&adwThreadId[1]
		);

	ahandleThreads[2] = CreateThread
		(
			NULL,
			0,
			netx10gs_threads_HandleConnectionArmgdb,
			NULL,
			0,
			&adwThreadId[2]
		);
	
	// catch errors while creating threads
	for ( i = 0x0; i < THREADS_COUNT; i++ )
	{
		if ( ahandleThreads[i] == NULL ) 
		{
			netx10gs_sockets_CleanUp();
			netx10gs_helper_log( LOG_ERROR, "could not create threads!" );
			ExitProcess(3);
		}
	}
	
	GetExitCodeThread(ahandleThreads[0],&dwExitCodeHandleGdbSession);
	
	WaitForMultipleObjects( 1, ahandleThreads, TRUE, INFINITE );
	#endif
//----------------------------------------- WINDOWS -----------------------------------------
	
	
//------------------------------------------ LINUX ------------------------------------------	
// create threads LINUX

	#ifdef GDB_SWITCH_LINUX
	cErrorValue = pthread_create( &threadHandleGdbSession,
                    NULL,
                    &netx10gs_threads_HandleGdbSession,
                    NULL );
	if( cErrorValue != 0 )
	{
		netx10gs_sockets_CleanUp();
		netx10gs_helper_log( LOG_ERROR, "could not create threads!" );
		exit(1);
	}
	
	cErrorValue = pthread_create( &threadHandleConnectionXpicgdb,
                    NULL,
                    &netx10gs_threads_HandleConnectionXpicgdb,
                    NULL );
	if( cErrorValue != 0 )
	{
		netx10gs_sockets_CleanUp();
		netx10gs_helper_log( LOG_ERROR, "could not create threads!" );
		exit(1);
	}
	
	cErrorValue = pthread_create( &threadHandleConnectionArmgdb,
                    NULL,
                    &netx10gs_threads_HandleConnectionArmgdb,
                    NULL );
	if( cErrorValue != 0 )
	{
		netx10gs_sockets_CleanUp();
		netx10gs_helper_log( LOG_ERROR, "could not create threads!" );
		exit(1);
	}
	
	pthread_join( threadHandleConnectionXpicgdb, NULL );
	pthread_join( threadHandleConnectionArmgdb, NULL );
	pthread_join( threadHandleGdbSession, NULL );
	
	#endif
//------------------------------------------ LINUX ------------------------------------------
	
	
	
	netx10gs_sockets_CleanUp();
	netx10gs_helper_log( LOG_VERBOSE, "programm finished." );
	netx10gs_helper_log( LOG_DEBUG, "netx10gs_main - end" );
    return GDB_SWITCH_OK;
}
//==============================================================================
