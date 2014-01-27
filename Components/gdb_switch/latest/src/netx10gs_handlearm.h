/*
 *******************************************************************************
 *                                                                             *
 * netx10gs_handlearm.h                                                        *
 *                                                                             *
 * author: Daniel Kramlinger                                                   *
 *         Hilscher Gesellschaft für Systemautomation mbH                      *
 * date: 18.03.2011                                                            *
 *                                                                             *
 *******************************************************************************
*/

#ifndef NETX10GS_HANDLEARM_H
#define NETX10GS_HANDLEARM_H

void netx10gs_handlearm_ControlArm( char value );
char netx10gs_handlearm_CheckArmRunning( void );

#endif
