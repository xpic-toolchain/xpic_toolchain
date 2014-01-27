/*
 *******************************************************************************
 *                                                                             *
 * netx10gs_helper.h                                                           *
 *                                                                             *
 * author: Daniel Kramlinger                                                   *
 *         Hilscher Gesellschaft für Systemautomation mbH                      *
 * date: 15.04.2011                                                            *
 *                                                                             *
 *******************************************************************************
*/


#ifndef NETX10GS_HELPER_H
#define NETX10GS_HELPER_H

#include "netx10gs_globals.h"

void netx10gs_helper_CheckArguments( int argc, char **argv );

char netx10gs_helper_InitGlobals( void );

char netx10gs_helper_Char2Hex( unsigned char ucVal );

void netx10gs_helper_GdbVal2UnsignedInt( char *pcBuffer, struct structUiValue_t *pstructUiValue );

char netx10gs_helper_CheckAnswer( char *pcBuffer );

char netx10gs_helper_CalculateChecksum( char *pcBuffer, unsigned int uiBufferSize );

char netx10gs_helper_BuildGdbMsg( char *pcBuffer, unsigned int uiBufferSize, char *pcStartMsg, char *pcEndMsg, char cValueType, int iValue, char cCalcChecksum );

void netx10gs_helper_log( char cModus, const char *format, ... );

char netx10gs_helper_GdbCmp( char pcBuffer[], char *pcMsg );

#endif
