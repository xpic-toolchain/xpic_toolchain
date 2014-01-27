/*
 *******************************************************************************
 *                                                                             *
 * netx10gs_helper.c                                                           *
 *                                                                             *
 * author: Daniel Kramlinger                                                   *
 *         Hilscher Gesellschaft für Systemautomation mbH                      *
 * date: 14.04.2011                                                            *
 *                                                                             *
 *******************************************************************************
*/


#include <stdio.h>
#ifdef GDB_SWITCH_LINUX
#include <stdarg.h>
#endif

#include "netx10gs_helper.h"
#include "netx10gs_globals.h"
#include "netx10gs_gdbproto.h"
#include "netx10gs_handlegdbproto.h"
#include "netx10gs_sockets.h"


#define DEFAULT_OPENOCD_PORT	"3333" ///< @brief Port 3333 is the default port for the netX GDB switch to connect to OpenOCD
#define DEFAULT_ARMGDB_PORT 	"3334" ///< @brief Port 3334 is the default port for the netX GDB switch to listen on for ARM-GDB
#define DEFAULT_XPICGDB_PORT	"3335" ///< @brief Port 3333 is the default port for the netX GDB switch to listen on for XPIC-GDB


#ifdef GDB_SWITCH_LINUX
void strcpy_s( char *pcBuffer, int iCnt, char *pcString )
{
	strcpy( pcBuffer, pcString );
}

int _strnicmp( char *pcString1, char *pcString2, int cnt )
{
	return strncmp( pcString1, pcString2, cnt );
}

int strcat_s( char *pcBuffer, int dummy, char *pcString )
{
	strcat( pcBuffer, pcString );
	return 0; // hack!
}

int strncat_s( char *pcBuffer, int dummy, char *pcString, int cnt )
{
	strncat( pcBuffer, pcString, cnt );
	return 0; // hack!
}
#endif

//******************************************************************************
// LOCAL FUNCTIONS                                                             *
//******************************************************************************


//==============================================================================
// NETX10GS_HELPER_USAGE
//==============================================================================
///
/// @brief Prints the usage message and quits the switch.
///
//==============================================================================
void netx10gs_helper_Usage()
{
	fprintf(stderr,"Usage\n%s [-a port] [-x port] [-o port]\n"
			"Where:\n"
			"\t-a port   - port to listen on for arm-gdb\n"
			"\t-x port   - port to listen on for xpic-gdb\n"
			"\t-o port   - port to connect to openocd\n"
			"\t-v        - activate verbose mode\n"
			"\n"
			"Defaults are openocd:3333, arm-gdb:3334, xpic-gdb:3335; verbose mode off\n",
			g_pcProgname
			);
	exit(1);
}
//==============================================================================


//==============================================================================
// NETX10GS_HELPER_CHECKPORTS
//==============================================================================
///
///	@brief Checks the port values. If one of them is not correct, usage will
///			be printed and the gdb switch closed.
///
//==============================================================================
void netx10gs_helper_CheckPorts( void )
{
	// check given ports	
	if ( ( _strnicmp(g_pcArmgdbPort, "0", 1) == 0) || ( _strnicmp(g_pcXpicgdbPort, "0", 1) == 0) || ( _strnicmp(g_pcOpenocdPort, "0", 1) == 0) )
	{
		netx10gs_helper_Usage();
	}
	
	if ( (strcmp( g_pcOpenocdPort, g_pcArmgdbPort ) == 0) || (strcmp( g_pcOpenocdPort, g_pcXpicgdbPort ) == 0) || (strcmp( g_pcArmgdbPort, g_pcXpicgdbPort) == 0 ) )
	{
		fprintf( stderr, "ERROR: CheckPorts() - do not use same portnumber for xpicgdb and armgdb and openocd!\n" );
		netx10gs_helper_Usage();
	}
	
	if ( g_bVerbose )
	{
		fprintf( stdout, "ports: armgdb:%s, xpicgdb:%s, openocd:%s\n", g_pcArmgdbPort, g_pcXpicgdbPort, g_pcOpenocdPort );
	}
}
//==============================================================================



//******************************************************************************
// GLOBAL FUNCTIONS                                                            *
//******************************************************************************


