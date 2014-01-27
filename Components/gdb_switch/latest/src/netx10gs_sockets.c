/*
 *******************************************************************************
 *                                                                             *
 * netx10gs_sockets.c                                                          *
 *                                                                             *
 * author: Daniel Kramlinger                                                   *
 * Hilscher Gesellschaft für Systemautomation mbH                              *
 * date: 14.04.2011                                                            *
 *                                                                             *
 *******************************************************************************
*/

/// @brief Prevents conflicts caused by including winsock2 (backword compatibility with winsock)

// includes for WINDOWS
#ifdef GDB_SWITCH_WINDOWS

#ifndef WIN32_LEAN_AND_MEAN
	#define WIN32_LEAN_AND_MEAN
#endif

#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>

#endif


// includes for LINUX
#ifdef GDB_SWITCH_LINUX

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>

#define SOCKET	int
#define SOCKET_ERROR	-1
#define WSAEWOULDBLOCK	EWOULDBLOCK
#define FIONBIO			F_SETFL
#define WSAGetLastError()	errno
// #define WSAGetLastError	perror
#define closesocket	close
#endif


#include "netx10gs_globals.h"
#include "netx10gs_gdbproto.h"
#include "netx10gs_sockets.h"
#include "netx10gs_handlegdbproto.h"
#include "netx10gs_helper.h"
#include "netx10gs_threads.h"




SOCKET
	socketServerArmgdb,
	socketClientArmgdb,
	socketServerXpicgdb,
	socketClientXpicgdb,
	socketClientOpenocd;

// resources for WINDOWS

struct addrinfo
	addrinfoHints,
	*paddrinfoArmgdb,
	*paddrinfoXpicgdb,
	*paddrinfoOpenocd;

#ifdef GDB_SWITCH_WINDOWS
WSADATA
	wsaData;
#endif


// resources for LINUX
#ifdef GDB_SWITCH_LINUX
struct sockaddr_in server;
int thread_retval=815;
#endif






//******************************************************************************
// GLOBAL FUNCTIONS                                                            *
//******************************************************************************

//==============================================================================
// NETX10GS_SOCKETS_INITSOCKETS
//==============================================================================
///
/// @brief netx10gs_sockets_InitSockets initializes sockets.
///
/// @return Errorcode.
///
//==============================================================================
void netx10gs_sockets_ResetSession( void )
{
	netx10gs_helper_log( LOG_ERROR, "sockets_ResetSession - resetting session" );
	
	g_bNoAckModeStartedXpic = FALSE;
	g_bNoAckModeStartedArm = FALSE;
	
	EnterCriticalSection( &criticalSectionXpicgdb );
		g_cXpicgdbConnected = GDB_NOT_CONNCECTED;
	LeaveCriticalSection( &criticalSectionXpicgdb );
	
	EnterCriticalSection( &criticalSectionArmgdb );
		g_cArmgdbConnected = GDB_NOT_CONNCECTED;
	LeaveCriticalSection( &criticalSectionArmgdb );
	
}
//==============================================================================


//==============================================================================
// NETX10GS_SOCKETS_INITSOCKETS
//==============================================================================
///
/// @brief netx10gs_sockets_InitSockets initializes sockets.
///
/// @return Errorcode.
///
//==============================================================================
void netx10gs_sockets_InitSockets( void )
{
	int
		iRetval = 0x0;
	
	netx10gs_helper_log( LOG_INITSOCKETS, "sockets_InitSockets - start" );
	
	// init addrinfos
	paddrinfoArmgdb		= NULL;
	paddrinfoXpicgdb	= NULL;
	paddrinfoOpenocd	= NULL;
	
	// init sockets
	socketServerArmgdb	= INVALID_SOCKET;
	socketClientArmgdb	= INVALID_SOCKET;
	socketServerXpicgdb	= INVALID_SOCKET;
	socketClientXpicgdb	= INVALID_SOCKET;
	socketClientOpenocd	= INVALID_SOCKET;
	
	// set protocol details
	memset(&addrinfoHints, 0, sizeof(addrinfoHints));
	addrinfoHints.ai_family   = AF_INET;
	addrinfoHints.ai_socktype = SOCK_STREAM;
	addrinfoHints.ai_protocol = IPPROTO_TCP;
	addrinfoHints.ai_flags    = AI_PASSIVE;

	netx10gs_helper_log( LOG_VERBOSE, "WSAStartup ..." );

#ifdef GDB_SWITCH_WINDOWS
	// init windows sockets with WSAStartup()
	if ( ( iRetval = WSAStartup( 0x202, &wsaData ) ) != 0 )
	{
		netx10gs_helper_log( LOG_ERROR, "sockets_InitSockets - WSAStartup() failed: %d", iRetval );
		WSACleanup();
		exit(1);
	}
#endif

#ifdef GDB_SWITCH_LINUX
	memset( &server, 0, sizeof (server));
#endif
	
	netx10gs_helper_log( LOG_VERBOSE, "arm initializing done." );
	netx10gs_helper_log( LOG_INITSOCKETS, "sockets_InitSockets - end" );
}
//==============================================================================


