/*
 *******************************************************************************
 *                                                                             *
 * tester_socket.h                                                             *
 *                                                                             *
 * author: Daniel Kramlinger                                                   *
 *         Hilscher Gesellschaft für Systemautomation mbH                      *
 * date: 11.02.2011                                                            *
 *                                                                             *
 *******************************************************************************
*/

#ifndef TESTER_SOCKET_H
#define TESTER_SOCKET_H

#ifndef WIN32_LEAN_AND_MEAN
	#define WIN32_LEAN_AND_MEAN
#endif
	
#include <winsock2.h>
#include <ws2tcpip.h>


SOCKET
	socketServerOpenocd,
	socketClientOpenocd,
	socketClientXpicgdb;

void InitSockets( void );
char InitOpenocdConnection( void );
// char InitGdbSwitchConnection( void );
char ConnectToGdbSwitch( void );
void CleanUp( void );
char TesterSend( SOCKET socketReceiver, const char* cpcMsg );
char TesterReceive( SOCKET socketSender );

#endif