//==============================================================================
// NETX10GS_HELPER_LOG
//==============================================================================
///
/// @brief Controls the output of debug/verbose/error messages.
///
/// @param [in] cModus switches between debug/verbose/error output.
/// @param [in] format variable parameter vector.
///
//==============================================================================
void netx10gs_helper_log( char cModus, const char *format, ... )
{
	va_list vargzeiger;
	va_start( vargzeiger, format );
	
	cModus = cModus;

	switch ( cModus )
	{
		#ifdef GDB_SWITCH_DEBUG
		case LOG_DEBUG:
			fprintf(  stdout, "DEBUG: " );
			vfprintf( stdout, format, vargzeiger );
			fprintf(  stdout, "\n" );
			break;
		#endif
		
		case LOG_VERBOSE:
			if ( g_bVerbose )
			{
				fprintf(  stdout, "MSG  : " );
				vfprintf( stdout, format, vargzeiger );
				fprintf(  stdout, "\n" );
			}
			break;
		
		#ifndef	GDB_TESTER
		case LOG_ERROR:
			fprintf(  stderr, "ERROR: " );
			vfprintf( stderr, format, vargzeiger );
			fprintf(  stderr, "\n" );
			break;
		#endif
		
		case LOG_DONT_SHOW:
			break;
	}

}
//==============================================================================



//==============================================================================
// NETX10GS_HELPER_INITGLOBALS
//==============================================================================
///
/// @brief Initializes global variables.
///
/// @return Errorcode.
///
//==============================================================================
char netx10gs_helper_InitGlobals( void )
{
	g_cLog						= GDB_NO_LOGGING;
	g_bVerbose					= FALSE;
	g_bExitGdbSwitch			= FALSE;
	g_bXpicInitFinished			= FALSE;
	g_bXpicThreadInitialized	= FALSE;
	g_bClockActivated			= FALSE;
	g_bCheckForBreakpoint		= FALSE;
	g_bCheckForSinglestep		= FALSE;
	g_bResetXpicSoftBreakIrq	= FALSE;
	g_bArmInitFinished			= FALSE;
	g_bArmRunnig				= FALSE;
	g_bCheckForArmBreakpoint	= FALSE;
	g_bNoAckModeStartedXpic		= FALSE;
	g_bNoAckModeStartedArm		= FALSE;
	g_bNoAckModeStartedOpenocd	= FALSE;
	g_bNoAckModeActivated		= FALSE;
	g_bDoSingleStepAfterBreakpoint = FALSE;
	
	g_cArmgdbConnected			= GDB_NOT_CONNCECTED;
	g_cXpicgdbConnected			= GDB_NOT_CONNCECTED;
	g_cOpenocdConnected			= GDB_NOT_CONNCECTED;
	
	g_pcArmgdbPort  = DEFAULT_ARMGDB_PORT,
	g_pcXpicgdbPort = DEFAULT_XPICGDB_PORT,
	g_pcOpenocdPort = DEFAULT_OPENOCD_PORT;
	
	armgdb_buflen  = DEFAULT_BUFFER_LEN;
	xpicgdb_buflen = DEFAULT_BUFFER_LEN;
	
	strcpy_s( acHexchars, 17, "0123456789abcdef" );
	
	#ifdef GDB_SWITCH_WINDOWS
	// init critical section variables WINDWOS
	if ( ! InitializeCriticalSectionAndSpinCount( &criticalSectionArmgdb, 0x80000400 ) )
	{
		return GDB_SWITCH_ERROR;
	}

	if ( ! InitializeCriticalSectionAndSpinCount( &criticalSectionXpicgdb, 0x80000400 ) )
	{
		return GDB_SWITCH_ERROR;
	}
	
	if ( ! InitializeCriticalSectionAndSpinCount( &criticalSectionOpenocd, 0x80000400 ) )
	{
		return GDB_SWITCH_ERROR;
	}

	if ( ! InitializeCriticalSectionAndSpinCount( &criticalSectionGdbSwitch, 0x80000400 ) )
	{
		return GDB_SWITCH_ERROR;
	}
	#endif
	
	#ifdef GDB_SWITCH_LINUX
	// init critical section variables LINUX
 
	// criticalSectionGdbSwitch = PTHREAD_MUTEX_INITIALIZER;
	// criticalSectionArmgdb = PTHREAD_MUTEX_INITIALIZER;
	// criticalSectionXpicgdb = PTHREAD_MUTEX_INITIALIZER;
	// criticalSectionOpenocd = PTHREAD_MUTEX_INITIALIZER;
	#endif

	return GDB_SWITCH_OK;
}
//==============================================================================