//==============================================================================
// NETX10GS_SOCKETS_INITCONNECTIONARMGDB
//==============================================================================
///
/// @brief netx10gs_sockets_InitConnectionArmgdb initialzes armgdb connection.
///
/// @return Errorcode.
///
//==============================================================================
char netx10gs_sockets_InitConnectionArmgdb( void )
{
	int
		iRetval;
	
	
	netx10gs_helper_log( LOG_INITCONNECTIONARMGDB, "sockets_InitConnectionArmgdb - start" );
	netx10gs_helper_log( LOG_VERBOSE, "initialize arm connection ..." );

// #ifdef GDB_SWITCH_WINDOWS
//------------------------------------------------------------------------------
// get addrinfo and check for errors
//------------------------------------------------------------------------------
	netx10gs_helper_log( LOG_VERBOSE, "get arm addrinfo ..." );
	
	iRetval = getaddrinfo( NULL, g_pcArmgdbPort, &addrinfoHints, &paddrinfoArmgdb );
	
	if ( (iRetval != 0) || (paddrinfoArmgdb == NULL) )
	{
		netx10gs_helper_log( LOG_ERROR, "sockets_InitConnectionArmgdb - getaddrinfo() failed: %d", iRetval );
		return GDB_SWITCH_ERROR;
	}
// #endif

// #ifdef GDB_SWITCH_LINUX	
	// paddrinfoArmgdb = &addrinfoHints;
// #endif
	
//------------------------------------------------------------------------------
// get serversockets and check for errors
//------------------------------------------------------------------------------
	netx10gs_helper_log( LOG_VERBOSE, "get arm serversocket ..." );
	
	socketServerArmgdb = socket(paddrinfoArmgdb->ai_family, paddrinfoArmgdb->ai_socktype, paddrinfoArmgdb->ai_protocol);
	
	if ( socketServerArmgdb == INVALID_SOCKET )
	{
		netx10gs_helper_log( LOG_ERROR, "sockets_InitConnectionArmgdb - socket() failed: %ld", WSAGetLastError() );
		return GDB_SWITCH_ERROR;
	}
	
//------------------------------------------------------------------------------
// bind serversocket and check for errors
//------------------------------------------------------------------------------
	netx10gs_helper_log( LOG_VERBOSE, "bind arm serversocket ..." );

// #ifdef GDB_SWITCH_WINDOWS
	iRetval = bind( socketServerArmgdb, paddrinfoArmgdb->ai_addr, (int)paddrinfoArmgdb->ai_addrlen);
// #endif


// #ifdef GDB_SWITCH_LINUX
// // IPv4-Adresse
	// server.sin_family = AF_INET;
// // Jede IP-Adresse ist gültig
	// server.sin_addr.s_addr = htonl( INADDR_ANY );

// // Portnummer 1234
	// server.sin_port = htons( 3334 );

	// iRetval = bind( socketServerArmgdb, (struct sockaddr*)&server, sizeof( server));
   // //Fehler bei bind()
// #endif
	
	if ( iRetval == SOCKET_ERROR )
	{
		netx10gs_helper_log( LOG_ERROR, "sockets_InitConnectionArmgdb - bind() failed: %d", WSAGetLastError() );
		return GDB_SWITCH_ERROR;
	}

//------------------------------------------------------------------------------
// listen and check for errors
//------------------------------------------------------------------------------
	netx10gs_helper_log( LOG_VERBOSE, "listen to arm serversocket ..." );
	
	iRetval = listen( socketServerArmgdb, SOMAXCONN );
	
	if ( iRetval == SOCKET_ERROR )
	{
		netx10gs_helper_log( LOG_ERROR, "sockets_InitConnectionArmgdb - listen() failed: %d", WSAGetLastError() );
		return GDB_SWITCH_ERROR;
    }
	
	netx10gs_helper_log( LOG_INITCONNECTIONARMGDB, "sockets_InitConnectionArmgdb - end" );
	return GDB_SWITCH_OK;
}
//==============================================================================


//==============================================================================
// NETX10GS_SOCKETS_INITCONNECTIONXPICGDB
//==============================================================================
///
/// @brief netx10gs_sockets_InitConnectionXpicgdb initialzes xpicgdb connection.
///
/// @return Errorcode.
///
//==============================================================================
char netx10gs_sockets_InitConnectionXpicgdb( void )
{
	int
		iRetval = 0;
	
	
	netx10gs_helper_log( LOG_DEBUG, "sockets_InitConnectionXpicgdb - start" );
	netx10gs_helper_log( LOG_VERBOSE, "initialize xpic connection ..." );

// #ifdef GDB_SWITCH_WINDOWS
//------------------------------------------------------------------------------
// get addrinfo and check for errors
//------------------------------------------------------------------------------
	netx10gs_helper_log( LOG_VERBOSE, "get xpic addrinfo ..." );
	
	iRetval = getaddrinfo(NULL, g_pcXpicgdbPort, &addrinfoHints, &paddrinfoXpicgdb);
	
	if ( (iRetval != 0) || (paddrinfoXpicgdb == NULL) )
	{
		netx10gs_helper_log( LOG_ERROR, "sockets_InitConnectionXpicgdb - getaddrinfo() failed: %d\n", iRetval );
		return GDB_SWITCH_ERROR;
	}
	
	netx10gs_helper_log( LOG_VERBOSE, "getaddrinfo %i", iRetval );
// #endif

// #ifdef GDB_SWITCH_LINUX	
// paddrinfoXpicgdb = &addrinfoHints;
// #endif

//------------------------------------------------------------------------------
// get serversockets and check for errors
//------------------------------------------------------------------------------
	netx10gs_helper_log( LOG_VERBOSE, "get xpic serversocket ..." );
	
	socketServerXpicgdb = socket(paddrinfoXpicgdb->ai_family, paddrinfoXpicgdb->ai_socktype, paddrinfoXpicgdb->ai_protocol);
	
	if (socketServerXpicgdb == INVALID_SOCKET)
	{
		netx10gs_helper_log( LOG_ERROR, "sockets_InitConnectionXpicgdb - socket() failed: %ld\n", WSAGetLastError() );
		return GDB_SWITCH_ERROR;
	}

//------------------------------------------------------------------------------
// bind serversocket and check for errors
//------------------------------------------------------------------------------
	netx10gs_helper_log( LOG_VERBOSE, "bind xpic serversocket ..." );

// #ifdef GDB_SWITCH_WINDOWS
	iRetval = bind( socketServerXpicgdb, paddrinfoXpicgdb->ai_addr, (int)paddrinfoXpicgdb->ai_addrlen);
// #endif

	netx10gs_helper_log( LOG_VERBOSE, "bind 0 %i", iRetval );
// #ifdef GDB_SWITCH_LINUX

	// netx10gs_helper_log( LOG_VERBOSE, "0" );
	// server.sin_family = AF_INET;
	// netx10gs_helper_log( LOG_VERBOSE, "1" );
	// server.sin_addr.s_addr = htonl( INADDR_ANY );
	// netx10gs_helper_log( LOG_VERBOSE, "2" );
	// server.sin_port = htons( 3337 );
	// netx10gs_helper_log( LOG_VERBOSE, "3");
	// iRetval = bind( socketServerXpicgdb, (struct sockaddr*)&server, sizeof( server ));
	
	// netx10gs_helper_log( LOG_VERBOSE, "4" );
	// server.sin_port = htons( 3336 );
	// iRetval = bind( socketServerXpicgdb, (struct sockaddr*)&server, sizeof( server ));
	// netx10gs_helper_log( LOG_VERBOSE, "5" );
// #endif
	netx10gs_helper_log( LOG_VERBOSE, "bind 1 %i", iRetval );
	
	
	if ( iRetval == SOCKET_ERROR )
	{
	netx10gs_helper_log( LOG_VERBOSE, "bind 2 %i", iRetval );
		netx10gs_helper_log( LOG_ERROR, "sockets_InitConnectionXpicgdb - bind() failed: %d\n", WSAGetLastError() );
		return GDB_SWITCH_ERROR;
	}
	netx10gs_helper_log( LOG_VERBOSE, "bind 3 %i", iRetval );
//------------------------------------------------------------------------------
// listen and check for errors
//------------------------------------------------------------------------------
	netx10gs_helper_log( LOG_VERBOSE, "listen to xpic serversocket ..." );
	
	iRetval = listen( socketServerXpicgdb, SOMAXCONN );
	
	if (iRetval == SOCKET_ERROR)
	{
		netx10gs_helper_log( LOG_ERROR, "sockets_InitConnectionXpicgdb - listen() failed: %d\n", WSAGetLastError() );
		return GDB_SWITCH_ERROR;
    }
	
	netx10gs_helper_log( LOG_VERBOSE, "xpic initializing done." );
	netx10gs_helper_log( LOG_DEBUG, "sockets_InitConnectionXpicgdb - end" );
	return GDB_SWITCH_OK;
}
//==============================================================================


