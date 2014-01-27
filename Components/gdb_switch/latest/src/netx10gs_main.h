/*
 *******************************************************************************
 *                                                                             *
 * netx10gs_main.h                                                             *
 *                                                                             *
 * author: Daniel Kramlinger                                                   *
 * Hilscher Gesellschaft für Systemautomation mbH                              *
 * date: 19.05.2010                                                            *
 *                                                                             *
 *******************************************************************************
*/


#ifndef NETX10GS_MAIN_H
#define NETX10GS_MAIN_H


/// @brief Prevents conflicts caused by including winsock2 (backword compatibility with winsock)
#ifndef WIN32_LEAN_AND_MEAN
	#define WIN32_LEAN_AND_MEAN
#endif

#include <winsock2.h>
#include <ws2tcpip.h>
	

#endif