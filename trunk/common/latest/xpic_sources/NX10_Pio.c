#include "NX10_Pio.h" // function prototypes and defines
#include <string.h>

static NX10_MMIO_CTRL_AREA_T*      s_ptMmioCtrl    = (NX10_MMIO_CTRL_AREA_T*) Addr_NX10_mmio_ctrl;

/*****************************************************************************/
/*! Pio Set Mode
* \description
*   Enables/disable output for the selected Pio. MMIO must be in PIO mode.
* \class
*   PIO
* \params
*   ulMmioNum         [in]  Selected Pio
*   ulMode            [in]  NX10_PIO_MODE_IN  = Set the Pio to input mode
*                           NX10_PIO_MODE_OUT = Set the Pio to output mode
* \return
*                                                                            */
/*****************************************************************************/
void NX10_Pio_SetMode( unsigned long ulMmioNum, unsigned long ulMode )
{
  if( NX10_PIO_MODE_OUT == ulMode)
    s_ptMmioCtrl->ulMmio_pio_oe_line_cfg |= 1 << ulMmioNum;
  else
    s_ptMmioCtrl->ulMmio_pio_oe_line_cfg &= ~(1 << ulMmioNum);
}

/*****************************************************************************/
/*! PIO Set Output
* \description
*   Set output for the selected Pio within out_line_cfg register. MMIO must be in PIO mode.
* \class
*   PIO
* \params
*   ulMmioNum         [in]  Selected Pio
*   fEnable           [in]  0 = Disable the Output
*                           1 = Enable the Output
* \return
*                                                                            */
/*****************************************************************************/
void NX10_Pio_SetOutput( unsigned long ulMmioNum, int fEnable )
{
  if(fEnable) {
    s_ptMmioCtrl->ulMmio_pio_out_line_cfg |= 1 << ulMmioNum;
  } else {
    s_ptMmioCtrl->ulMmio_pio_out_line_cfg &= ~(1 << ulMmioNum);
  }
}

