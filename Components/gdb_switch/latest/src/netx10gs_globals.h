/*
 *******************************************************************************
 *                                                                             *
 * netx10gs_globals.h                                                          *
 *                                                                             *
 * author: Daniel Kramlinger                                                   *
 *         Hilscher Gesellschaft für Systemautomation mbH                      *
 * date: 14.04.2011                                                            *
 *                                                                             *
 *******************************************************************************
*/


#ifndef NETX10GS_GLOBALS_H
#define NETX10GS_GLOBALS_H

#ifdef GDB_SWITCH_WINDOWS
#include <windows.h>
#endif

#ifdef GDB_SWITCH_LINUX
#include <stdbool.h>
#include <stdlib.h>
#include <pthread.h>
#define FALSE	false
#define TRUE	true
#define INVALID_SOCKET -1
#define BOOLEAN	bool

#define EnterCriticalSection	pthread_mutex_lock
#define LeaveCriticalSection	pthread_mutex_unlock
// #define NULL	null
#endif

#define GDB_SWITCH_ARMGDB_DEBUG // XXX DEBUG

#define DEFAULT_BUFFER_LEN  	0x4000   // XXX OpenOCD sends qPacketSize=3fff. if this changes change DEFAULT_BUFFER_LEN!

// LOG and DEBUG
#define GDB_LOGGING				0x1
#define GDB_NO_LOGGING			0x0
#define LOG_DEBUG		 0
#define LOG_VERBOSE	 	 1
#define LOG_ERROR		 2
#define LOG_DONT_SHOW	-1
#define LOG_SHOW		 LOG_DEBUG


// sockets
#define LOG_INITSOCKETS				LOG_SHOW
#define LOG_INITCONNECTIONARMGDB	LOG_SHOW
#define LOG_SENDKEEPALIVE			LOG_DONT_SHOW
#define LOG_SEND					LOG_DONT_SHOW
#define LOG_RECEIVE					LOG_DONT_SHOW
#define LOG_SENDANDRECEIVE			LOG_DONT_SHOW
#define LOG_HANDLESOCKETERROR		LOG_DONT_SHOW

// handlexpic
#define LOG_STEPBACKPC				LOG_SHOW
#define LOG_READXPICREGISTERS		LOG_SHOW
#define LOG_SETSOFTWAREBREAKPOINT	LOG_SHOW
#define LOG_SINGLESTEPXPIC			LOG_SHOW
#define LOG_CHECKFORBREAKPOINT		LOG_DONT_SHOW
#define LOG_CHECKFORSINGLESTEP		LOG_SHOW
#define LOG_INITNETX10				LOG_SHOW
#define LOG_READSINGLEXPICREGISTER	LOG_SHOW

// handlegdbproto
#define LOG_GETGDBCOMMAND			LOG_DONT_SHOW
#define LOG_XPIC					LOG_DONT_SHOW
#define LOG_ARM						LOG_SHOW
#define LOG_TESTTEST				LOG_SHOW

// helper
#define LOG_BUILDGDBMSG				LOG_DONT_SHOW
#define LOG_GDBVAL2UNSIGNEDINT		LOG_DONT_SHOW

// handlearm
#define LOG_CHECKARMRUNNING			LOG_SHOW

// handleboard
#define LOG_POKE					LOG_SHOW
#define LOG_IRQFIQLOCK				LOG_SHOW
#define LOG_GETANDRETURNACCESSKEY	LOG_SHOW
#define LOG_ACTIVATESYSTEMCLOCKS	LOG_SHOW
#define LOG_SETBORDER				LOG_SHOW
#define LOG_SETSPEED				LOG_SHOW
#define LOG_SETCFG					LOG_SHOW
#define LOG_PFIFO_RESET				LOG_SHOW
#define LOG_CLEARVIC				LOG_SHOW
#define LOG_INITBOARD				LOG_SHOW




#define	SOCKET_OPENOCD			0x1
#define	SOCKET_XPICGDB			0x2
#define	SOCKET_ARMGDB			0x3
#define IOCTL_BLOCKING			0x0
#define IOCTL_NON_BLOCKING		0x1

#define GDB_CONNECTED			0x1
#define GDB_NOT_CONNCECTED		0x0

#define GDB_RUNNING				0x1
#define GDB_NOT_RUNNING			0x0

#define XPICMODE_ON				0x1
#define XPICMODE_OFF			0x0

#define	GDB_SWITCH_NOTHING_RECEIVED		1
#define	GDB_SWITCH_OK					0
#define	GDB_SWITCH_NOT_SUPPORTED		-1
#define GDB_SWITCH_ERROR				-2


#define	RESUME_ARM		0
#define HALT_ARM		1

#define XPIC_PRAM_STARTADDRESS			0x1018c000
#define XPIC_PRAM_SIZE					0x00002000


BOOLEAN
	g_cLog,
	g_bVerbose,
	g_bSinglemode,
	g_bExitGdbSwitch,
	g_bXpicModeActivated,
	g_bXpicInitFinished,
	g_bXpicThreadInitialized,
	g_bArmInitFinished,
	g_bArmRunnig,
	g_bDoSingleStepAfterBreakpoint,
	g_bCheckForArmBreakpoint,
	g_bClockActivated,
	g_bCheckForBreakpoint,
	g_bBreakpointHappened,
	g_bCheckForSinglestep,
	g_bResetXpicSoftBreakIrq,
	g_bNoAckModeStartedXpic,
	g_bNoAckModeStartedArm,
	g_bNoAckModeStartedOpenocd,
	g_bNoAckModeActivated,
	testarm;

char
	g_cArmgdbConnected,
	g_cXpicgdbConnected,
	g_cOpenocdConnected,
	buffer[DEFAULT_BUFFER_LEN],
	temp_buffer[DEFAULT_BUFFER_LEN],
	checksum[4],
	arm2oo_buf[DEFAULT_BUFFER_LEN],
	oo2arm_buf[DEFAULT_BUFFER_LEN],
	acXpicbuffer[DEFAULT_BUFFER_LEN],
	oo2xpic_buf[DEFAULT_BUFFER_LEN];

char
	*g_pcProgname,
	*g_pcArmgdbPort,
	*g_pcXpicgdbPort,
	*g_pcOpenocdPort;
	
unsigned int
	armgdb_buflen,
	xpicgdb_buflen;


struct gdbCommand
{
	BOOLEAN	bMsgError;
	int cCmd;
	unsigned int uiPosition; // XXX remove?
	int iFirstArg;
	char acSecondArg[20];
	unsigned int uiSecondArg;
} g_structGdbCommand;

struct structUiValue_t
{
	char cErrorCode;
	unsigned int uiRetval;
} g_structUiValue;

char acHexchars[17];

#ifdef GDB_SWITCH_WINDOWS
CRITICAL_SECTION criticalSectionGdbSwitch;
CRITICAL_SECTION criticalSectionArmgdb;
CRITICAL_SECTION criticalSectionXpicgdb;
CRITICAL_SECTION criticalSectionOpenocd;
#endif


#ifdef GDB_SWITCH_LINUX
static pthread_mutex_t criticalSectionGdbSwitch = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t criticalSectionArmgdb = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t criticalSectionXpicgdb = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t criticalSectionOpenocd = PTHREAD_MUTEX_INITIALIZER;
#endif

#endif
