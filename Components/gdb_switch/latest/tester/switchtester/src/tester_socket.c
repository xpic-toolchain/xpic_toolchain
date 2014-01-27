/*
 *******************************************************************************
 *                                                                             *
 * tester_socket.c                                                             *
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
#include "../../../src/netx10gs_gdbproto.h"


struct addrinfo
	addrinfoHints,
	*paddrinfoOpenocd,
	*paddrinfoGdbswitch;

WSADATA
	wsaData;




//==============================================================================
void InitSockets( void )
{
	// init addrinfos
	paddrinfoOpenocd	= NULL;
	paddrinfoGdbswitch	= NULL;
	
	// init sockets
	socketServerOpenocd		= INVALID_SOCKET;
	socketClientOpenocd		= INVALID_SOCKET;
	socketClientXpicgdb	= INVALID_SOCKET;
	
	// set protocol details
	memset(&addrinfoHints, 0, sizeof(addrinfoHints));
	addrinfoHints.ai_family   = AF_INET;
	addrinfoHints.ai_socktype = SOCK_STREAM;
	addrinfoHints.ai_protocol = IPPROTO_TCP;
	addrinfoHints.ai_flags    = AI_PASSIVE;
	
	// init windows sockets with WSAStartup()
	if ( WSAStartup( 0x202, &wsaData ) != 0x0 )
	{
		fprintf( stderr, "ERROR: WSAStartup failed!\n" );
		WSACleanup();
		exit(1);
	}
}

	
//==============================================================================
char InitOpenocdConnection( void )
{
	// int
		// iRetval = 0;
	
	unsigned long
		ulMode = 0x0;

	if ( g_cVerbose ) fprintf( stdout, "InitOpenocdConnection - init Openocd connection\n" );
	// get addrinfo and check for errors
	if ( ( getaddrinfo( NULL, "3333", &addrinfoHints, &paddrinfoOpenocd ) != 0 ) || ( paddrinfoOpenocd == NULL ) )
	{
		fprintf( stderr, "InitOpenocdConnection - getaddrinfo() failed!\n" );
		return -1;
	}

	// get serversockets and check for errors
	socketServerOpenocd = socket( paddrinfoOpenocd->ai_family, paddrinfoOpenocd->ai_socktype, paddrinfoOpenocd->ai_protocol );
	if ( socketServerOpenocd == INVALID_SOCKET )
	{
		fprintf( stderr, "InitOpenocdConnection - socket() failed: %ld\n", WSAGetLastError() );
		return -1;
	}

	// bind serversocket and check for errors
	if ( bind( socketServerOpenocd, paddrinfoOpenocd->ai_addr, (int)paddrinfoOpenocd->ai_addrlen ) == SOCKET_ERROR )
	{
		fprintf( stderr, "InitOpenocdConnection - bind() failed: %d\n", WSAGetLastError() );
		return -1;
	}
	
	// listen and check for errors
	if ( listen( socketServerOpenocd, SOMAXCONN ) == SOCKET_ERROR )
	{
		fprintf( stderr, "InitOpenocdConnection - listen() failed: %d\n", WSAGetLastError() );
		return -1;
    }
	
	// accept connection and check for errors
	socketClientOpenocd = accept( socketServerOpenocd, NULL, NULL );	
	if ( socketClientOpenocd == INVALID_SOCKET )
	{
		fprintf( stderr, "InitOpenocdConnection - accept() failed: %d\n", WSAGetLastError() );
		return -1;
	}

	// set non-blocking mode on
	ulMode = 1;
	ioctlsocket( socketClientOpenocd, FIONBIO, &ulMode );
	if ( g_cVerbose ) fprintf( stdout, "InitOpenocdConnection - Openocd connection ready\n" );
	return 0;
}
//==============================================================================


//==============================================================================
char ConnectToGdbSwitch( void )
{
	int
		iRetval = 0;
	
	struct addrinfo	*aiPtr = NULL;
	

	
	// get addrinfo and check for errors
	iRetval = getaddrinfo( "localhost", "3335", &addrinfoHints, &paddrinfoGdbswitch );
	
	if ( (iRetval != 0) || (paddrinfoGdbswitch == NULL) )
	{
		fprintf( stderr, "ERROR: getaddrinfo() failed: %d\n", iRetval );
		return -1;
	}

    // attempt to connect to an address until one succeeds
    for( aiPtr = paddrinfoGdbswitch; aiPtr != NULL; aiPtr=aiPtr->ai_next)
	{
		// create a SOCKET for connecting to GdbSwitch
        socketClientXpicgdb = socket( aiPtr->ai_family, aiPtr->ai_socktype, aiPtr->ai_protocol);
		
        if ( socketClientXpicgdb == INVALID_SOCKET )
		{
            fprintf( stderr, "ERROR: socket() failed: %ld\n", WSAGetLastError());
			return -1;
        }

        // connect to GdbSwitch
        iRetval = connect( socketClientXpicgdb, aiPtr->ai_addr, (int)aiPtr->ai_addrlen);
		
        if ( iRetval == SOCKET_ERROR )
		{
            closesocket(socketClientXpicgdb);
            socketClientXpicgdb = INVALID_SOCKET;
            continue;
        }
        break;
    }

    if ( socketClientXpicgdb == INVALID_SOCKET )
	{
        fprintf(stderr, "ERROR: unable to connect to gdbswitch!\n");
		return -1;
    }
	if ( g_cVerbose ) fprintf(stdout, "ConnectToGdbSwitch() - connected!\n");
	return 0;
}
//==============================================================================


//==============================================================================
void CleanUp( void )
{	
	// // shutdown the send half of the connection since no more data will be sent
	// if ( WSASendDisconnect( socketClientXpicgdb, NULL ) == SOCKET_ERROR )
	// {
		// fprintf( stdout, "shutdown failed: %d\n", WSAGetLastError() );
	// }

	// clear addrinfo for Gdbswitch
	if ( paddrinfoGdbswitch != NULL )
	{
		freeaddrinfo( paddrinfoGdbswitch );
		paddrinfoGdbswitch = NULL;
	}

	// clear addrinfo for Openocd
	if ( paddrinfoOpenocd != NULL )
	{
		freeaddrinfo( paddrinfoOpenocd );
		paddrinfoOpenocd = NULL;
	}
	
	// close client socket for Gdbswitch
	if ( socketClientXpicgdb != INVALID_SOCKET )
	{
		closesocket( socketClientXpicgdb );
	}
	
	// close client socket for Openocd
	if ( socketClientOpenocd != INVALID_SOCKET )
	{
		closesocket( socketClientOpenocd );
	}
	
	WSACleanup();
}
//==============================================================================


// //==============================================================================
// char TesterSend( SOCKET socketReceiver, const char* cpcMsg )
// {
	// int
		// iRetval = 0x0;
	
	
	// iRetval = send( socketReceiver, cpcMsg, (int) strlen( cpcMsg ), 0 );
	// if ( iRetval == -1 ) { return -1; }
	// return 0;
// }


// //==============================================================================
// char TesterReceive( SOCKET socketSender )
// {
	// int
		// iRetval = 0x0;
	
	
	// iRetval = recv( socketSender, buffer, BUFFER_LENGTH, 0 );
	// buffer[iRetval] = 0;
	// if ( iRetval == -1 ) { return -1; }
	// if ( iRetval == 0 ) { return 1; }
	// return 0;
// }