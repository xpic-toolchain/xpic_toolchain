#ifndef __NX10_PWM_H
#define __NX10_PWM_H

/*****************************************************************************/
/*! Result codes for functions                                               */
/*****************************************************************************/
typedef unsigned long NX10_MOTION_RESULT;
#define NX10_MOTION_OKAY                     1 /**< Successful                                   */
#define NX10_MOTION_ERR_INIT_FAILED          2 /**< Initialization has failed                    */
#define NX10_MOTION_ERR_INVAL_PARAM          3 /**< Invalid parameter                            */
#define NX10_MOTION_ERR_NOT_READY            4 /**< Module not ready                             */



#define NX10_PWM_FAILURE       0
#define NX10_PWM_NO_FAILURE    1
#define NX10_PWM_CHANNELS      8 /**< Number of PWM pins */
#define NX10_PWM_NUM_CNT       2 /**< Number of PWM counters */
#define NX10_PWM_IRQ_SRC_NUM   2 /**< Number of Interrupt sources */

/*****************************************************************************/
/*! Configuration for PWM Pin                                                */
/*****************************************************************************/
typedef unsigned long NX10_PWM_PINCFG;
#define NX10_PWM_PINCFG_SET_OUTPUT_1                0 /**< 000: set output to 1                                          */
#define NX10_PWM_PINCFG_SET_OUTPUT_0                1 /**< 001: set output to 0                                          */
#define NX10_PWM_PINCFG_SET_OUTPUT_PWM_SIG_CNT0     2 /**< 010: set output to PWM signal referenced on counter0          */
#define NX10_PWM_PINCFG_SET_OUTPUT_INV_PWM_SIG_CNT0 3 /**< 011: set output to invertet PWM signal referenced on counter0 */
#define NX10_PWM_PINCFG_SET_OUTPUT_HIGHZ            4 /**< 100: set output to High-Z                                     */
#define NX10_PWM_PINCFG_RESERVED                    5 /**< 101: reserved for future use                                  */
#define NX10_PWM_PINCFG_SET_OUTPUT_PWM_SIG_CNT1     6 /**< 110: set output to PWM signal referenced on counter1          */
#define NX10_PWM_PINCFG_SET_OUTPUT_INV_PWM_SIG_CNT1 7 /**< 111: set output to invertet PWM signal referenced on counter1 */


/*****************************************************************************/
/*! Configuration for PWM Pin in case of failure                             */
/*****************************************************************************/
typedef unsigned long NX10_PWM_FAILCFG;
#define NX10_PWM_FAILCFG_SET_OUTPUT_0              0 /**< 00: set output to 0                                          */
#define NX10_PWM_FAILCFG_SET_OUTPUT_1              1 /**< 01: set output to 1                                          */
#define NX10_PWM_FAILCFG_SET_OUTPUT_HIGHZ          2 /**< 10: set output to High-Z                                     */
#define NX10_PWM_FAILCFG_LEAVE_OUTPUT_AS_DEFINED   3 /**< 11: leave output as defined in mpwm_config_pins              */


