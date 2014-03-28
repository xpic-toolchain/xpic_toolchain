/*! \file  NX10_Enc.c

    \brief HAL for Encoder module

    Implementation of functions

    \date 21.10.2011
    \author Artem Rudskyy
    \version Initial
*/

#include "regdef_netx10.h"
#include "NX10_Enc.h"
#include "NX10_Pwm.h"

/*! Configuration of the encoder module
 *  \parameter uEncNum Number of encoder module: NX10_ENC_NUM_0 or NX10_ENC_NUM_1
*   \returns NX10_MOTION_OKAY on success or NX10_MOTION_ERR_INVAL_PARAM on wrong uEncNum passed
 */
unsigned int NX10_Enc_Config(
		unsigned int uEncNum ,
		unsigned int uCountDir ,
		unsigned int uFilterSampleRate ,
		unsigned int uMPEnable ,
		unsigned int uMPFilterSampleRate )
{
	// Setting pulENC_Config - a pointer to Encoder Configuration Register:
	volatile unsigned long  *pulENC_Config = (volatile unsigned long  *) Adr_NX10_menc_config;

	// creating a variable to put into the Encoder Configuration Register:
	NX10_MENC_CONFIG_T   tEncConf;  	// a union of val (unsigned int) and bf (structure of bit-fields)
	tEncConf.val = 0;					// setting all bit-fields in the variable to 0

	switch (uEncNum)
	{
	case 0:
		tEncConf.bf.enc0_en 				= 1;
		tEncConf.bf.enc0_count_dir 			= uCountDir;
		tEncConf.bf.enc0_filter_sample_rate = uFilterSampleRate;
		tEncConf.bf.mp0_en					= uMPEnable;
		tEncConf.bf.mp0_filter_sample_rate	= uMPFilterSampleRate;
		break;
	case 1:
		tEncConf.bf.enc1_en 				= 1;
		tEncConf.bf.enc1_count_dir 			= uCountDir;
		tEncConf.bf.enc1_filter_sample_rate = uFilterSampleRate;
		tEncConf.bf.mp1_en					= uMPEnable;
		tEncConf.bf.mp1_filter_sample_rate	= uMPFilterSampleRate;
		break;
	default:
		return NX10_MOTION_ERR_INVAL_PARAM;
	}

	*pulENC_Config |= tEncConf.val;
	return NX10_MOTION_OKAY;
}

/*! Getting position from the position register
 *  \parameter uEncNum Number of encoder module: NX10_ENC_NUM_0 or NX10_ENC_NUM_1
 *  \returns the position written in Adr_NX10_menc_enc0_position or Adr_NX10_menc_enc1_position
 */
unsigned long NX10_Enc_Position( unsigned int uEncNum )
{
	if ( uEncNum == 0 )
	{
	volatile unsigned long  *pulENC_Pos = (volatile unsigned long  *) Adr_NX10_menc_enc0_position;
	return *pulENC_Pos;
	}
	else
	{
	volatile unsigned long  *pulENC_Pos = (volatile unsigned long  *) Adr_NX10_menc_enc1_position;
	return *pulENC_Pos;
	}
}

/*! Setting position register to 0
 *  \parameter uEncNum Number of encoder module:  NX10_ENC_NUM_0 or NX10_ENC_NUM_1
 *  \returns NX10_MOTION_OKAY on success or NX10_MOTION_ERR_INVAL_PARAM on wrong uEncNum passed
 */
unsigned int  NX10_Enc_Position_Set_Null( unsigned int uEncNum )
{
	switch(uEncNum)
	{
	case 0:
		{
			volatile unsigned long  *pulENC_Pos = (volatile unsigned long  *) Adr_NX10_menc_enc0_position;
			*pulENC_Pos = 0;
			break;
		}
	case 1:
		{
			volatile unsigned long  *pulENC_Pos = (volatile unsigned long  *) Adr_NX10_menc_enc1_position;
			*pulENC_Pos = 0;
			break;
		}
	default:
		return NX10_MOTION_ERR_INVAL_PARAM;
	}

	return NX10_MOTION_OKAY;
}