//==============================================================================
// NETX10GS_SOCKETS_CONNECTTOARMGDB
//==============================================================================
///
/// @brief netx10gs_sockets_ConnectToArmgdb waits for a arm gdb connection.
///
/// @return Errorcode.
///
//==============================================================================
char netx10gs_sockets_ConnectToArmgdb( void )
{
	unsigned long
		ulMode;
	
	
	netx10gs_helper_log( LOG_DEBUG, "sockets_ConnectToArmgdb - start" );
	
	// accept connection and check for errors
	netx10gs_helper_log( LOG_VERBOSE, "accept armgdb client ..." );
	socketClientArmgdb = accept( socketServerArmgdb, NULL, NULL );
	
	if ( socketClientArmgdb == INVALID_SOCKET )
	{	
		netx10gs_helper_log( LOG_ERROR, "sockets_ConnectToArmgdb - accept() failed: %d\n", WSAGetLastError() );
		return GDB_SWITCH_ERROR;
	}

	// set non-blocking mode on
	#ifdef GDB_SWITCH_WINDOWS
	ulMode = 1;
	ioctlsocket( socketClientArmgdb, FIONBIO, &ulMode );
	#endif
	#ifdef GDB_SWITCH_LINUX
	fcntl( socketClientArmgdb, F_SETFL, O_NONBLOCK);
	#endif
	
	netx10gs_helper_log( LOG_VERBOSE, "armgdb client accepted." );
	netx10gs_helper_log( LOG_DEBUG, "sockets_ConnectToArmgdb - end" );
	return GDB_SWITCH_OK;
}
//==============================================================================

	
	
//==============================================================================
// NETX10GS_SOCKETS_CONNECTTOXPICGDB
//==============================================================================
///
/// @brief netx10gs_sockets_ConnectToXpicgdb waits for a xpic gdb connection.
///
/// @return Errorcode.
///
//==============================================================================
char netx10gs_sockets_ConnectToXpicgdb( void )
{
	unsigned long
		ulMode;
	
	
	netx10gs_helper_log( LOG_DEBUG, "sockets_ConnectToXpicgdb - start" );
	
	// accept connection and check for errors
	netx10gs_helper_log( LOG_VERBOSE, "accept xpicgdb client ..." );
	socketClientXpicgdb = accept( socketServerXpicgdb, NULL, NULL );
	
	if ( socketClientXpicgdb == INVALID_SOCKET )
	{
		netx10gs_helper_log( LOG_ERROR, "sockets_ConnectToXpicGDB - accept() failed: %d\n", WSAGetLastError() );
		return GDB_SWITCH_ERROR;
	}

	// set non-blocking mode on
	#ifdef GDB_SWITCH_WINDOWS
	ulMode = 1;
	ioctlsocket( socketClientXpicgdb, FIONBIO, &ulMode );
	#endif
	#ifdef GDB_SWITCH_LINUX
	fcntl( socketClientXpicgdb, F_SETFL, O_NONBLOCK );
	#endif
	
	netx10gs_helper_log( LOG_VERBOSE, "xpicgdb client accepted." );
	netx10gs_helper_log( LOG_DEBUG, "sockets_ConnectToXpicgdb - end" );
	return GDB_SWITCH_OK;
}
//==============================================================================