/*****************************************************************************/
/*! Select PWM pin/condition for interrupt                                   */
/*****************************************************************************/
typedef unsigned long NX10_PWM_IRQCFG;
#define NX10_PWM_IRQCFG_DISABLE_IRQ                0 /**< 00000: disable interrupt                                     */
#define NX10_PWM_IRQCFG_GEN_IRQ_CNT0_EQ_0          1 /**< 00001: generate interrupt at pwm_cnt0 == 0                   */
#define NX10_PWM_IRQCFG_GEN_IRQ_CNT1_EQ_0          2 /**< 00010: generate interrupt at pwm_cnt1 == 0                   */
#define NX10_PWM_IRQCFG_GEN_IRQ_CNT0_EQ_MAX        3 /**< 00011: generate interrupt at pwm_cnt0 == max                 */
#define NX10_PWM_IRQCFG_GEN_IRQ_CNT1_EQ_MAX        4 /**< 00100: generate interrupt at pwm_cnt1 == max                 */
#define NX10_PWM_IRQCFG_RES0                       5 /**< 00101: reserved                                              */
#define NX10_PWM_IRQCFG_RES1                       6 /**< 00110: reserved                                              */
#define NX10_PWM_IRQCFG_RES2                       7 /**< 00111: reserved                                              */
#define NX10_PWM_IRQCFG_RES3                       8 /**< 01000: reserved                                              */
#define NX10_PWM_IRQCFG_RES4                       9 /**< 01001: reserved                                              */
#define NX10_PWM_IRQCFG_RES5                      10 /**< 01010: reserved                                              */
#define NX10_PWM_IRQCFG_RES6                      11 /**< 01011: reserved                                              */
#define NX10_PWM_IRQCFG_RES7                      12 /**< 01100: reserved                                              */
#define NX10_PWM_IRQCFG_RES8                      13 /**< 01101: reserved                                              */
#define NX10_PWM_IRQCFG_RES9                      14 /**< 01110: reserved                                              */
#define NX10_PWM_IRQCFG_RESA                      15 /**< 01111: reserved                                              */
#define NX10_PWM_IRQCFG_GEN_IRQ_POSEDGE_PIN0      16 /**< 10000: generate interrupt at posedge of PWM pin 0            */
#define NX10_PWM_IRQCFG_GEN_IRQ_POSEDGE_PIN1      17 /**< 10001: generate interrupt at posedge of PWM pin 1            */
#define NX10_PWM_IRQCFG_GEN_IRQ_POSEDGE_PIN2      18 /**< 10010: generate interrupt at posedge of PWM pin 2            */
#define NX10_PWM_IRQCFG_GEN_IRQ_POSEDGE_PIN3      19 /**< 10011: generate interrupt at posedge of PWM pin 3            */
#define NX10_PWM_IRQCFG_GEN_IRQ_POSEDGE_PIN4      20 /**< 10100: generate interrupt at posedge of PWM pin 4            */
#define NX10_PWM_IRQCFG_GEN_IRQ_POSEDGE_PIN5      21 /**< 10101: generate interrupt at posedge of PWM pin 5            */
#define NX10_PWM_IRQCFG_GEN_IRQ_POSEDGE_PIN6      22 /**< 10110: generate interrupt at posedge of PWM pin 6            */
#define NX10_PWM_IRQCFG_GEN_IRQ_POSEDGE_PIN7      23 /**< 10111: generate interrupt at posedge of PWM pin 7            */
#define NX10_PWM_IRQCFG_GEN_IRQ_NEGEDGE_PIN0      24 /**< 11000: generate interrupt at negedge of PWM pin 0            */
#define NX10_PWM_IRQCFG_GEN_IRQ_NEGEDGE_PIN1      25 /**< 11001: generate interrupt at negedge of PWM pin 1            */
#define NX10_PWM_IRQCFG_GEN_IRQ_NEGEDGE_PIN2      26 /**< 11010: generate interrupt at negedge of PWM pin 2            */
#define NX10_PWM_IRQCFG_GEN_IRQ_NEGEDGE_PIN3      27 /**< 11011: generate interrupt at negedge of PWM pin 3            */
#define NX10_PWM_IRQCFG_GEN_IRQ_NEGEDGE_PIN4      28 /**< 11100: generate interrupt at negedge of PWM pin 4            */
#define NX10_PWM_IRQCFG_GEN_IRQ_NEGEDGE_PIN5      29 /**< 11101: generate interrupt at negedge of PWM pin 5            */
#define NX10_PWM_IRQCFG_GEN_IRQ_NEGEDGE_PIN6      30 /**< 11110: generate interrupt at negedge of PWM pin 6            */
#define NX10_PWM_IRQCFG_GEN_IRQ_NEGEDGE_PIN7      31 /**< 11111: generate interrupt at negedge of PWM pin 7            */