/*! Resetting Status register - all bits to "0"
 *  \returns void
 */
void NX10_Enc_Status_Reset ()
{
	// To reset an IRQ, write 1 to appropriate bit (except enc?_dir_ro).
	volatile unsigned long  *pulStatus = (volatile unsigned long  *) Adr_NX10_menc_status;
	// Trying to write "1" to everywhere where it was before.
	NX10_MENC_STATUS_T 	tStatus;
	tStatus.val = *pulStatus;
	*pulStatus = tStatus.val;
}

/*! Read content of capture unit
 *  \parameter uCapUnitNumber   Number of capture module: NX10_ENC_CAP_UNIT_0 - NX10_ENC_CAP_UNIT_3
 *  \returns Captured value
 */
unsigned long NX10_Enc_Capture ( unsigned long uCapUnitNumber )
{
	volatile unsigned long  *pulCaptured_Value_0 = (volatile unsigned long  *) Adr_NX10_menc_capture0_val;
	volatile unsigned long  *pulCaptured_Value_1 = (volatile unsigned long  *) Adr_NX10_menc_capture1_val;
	volatile unsigned long  *pulCaptured_Value_2 = (volatile unsigned long  *) Adr_NX10_menc_capture2_val;
	volatile unsigned long  *pulCaptured_Value_3 = (volatile unsigned long  *) Adr_NX10_menc_capture3_val;

	volatile unsigned long Captured_Value ;

	switch (uCapUnitNumber)
	{
	case 0:
		Captured_Value = *pulCaptured_Value_0;
		break;
	case 1:
		Captured_Value = *pulCaptured_Value_1;
		break;
	case 2:
		Captured_Value = *pulCaptured_Value_2;
		break;
	case 3:
		Captured_Value = *pulCaptured_Value_3;
		break;
	}
	return Captured_Value;
}

/*! Setup encoder capture unit
 *  \parameter uCapUnitNumber   Number of capture module: NX10_ENC_CAP_UNIT_0 - NX10_ENC_CAP_UNIT_3
 *  \parameter uIfInvert        Positive ore negative edge as capture start signal
 *  \parameter uWhenToCapture   Event to capture on: NX10_ENC_CAP_CONF_ENC0_N/NX10_ENC_CAP_CONF_ENC0_N/...
 *  \parameter uWhatToCapture   Which value to capture: NX10_ENC_CAP_CONF_SOURCE_NS/NX10_ENC_CAP_CONF_SOURCE_POSITION/...
 *  \parameter uEncNum          Number of encoder module:  NX10_ENC_NUM_0 or NX10_ENC_NUM_1
 *  \parameter uIfCaptureOnce   Capture continue (always on event) or once (capture only, if menc_status.cap? == 0 )
 *  \returns NX10_MOTION_OKAY on success or NX10_MOTION_ERR_INVAL_PARAM on wrong uCapUnitNumber passed
 */
unsigned int NX10_Enc_Capture_Config (
		unsigned long uCapUnitNumber ,
		unsigned long uIfInvert ,
		unsigned long uWhenToCapture ,
		unsigned long uWhatToCapture ,
		unsigned long uEncNum ,
		unsigned long uIfCaptureOnce )
{
	NX10_MENC_CAPTURE0_CONFIG_T 	tCaptureConfig;
	volatile unsigned long Capture_Config;
	switch (uCapUnitNumber)  // taking the address of the chosen CaptureUnit
	{
	case 0:
		Capture_Config = Adr_NX10_menc_capture0_config;
		break;
	case 1:
		Capture_Config = Adr_NX10_menc_capture1_config;
		break;
	case 2:
		Capture_Config = Adr_NX10_menc_capture2_config;
		break;
	case 3:
		Capture_Config = Adr_NX10_menc_capture3_config;
		break;
	default:
		return NX10_MOTION_ERR_INVAL_PARAM;
	}

	volatile unsigned long  *pulCapture_Config = (volatile unsigned long  *) Capture_Config;

	tCaptureConfig.val = 0 ;

	tCaptureConfig.bf.inv 		= uIfInvert ;
	tCaptureConfig.bf.sel 		= uWhenToCapture ;  	// when to capture
	tCaptureConfig.bf.src 		= uWhatToCapture ;  	// what to capture
	tCaptureConfig.bf.src_nr 	= uEncNum ;  			// setting the encoder number
	tCaptureConfig.bf.conce 	= uIfCaptureOnce ;  	// capture once: capture only, if ( menc_status.cap? == 0 )

	*pulCapture_Config 	= tCaptureConfig.val ;
	return NX10_MOTION_OKAY;
}

