#ifndef __NX10_ENC_H
#define __NX10_ENC_H


// Constants used as uEncNum:
#define NX10_ENC_NUM_0					0
#define NX10_ENC_NUM_1					1

// Constants used as uCountDir:
#define NX10_ENC_COUNT_DIR_STANDARD 	0
#define NX10_ENC_COUNT_DIR_INVERTED 	1

// Constants used as uFilterSampleRate:
#define NX10_ENC_FILTER_DISABLE 		0 		// Filter is disabled
#define NX10_ENC_FSR_FROM_20ns			1 		// pulses >  20ns will pass. Pulses <  10ns  will be blocked.
#define NX10_ENC_FSR_FROM_50ns			2 		// pulses >  50ns will pass. Pulses <  20ns  will be blocked.
#define NX10_ENC_FSR_FROM_100ns			3 		// pulses > 100ns will pass. Pulses <  50ns  will be blocked.
#define NX10_ENC_FSR_FROM_200ns			4 		// pulses > 200ns will pass. Pulses < 100ns  will be blocked.
#define NX10_ENC_FSR_FROM_500ns			5 		// pulses > 500ns will pass. Pulses < 200ns  will be blocked.
#define NX10_ENC_FSR_FROM_1us			6 		// pulses >   1us will pass. Pulses < 500ns  will be blocked.
#define NX10_ENC_FSR_FROM_2us			7 		// pulses >   2us will pass. Pulses <   1us  will be blocked.

// Constants used as uMPEnable:
#define NX10_ENC_MES_POINT_DISABLED 	0 		// disable interrupts based on mesurement point signal
#define NX10_ENC_MES_POINT_ENABLED 		1 		// mesurement point is enabled

// Constants used as uMPFilterSampleRate:
#define NX10_ENC_MP_FILTER_DISABLE 		0 		// Filter is disabled
#define NX10_ENC_MP_FSR_FROM_20ns		1 		// pulses >  20ns will pass. Pulses <  10ns  will be blocked.
#define NX10_ENC_MP_FSR_FROM_50ns		2 		// pulses >  50ns will pass. Pulses <  20ns  will be blocked.
#define NX10_ENC_MP_FSR_FROM_100ns		3 		// pulses > 100ns will pass. Pulses <  50ns  will be blocked.
#define NX10_ENC_MP_FSR_FROM_200ns		4 		// pulses > 200ns will pass. Pulses < 100ns  will be blocked.
#define NX10_ENC_MP_FSR_FROM_500ns		5 		// pulses > 500ns will pass. Pulses < 200ns  will be blocked.
#define NX10_ENC_MP_FSR_FROM_1us		6 		// pulses >   1us will pass. Pulses < 500ns  will be blocked.
#define NX10_ENC_MP_FSR_FROM_2us		7 		// pulses >   2us will pass. Pulses <   1us  will be blocked.


/*! Disable all encoder modules
 */
#define       NX10_Enc_Disable() ( *((volatile unsigned long  *)Adr_NX10_menc_config) = 0 )
unsigned int  NX10_Enc_Config( unsigned int uEncNum , unsigned int uCountDir , unsigned int uFilterSampleRate , unsigned int uMPEnable , unsigned int uMPFilterSampleRate );
unsigned long NX10_Enc_Position( unsigned int uEncNum );
unsigned int  NX10_Enc_Position_Set_Null( unsigned int uEncNum );
void NX10_Enc_Status_Reset ();
unsigned long NX10_Enc_Capture ( unsigned long uCapUnitNumber );


// Constants used as uCapUnitNumber:
#define NX10_ENC_CAP_UNIT_0		0
#define NX10_ENC_CAP_UNIT_1		1
#define NX10_ENC_CAP_UNIT_2		2
#define NX10_ENC_CAP_UNIT_3		3

// Constants used as uIfInvert:
#define NX10_ENC_CAP_CONF_POS	0 	// Positive edge as capture start signal
#define NX10_ENC_CAP_CONF_NEG	1 	// Negative edge as capture start signal - Invert capture start signal

// Constants used as uWhenToCapture (as capture start signal):
#define NX10_ENC_CAP_CONF_OFF			0	// no capture
#define NX10_ENC_CAP_CONF_ENC0_N		1	// encoder 0 N-pulse - usually once per revelation
#define NX10_ENC_CAP_CONF_ENC1_N		2	// encoder 1 N-pulse - usually once per revelation
#define NX10_ENC_CAP_CONF_ENC0_EDGE		3	// encoder 0 edge
#define NX10_ENC_CAP_CONF_ENC1_EDGE		4	// encoder 1 edge
#define NX10_ENC_CAP_CONF_MP0			5	// measurement point 0
#define NX10_ENC_CAP_CONF_MP1			6	// measurement point 1
#define NX10_ENC_CAP_CONF_PWM_CNT0_MIN	7	// pwm counter 0 minimal value
#define NX10_ENC_CAP_CONF_PWM_CNT0_MAX	8	// pwm counter 0 maximal value
#define NX10_ENC_CAP_CONF_PWM_CNT1_MIN	9	// pwm counter 1 minimal value
#define NX10_ENC_CAP_CONF_PWM_CNT1_MAX	10	// pwm counter 1 maximal value
#define NX10_ENC_CAP_CONF_PWM_T0		11	//
#define NX10_ENC_CAP_CONF_PWM_T1		12	//
#define NX10_ENC_CAP_CONF_PWM_T2		13	//
#define NX10_ENC_CAP_CONF_PWM_T3		14	//
#define NX10_ENC_CAP_CONF_PWM_T4		15	//
#define NX10_ENC_CAP_CONF_PWM_T5		16	//
#define NX10_ENC_CAP_CONF_PWM_T6		17	//
#define NX10_ENC_CAP_CONF_PWM_T7		18	//
#define NX10_ENC_CAP_CONF_XPIC_TIMER_0	19	//
#define NX10_ENC_CAP_CONF_XPIC_TIMER_1	20	//
#define NX10_ENC_CAP_CONF_XPIC_TIMER_2	21	//
#define NX10_ENC_CAP_CONF_ARM_TIMER_0	22	//
#define NX10_ENC_CAP_CONF_ARM_TIMER_1	23	//

// Constants used as uWhatToCapture (as capture source signal):
#define NX10_ENC_CAP_CONF_SOURCE_NS				0 // system time (independent of src_nr)
#define NX10_ENC_CAP_CONF_SOURCE_POSITION		1 // position of channel 0/1
#define NX10_ENC_CAP_CONF_SOURCE_TA				2 // Ta of channel 0/1
#define NX10_ENC_CAP_CONF_SOURCE_TE				3 // Te of channel 0/1
#define NX10_ENC_CAP_CONF_SOURCE_TA_PLUS_TE		4 // Ta + Te of channel 0/1
#define NX10_ENC_CAP_CONF_SOURCE_PERIOD_IN_CLK	5 // period in clock cycles (independent of src_nr)

// Constants used as uIfCaptureOnce (as capture source signal):
#define NX10_ENC_CAP_CONF_CAPTURE_CONT		0 // continuous capture: each event overwrites old captured register
#define NX10_ENC_CAP_CONF_CAPTURE_ONCE		1 // capture once: capture only, if menc_stats.cap0=0

unsigned int NX10_Enc_Capture_Config    ( unsigned long uCapUnitNumber , unsigned long uIfInvert , unsigned long uWhenToCapture ,	unsigned long uWhatToCapture , unsigned long uEncNum , unsigned long uIfCaptureOnce );
unsigned int NX10_Enc_Reset_Capture_Unit( unsigned long uCapUnitNumber );


#endif /* __NX10_ENC_H */