/***********************************************************************************************/
/*! Parameters ID that can be set/get by Pwm_SetPrm()/Pwm_GetPrm()                             */
/***********************************************************************************************/
typedef unsigned long NX10_PWM_PRM;
#define NX10_PWM_PRM_RESERVED                      1 /**< Reserved                                                   */

  /* Config_Counter */
#define NX10_PWM_PRM_RUN_CNT0                      2 /**< Run counter0 (read-only)                                   */
#define NX10_PWM_PRM_WAVEFORM_CNT0                 3 /**< Waveform of counter0                                       */
#define NX10_PWM_PRM_DUAL_EDGE0                    4 /**< Dual Edge Mode for shadow registers references on counter0 */
#define NX10_PWM_PRM_RUN_CNT1                      5 /**< Run counter1 (read-only)                                   */
#define NX10_PWM_PRM_WAVEFORM_CNT1                 6 /**< Waveform of counter1                                       */
#define NX10_PWM_PRM_DUAL_EDGE1                    7 /**< Dual Edge Mode for shadow registers references on counter1 */

  /* Config_Pins */
#define NX10_PWM_PRM_CFG_PIN0                      8 /**< Configuration for PWM Pin0                                 */
#define NX10_PWM_PRM_SHADOW0                       9 /**< Shadow Mode t0                                             */
#define NX10_PWM_PRM_CFG_PIN1                     10 /**< Configuration for PWM Pin1                                 */
#define NX10_PWM_PRM_SHADOW1                      11 /**< Shadow Mode t1                                             */
#define NX10_PWM_PRM_CFG_PIN2                     12 /**< Configuration for PWM Pin2                                 */
#define NX10_PWM_PRM_SHADOW2                      13 /**< Shadow Mode t2                                             */
#define NX10_PWM_PRM_CFG_PIN3                     14 /**< Configuration for PWM Pin3                                 */
#define NX10_PWM_PRM_SHADOW3                      15 /**< Shadow Mode t3                                             */
#define NX10_PWM_PRM_CFG_PIN4                     16 /**< Configuration for PWM Pin4                                 */
#define NX10_PWM_PRM_SHADOW4                      17 /**< Shadow Mode t4                                             */
#define NX10_PWM_PRM_CFG_PIN5                     18 /**< Configuration for PWM Pin5                                 */
#define NX10_PWM_PRM_SHADOW5                      19 /**< Shadow Mode t5                                             */
#define NX10_PWM_PRM_CFG_PIN6                     20 /**< Configuration for PWM Pin6                                 */
#define NX10_PWM_PRM_SHADOW6                      21 /**< Shadow Mode t6                                             */
#define NX10_PWM_PRM_CFG_PIN7                     22 /**< Configuration for PWM Pin7                                 */
#define NX10_PWM_PRM_SHADOW7                      23 /**< Shadow Mode t7                                             */

  /* Config_Failure */
#define NX10_PWM_PRM_CFG_FAIL0                    24 /**< Configuration for PWM Pin0 in case of failure              */
#define NX10_PWM_PRM_CFG_FAIL1                    25 /**< Configuration for PWM Pin1 in case of failure              */
#define NX10_PWM_PRM_CFG_FAIL2                    26 /**< Configuration for PWM Pin2 in case of failure              */
#define NX10_PWM_PRM_CFG_FAIL3                    27 /**< Configuration for PWM Pin3 in case of failure              */
#define NX10_PWM_PRM_CFG_FAIL4                    28 /**< Configuration for PWM Pin4 in case of failure              */
#define NX10_PWM_PRM_CFG_FAIL5                    29 /**< Configuration for PWM Pin5 in case of failure              */
#define NX10_PWM_PRM_CFG_FAIL6                    30 /**< Configuration for PWM Pin6 in case of failure              */
#define NX10_PWM_PRM_CFG_FAIL7                    31 /**< Configuration for PWM Pin7 in case of failure              */
#define NX10_PWM_PRM_FAIL_INPUT_EN                32 /**< Failure input enable                                       */
#define NX10_PWM_PRM_FAIL_FAILURE                 33 /**< Failure bit can be set by software or by config_failure-enable, reset by software */

  /* Irq_Config */