//==============================================================================
// NETX10GS_SOCKETS_CONNECTTOOPENOCD
//==============================================================================
///
/// @brief netx10gs_sockets_ConnectToOpenocd connects to openocd.
///
/// @return Errorcode.
///
//==============================================================================
char netx10gs_sockets_ConnectToOpenocd( void )
{
	int
		iRetval = 0;
		
	struct addrinfo	*aiPtr = NULL;
	
	#ifndef GDB_SWITCHTESTER
		struct gdbCommand structGdbCommand;
	#endif
	
	
	netx10gs_helper_log( LOG_DEBUG, "sockets_ConnectToOpenocd - start" );
	netx10gs_helper_log( LOG_VERBOSE, "connecting to openocd ..." );
	
	// get addrinfo and check for errors
	netx10gs_helper_log( LOG_VERBOSE, "get addrinfo ..." );
	iRetval = getaddrinfo( "localhost", g_pcOpenocdPort, &addrinfoHints, &paddrinfoOpenocd );
	
	if ( (iRetval != 0) || (paddrinfoOpenocd == NULL) )
	{
		netx10gs_helper_log( LOG_ERROR, "sockets_ConnectToOpenocd - getaddrinfo() failed: %d\n", iRetval );
		return GDB_SWITCH_ERROR;
	}
	
    // attempt to connect to an address until one succeeds
    for( aiPtr = paddrinfoOpenocd; aiPtr != NULL; aiPtr=aiPtr->ai_next)
	{
		// create a SOCKET for connecting to openocd
        socketClientOpenocd = socket( aiPtr->ai_family, aiPtr->ai_socktype, aiPtr->ai_protocol);
		
        if ( socketClientOpenocd == INVALID_SOCKET )
		{
            netx10gs_helper_log( LOG_ERROR, "sockets_ConnectToOpenocd - socket() failed: %ld\n", WSAGetLastError() );
			return GDB_SWITCH_ERROR;
        }
		
        // connect to openocd
        if ( connect( socketClientOpenocd, aiPtr->ai_addr, (int)aiPtr->ai_addrlen) == SOCKET_ERROR )
		{
            closesocket(socketClientOpenocd);	
            socketClientOpenocd = INVALID_SOCKET;
            continue;
        }
        break;
    }

    if ( socketClientOpenocd == INVALID_SOCKET )
	{
        netx10gs_helper_log( LOG_ERROR, "sockets_ConnectToOpenocd - unable to connect to openocd!");
		return GDB_SWITCH_ERROR;
    }
	
	#ifndef GDB_SWITCHTESTER
	// activate NoAckMode for OpenOCD
	netx10gs_helper_log( LOG_VERBOSE, "activating NoAckMode for openocd ..." );
	netx10gs_sockets_SendAndReceive( SOCKET_OPENOCD, GDB_STARTNOACKMODE, buffer, DEFAULT_BUFFER_LEN, GDB_DONT_RETURN_ANSWER, INVALID_SOCKET, GDB_DONT_WAIT_FOR_OK, "sockets_ConnectToOpenocd - unable to activate NoAckMode!" );
	netx10gs_handlegdbproto_GetGdbCmd( buffer, &structGdbCommand );
	if ( structGdbCommand.cCmd != GDB_CMD_OK )
	{
		netx10gs_helper_log( LOG_ERROR, "sockets_ConnectToOpenocd - unable to activate NoAckMode!");
		return GDB_SWITCH_ERROR;
	}
	netx10gs_helper_log( LOG_VERBOSE, "NoAckMode for openocd activated." );
	#endif
	
	netx10gs_helper_log( LOG_VERBOSE, "connection to openocd established." );
	netx10gs_helper_log( LOG_DEBUG, "sockets_ConnectToOpenocd - end" );
	return GDB_SWITCH_OK;
}
//==============================================================================


//==============================================================================
// NETX10GS_SOCKETS_CLEANUPARM
//==============================================================================
///
/// @brief netx10gs_sockets_CleanUpArm closes arm gdb client socket.
///
/// @return Errorcode.
///
//==============================================================================
void netx10gs_sockets_CleanUpArm( void )
{
	netx10gs_helper_log( LOG_DEBUG, "sockets_CleanUpArm - start" );
	netx10gs_helper_log( LOG_VERBOSE, "cleaning arm socket resources ..." );
	
	// close client socket for xpicgdb
	if ( socketClientArmgdb != INVALID_SOCKET )
	{
		closesocket( socketClientArmgdb );
	}
	
	EnterCriticalSection( &criticalSectionArmgdb );	
		g_cArmgdbConnected	= GDB_NOT_CONNCECTED;
	LeaveCriticalSection( &criticalSectionArmgdb );
	
	netx10gs_helper_log( LOG_VERBOSE, "arm socket resources have been cleaned." );
	netx10gs_helper_log( LOG_DEBUG, "sockets_CleanUpArm - end" );
}
//==============================================================================


//==============================================================================
// NETX10GS_SOCKETS_CLEANUPXPIC
//==============================================================================
///
/// @brief netx10gs_sockets_CleanUpXpic closes arm xpic client socket.
///
/// @return Errorcode.
///
//==============================================================================
void netx10gs_sockets_CleanUpXpic( void )
{
	netx10gs_helper_log( LOG_DEBUG, "sockets_CleanUpXpic - start" );
	netx10gs_helper_log( LOG_VERBOSE, "cleaning xpic socket resources ..." );
	
	// close client socket for xpicgdb
	if ( socketClientXpicgdb != INVALID_SOCKET )
	{
		closesocket( socketClientXpicgdb );
	}
	
	EnterCriticalSection( &criticalSectionXpicgdb );	
		g_cXpicgdbConnected	= GDB_NOT_CONNCECTED;
	LeaveCriticalSection( &criticalSectionXpicgdb );
	
	netx10gs_helper_log( LOG_VERBOSE, "xpic socket resources have been cleaned." );
	netx10gs_helper_log( LOG_DEBUG, "sockets_CleanUpXpic - end" );
}
//==============================================================================


//==============================================================================
// NETX10GS_SOCKETS_CLEANUP
//==============================================================================
///
/// @brief netx10gs_sockets_CleanUp closes all socket resources.
///
/// @return Errorcode.
///
//==============================================================================
void netx10gs_sockets_CleanUp( void )
{
	netx10gs_helper_log( LOG_DEBUG, "sockets_CleanUp - start" );
	netx10gs_helper_log( LOG_VERBOSE, "cleaning up socket resources ..." );

	//-------------------------------------------
	// clean up armgdb
	//-------------------------------------------
	
	#ifdef GDB_SWITCH_WINDOWS
	// clear addrinfo for armgdb
	if ( paddrinfoArmgdb != NULL )
	{
		freeaddrinfo( paddrinfoArmgdb );
		paddrinfoArmgdb = NULL;
	}
	#endif

	// close server socket for armgdb
	if ( socketServerArmgdb != INVALID_SOCKET )
	{
		closesocket( socketServerArmgdb );
	}
	
	// close client socket for armgdb
	if ( socketClientArmgdb != INVALID_SOCKET )
	{
		closesocket( socketClientArmgdb );
	}

	
	//-------------------------------------------
	// clean up xpicgdb
	//-------------------------------------------
	
	#ifdef GDB_SWITCH_WINDOWS
	// clear addrinfo for xpicgdb
	if ( paddrinfoXpicgdb != NULL )
	{
		freeaddrinfo( paddrinfoXpicgdb );
		paddrinfoXpicgdb = NULL;
	}
	#endif

	// close server socket for xpicgdb
	if ( socketServerXpicgdb != INVALID_SOCKET )
	{
		closesocket( socketServerXpicgdb );
	}
	
	// close client socket for xpicgdb
	if ( socketClientXpicgdb != INVALID_SOCKET )
	{
		closesocket( socketClientXpicgdb );
	}

	
	//-------------------------------------------
	// clean up openocd
	//-------------------------------------------
	
	#ifdef GDB_SWITCH_WINDOWS
	// clear addrinfo for openocd
	if ( paddrinfoOpenocd != NULL )
	{
		freeaddrinfo( paddrinfoOpenocd );
		paddrinfoOpenocd = NULL;
	}
	#endif

	// close client socket for openocd
	if ( socketClientOpenocd != INVALID_SOCKET )
	{
		closesocket( socketClientOpenocd );
	}
	
	#ifdef GDB_SWITCH_WINDOWS
	WSACleanup();
	#endif
	netx10gs_helper_log( LOG_VERBOSE, "all socket resources cleaned." );
	netx10gs_helper_log( LOG_DEBUG, "sockets_CleanUp - end" );
}
//==============================================================================


