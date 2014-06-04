#include "NX10_MMIO.h" // function prototypes and defines


/*****************************************************************************/
/*! MMIO Set Configuration
* \description
*   Configures a MMIO pin.
* \class 
*   MMIO 
* \params
*   uMmioNum          [in]  Selected MMIO
*   bFunction         [in]  Signal Selection
*   fInvertOut        [in]  Invert Output Signal
*   fInvertIn         [in]  Invert Input Signal
* \return
*                                                                            */

  static NX10_ASIC_CTRL_AREA_T*      s_ptAsicCtrl    = (NX10_ASIC_CTRL_AREA_T*) Addr_NX10_asic_ctrl;
  static NX10_MMIO_CTRL_AREA_T*      s_ptMmioCtrl    = (NX10_MMIO_CTRL_AREA_T*) Addr_NX10_mmio_ctrl;

/*****************************************************************************/
void NX10_MMIO_SetCfg( unsigned int uMmioNum, unsigned char bFunction, int fInvertOut, int fInvertIn )
{
  // read access key, write back access key
  s_ptAsicCtrl->ulAsic_ctrl_access_key = s_ptAsicCtrl->ulAsic_ctrl_access_key;
    
  s_ptMmioCtrl->aulMmio_cfg[uMmioNum] =  bFunction << SRT_NX10_mmio0_cfg_mmio_sel 
                                        |(fInvertOut ? MSK_NX10_mmio0_cfg_mmio_out_inv : 0) 
                                        |(fInvertIn  ? MSK_NX10_mmio0_cfg_mmio_in_inv  : 0);
  
}

/*****************************************************************************/
/*! MMIO Get Configuration
* \description
*   Get configuration of a MMIO pin.
* \class 
*   MMIO 
* \params
*   uMmioNum          [in]  Selected MMIO
*   pbFunction        [out]  Pointer to Signal Selection
*   pfInvertOut       [out]  Pointer to Invert Output Signal
*   pfInvertIn        [out]  Pointer to Invert Input Signal
* \return
*                                                                            */
/*****************************************************************************/
void NX10_MMIO_GetCfg( unsigned int uMmioNum, unsigned char* pbFunction, int* pfInvertOut, int* pfInvertIn )
{
  unsigned long ulTemp = s_ptMmioCtrl->aulMmio_cfg[uMmioNum];
  
  *pbFunction   = (ulTemp & MSK_NX10_mmio0_cfg_mmio_sel)     >> SRT_NX10_mmio0_cfg_mmio_sel;
  *pfInvertOut  = (ulTemp & MSK_NX10_mmio0_cfg_mmio_out_inv) >> SRT_NX10_mmio0_cfg_mmio_out_inv;
  *pfInvertIn   = (ulTemp & MSK_NX10_mmio0_cfg_mmio_in_inv)  >> SRT_NX10_mmio0_cfg_mmio_in_inv;
}

/*****************************************************************************/
/*! MMIO Get Line
* \description
*  Read the MMIO output level line. MMIOs must be in PIO mode.
* \class 
*   PIO 
* \params
* \return
*   Pio_line value                                                          */
/*****************************************************************************/
unsigned long NX10_MMIO_GetLine( void )
{
  return ( s_ptMmioCtrl->ulMmio_pio_out_line_cfg );
}

/*****************************************************************************/
/*! MMIO Set Line
* \description
*   Set MMIO output level line. MMIOs must be in PIO mode.
* \class 
*   PIO 
* \params
*   ulVal          [in]   Value of PIO Line 
* \return
*                                                                            */
/*****************************************************************************/
void NX10_MMIO_SetLine( unsigned long ulVal )
{
  s_ptMmioCtrl->ulMmio_pio_out_line_cfg = ulVal;
}

