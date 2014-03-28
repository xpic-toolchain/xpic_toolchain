#ifndef __NX10_XPIC_TIMER_H
#define __NX10_XPIC_TIMER_H

#include "regdef_netx10.h"
#include "defines.h"

#define NX10_XPICTIMER_CHANNELS      3   /**< Number of timers */

/*****************************************************************************/
/*! Result codes for functions                                               */
/*****************************************************************************/
typedef unsigned long NX10_XPICTIMER_RESULT;
#define NX10_XPICTIMER_OKAY                        1 /**< Successful          */
#define NX10_XPICTIMER_ERR_INVAL_PARAM             2 /**< Invalid parameter   */


/*****************************************************************************/
/*! Configuration for XPIC timer                                             */
/*****************************************************************************/
typedef unsigned long NX10_XPICTIMER_CFG_MODE;
#define NX10_XPICTIMER_CFG_MODE_STOP_AT_0         0 /**< 0: Timer stops at 0                          */
#define NX10_XPICTIMER_CFG_MODE_PRELOAD_AT_0      1 /**< 1: Timer is reset to preload value at zero   */
#define NX10_XPICTIMER_CFG_MODE_SYSTIME           2 /**< 2: Timer (value) compare with systime (once) */


/***********************************************************************************************/
/*! Parameters ID that can be get by NX10_XpicTimer_GetPrm()                                   */
/***********************************************************************************************/
typedef unsigned long NX10_XPICTIMER_PRM;
#define NX10_XPICTIMER_PRM_CONFIG_TIMER0         1 /**< Register xpic_timer_config_timer0      */
#define NX10_XPICTIMER_PRM_CONFIG_TIMER1         2 /**< Register xpic_timer_config_timer1      */
#define NX10_XPICTIMER_PRM_CONFIG_TIMER2         3 /**< Register xpic_timer_config_timer2      */
#define NX10_XPICTIMER_PRM_PRELOAD_TIMER0        4 /**< Register xpic_timer_preload_timer0     */
#define NX10_XPICTIMER_PRM_PRELOAD_TIMER1        5 /**< Register xpic_timer_preload_timer1     */
#define NX10_XPICTIMER_PRM_PRELOAD_TIMER2        6 /**< Register xpic_timer_preload_timer2     */
#define NX10_XPICTIMER_PRM_TIMER0                7 /**< Register xpic_timer_timer0             */
#define NX10_XPICTIMER_PRM_TIMER1                8 /**< Register xpic_timer_timer1             */
#define NX10_XPICTIMER_PRM_TIMER2                9 /**< Register xpic_timer_timer1             */
#define NX10_XPICTIMER_PRM_SYSTIME_NS           10 /**< Register xpic_timer_systime_ns         */
#define NX10_XPICTIMER_PRM_SYSTIME_S            11 /**< Register xpic_timer_systime_s          */
#define NX10_XPICTIMER_PRM_IRQMSKSET            12 /**< interrupt mask enable                  */
#define NX10_XPICTIMER_PRM_IRQMSKRST            13 /**< interrupt mask disable                 */


/***********************************************************************************************/
/*! Interrupt masks that can be modified by _EnIrq() _DisIrq() _ConfirmIrq()\
 *  and returned by _GetIrq() _GetIrqRaw()                                                     */
/***********************************************************************************************/
typedef unsigned long NX10_XPICTIMER_IRQ;
#define NX10_XPICTIMER_IRQ_TIMER0     MSK_NX10_xpic_timer_irq_raw_timer0_irq
#define NX10_XPICTIMER_IRQ_TIMER1     MSK_NX10_xpic_timer_irq_raw_timer1_irq
#define NX10_XPICTIMER_IRQ_TIMER2     MSK_NX10_xpic_timer_irq_raw_timer2_irq
#define NX10_XPICTIMER_IRQ_SYSTIME_S  MSK_NX10_xpic_timer_irq_raw_systime_s_irq

#define NX10_XPICTIMER_IRQ_ALL        (NX10_XPICTIMER_IRQ_TIMER0|NX10_XPICTIMER_IRQ_TIMER1|NX10_XPICTIMER_IRQ_TIMER2|NX10_XPICTIMER_IRQ_SYSTIME_S)
 
/***********************************************************************************************/
/* XPICTIMER Service Class: Functions                                                          */
/***********************************************************************************************/

#define NX10_XpicTimer_EnIrq(mask)      POKE(Adr_NX10_xpic_timer_irq_msk_set,mask)
#define NX10_XpicTimer_DisIrq(mask)     POKE(Adr_NX10_xpic_timer_irq_msk_reset,mask)
#define NX10_XpicTimer_ConfirmIrq(mask) POKE(Adr_NX10_xpic_timer_irq_raw,mask)
#define NX10_XpicTimer_GetIrq()         PEEK(Adr_NX10_xpic_timer_irq_masked)
#define NX10_XpicTimer_GetIrqRaw()      PEEK(Adr_NX10_xpic_timer_irq_raw)

NX10_XPICTIMER_RESULT NX10_XpicTimer_Start( unsigned int uInst, NX10_XPICTIMER_CFG_MODE eMode, unsigned long ulVal );
NX10_XPICTIMER_RESULT NX10_XpicTimer_Stop( unsigned int uInst );
NX10_XPICTIMER_RESULT NX10_XpicTimer_GetPrm( unsigned long ulPrmID, unsigned long* pulPrmVal );
void NX10_XpicTimer_StartSystimeSecCmp( unsigned long ulVal );

void NX10_XpicTimer_SetSystime( unsigned long ulSystime_s, unsigned long ulSystime_ns );
void NX10_XpicTimer_GetSystime( unsigned long* pulSystime_s, unsigned long* pulSystime_ns );
void NX10_XpicTimer_GetSystimeDiff( unsigned long* pulTime_s, unsigned long* pulTime_ns );

#endif /* __NX10_XPIC_TIMER_H */
                                  
