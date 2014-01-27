/*
 *******************************************************************************
 *                                                                             *
 * netx10gs_handlegdbproto.h                                                   *
 *                                                                             *
 * author: Daniel Kramlinger                                                   *
 *         Hilscher Gesellschaft für Systemautomation mbH                      *
 * date: 22.03.2011                                                            *
 *                                                                             *
 *******************************************************************************
*/


#ifndef NETX10GS_HANDLEGDBPROTO_H
#define NETX10GS_HANDLEGDBPROTO_H

void netx10gs_handlegdbproto_GetGdbCmd( char *pcBuffer, struct gdbCommand *pGdbCommand );
char netx10gs_handlegdbproto_Arm( void );
char netx10gs_handlegdbproto_Xpic( void );

#endif
