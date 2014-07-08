#include "NX51_MMIO.h" // function prototypes and defines


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

  static NX51_ASIC_CTRL_AREA_T*      s_ptAsicCtrl    = (NX51_ASIC_CTRL_AREA_T*) Addr_NX51_asic_ctrl;
  static NX51_MMIO_CTRL_AREA_T*      s_ptMmioCtrl    = (NX51_MMIO_CTRL_AREA_T*) Addr_NX51_mmio_ctrl;

/*****************************************************************************/
void NX51_MMIO_SetCfg( unsigned int uMmioNum, unsigned char bFunction, int fInvertOut, int fInvertIn )
{
  // read access key, write back access key
  s_ptAsicCtrl->ulAsic_ctrl_access_key = s_ptAsicCtrl->ulAsic_ctrl_access_key;
    
  s_ptMmioCtrl->aulMmio_cfg[uMmioNum] =  bFunction << SRT_NX51_mmio0_cfg_mmio_sel
                                        |(fInvertOut ? MSK_NX51_mmio0_cfg_mmio_out_inv : 0)
                                        |(fInvertIn  ? MSK_NX51_mmio0_cfg_mmio_in_inv  : 0);
  
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
void NX51_MMIO_GetCfg( unsigned int uMmioNum, unsigned char* pbFunction, int* pfInvertOut, int* pfInvertIn )
{
  unsigned long ulTemp = s_ptMmioCtrl->aulMmio_cfg[uMmioNum];
  
  *pbFunction   = (ulTemp & MSK_NX51_mmio0_cfg_mmio_sel)     >> SRT_NX51_mmio0_cfg_mmio_sel;
  *pfInvertOut  = (ulTemp & MSK_NX51_mmio0_cfg_mmio_out_inv) >> SRT_NX51_mmio0_cfg_mmio_out_inv;
  *pfInvertIn   = (ulTemp & MSK_NX51_mmio0_cfg_mmio_in_inv)  >> SRT_NX51_mmio0_cfg_mmio_in_inv;
}

/*****************************************************************************/
/*! MMIO Get Line
* \description
*  Read the MMIO output level line. MMIOs must be in PIO mode.
* \class 
*   PIO 
* \params
*   ulReg01          [in]   Register 0 (mmio0-31) or 1 (mmio32-48) of PIO Line
* \return
*   Pio_line value                                                          */
/*****************************************************************************/
unsigned long NX51_MMIO_GetLine( unsigned long ulReg01 )
{
  return ( s_ptMmioCtrl->aulMmio_pio_out_line_cfg[ulReg01&1] );
}

/*****************************************************************************/
/*! MMIO Set Line
* \description
*   Set MMIO output level line. MMIOs must be in PIO mode.
* \class 
*   PIO 
* \params
*   ulReg01          [in]   Register 0 (mmio0-31) or 1 (mmio32-48) of PIO Line
*   ulVal            [in]   Value of PIO Line
* \return
*                                                                            */
/*****************************************************************************/
void NX51_MMIO_SetLine( unsigned long ulReg01,  unsigned long ulVal )
{
  s_ptMmioCtrl->aulMmio_pio_out_line_cfg[ulReg01] = ulVal;
}