#define NX10_PWM_PRM_IRQCFG0                      34 /**< select PWM pin/mode for interrupt 0                            */
#define NX10_PWM_PRM_IRQDELAY0                    35 /**< Delay from event to generation of interrupt 0 in steps of 20ns */
#define NX10_PWM_PRM_IRQCFG1                      36 /**< select PWM pin/mode for interrupt 1                            */
#define NX10_PWM_PRM_IRQDELAY1                    37 /**< Delay from event to generation of interrupt 1 in steps of 20ns */

  /* Irq_Msk_Set/Irq_Msk_Reset */
#define NX10_PWM_PRM_IRQMSKSET                    38 /**< interrupt mask enable                                          */
#define NX10_PWM_PRM_IRQMSKRST                    39 /**< interrupt mask disable                                         */

  /* Cnt0/Cnt1 */
#define NX10_PWM_PRM_CNT0PERIOD                   40 /**< PWM period of counter 0: Sawtooth mode: period length in steps of 10ns = cnt0_period[19:2], Triangle mode: period length in steps of 20ns = cnt0_period[19:3] */
#define NX10_PWM_PRM_CNT0                         41 /**< actual value of PWM counter 0 (read-only) */
#define NX10_PWM_PRM_CNT0SYSTIME                  42 /**< Captured Systime at cnt0==0 (read-only) */
#define NX10_PWM_PRM_CNT0WDG                      43 /**< Cnt0 watchdog (read-only) */
#define NX10_PWM_PRM_CNT1PERIOD                   44 /**< PWM period of counter 0: Sawtooth mode: period length in steps of 10ns = cnt0_period[19:2], Triangle mode: period length in steps of 20ns = cnt0_period[19:3] */
#define NX10_PWM_PRM_CNT1                         45 /**< actual value of PWM counter 1 (read-only) */
#define NX10_PWM_PRM_CNT1SYSTIME                  46 /**< Captured Systime at cnt1==0 (read-only) */
#define NX10_PWM_PRM_CNT1WDG                      47 /**< Cnt1 watchdog (read-only) */

  /* T0..T8 */
#define NX10_PWM_PRM_T0                           48 /**< Width of channel 0 low phase in steps of 2,5ns */
#define NX10_PWM_PRM_T1                           49 /**< Width of channel 1 low phase in steps of 2,5ns */
#define NX10_PWM_PRM_T2                           50 /**< Width of channel 2 low phase in steps of 2,5ns */
#define NX10_PWM_PRM_T3                           51 /**< Width of channel 3 low phase in steps of 2,5ns */
#define NX10_PWM_PRM_T4                           52 /**< Width of channel 4 low phase in steps of 2,5ns */
#define NX10_PWM_PRM_T5                           53 /**< Width of channel 5 low phase in steps of 2,5ns */
#define NX10_PWM_PRM_T6                           54 /**< Width of channel 6 low phase in steps of 2,5ns */
#define NX10_PWM_PRM_T7                           55 /**< Width of channel 7 low phase in steps of 2,5ns */

  /* T0_Shadow..T8_Shadow */
