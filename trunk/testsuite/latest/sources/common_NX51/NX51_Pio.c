 // function prototypes and defines
#include "NX51_Pio.h"

static NX51_MMIO_CTRL_AREA_T*      s_ptMmioCtrl    = (NX51_MMIO_CTRL_AREA_T*) Addr_NX51_mmio_ctrl;
static NX51_ASIC_CTRL_AREA_T*      s_ptAsicCtrl    = (NX51_ASIC_CTRL_AREA_T*) Addr_NX51_asic_ctrl;
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
void NX51_Pio_SetMode( unsigned long ulMmioNum, unsigned long ulMode )
{
  int reg01 = 0;
  if(ulMmioNum>31)
  {
	  ulMmioNum = ulMmioNum & 0x1ff;
	  reg01 = 1;
  }

  if( NX51_PIO_MODE_OUT == ulMode)
    s_ptMmioCtrl->aulMmio_pio_oe_line_cfg[reg01] |= 1 << ulMmioNum;
  else
    s_ptMmioCtrl->aulMmio_pio_oe_line_cfg[reg01] &= ~(1 << ulMmioNum);
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
void NX51_Pio_SetOutput( unsigned long ulMmioNum, int fEnable )
{
  int reg01 = 0;
  if(ulMmioNum>31)
  {
	  ulMmioNum = ulMmioNum & 0x1ff;
	  reg01 = 1;
  }

  if(fEnable) {
    s_ptMmioCtrl->aulMmio_pio_out_line_cfg[reg01] |= 1 << ulMmioNum;
  } else {
    s_ptMmioCtrl->aulMmio_pio_out_line_cfg[reg01] &= ~(1 << ulMmioNum);
  }
}

/*****************************************************************************/
/*! PIO Set Get Input
* \description
*   Reads input for the selected Pio within Adr_NX51_mmio_ctrl_mmio_in_line_status0/1 register. MMIO must be in PIO mode.
* \class
*   PIO
* \params
*   ulMmioNum         [in]  Selected Pio
* \return
*   Pio input value
*                                                                            */
/*****************************************************************************/
unsigned long NX51_Pio_GetInput( unsigned long ulMmioNum)
{
	if(ulMmioNum>31)
		return ((PEEK(Adr_NX51_mmio_ctrl_mmio_in_line_status1)>>(ulMmioNum&0x1f))&1);
	else
		return ((PEEK(Adr_NX51_mmio_ctrl_mmio_in_line_status0)>>ulMmioNum)&1);
}

/*****************************************************************************/
/*! PIO Set Get Output
* \description
*   Reads output for the selected Pio within Adr_NX51_mmio_pio_out_line_cfg0/1 register. MMIO must be in PIO mode.
* \class
*   PIO
* \params
*   ulMmioNum         [in]  Selected Pio
* \return
*   Pio output value
*                                                                            */
/*****************************************************************************/
unsigned long  NX51_Pio_GetOutput(unsigned long ulMmioNum)
{
	if(ulMmioNum>31)
		return ((PEEK(Adr_NX51_mmio_pio_out_line_cfg1)>>(ulMmioNum&0xf1))&1);
    else
    	return ((PEEK(Adr_NX51_mmio_pio_out_line_cfg0)>>ulMmioNum)&1);
}