//==============================================================================
// NETX10GS_HELPER_CHECKARGUMENTS
//==============================================================================
///
/// @brief Checks the gdb-switch start arguments.
///
/// @param [in] argc argument counter.
/// @param [in] argv argument vector.
///
 //==============================================================================
void netx10gs_helper_CheckArguments( int argc, char **argv )
{
	int
		i = 0;
		
	
	if ( argc > 1 )
	{
		for ( i = 1; i < argc; i++ )
		{
			if ( (strlen(argv[i]) == 2) && (argv[i][0] == '-') )
			{
				switch ( tolower( argv[i][1] ) )
				{
				case 'a':
					if ( argv[++i] != NULL )
						g_pcArmgdbPort = argv[i];
					break;

				case 'x':
					if ( argv[++i] != NULL )
						g_pcXpicgdbPort = argv[i];
					break;

				case 'o':
					if ( argv[++i] != NULL )
						g_pcOpenocdPort = argv[i];
					break;
					
				case 'v':
					g_bVerbose = TRUE;
					break;
				
				case 'l':
					g_cLog = GDB_LOGGING;
					break;

				default:
					netx10gs_helper_Usage();
					break;
				}
			}
			else
				netx10gs_helper_Usage();
		} // end for
	} // end if
	
	netx10gs_helper_CheckPorts();
}
//==============================================================================



//==============================================================================
// NETX10GS_HELPER_netx10gs_helper_Char2Hex
//==============================================================================
///
/// @brief Calculates the hex value for a given character.
///
/// @param [in] ucVal is the character.
///
/// @return Hex value or GDB_SWITCH_ERROR
///
//==============================================================================
char netx10gs_helper_Char2Hex( unsigned char ucVal )
{
	if ( ucVal >= 'a' && ucVal <= 'f' )
	{
		return ucVal - 'a' + 10;
	}
	
	if ( ucVal >= '0' && ucVal <= '9' )
	{
		return ucVal - '0';
	}
	
	if ( ucVal >= 'A' && ucVal <= 'F' )
	{
		return ucVal -'A' + 10;
	}
	
	netx10gs_helper_log( LOG_ERROR, "netx10gs_helper_Char2Hex - non valid value: %i (%c)!", ucVal, ucVal );
	return GDB_SWITCH_ERROR;
}
//==============================================================================


//==============================================================================
// NETX10GS_HELPER_GDBVAL2UNSIGNEDINT
//==============================================================================
///
/// @brief Casts a given GDB value to unsigned int. E.g.: 78563412 -> 0x12345678.
///
///
/// @param [in] pcBuffer pointer to buffer with input string.
/// @param [in/out] pstructUiValue pointer to struct which holds the
///					calculated value.
///
//==============================================================================
void netx10gs_helper_GdbVal2UnsignedInt( char *pcBuffer, struct structUiValue_t *pstructUiValue )
{

	char
		cCnt = 0x0,
		acTemp[8];	
	
	
	pstructUiValue->cErrorCode = GDB_SWITCH_OK;
	pstructUiValue->uiRetval = 0x0;
	
	if ( pcBuffer == NULL )
	{
		netx10gs_helper_log( LOG_ERROR, "netx10gs_helper_GdbVal2UnsignedInt - nullpointer exception." );
		pstructUiValue->cErrorCode = GDB_SWITCH_ERROR;
		return;
	}
	
	if ( strlen( pcBuffer ) < 8 )
	{
		netx10gs_helper_log( LOG_ERROR, "helper_GdbVal2UnsignedInt - buffer too short!" );
		pstructUiValue->cErrorCode = GDB_SWITCH_ERROR;
		return;
	}
	
	acTemp[0] = netx10gs_helper_Char2Hex( pcBuffer[1] );
	acTemp[1] = netx10gs_helper_Char2Hex( pcBuffer[0] );
	acTemp[2] = netx10gs_helper_Char2Hex( pcBuffer[3] );
	acTemp[3] = netx10gs_helper_Char2Hex( pcBuffer[2] );
	acTemp[4] = netx10gs_helper_Char2Hex( pcBuffer[5] );
	acTemp[5] = netx10gs_helper_Char2Hex( pcBuffer[4] );
	acTemp[6] = netx10gs_helper_Char2Hex( pcBuffer[7] );
	acTemp[7] = netx10gs_helper_Char2Hex( pcBuffer[6] );
	
	for ( cCnt = 0x0; cCnt < 8; cCnt++ )
	{
		if ( acTemp[cCnt] == GDB_SWITCH_ERROR )
		{
			netx10gs_helper_log( LOG_ERROR, "helper_GdbVal2UnsignedInt - wrong value!" );
			pstructUiValue->cErrorCode = GDB_SWITCH_ERROR;
			return;
		}
		pstructUiValue->uiRetval += (unsigned int) acTemp[cCnt] << ( cCnt * 4 );
	}
	netx10gs_helper_log( LOG_GDBVAL2UNSIGNEDINT, "helper_GdbVal2UnsignedInt - value: 0x%x", pstructUiValue->uiRetval );
	return;
}
//==============================================================================


