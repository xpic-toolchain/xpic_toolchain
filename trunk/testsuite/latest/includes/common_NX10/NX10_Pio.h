#ifndef __NX10_PIO_H
#define __NX10_PIO_H

#include "regdef_netx10.h"
#include "defines.h"


/*****************************************************************************/
/*! Input/Output Configuration                                               */
/*****************************************************************************/
typedef unsigned long NX10_PIO_MODE;
#define NX10_PIO_MODE_IN        0  /**< PIO is an input      */
#define NX10_PIO_MODE_OUT       1  /**< PIO output enabled   */

void NX10_Pio_SetMode( unsigned long ulMmioNum, unsigned long ulMode );


#define NX10_Pio_GetIn()         PEEK(Adr_NX10_mmio_ctrl_mmio_in_line_status)
#define NX10_Pio_GetOut()        PEEK(Adr_NX10_mmio_pio_out_line_cfg)
#define NX10_Pio_SetOut(line)    POKE(Adr_NX10_mmio_pio_out_line_cfg,line)

#define NX10_Pio_GetInput(mmio)  ((PEEK(Adr_NX10_mmio_ctrl_mmio_in_line_status)>>mmio)&1)
#define NX10_Pio_GetOutput(mmio) ((PEEK(Adr_NX10_mmio_pio_out_line_cfg)>>mmio)&1)

void NX10_Pio_SetOutput( unsigned long ulMmioNum, int fEnable );

#endif /* __NX10_PIO_H */
