/*
 *******************************************************************************
 *                                                                             *
 * tester_threads.c                                                            *
 *                                                                             *
 * author: Daniel Kramlinger                                                   *
 *         Hilscher Gesellschaft für Systemautomation mbH                      *
 * date: 18.02.2011                                                            *
 *                                                                             *
 *******************************************************************************
*/

#include <stdio.h>

// #include "netx10gs_globals.h"
#include "tester_socket.h"
#include "tester_threads.h"
#include "tester_testcases.h"


#define BUFFER_LENGTH 4000


char
	cCaseCounter = 0x0,
	cExitTester = 0x0;


//==============================================================================
void GdbProtoHandler( char *pacBuffer )
{
	// fprintf( stderr, "ddd: %s\n", pacBuffer );
	switch( pacBuffer[1] )
	{
		case 'm':
			strcpy_s( pacBuffer, BUFFER_LENGTH, "$12345678#42" );
			break;
		
		case 'M':
			strcpy_s( pacBuffer, BUFFER_LENGTH, "$OK#9a" );
			break;
	}
}

//==============================================================================
DWORD WINAPI TesterOpenocd( LPVOID lpParam )
{
	
	
	char
		cDummy = 0x0,
		cPrivateCaseCounter = 0x0,
		cOpenocdRequestCnt = 0x0,
		acBuffer[BUFFER_LENGTH];
	
	int
		iRetval = 0x0;
	
	
	lpParam = lpParam;
	
	if ( g_cVerbose ) fprintf( stdout, "TesterOpenocd - thread started\n" );
	// while ( cDummy == cDummy ) {cDummy = cDummy;}
	while ( cDummy == cDummy )
	{
		EnterCriticalSection( &criticalSectionOpenocd );
		if ( cExitTester == 0x1 )
		{
			LeaveCriticalSection( &criticalSectionOpenocd );
			return 0;
		}
		LeaveCriticalSection( &criticalSectionOpenocd );
		
		iRetval = recv( socketClientOpenocd, acBuffer, BUFFER_LENGTH, 0 );
		acBuffer[iRetval] = 0;
		// return if socketerror occured
		if ( ( iRetval == -1 ) && ( WSAGetLastError() != WSAEWOULDBLOCK ) )
		{
			fprintf( stderr, "TesterOpenocd - error while receiving s->o\n" );
			// EnterCriticalSection( &criticalSectionOpenocd );
				// cExitTester = 0x1;
			// LeaveCriticalSection( &criticalSectionOpenocd );
			return 1;
		}
		if ( iRetval > 0 )
		{
			if ( g_cVerbose ) fprintf( stdout, "TesterOpenocd - s->o: %s\n", acBuffer );
			
			// cPrivateCaseCounter aktualisieren
			EnterCriticalSection( &criticalSectionOpenocd );
				if ( cPrivateCaseCounter != cCaseCounter )
				{
					cPrivateCaseCounter = cCaseCounter;
					cOpenocdRequestCnt = 0;
				}
			LeaveCriticalSection( &criticalSectionOpenocd );
			
			// cOpenocdRequestCnt ueberpruefen
			if ( cOpenocdRequestCnt >= pTestCases[cPrivateCaseCounter]->ucOpenocdRequestElements )
			{
				fprintf( stderr, "TesterOpenocd - error index cOpenocdRequestCnt out of range!\n" );
				// EnterCriticalSection( &criticalSectionOpenocd );
					// cExitTester = 0x1;
				// LeaveCriticalSection( &criticalSectionOpenocd );
				return 1;
			}
			
			if ( acBuffer[0] != '$' && acBuffer[0] != '-' || pTestCases[cPrivateCaseCounter]->ppsOpenocdRequest[cOpenocdRequestCnt] != NULL && ( strcmp( acBuffer, pTestCases[cPrivateCaseCounter]->ppsOpenocdRequest[cOpenocdRequestCnt] ) != 0 ) )
			{
				fprintf( stderr, "TesterOpenocd - error in testcase: %s %i -> received: %s, expected: %s\n", pTestCases[cPrivateCaseCounter]->psTestCaseName, pTestCases[cPrivateCaseCounter]->ucTestCaseNumber, acBuffer, pTestCases[cPrivateCaseCounter]->ppsOpenocdRequest[cOpenocdRequestCnt] );
				iErrorCounter++;
			}
			
			// ohne angabe was openocd dem switch antworten soll
			if ( pTestCases[cPrivateCaseCounter]->ppsOpenocdAnswer[cOpenocdRequestCnt] == NULL )
			{
				GdbProtoHandler( acBuffer );
			}
			else
			{
				strcpy_s( acBuffer, BUFFER_LENGTH, pTestCases[cPrivateCaseCounter]->ppsOpenocdAnswer[cOpenocdRequestCnt] );
			}
			
			if ( g_cVerbose ) fprintf( stdout, "TesterOpenocd - o->s: %s\n", acBuffer );
			
			iRetval = send( socketClientOpenocd, acBuffer, (int) strlen( acBuffer ), 0 );
			// return if socketerror occured
			if ( iRetval == -1 )
			{
				fprintf( stderr, "TesterOpenocd - error while sending o->s\n" );
				// EnterCriticalSection( &criticalSectionOpenocd );
					// cExitTester = 0x1;
				// LeaveCriticalSection( &criticalSectionOpenocd );
				return 1;
			}
			cOpenocdRequestCnt++;
			// cMemoryCounter++; // XXX
		}
		Sleep( 10 );
	} // end while
	
	return 0;
}