//==============================================================================
// NETX10GS_HELPER_CALCULATECHECKSUM
//==============================================================================
///
/// @brief Calculates the GDB Checksum for a given input string.
///
///
/// @param [in] pcBuffer pointer to buffer with input string.
/// @param [in] uiBufferSize size of buffer.
///
/// @return Errorcode.
///
//==============================================================================
char netx10gs_helper_CalculateChecksum( char *pcBuffer, unsigned int uiBufferSize )
{
	unsigned char
		uCChecksum = 0x0;
	
	int
		iMsgLength = 0x0,
		i = 0x0;
	
	char
		acTempBuffer[4];

	
	if ( pcBuffer == NULL )
	{
		return GDB_SWITCH_ERROR;
	}
	
	iMsgLength = (int) strlen( pcBuffer );
	
	// find start
	for ( i = 0; i < iMsgLength; i++ )
	{
		if ( pcBuffer[i] == '$' )
		{
			break;
		}
	}
	
	if ( i >= iMsgLength )
	{
		return GDB_SWITCH_ERROR;
	}
		
	// build checksum
	for ( i = i + 1; i < iMsgLength; i++ )
	{
		uCChecksum = uCChecksum + pcBuffer[i];
	}
	
	acTempBuffer[0] = '#';
	acTempBuffer[1] = acHexchars[ uCChecksum >> 4 ];
	acTempBuffer[2] = acHexchars[ uCChecksum & 0x0F ];
	acTempBuffer[3] = 0;
	
	if ( strncat_s( pcBuffer, uiBufferSize, acTempBuffer, 4 ) != GDB_SWITCH_OK )
	{
		return GDB_SWITCH_ERROR;
	}
	
	return GDB_SWITCH_OK;
}
//==============================================================================


//==============================================================================
// NETX10GS_HELPER_CHECKANSWER
//==============================================================================
///
/// @brief Checks given string for value GDB_SWITCH_OK or GDB_SWITCH_NOT_SUPPORTED
///			and	returns GDB_SWITCH_OK or GDB_SWITCH_NOT_SUPPORTED.
///			Else GDB_SWITCH_ERROR will be returned.
///
/// @param [in] pcBuffer pointer to buffer with input string.
///
/// @return Errorcode.
///
//==============================================================================
char netx10gs_helper_CheckAnswer( char *pcBuffer )
{
	if ( strcmp( pcBuffer, GDB_OK ) == 0 )
	{
		return GDB_SWITCH_OK;
	}
	
	if ( ( pcBuffer[0] == '$' ) && ( pcBuffer[1] == '#' ) )
	{
		return GDB_SWITCH_NOT_SUPPORTED;
	}

	return GDB_SWITCH_ERROR;
}
//==============================================================================


//==============================================================================
// NETX10GS_HELPER_STRCAT
//==============================================================================
///
/// @brief Concats two strings.
///
/// @param [in/out] pcFirstBuffer pointer to the first buffer.
/// @param [in] iSize size of the first buffer.
/// @param [in] pcSecondBuffer pointer to the first buffer.
///
/// @return Error code.
///
//==============================================================================
char netx10gs_helper_StrCat( char *pcFirstBuffer, unsigned int uiBufferSize, char *pcSecondBuffer )
{

	if ( ( pcFirstBuffer == NULL ) || ( pcSecondBuffer == NULL ) || ( uiBufferSize <= 0) )
	{
		return GDB_SWITCH_ERROR;
	}

	// checks if first buffer is too small
	if ( strlen(pcFirstBuffer) + strlen(pcSecondBuffer) + 1 > uiBufferSize )
	{
		return GDB_SWITCH_ERROR;
	}

	strcat_s( pcFirstBuffer, uiBufferSize, pcSecondBuffer );
	
	return GDB_SWITCH_OK;
}
//==============================================================================