//==============================================================================
// NETX10GS_HELPER_HANDLESOCKETERROR
//==============================================================================
///
/// @brief Checks if a socket error is WSAEWOULDBLOCK or not.
///
/// @return Errorcode.
///
//==============================================================================
char netx10gs_sockets_HandleSocketError( void )
{
	int
		iErrorValue = 0x0;
	
	
	netx10gs_helper_log( LOG_HANDLESOCKETERROR, "sockets_HandleSocketError - start" );
	

	iErrorValue = WSAGetLastError();
	// netx10gs_helper_log( LOG_HANDLESOCKETERROR, "error: %i", iErrorValue );
	// iErrorValue = WSAEWOULDBLOCK;

	
	if ( iErrorValue != WSAEWOULDBLOCK )
	{
		netx10gs_helper_log( LOG_ERROR, "socket error: %i", iErrorValue );
		#ifdef GDB_SWITCH_WINDOWS
		switch ( iErrorValue )
		{
			case WSA_INVALID_HANDLE:
				netx10gs_helper_log( LOG_ERROR, "WSA_INVALID_HANDLE" );
				break;
			
			case WSA_NOT_ENOUGH_MEMORY:
				netx10gs_helper_log( LOG_ERROR, "WSA_NOT_ENOUGH_MEMORY" );
				break;
			
			case WSA_INVALID_PARAMETER:
				netx10gs_helper_log( LOG_ERROR, "WSA_INVALID_PARAMETER" );
				break;
			
			case WSA_OPERATION_ABORTED:
				netx10gs_helper_log( LOG_ERROR, "WSA_OPERATION_ABORTED" );
				break;
			
			case WSA_IO_INCOMPLETE:
				netx10gs_helper_log( LOG_ERROR, "WSA_IO_INCOMPLETE" );
				break;
			
			case WSA_IO_PENDING:
				netx10gs_helper_log( LOG_ERROR, "WSA_IO_PENDING" );
				break;
			
			case WSAEINTR:
				netx10gs_helper_log( LOG_ERROR, "WSAEINTR" );
				break;
			
			case WSAEBADF:
				netx10gs_helper_log( LOG_ERROR, "WSAEBADF" );
				break;
			
			case WSAEACCES:
				netx10gs_helper_log( LOG_ERROR, "WSAEACCES" );
				break;
			
			case WSAEFAULT:
				netx10gs_helper_log( LOG_ERROR, "WSAEFAULT" );
				break;
			
			case WSAEINVAL:
				netx10gs_helper_log( LOG_ERROR, "WSAEINVAL" );
				break;
			
			case WSAEMFILE:
				netx10gs_helper_log( LOG_ERROR, "WSAEMFILE" );
				break;
			
			case WSAEWOULDBLOCK:
				netx10gs_helper_log( LOG_ERROR, "WSAEWOULDBLOCK" );
				break;
			
			case WSAEINPROGRESS:
				netx10gs_helper_log( LOG_ERROR, "WSAEINPROGRESS" );
				break;
			
			case WSAEALREADY:
				netx10gs_helper_log( LOG_ERROR, "WSAEALREADY" );
				break;
			
			case WSAENOTSOCK:
				netx10gs_helper_log( LOG_ERROR, "WSAENOTSOCK" );
				break;
			
			case WSAEDESTADDRREQ:
				netx10gs_helper_log( LOG_ERROR, "WSAEDESTADDRREQ" );
				break;
			
			case WSAEMSGSIZE:
				netx10gs_helper_log( LOG_ERROR, "WSAEMSGSIZE" );
				break;
			
			case WSAEPROTOTYPE:
				netx10gs_helper_log( LOG_ERROR, "WSAEPROTOTYPE" );
				break;
			
			case WSAENOPROTOOPT:
				netx10gs_helper_log( LOG_ERROR, "WSAENOPROTOOPT" );
				break;
			
			case WSAEPROTONOSUPPORT:
				netx10gs_helper_log( LOG_ERROR, "WSAEPROTONOSUPPORT" );
				break;
			
			case WSAESOCKTNOSUPPORT:
				netx10gs_helper_log( LOG_ERROR, "WSAESOCKTNOSUPPORT" );
				break;
			
			case WSAEOPNOTSUPP:
				netx10gs_helper_log( LOG_ERROR, "WSAEOPNOTSUPP" );
				break;
			
			case WSAEPFNOSUPPORT:
				netx10gs_helper_log( LOG_ERROR, "WSAEPFNOSUPPORT" );
				break;
			
			case WSAEAFNOSUPPORT:
				netx10gs_helper_log( LOG_ERROR, "WSAEAFNOSUPPORT" );
				break;
			
			case WSAEADDRINUSE:
				netx10gs_helper_log( LOG_ERROR, "WSAEADDRINUSE" );
				break;
			
			case WSAEADDRNOTAVAIL:
				netx10gs_helper_log( LOG_ERROR, "WSAEADDRNOTAVAIL" );
				break;
			
			case WSAENETDOWN:
				netx10gs_helper_log( LOG_ERROR, "WSAENETDOWN" );
				break;
			
			case WSAENETUNREACH:
				netx10gs_helper_log( LOG_ERROR, "WSAENETUNREACH" );
				break;
			
			case WSAENETRESET:
				netx10gs_helper_log( LOG_ERROR, "WSAENETRESET" );
				break;
			
			case WSAECONNABORTED:
				netx10gs_helper_log( LOG_ERROR, "WSAECONNABORTED" );
				break;
			
			case WSAECONNRESET:
				netx10gs_helper_log( LOG_ERROR, "WSAECONNRESET" );
				break;
			
			case WSAENOBUFS:
				netx10gs_helper_log( LOG_ERROR, "WSAENOBUFS" );
				break;
			
			case WSAEISCONN:
				netx10gs_helper_log( LOG_ERROR, "WSAEISCONN" );
				break;
			
			default:
				netx10gs_helper_log( LOG_ERROR, "see list" );
				break;
		}
		#endif
		
		return GDB_SWITCH_ERROR;
	}
	else
	{
		netx10gs_helper_log( LOG_HANDLESOCKETERROR, "sockets_HandleSocketError - WSAEWOULDBLOCK" );
		netx10gs_helper_log( LOG_HANDLESOCKETERROR, "sockets_HandleSocketError - end" );
		return GDB_SWITCH_NOTHING_RECEIVED; // XXX value?
	}
}
//==============================================================================


