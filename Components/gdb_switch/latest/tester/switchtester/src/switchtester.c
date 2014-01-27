/*
 *******************************************************************************
 *                                                                             *
 * switchtester.c                                                              *
 *                                                                             *
 * author: Daniel Kramlinger                                                   *
 *         Hilscher Gesellschaft für Systemautomation mbH                      *
 * date: 18.02.2011                                                            *
 *                                                                             *
 *******************************************************************************
*/



#include <stdlib.h>
#include <stdio.h>

#include "tester_socket.h"
#include "tester_testcases.h"
#include "tester_threads.h"

#include "../../../src/netx10gs_gdbproto.h"
// #include "../../../src/netx10gs_globals.h"


// void printErrorMsg( void )
// {
		// fprintf( stdout, "ERROR in %s - testcase: %i\n", pcText, cCaseCnt );
		// cErrorCnt++;
// }

//==============================================================================
// MAIN
//==============================================================================

int main( int argc, char **argv )
{
	#define THREADS_COUNT	0x2
	
	int
		i = 0x0;
	
	
	HANDLE ahandleThreads[ THREADS_COUNT ];
	DWORD  adwThreadId[ THREADS_COUNT ];
	
	// // TEST
	// InitTestCases();
	// fprintf ( stdout, "%s\n" , pTC[0].psXpicgdbRequest );
	// fprintf ( stdout, "%s\n" , pTC[1].psXpicgdbRequest );
	
	// if ( i == 0x0 )
		// return 0;
	// // END TEST
	
	iErrorCounter = 0x0;
	g_cVerbose = 0x0;
	// check arguments
	if ( argc > 1 )
	{
		for ( i = 1; i < argc; i++ )
		{
			if ( (strlen(argv[i]) == 2) && (argv[i][0] == '-') )
			{
				switch ( tolower( argv[i][1] ) )
				{
					case 'v':
						g_cVerbose = 0x1;
						break;
				}
			}
		}
	}

	
	fprintf ( stdout, "\nstarting tester: switchtester\n" );
	

	// init sockets and connections
	InitSockets();
	if ( InitOpenocdConnection() != 0x0 ) { CleanUp(); exit(0); }
	if ( ConnectToGdbSwitch() != 0x0 ) { CleanUp(); exit(0); }
	
	// init testcasese
	InitTestCases();
	// fprintf( stdout, "1: %p\n", astructTestCases.psXpicgdbRequest );
	// fprintf( stdout, "2: %p\n", &pTestCases );
	// fprintf( stdout, "3: %p\n", pTestCases[0] );
	// fprintf( stdout, "3: %p\n", pTestCases[1] );
	// fprintf( stdout, "x->s: %c\n", pTestCases[0]->psXpicgdbRequest[0] );
	// fprintf( stdout, "x->s: %s\n", pTestCases[1]->psXpicgdbRequest );
	// fprintf( stdout, "s->o: %s\n", pTestCases[1]->ppsOpenocdRequest[0] );
	// fprintf( stdout, "o->s: %s\n", pTestCases[1]->psOpenocdAnswer );
	// fprintf( stdout, "s->x: %s\n", pTestCases[1]->psGdbswitchAnswer );
	
	// if ( argc == argc ) exit(0);
	// =================  THREADS  ====================
	// init critical section variables
	if ( ! InitializeCriticalSectionAndSpinCount( &criticalSectionOpenocd, 0x80000400 ) )
	{
		return -1;
	}
	if ( ! InitializeCriticalSectionAndSpinCount( &criticalSectionTestCnt, 0x80000400 ) )
	{
		return -1;
	}
	
	// create threads
	ahandleThreads[0] = CreateThread
		(
			NULL,
			0,
			TesterOpenocd,
			NULL,
			0,
			&adwThreadId[0]
		);
	
	ahandleThreads[1] = CreateThread
		(
			NULL,
			0,
			TesterXpicgdb,
			NULL,
			0,
			&adwThreadId[1]
		);	
		
	
	// catch errors while creating threads
	for ( i = 0x0; i < THREADS_COUNT; i++ )
	{
		if ( ahandleThreads[i] == NULL ) 
		{
			CleanUp();
			ExitProcess(3);
		}
	}
	
	WaitForMultipleObjects( 2, ahandleThreads, TRUE, INFINITE );
	// ================================================
	
	
	
	// close connections
	CleanUp();
	
	fprintf( stdout, "tester finished with %i error(s)\n", iErrorCounter );
	
	return 0;
}