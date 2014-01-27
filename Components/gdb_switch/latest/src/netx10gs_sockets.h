/*
 *******************************************************************************
 *                                                                             *
 * netx10gs_sockets.h                                                          *
 *                                                                             *
 * author: Daniel Kramlinger                                                   *
 * Hilscher Gesellschaft für Systemautomation mbH                              *
 * date: 15.04.2011                                                            *
 *                                                                             *
 *******************************************************************************
*/



#ifndef NETX10GS_SOCKETS_H
#define NETX10GS_SOCKETS_H
void netx10gs_sockets_ResetSession( void );
void netx10gs_sockets_InitSockets( void );

char netx10gs_sockets_InitConnectionArmgdb( void );

char netx10gs_sockets_InitConnectionXpicgdb( void );

char netx10gs_sockets_ConnectToArmgdb( void );
char netx10gs_sockets_ConnectToXpicgdb( void );
char netx10gs_sockets_ConnectToOpenocd( void );

void netx10gs_sockets_CleanUpArm( void );
void netx10gs_sockets_CleanUpXpic( void );
void netx10gs_sockets_CleanUp( void );

char netx10gs_sockets_HandleSocketError( void );
void netx10gs_sockets_Send( char cSocket, char *pcGdbMsg, char *pcErrorMsg );
char netx10gs_sockets_Receive( char cSocket, char *pcBuffer, unsigned int uiBufferLength );
void netx10gs_sockets_SendAndReceive( char cSocketReceiver, char *pcGdbMsg, char *pcBuffer, unsigned int uiBufferLength, char cReturnAnswer, char cSocketSender, char cWaitForOk, char *pcErrorMsg );
void netx10gs_sockets_IoctlSocket( unsigned long ulModeValue );
char netx10gs_sockets_SendKeepAlive( void );

#endif