//==============================================================================
// NETX10GS_HELPER_SEND
//==============================================================================
///
///	@brief Sends a message to socket cSocket.
///
/// @param [in] cSocket socket of receiver.
/// @param [in] pcGdbMsg the message to be send.
/// @param [in] pcErrorMsg error message if an error occures.
///
/// @return Error code.
///
//==============================================================================
void netx10gs_sockets_Send( char cSocket, char *pcGdbMsg, char *pcErrorMsg )
{
	SOCKET socketReceiver = INVALID_SOCKET;
	
	netx10gs_helper_log( LOG_SEND, "sockets_Send - start" );
	netx10gs_helper_log( LOG_SEND, "sockets_Send - config:\nsocket: %i (o: %i, x: %i, a: %i)\nbuffer: %s", cSocket, socketClientOpenocd, socketClientXpicgdb, socketClientArmgdb, pcGdbMsg );
	
	switch ( cSocket )
	{
		case SOCKET_OPENOCD:
			socketReceiver = socketClientOpenocd;
			break;
		
		case SOCKET_XPICGDB:
			socketReceiver = socketClientXpicgdb;
			break;
		
		case SOCKET_ARMGDB:
			socketReceiver = socketClientArmgdb;
			break;
		
		default:
			socketReceiver = INVALID_SOCKET;
			break;
	}
		
	if ( socketReceiver == INVALID_SOCKET )
	{
		netx10gs_helper_log( LOG_ERROR, "sockets_Send - invalid socket!" );
		netx10gs_helper_log( LOG_ERROR, "%s", pcErrorMsg );

		netx10gs_sockets_ResetSession();
		
		// #ifdef GDB_SWITCH_WINDOWS
		// ExitThread( dwExitCodeHandleGdbSession );
		// #endif
		// #ifdef GDB_SWITCH_LINUX
		// pthread_exit( &thread_retval );
		// #endif
	}
	
	if ( pcGdbMsg == NULL )
	{
		netx10gs_helper_log( LOG_ERROR, "sockets_Send - message is NULL!" );
		netx10gs_helper_log( LOG_ERROR, "%s", pcErrorMsg );

		netx10gs_sockets_ResetSession();
		
		// #ifdef GDB_SWITCH_WINDOWS
		// ExitThread( dwExitCodeHandleGdbSession );
		// #endif
		// #ifdef GDB_SWITCH_LINUX
		// pthread_exit( &thread_retval );
		// #endif
	}
	
	// sending msg, catch error
	if ( send( socketReceiver, pcGdbMsg, (int) strlen( pcGdbMsg ), 0 ) == SOCKET_ERROR )
	{
		netx10gs_sockets_ResetSession();
		
		// netx10gs_sockets_HandleSocketError();
		// netx10gs_helper_log( LOG_ERROR, "%s", pcErrorMsg );
		// #ifdef GDB_SWITCH_WINDOWS
		// ExitThread( dwExitCodeHandleGdbSession );
		// #endif
		// #ifdef GDB_SWITCH_LINUX
		// pthread_exit( &thread_retval );
		// #endif
	}
	
	netx10gs_helper_log( LOG_SEND, "sockets_Send - end" );
}
//==============================================================================


