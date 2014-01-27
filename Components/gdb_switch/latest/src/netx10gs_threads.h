/*
 *******************************************************************************
 *                                                                             *
 * netx10gs_threads.h                                                          *
 *                                                                             *
 * author: Daniel Kramlinger                                                   *
 * Hilscher Gesellschaft für Systemautomation mbH                              *
 * date: 18.03.2011                                                            *
 *                                                                             *
 *******************************************************************************
*/



#ifndef NETX10GS_THREADS_H
#define NETX10GS_THREADS_H

#ifdef GDB_SWITCH_WINDOWS
DWORD WINAPI netx10gs_threads_HandleConnectionXpicgdb( LPVOID lpParam );
DWORD WINAPI netx10gs_threads_HandleConnectionArmgdb( LPVOID lpParam );
DWORD WINAPI netx10gs_threads_HandleGdbSession( LPVOID lpParam );
DWORD dwExitCodeHandleGdbSession;
#endif

#ifdef GDB_SWITCH_LINUX
#include <pthread.h>

pthread_t
	threadHandleConnectionXpicgdb,
	threadHandleConnectionArmgdb,
	threadHandleGdbSession;

void *netx10gs_threads_HandleConnectionXpicgdb( void *lpParam );
void *netx10gs_threads_HandleConnectionArmgdb( void *lpParam );
void *netx10gs_threads_HandleGdbSession( void *lpParam );
int dwExitCodeHandleGdbSession; // AAA WAS DAS????
#endif

#endif
