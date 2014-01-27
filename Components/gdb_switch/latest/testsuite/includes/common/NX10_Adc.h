#ifndef __NX10_ADC_H
#define __NX10_ADC_H

#include "NX10_PWM.h" // NX10_MOTION_RESULT

#define NX10_ADC_NUM             2 /**< Number of ADC units             */
#define NX10_ADC_CHANNELS        8 /**< Number of channels per ADC unit */
#define NX10_ADC_OPERATIONAL     1 /**< Operational mode                */
#define NX10_ADC_POWER_DOWN      0 /**< Power-Down mode                 */
#define NX10_ADC_POWERUP_DELAY  20 /**< PowerUp delay (>1µs)            */
#define NX10_ADC_SEL2CONV_DELAY  5 /**< Select2 Convert delay (>80ns)   */

/*****************************************************************************/
/*! Select Event for AD-conversion start                                     */
/*****************************************************************************/
typedef unsigned long NX10_ADC_AUTOCFG_EVENTSEL;
#define NX10_ADC_AUTOCFG_EVENTSEL_STARTIMM         0 /**< 0 : start immediately */
#define NX10_ADC_AUTOCFG_EVENTSEL_ENC0_N           1 /**< 1 : enc0_n */
#define NX10_ADC_AUTOCFG_EVENTSEL_ENC1_N           2 /**< 2 : enc1_n */
#define NX10_ADC_AUTOCFG_EVENTSEL_ENC0_EDGE        3 /**< 3 : enc0_edge */
#define NX10_ADC_AUTOCFG_EVENTSEL_ENC1_EDGE        4 /**< 4 : enc1_edge */
#define NX10_ADC_AUTOCFG_EVENTSEL_MP0              5 /**< 5 : mp0 */
#define NX10_ADC_AUTOCFG_EVENTSEL_MP1              6 /**< 6 : mp1 */
#define NX10_ADC_AUTOCFG_EVENTSEL_PWM_CNT0_MIN     7 /**< 7 : pwm_cnt0_min */
#define NX10_ADC_AUTOCFG_EVENTSEL_PWM_CNT1_MIN     8 /**< 8 : pwm_cnt0_max */
#define NX10_ADC_AUTOCFG_EVENTSEL_PWM_CNT0_MAX     9 /**< 9 : pwm_cnt1_min */
#define NX10_ADC_AUTOCFG_EVENTSEL_PWM_CNT1_MAX    10 /**< 10: pwm_cnt1_max */
#define NX10_ADC_AUTOCFG_EVENTSEL_PWM_T0          11 /**< 11: pwm_t0 */
#define NX10_ADC_AUTOCFG_EVENTSEL_PWM_T1          12 /**< 12: pwm_t1 */
#define NX10_ADC_AUTOCFG_EVENTSEL_PWM_T2          13 /**< 13: pwm_t2 */
#define NX10_ADC_AUTOCFG_EVENTSEL_PWM_T3          14 /**< 14: pwm_t3 */
#define NX10_ADC_AUTOCFG_EVENTSEL_PWM_T4          15 /**< 15: pwm_t4 */
#define NX10_ADC_AUTOCFG_EVENTSEL_PWM_T5          16 /**< 16: pwm_t5 */
#define NX10_ADC_AUTOCFG_EVENTSEL_PWM_T6          17 /**< 17: pwm_t6 */
#define NX10_ADC_AUTOCFG_EVENTSEL_PWM_T7          18 /**< 18: pwm_t7 */
#define NX10_ADC_AUTOCFG_EVENTSEL_XPIC_TIMER0     19 /**< 19: xpic_timer0 */
#define NX10_ADC_AUTOCFG_EVENTSEL_XPIC_TIMER1     20 /**< 20: xpic_timer1 */
#define NX10_ADC_AUTOCFG_EVENTSEL_XPIC_TIMER2     21 /**< 21: xpic_timer2 */
#define NX10_ADC_AUTOCFG_EVENTSEL_ARM_TIMER0      22 /**< 22: arm_timer0  */
#define NX10_ADC_AUTOCFG_EVENTSEL_ARM_TIMER1      23 /**< 23: arm_timer1  */

NX10_MOTION_RESULT NX10_Adc_SetOpmode( unsigned int uAdcNum, int fOpMode );
NX10_MOTION_RESULT NX10_Adc_StartAuto( int fStartAdc0, int fStartAdc1, int fStart2ndAdc0, int fStart2ndAdc1 );
NX10_MOTION_RESULT NX10_Adc_ConfigAuto( unsigned int uAdcNum, unsigned int uChannel, unsigned int uPwrDown, unsigned int uInv, NX10_ADC_AUTOCFG_EVENTSEL eEventSel );
NX10_MOTION_RESULT NX10_Adc_StartMan( unsigned int uAdcNum, unsigned int uChannel );
NX10_MOTION_RESULT NX10_Adc_GetStatus( unsigned int uAdcNum, int* pfFin, int* pfEocb );
NX10_MOTION_RESULT NX10_Adc_GetAdcVal( unsigned int uAdcNum, unsigned short* pusVal );
NX10_MOTION_RESULT NX10_Adc_EnableIrq( unsigned long ulIrqMask );
NX10_MOTION_RESULT NX10_Adc_DisableIrq( unsigned long ulIrqMask );
NX10_MOTION_RESULT NX10_Adc_GetIrq( unsigned long *pulIrq );
NX10_MOTION_RESULT NX10_Adc_GetIrqRaw( unsigned long *pulIrq );
NX10_MOTION_RESULT NX10_Adc_ConfirmIrq( unsigned long ulConfirmIrqMask );

#endif /* __NX10_ADC_H */