//==============================================================================
// NETX10GS_HELPER_RECEIVE
//==============================================================================
///
///	@brief Receives a message from socket socketReceiver.
///
/// @param [in] socketReceiver socket of sender.
/// @param [in] pcBuffer buffer for the message.
/// @param [in] uiBufferLength size of buffer.
///
/// @return Error code.
///
//==============================================================================
char netx10gs_sockets_Receive( char cSocket, char *pcBuffer, unsigned int uiBufferLength )
{
	char
		cSignal = 0x0,
		cTestCnt = 0x0,
		acTempBuffer[DEFAULT_BUFFER_LEN];
	
	int
		j,k,
		iRetval = 0x0;
	
	SOCKET socketReceiver = INVALID_SOCKET;
	
	netx10gs_helper_log( LOG_RECEIVE, "sockets_Receive - start" );
	
	switch ( cSocket )
	{
		case SOCKET_OPENOCD:
			socketReceiver = socketClientOpenocd;
			break;
		
		case SOCKET_XPICGDB:
			socketReceiver = socketClientXpicgdb;
			break;
		
		case SOCKET_ARMGDB:
			socketReceiver = socketClientArmgdb;
			break;
		
		default:
			socketReceiver = INVALID_SOCKET;
			break;
	}
	
	if ( socketReceiver == INVALID_SOCKET )
	{
		netx10gs_helper_log( LOG_ERROR, "sockets_Receive - invalid socket!" );
		return GDB_SWITCH_ERROR;
	}
	
	iRetval = recv( socketReceiver, acTempBuffer, uiBufferLength, 0 );
	if ( iRetval == SOCKET_ERROR )
	{
		netx10gs_helper_log( LOG_RECEIVE, "sockets_Receive - end" );
		return netx10gs_sockets_HandleSocketError();
	}
	
	if ( iRetval == 0 )
	{
		netx10gs_helper_log( LOG_RECEIVE, "sockets_Receive - nothing received" );
		netx10gs_helper_log( LOG_RECEIVE, "sockets_Receive - end" );
		return GDB_SWITCH_NOTHING_RECEIVED;
	}
	
	acTempBuffer[iRetval] = 0;
	strcpy_s( pcBuffer, DEFAULT_BUFFER_LEN, acTempBuffer );
	netx10gs_helper_log( LOG_RECEIVE, "sockets_Receive - config:\nsocket: %i (o: %i, x: %i, a: %i)\nbuffer: %s\nbufferlength: %i", socketReceiver, socketClientOpenocd, socketClientXpicgdb, socketClientArmgdb, pcBuffer, uiBufferLength );
	
	// check if gdb msg is devided in two or more socket transfers, if so concat both msgs
	for ( j = 0x0; j < DEFAULT_BUFFER_LEN; j++ )
	{
		if ( acTempBuffer[j] == 0x0 ) break;
		
		if ( acTempBuffer[j] == '$' )
		{
			cSignal = 0x1;
			break;
		}
	}
	
	if ( cSignal == 0x1 )
	{
		cSignal = 0x0;
		
		for ( j = 0x0; j < 0xa; j++ )
		{
			// search for '#' in buffer
			for ( k = 0; k < DEFAULT_BUFFER_LEN; k++ )
			{
				if ( acTempBuffer[k] == 0x0 )
				{
					break;
				}
				
				if ( acTempBuffer[k] == '#' )
				{
					cSignal = 0x1;
					break;
				}
			}
			
			if ( cSignal == 0x0 )
			{
				cTestCnt++;
				iRetval =  recv( socketReceiver, acTempBuffer, uiBufferLength, 0 );
				acTempBuffer[iRetval] = 0;
				// netx10gs_helper_log( LOG_ERROR, "\n\n\nda ist er ja: %i\n\n\n", cTestCnt );
				strcat_s( pcBuffer, DEFAULT_BUFFER_LEN, acTempBuffer );
			}
			else
			{
				break;
			}
		}
	}
	
	
	
	if ( ( ! g_bNoAckModeStartedXpic ) && ( socketReceiver == socketClientXpicgdb ) && ( pcBuffer[0] != GDB_NEGACK ) )
	{
		if ( send( socketClientXpicgdb, "+", (int) strlen( "+" ), 0 ) == SOCKET_ERROR )
		{
			netx10gs_helper_log( LOG_ERROR, "sockets_Receive - could not send!" );
			return GDB_SWITCH_ERROR;
		}
	}
	
	if ( ( ! g_bNoAckModeStartedArm ) && ( socketReceiver == socketClientArmgdb ) && ( pcBuffer[0] != GDB_NEGACK ) )
	{
		if ( send( socketClientArmgdb, "+", (int) strlen( "+" ), 0 ) == SOCKET_ERROR )
		{
			netx10gs_helper_log( LOG_ERROR, "sockets_Receive - could not send!" );
			return GDB_SWITCH_ERROR;
		}
	}
	
	netx10gs_helper_log( LOG_RECEIVE, "sockets_Receive - end" );
	return GDB_SWITCH_OK;
}
//==============================================================================


