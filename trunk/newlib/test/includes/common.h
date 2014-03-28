#ifndef __common_h
#define __common_h

#ifdef __NETX51

#define XPIC_TIMER_H                "NX51_XpicTimer.h"
#define XpicTimer_GetSystime        NX51_XpicTimer_GetSystime
#define XpicTimer_GetSystimeDiff    NX51_XpicTimer_GetSystimeDiff



#else /* default is netx10 */

#define XPIC_TIMER_H                "NX10_XpicTimer.h"
#define XpicTimer_GetSystime        NX10_XpicTimer_GetSystime
#define XpicTimer_GetSystimeDiff    NX10_XpicTimer_GetSystimeDiff

#endif




#endif /* __common_h */