//==============================================================================
DWORD WINAPI TesterXpicgdb( LPVOID lpParam )
{
	char
		acBuffer[BUFFER_LENGTH];
		
	unsigned char
		ucCaseCnt = 0x0;	
		
	int
		iStrCmpResult = 0x0,
		iRetval = 0x0;
	

	
	lpParam = lpParam;

	// fprintf( stdout, "uu: %s\n", ppTestCases[0] );
	if ( g_cVerbose ) fprintf( stdout, "TesterXpicgdb - thread started\n" );
	// fprintf( stdout, "1: %p\n", pTestCases );
	// fprintf( stdout, "2: %p\n", &pTestCases );
	// fprintf( stdout, "t3: %p\n", pTestCases[0] );
	// fprintf( stdout, "t3: %p\n", pTestCases[1] );
	// fprintf( stdout, "tx->s: %s\n", pTestCases[0]->psXpicgdbRequest );
	for ( ucCaseCnt = 0x0; ucCaseCnt < uiNumberOfCases; ucCaseCnt++ )
	// while( i == i )
	{
		// i++;
		// check ob testcases zuende
		// if ( strcmp( "EXIT", pTestCases[i]->psXpicgdbRequest ) == 0 )
		// {
			// EnterCriticalSection( &criticalSectionOpenocd );
				// cExitTester = 0x1;
			// LeaveCriticalSection( &criticalSectionOpenocd );
			// return 0;
		// }
		// fprintf( stdout, "x->s: %c\n", pTestCases[0]->psXpicgdbRequest[0] );
		// fprintf( stdout, "WURST %c\n", pTestCases[ucCaseCnt]->psXpicgdbRequest[0] );
		// zusaetzliche ausgaben im verbose mode
		if ( g_cVerbose )
		{
			// check for CTRL-C
			if ( pTestCases[ucCaseCnt]->psXpicgdbRequest[0] == 0x3 )
			{
				fprintf( stdout, "\nTESTCASE: %s %i\nTesterXpicgdb - x->s: CTRL-C\n", pTestCases[ucCaseCnt]->psTestCaseName, pTestCases[ucCaseCnt]->ucTestCaseNumber );
			}
			else
			{
				fprintf( stdout, "\nTESTCASE: %s %i\nTesterXpicgdb - x->s: %s\n", pTestCases[ucCaseCnt]->psTestCaseName, pTestCases[ucCaseCnt]->ucTestCaseNumber, pTestCases[ucCaseCnt]->psXpicgdbRequest );
			}
		}
		// fprintf( stdout, "WURST3\n" );
		// sende request an openocd
		iRetval = send( socketClientXpicgdb, pTestCases[ucCaseCnt]->psXpicgdbRequest, (int) strlen( pTestCases[ucCaseCnt]->psXpicgdbRequest ), 0 );
		 // return if socketerror occured
		if ( iRetval == -1 )
		{
			fprintf( stderr, "TesterXpicgdb - error while sending x->s\n" );
			return 1;
		}
		
		// empfange antwort
		iRetval = recv( socketClientXpicgdb, acBuffer, BUFFER_LENGTH, 0 );
		acBuffer[iRetval] = 0;
		// return if socketerror occured
		if ( iRetval == -1 )
		{
			fprintf( stderr, "TesterXpicgdb - error while receiving s->x\n" );
			return 1;
		}
		if ( ( iRetval != 0 ) && ( g_cVerbose ) ) { fprintf( stdout, "TesterXpicgdb - s->x: %s\n", acBuffer ); }
		
		if ( ( cNoAckModeStartedXpic == 0x0 ) && ( pTestCases[ucCaseCnt]->psXpicgdbRequest[0] != '-' ) )
		// if ( ( cNoAckModeStartedXpic == 0x0 ) )
		{
			if ( !( acBuffer[0] == '+' && acBuffer[1] == 0 ) )
			{
				fprintf( stdout, "TesterXpicgdb - error in testcase: %s %i -> xpicgdb expected ACK, received:%s\n", pTestCases[ucCaseCnt]->psTestCaseName, pTestCases[ucCaseCnt]->ucTestCaseNumber, acBuffer );
				EnterCriticalSection( &criticalSectionOpenocd );
					cCaseCounter++;
					iErrorCounter++;
				LeaveCriticalSection( &criticalSectionOpenocd );
				continue;
			}
			
			// receive again
			iRetval = recv( socketClientXpicgdb, acBuffer, BUFFER_LENGTH, 0 );
			acBuffer[iRetval] = 0;
			// return if socketerror occured
			if ( iRetval == -1 )
			{
				fprintf( stderr, "TesterXpicgdb - error while receiving again s->x\n" );
				return 1;
			}
			if ( g_cVerbose ) fprintf( stdout, "TesterXpicgdb - s->x: %s\n", acBuffer );
		}
		
		// check if received msg is correct
		iStrCmpResult = strcmp( acBuffer, pTestCases[ucCaseCnt]->psGdbswitchAnswer);
		if ( ( iStrCmpResult != 0 ) || ( pTestCases[ucCaseCnt]->psGdbswitchAnswer == NULL ) )
		{
			// print error message
			if ( iStrCmpResult != 0 )
			{
				fprintf( stderr, "TesterXpicgdb - error in testcase: %s %i -> xpicgdb expected: %s, received: %s\n", pTestCases[ucCaseCnt]->psTestCaseName, pTestCases[ucCaseCnt]->ucTestCaseNumber, pTestCases[ucCaseCnt]->psGdbswitchAnswer, acBuffer );
			}
			// increment counter
			EnterCriticalSection( &criticalSectionOpenocd );
					cCaseCounter++;
					iErrorCounter++;
			LeaveCriticalSection( &criticalSectionOpenocd );
			continue;
		}
		
		// receive again if sent msg is "$qRcmdXX"
		if ( strncmp( "$qRcmd", pTestCases[ucCaseCnt]->psXpicgdbRequest, 6 ) == 0 )
		{
			iRetval = recv( socketClientXpicgdb, acBuffer, BUFFER_LENGTH, 0 );
			acBuffer[iRetval] = 0;
			// return if socketerror occured
			if ( iRetval == -1 )
			{
				fprintf( stderr, "TesterXpicgdb - error while receiving ok s->x\n" );
				return 1;
			}
			
			if ( g_cVerbose ) fprintf( stdout, "TesterXpicgdb - s->x: %s\n", acBuffer );
			
			// check if received msg is OK
			if ( strcmp( acBuffer, "$OK#9a" ) != 0 )
			{
				fprintf( stderr, "TesterXpicgdb - error in testcase: %s %i -> expected: $OK#9a, received: %s\n", pTestCases[ucCaseCnt]->psTestCaseName, pTestCases[ucCaseCnt]->ucTestCaseNumber, acBuffer );
				EnterCriticalSection( &criticalSectionOpenocd );
						cCaseCounter++;
						iErrorCounter++;
				LeaveCriticalSection( &criticalSectionOpenocd );
				continue;
			}
		}
		
		// aktivate cNoAckModeStartedXpic
		if ( strcmp( "$QStartNoAckMode#b0", pTestCases[ucCaseCnt]->psXpicgdbRequest ) == 0 && ( strcmp( "$OK#9a", pTestCases[ucCaseCnt]->psGdbswitchAnswer ) == 0 ) )
		{
			cNoAckModeStartedXpic = 0x1;
		}
		
		EnterCriticalSection( &criticalSectionOpenocd );
			cCaseCounter++;
		LeaveCriticalSection( &criticalSectionOpenocd );
	} // end for
	
		EnterCriticalSection( &criticalSectionOpenocd );
			cExitTester = 0x1;
		LeaveCriticalSection( &criticalSectionOpenocd );
	
	return 0;
}