//==============================================================================
// NETX10GS_HELPER_SENDANDRECEIVE
//==============================================================================
///
///	@brief Receives a message from socket socketReceiver.
///
/// @param [in] socketReceiver socket of sender.
/// @param [in] pcBuffer buffer for the message.
/// @param [in] uiBufferLength size of buffer.
///
//==============================================================================
void netx10gs_sockets_SendAndReceive( char cSocketReceiver, char *pcGdbMsg, char *pcBuffer, unsigned int uiBufferLength, char cReturnAnswer, char cSocketSender, char cWaitForOk, char *pcErrorMsg )
{

	char
		cSendAgainCnt = 0x0;
	
	int
		i = 0x0,
		j = 0x0,
		k = 0x0,
		iRetval = 0;
	
	struct gdbCommand structGdbCommand;
	
	SOCKET socketReceiver = INVALID_SOCKET;
	SOCKET socketSender = INVALID_SOCKET;
	
	netx10gs_helper_log( LOG_SENDANDRECEIVE, "sockets_SendAndReceive - start" );
	
	switch ( cSocketReceiver )
	{
		case SOCKET_OPENOCD:
			socketReceiver = socketClientOpenocd;
			break;
		
		case SOCKET_XPICGDB:
			socketReceiver = socketClientXpicgdb;
			break;
		
		case SOCKET_ARMGDB:
			socketReceiver = socketClientArmgdb;
			break;
		
		default:
			socketReceiver = INVALID_SOCKET;
			break;
	}
	
	switch ( cSocketSender )
	{
		case SOCKET_OPENOCD:
			socketSender = socketClientOpenocd;
			break;
		
		case SOCKET_XPICGDB:
			socketSender = socketClientXpicgdb;
			break;
		
		case SOCKET_ARMGDB:
			socketSender = socketClientArmgdb;
			break;
		
		default:
			socketSender = INVALID_SOCKET;
			break;
	}
	
	netx10gs_helper_log( LOG_SENDANDRECEIVE, "sockets_SendAndReceive - config:\nsocket: %i (o: %i, x: %i, a: %i)\nbuffer: %s\nbufferlength: %i\ncReturnAnswer: %i\nsender: %i\ncWaitForOk: %i", socketReceiver, socketClientOpenocd, socketClientXpicgdb, socketClientArmgdb, pcGdbMsg, uiBufferLength, cReturnAnswer, socketSender, cWaitForOk );
	
Send_Again:

	i = 0x0;
	j = 0x0;
	k = 0x0;
	iRetval = 0;
	
	// sending msg
	if ( send( socketReceiver, pcGdbMsg, (int) strlen( pcGdbMsg ), 0 ) == SOCKET_ERROR )
	{
		netx10gs_helper_log( LOG_ERROR, "sockets_SendAndReceive - error while sending" );
		netx10gs_helper_log( LOG_ERROR, "%s", pcErrorMsg );
		
		netx10gs_sockets_ResetSession();
		
		// #ifdef GDB_SWITCH_WINDWOS
		// ExitThread( dwExitCodeHandleGdbSession );
		// #endif
		// #ifdef GDB_SWITCH_LINUX
		// pthread_exit( &thread_retval );
		// #endif
	}
	
	
	while ( j == 0 )
	{
	
		if ( cWaitForOk == GDB_DONT_WAIT_FOR_OK )
		{
			j = 1;
		}
		
		// if answer is ACK '+' recv again | XXX check only 3 times?
		for ( i = 0; i < 3; i++ )
		{
			// receive answer from receiver
			iRetval = recv( socketReceiver, pcBuffer, uiBufferLength, 0 );
			if ( iRetval == SOCKET_ERROR )
			{
				netx10gs_helper_log( LOG_ERROR, "sockets_SendAndReceive - error while receiving" );
				netx10gs_helper_log( LOG_ERROR, "%s", pcErrorMsg );
				
				netx10gs_sockets_ResetSession();
				
				// #ifdef GDB_SWITCH_WINDWOS
				// ExitThread( dwExitCodeHandleGdbSession );
				// #endif
				// #ifdef GDB_SWITCH_LINUX
				// pthread_exit( &thread_retval );
				// #endif
			}
			pcBuffer[iRetval] = 0;
			
			netx10gs_helper_log( LOG_SENDANDRECEIVE, "sockets_SendAndReceive - received: %s", pcBuffer );
			netx10gs_handlegdbproto_GetGdbCmd( pcBuffer, &structGdbCommand );
			
			if ( structGdbCommand.cCmd == GDB_CMD_NEGACK )
			{
				cSendAgainCnt++;
				if ( cSendAgainCnt > 3 )
				{
					netx10gs_helper_log( LOG_ERROR, "%s", pcErrorMsg );
					#ifdef GDB_SWITCH_WINDWOS
					ExitThread( dwExitCodeHandleGdbSession );
					#endif
					#ifdef GDB_SWITCH_LINUX
					pthread_exit( &thread_retval );
					#endif
				}
				goto Send_Again;
			}
			
			if ( ( socketSender == socketClientXpicgdb ) && ( structGdbCommand.cCmd == GDB_CMD_T ) )
			{
				if ( send( socketClientArmgdb, buffer, (int) strlen( buffer ), 0 ) == SOCKET_ERROR )
				{
					netx10gs_helper_log( LOG_ERROR, "%s", pcErrorMsg );
					
					netx10gs_sockets_ResetSession();
					
					// #ifdef GDB_SWITCH_WINDWOS
					// ExitThread( dwExitCodeHandleGdbSession );
					// #endif
					// #ifdef GDB_SWITCH_LINUX
					// pthread_exit( &thread_retval );
					// #endif
				}
				continue;
			}
			
			// return answer to sender
			if ( cReturnAnswer == GDB_RETURN_ANSWER )
			{
				if ( testarm && ! g_bNoAckModeStartedArm )
				{
					iRetval = send( socketSender, "+", (int) strlen( "+" ), 0 );
				}

				//--------------------------------------------------------------
				// handle more than one msg in one transfer, e.g.: $x#xx$x#xx
				//--------------------------------------------------------------
				k = 0x0;
				for ( i = 0x0; i < DEFAULT_BUFFER_LEN; i++ )
				{
					
					if ( pcBuffer[i] == '#' )
					{
						if ( pcBuffer[i + 3] == 0x0 )
						{
							iRetval = send( socketSender, pcBuffer, (int) strlen( pcBuffer ), 0 );
							break;
						}
						else
						{
							iRetval = send( socketSender, &pcBuffer[k], i + 2, 0 );
							k = i + 3;
							i = k;
						}
					}
					
					if ( ( pcBuffer[i] == '+' ) && ( pcBuffer[i + 1] == 0x0 ) )
					{
						iRetval = send( socketSender, pcBuffer, (int) strlen( pcBuffer ), 0 );
						break;
					}
				}
				//--------------------------------------------------------------
				
				// catch error and print error message
				if ( iRetval == SOCKET_ERROR )
				{
					netx10gs_helper_log( LOG_ERROR, "%s", pcErrorMsg );
					
					netx10gs_sockets_ResetSession();
					
					// #ifdef GDB_SWITCH_WINDWOS
					// ExitThread( dwExitCodeHandleGdbSession );
					// #endif
					// #ifdef GDB_SWITCH_LINUX
					// pthread_exit( &thread_retval );
					// #endif
				}				
			}
			
			// if answer is not a single ACK '+' | e.g. +$XX#XX or $XX#XX
			if ( structGdbCommand.cCmd != GDB_CMD_ACK )
			{
				break;
			}
		} // end for
		
		// if answer is GDB_OK
		if ( netx10gs_helper_GdbCmp( &pcBuffer[k], GDB_OK ) )
		{
			break;
		}
		
	} // end while
	
	netx10gs_helper_log( LOG_SENDANDRECEIVE, "sockets_SendAndReceive - end" );
}
//==============================================================================


//==============================================================================
// NETX10GS_SOCKETS_IOCTLSOCKET
//==============================================================================
///
///	@brief activates or deactivates blocking mode for openocd client socket.
///
/// @param [in] ulModeValue mode: BLOCKING or NON_BLOCKING.
///
//==============================================================================
void netx10gs_sockets_IoctlSocket( unsigned long ulModeValue )
{
	unsigned long
			ulMode = ulModeValue;
	
	#ifdef GDB_SWITCH_WINDOWS
	ioctlsocket( socketClientOpenocd, FIONBIO, &ulMode );
	#endif
}
//==============================================================================


//==============================================================================
// NETX10GS_HELPER_SENDKEEPALIVE
//==============================================================================
///
///	@brief Sends a keepalive packet to OpenOCD.
///
/// @return Error code.
///
//==============================================================================
char netx10gs_sockets_SendKeepAlive( void )
{
	netx10gs_helper_log( LOG_SENDKEEPALIVE, "sockets_SendKeepAlive - start" );
	
	netx10gs_sockets_Send( SOCKET_OPENOCD, GDB_KEEP_ALIVE, "sockets_SendKeepAlive - could not send keep alive!" );

	netx10gs_helper_log( LOG_SENDKEEPALIVE, "sockets_SendKeepAlive - end" );
	return GDB_SWITCH_OK;
}
