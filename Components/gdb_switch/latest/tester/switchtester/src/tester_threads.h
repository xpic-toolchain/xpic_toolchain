/*
 *******************************************************************************
 *                                                                             *
 * tester_threads.h                                                            *
 *                                                                             *
 * author: Daniel Kramlinger                                                   *
 * Hilscher Gesellschaft für Systemautomation mbH                              *
 * date: 14.02.2011                                                            *
 *                                                                             *
 *******************************************************************************
*/



#ifndef TESTER_THREADS_H
#define TESTER_THREADS_H

CRITICAL_SECTION criticalSectionOpenocd;
CRITICAL_SECTION criticalSectionTestCnt;

DWORD WINAPI TesterOpenocd( LPVOID lpParam );
DWORD WINAPI TesterXpicgdb( LPVOID lpParam );


int
	iErrorCounter;
	
#endif