/*
 *******************************************************************************
 *                                                                             *
 * netx10gs_handlexpic.h                                                       *
 *                                                                             *
 * author: Daniel Kramlinger                                                   *
 *         Hilscher Gesellschaft für Systemautomation mbH                      *
 * date: 15.04.2011                                                            *
 *                                                                             *
 *******************************************************************************
*/

#ifndef NETX10GS_HANDLEXPIC_H
#define NETX10GS_HANDLEXPIC_H


void netx10gs_handlexpic_HaltXpic( void );
void netx10gs_handlexpic_ResumeXpic( void );
void netx10gs_handlexpic_StepBackPC( unsigned int uiValue );
char netx10gs_handlexpic_ReadXpicRegisters( void );
char netx10gs_handlexpic_SetSoftwareBreakpoint( unsigned int uiAddress );
void netx10gs_handlexpic_RemoveSoftwareBreakpoint( unsigned int uiAddress );
char netx10gs_handlexpic_SetHardwareBreakpoint( unsigned int uiAddress );
char netx10gs_handlexpic_RemoveHardwareBreakpoint( void );
void netx10gs_handlexpic_SinglestepXpic( void );
char netx10gs_handlexpic_GetAndReturnAccessKey( void );
char netx10gs_handlexpic_ActivateXpicClock( void );
char netx10gs_handlexpic_ResetXpic( void );
char netx10gs_handlexpic_InitNetx10( void );
char netx10gs_handlexpic_ReadSingleXpicRegister( unsigned char ucRegister );
char netx10gs_handlexpic_WriteSingleXpicRegister( unsigned char ucRegister );
char netx10gs_handlexpic_CheckForSinglestep( char *pcSendSigtrap );
char netx10gs_handlexpic_CheckForBreakpoint( void );

#endif