#define NX10_PWM_PRM_T0_SHADOW                    56 /**< Value will be updated from mpwm_t0 at end of period if mpwm_config_pins-shadow0, This register is writable but can also be changed by hardware */
#define NX10_PWM_PRM_T1_SHADOW                    57 /**< Value will be updated from mpwm_t1 at end of period if mpwm_config_pins-shadow1, This register is writable but can also be changed by hardware */
#define NX10_PWM_PRM_T2_SHADOW                    58 /**< Value will be updated from mpwm_t2 at end of period if mpwm_config_pins-shadow2, This register is writable but can also be changed by hardware */
#define NX10_PWM_PRM_T3_SHADOW                    59 /**< Value will be updated from mpwm_t3 at end of period if mpwm_config_pins-shadow3, This register is writable but can also be changed by hardware */
#define NX10_PWM_PRM_T4_SHADOW                    60 /**< Value will be updated from mpwm_t4 at end of period if mpwm_config_pins-shadow4, This register is writable but can also be changed by hardware */
#define NX10_PWM_PRM_T5_SHADOW                    61 /**< Value will be updated from mpwm_t5 at end of period if mpwm_config_pins-shadow5, This register is writable but can also be changed by hardware */
#define NX10_PWM_PRM_T6_SHADOW                    62 /**< Value will be updated from mpwm_t6 at end of period if mpwm_config_pins-shadow6, This register is writable but can also be changed by hardware */
#define NX10_PWM_PRM_T7_SHADOW                    63 /**< Value will be updated from mpwm_t7 at end of period if mpwm_config_pins-shadow7, This register is writable but can also be changed by hardware */



/***********************************************************************************************/
/* PWM Service Class: Function Prototypes                                                      */
/***********************************************************************************************/
NX10_MOTION_RESULT NX10_Pwm_ConfigCnt( unsigned int uInst, int fWaveForm, int fDualEdge );
NX10_MOTION_RESULT NX10_Pwm_ConfigFailure( int fEn );
NX10_MOTION_RESULT NX10_Pwm_SetSwFailure( int fEn );
NX10_MOTION_RESULT NX10_Pwm_ConfigCh( unsigned int uInst, NX10_PWM_PINCFG ePinCfg, int fShadowEn, NX10_PWM_FAILCFG eFailureCfg );
NX10_MOTION_RESULT NX10_Pwm_SetCntPeriod( unsigned int uInst, unsigned long ulPeriod );
NX10_MOTION_RESULT NX10_Pwm_RetriggerCnt( unsigned int uInst, unsigned long ulWdgVal );
NX10_MOTION_RESULT NX10_Pwm_SetChWidth( unsigned int uInst, unsigned long ulWidth );
NX10_MOTION_RESULT NX10_Pwm_CfgIrqSrc( unsigned int uInst, NX10_PWM_IRQCFG eIrqCfg, unsigned int uDelay );
NX10_MOTION_RESULT NX10_Pwm_GetCntStat( unsigned int uInst, unsigned long* pulValue, unsigned int* pulSystime );
NX10_MOTION_RESULT NX10_Pwm_Start( int fCnt0En, int fCnt1En );
NX10_MOTION_RESULT NX10_Pwm_Stop( int fCnt0Dis, int fCnt1Dis );
NX10_MOTION_RESULT NX10_Pwm_EnIrq( int fSrc0IrqEn, int fSrc1IrqEn, int fFailureIrqEn );
NX10_MOTION_RESULT NX10_Pwm_DisIrq( int fSrc0IrqDis, int fSrc1IrqDis, int fFailureIrqDis );
NX10_MOTION_RESULT NX10_Pwm_GetIrq( unsigned long *pulIrq );
NX10_MOTION_RESULT NX10_Pwm_GetIrqRaw( unsigned long *pulIrq );
NX10_MOTION_RESULT NX10_Pwm_ConfirmIrq( unsigned long ulConfirmIrqMask );
NX10_MOTION_RESULT NX10_Pwm_SetPrm( unsigned long ulPrmID, unsigned long ulPrmVal );
NX10_MOTION_RESULT NX10_Pwm_GetPrm( unsigned long ulPrmID, unsigned long* pulPrmVal );

#endif /* __NX10_PWM_H */