//==============================================================================
// NETX10GS_HELPER_STRNCAT
//==============================================================================
///
/// @brief Concats two strings.
///
/// @param [inout] pcFirstBuffer pointer to the first buffer.
/// @param [in] iSize size of the first buffer.
/// @param [in] pcSecondBuffer pointer to the first buffer.
/// @param [in] uiN size of the second string.
///
/// @return Error code.
///
//==============================================================================
char netx10gs_helper_StrNCat( char *pcFirstBuffer, unsigned int uiBufferSize, char *pcSecondBuffer, int uiN )
{

	if ( ( pcFirstBuffer == NULL ) || ( pcSecondBuffer == NULL ) || ( uiBufferSize <= 0 ) || ( uiN <= 0 ) )
	{
		return GDB_SWITCH_ERROR;
	}

	// checks if first buffer is too small
	if ( strlen(pcFirstBuffer) + strlen(pcSecondBuffer) + 1 > uiBufferSize )
	{
		return GDB_SWITCH_ERROR;
	}

	strncat_s( pcFirstBuffer, uiBufferSize, pcSecondBuffer, uiN );
	
	return GDB_SWITCH_OK;
}
//==============================================================================


//==============================================================================
// NETX10GS_HELPER_BUILDGDBMSG
//==============================================================================
///
/// @brief Builds a gdb message.
///
/// @param [out] pcBuffer pointer to the output buffer.
/// @param [in] uiBufferSize size of the output buffer.
/// @param [in] pcStartMsg start message.
/// @param [in] pcEndMsg end message.
/// @param [in] cValueType type of value: GDB_VALUE_TYPE_NORMAL, GDB_VALUE_TYPE_GDB, GDB_VALUE_TYPE_NO_VALUE, GDB_VALUE_TYPE_MONITOR.
/// @param [in] iValue value to be included into the message.
/// @param [in] cCalcChecksum activates or deactivates a checksum calculation for the message.
///
/// @return Error code.
///
//==============================================================================
char netx10gs_helper_BuildGdbMsg( char *pcBuffer, unsigned int uiBufferSize, char *pcStartMsg, char *pcEndMsg, char cValueType, int iValue, char cCalcChecksum )
{
	char
		cTempValue,
		acTempBuffer[8];

	unsigned int
		uiCnt = 0x0;
	
	
	netx10gs_helper_log( LOG_BUILDGDBMSG, "helper_BuildGdbMsg - start" );
	netx10gs_helper_log( LOG_BUILDGDBMSG, "helper_BuildGdbMsg - config:\npcStartMsg: %s\npcEndMsg: %s\ncValueType: %i\niValue: %i\ncCalcChecksum: %i", pcStartMsg, pcEndMsg, cValueType, iValue, cCalcChecksum );
	
	// check for errors
	if ( ( pcBuffer == NULL ) || ( uiBufferSize < 1 ) )
	{
		return GDB_SWITCH_ERROR;
	}	
	
	// init
	pcBuffer[0] = 0;
	
	if ( pcStartMsg != NULL )
	{
		if ( netx10gs_helper_StrCat( pcBuffer, uiBufferSize, pcStartMsg ) != GDB_SWITCH_OK )
		{
			return GDB_SWITCH_ERROR;
		}
	}

	// build value gdb style
	if ( cValueType == GDB_VALUE_TYPE_GDB )
	{
		acTempBuffer[6] = acHexchars[ ( iValue >> 28 ) & 0xf ];
		acTempBuffer[7] = acHexchars[ ( iValue >> 24 ) & 0xf ];
		acTempBuffer[4] = acHexchars[ ( iValue >> 20 ) & 0xf ];
		acTempBuffer[5] = acHexchars[ ( iValue >> 16 ) & 0xf ];
		acTempBuffer[2] = acHexchars[ ( iValue >> 12 ) & 0xf ];
		acTempBuffer[3] = acHexchars[ ( iValue >> 8  ) & 0xf ];
		acTempBuffer[0] = acHexchars[ ( iValue >> 4  ) & 0xf ];
		acTempBuffer[1] = acHexchars[   iValue         & 0xf ];
	}
	
	// build value normal style
	if ( cValueType == GDB_VALUE_TYPE_NORMAL )
	{
		acTempBuffer[0] = acHexchars[ ( iValue >> 28 ) & 0xf ];
		acTempBuffer[1] = acHexchars[ ( iValue >> 24 ) & 0xf ];
		acTempBuffer[2] = acHexchars[ ( iValue >> 20 ) & 0xf ];
		acTempBuffer[3] = acHexchars[ ( iValue >> 16 ) & 0xf ];
		acTempBuffer[4] = acHexchars[ ( iValue >> 12 ) & 0xf ];
		acTempBuffer[5] = acHexchars[ ( iValue >> 8  ) & 0xf ];
		acTempBuffer[6] = acHexchars[ ( iValue >> 4  ) & 0xf ];
		acTempBuffer[7] = acHexchars[   iValue         & 0xf ];
	}
	
	// concat value, normal or gdb style
	if ( ( cValueType == GDB_VALUE_TYPE_GDB ) || ( cValueType == GDB_VALUE_TYPE_NORMAL ) )
	{
		if ( netx10gs_helper_StrNCat( pcBuffer, uiBufferSize, acTempBuffer, 8 ) != GDB_SWITCH_OK )
		{
			return GDB_SWITCH_ERROR;
		}
	}
	
	if ( pcEndMsg != NULL )
	{
		// build monitor msg
		if ( cValueType == GDB_VALUE_TYPE_MONITOR )
		{
			while ( cValueType == cValueType )
			{
				if ( pcEndMsg[uiCnt] == 0 )
				{
					break;
				}
				
				cTempValue = ( pcEndMsg[uiCnt] >> 4 ) & 0x0f;
				cTempValue = acHexchars[ cTempValue ];
				
				if ( netx10gs_helper_StrNCat( pcBuffer, uiBufferSize, &cTempValue, 0x1 ) != GDB_SWITCH_OK )
				{
					return GDB_SWITCH_ERROR;
				}
				
				cTempValue = pcEndMsg[uiCnt] & 0x0f;
				cTempValue = acHexchars[ cTempValue ];
				
				if ( netx10gs_helper_StrNCat( pcBuffer, uiBufferSize, &cTempValue, 0x1 ) != GDB_SWITCH_OK )
				{
					return GDB_SWITCH_ERROR;
				}
				
				uiCnt++;
			}
		}
		else
		{
			if ( netx10gs_helper_StrNCat( pcBuffer, uiBufferSize, pcEndMsg, (int) strlen( pcEndMsg ) ) != GDB_SWITCH_OK )
				{
					return GDB_SWITCH_ERROR;
				}
		}
	}
	
	if ( cCalcChecksum == GDB_CALC_CHECKSUM )
	{
		// check for errors
		if ( netx10gs_helper_CalculateChecksum( pcBuffer, uiBufferSize ) != GDB_SWITCH_OK )
		{
			return GDB_SWITCH_ERROR;
		}
	}
	
	netx10gs_helper_log( LOG_BUILDGDBMSG, "helper_BuildGdbMsg - end" );
	return GDB_SWITCH_OK;
}
//==============================================================================


//==============================================================================
// NETX10GS_HELPER_GDBCMP
//==============================================================================
///
/// @brief Compares two strings. First string is splitted in substrings. Needed
///			if the original string has more than one gmd message. E.g. $g#xx$c#xx.
///
/// @return	Errorcode.
///
//==============================================================================
char netx10gs_helper_GdbCmp( char *pcBuffer, char *pcMsg )
{
	unsigned int
		i,
		uiMsgStartIndex = 0x0,
		uiMsgEndIndex = 0x0;
	
	for ( i = 0x0; i < strlen( pcBuffer ); i++ )
	{
		if ( pcBuffer[i] == '$' )
		{
			uiMsgStartIndex = i;
		}
		
		if ( pcBuffer[i] == '#' )
		{
			uiMsgEndIndex = i + 2;
			
			if ( strncmp( &pcBuffer[uiMsgStartIndex], pcMsg, uiMsgEndIndex - uiMsgStartIndex + 1 ) == 0 )
			{
				return 1;
			}
		}
	}
	
	return 0;
}
//==============================================================================