// Do not include this function to the hal-library
#if 0
float NX10_Get_Velocity ( unsigned long uEncNum , unsigned long uCapUnitNumber )
{
	unsigned int i=0;
	float velocity;
	unsigned long Te;
	volatile unsigned long  *pulStatus = (volatile unsigned long  *) Adr_NX10_menc_status;
	NX10_MENC_STATUS_T 	tStatus;
	tStatus.val = 0;
// if the Capture Unit had captured something before and written "1" in Status Register, we change "1" to "0"
	switch (uCapUnitNumber)
	{
	case 0:
		tStatus.bf.cap0 = 1; break;
	case 1:
		tStatus.bf.cap1 = 1; break;
	case 2:
		tStatus.bf.cap2 = 1; break;
	case 3:
		tStatus.bf.cap3 = 1; break;
	}
	*pulStatus = tStatus.val;
// Configure Capture Unit to capture Te once when an edge comes
	NX10_Enc_Capture_Config (
			uCapUnitNumber ,
			NX10_ENC_CAP_CONF_POS ,
			NX10_ENC_CAP_CONF_ENC0_EDGE ,
			NX10_ENC_CAP_CONF_SOURCE_TE ,
			uEncNum ,
			NX10_ENC_CAP_CONF_CAPTURE_ONCE );


	while(1)												// waiting for a capture (10ms)
	{
		tStatus.val = *pulStatus; 							// get the value of status register
		if( tStatus.bf.cap0 == 1)  							// check if the CaptureUnit captured something
		{													// if it did
			Te = NX10_Enc_Capture(NX10_ENC_CAP_UNIT_0); 	// get the captured value Te
			velocity = 50000.0f /( (float)Te );				// calculate velocity
			return velocity;								// return and step out of function
		}													// if it did'nt captured something, get the value of status register again and ...
		i++;
		if( i>100000 )										// if we've been waiting too long, we say that encoder disk didn't move and the velocity is 0
			return -4;
	}
}
#endif


/*! Reset encoder capture unit
 *  \parameter uCapUnitNumber Number of capture module: NX10_ENC_CAP_UNIT_0 - NX10_ENC_CAP_UNIT_3
 *  \returns NX10_MOTION_OKAY on success or NX10_MOTION_ERR_INVAL_PARAM on wrong uCapUnitNumber passed
 */
unsigned int NX10_Enc_Reset_Capture_Unit ( unsigned long uCapUnitNumber )
{
	volatile unsigned long Capture_Config;
	switch (uCapUnitNumber)  // taking the address of the chosen CaptureUnit
	{
	case 0:
		Capture_Config = Adr_NX10_menc_capture0_config;
		break;
	case 1:
		Capture_Config = Adr_NX10_menc_capture1_config;
		break;
	case 2:
		Capture_Config = Adr_NX10_menc_capture2_config;
		break;
	case 3:
		Capture_Config = Adr_NX10_menc_capture3_config;
		break;
	default:
		return NX10_MOTION_ERR_INVAL_PARAM;
	}

	volatile unsigned long  *pulCapture_Config = (volatile unsigned long  *) Capture_Config;
	*pulCapture_Config = 0;
	return NX10_MOTION_OKAY;
}
