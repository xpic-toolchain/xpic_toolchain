#ifndef __NX51_PIO_H
#define __NX51_PIO_H

#include "netx51_regdef.h"
#include "defines.h"


/*****************************************************************************/
/*! Input/Output Configuration                                               */
/*****************************************************************************/
typedef unsigned long NX51_PIO_MODE;
#define NX51_PIO_MODE_IN        0  /**< PIO is an input      */
#define NX51_PIO_MODE_OUT       1  /**< PIO output enabled   */

void NX51_Pio_SetMode( unsigned long ulMmioNum, unsigned long ulMode );
void NX51_Pio_SetOutput( unsigned long ulMmioNum, int fEnable );
unsigned long NX51_Pio_GetInput( unsigned long ulMmioNum);
unsigned long  NX51_Pio_GetOutput(unsigned long ulMmioNum);

/* MMIO 0-31 */
#define NX51_Pio_GetIn0()         PEEK(Adr_NX51_mmio_ctrl_mmio_in_line_status0)
#define NX51_Pio_GetOut0()        PEEK(Adr_NX51_mmio_pio_out_line_cfg0)
#define NX51_Pio_SetOut0(line)    POKE(Adr_NX51_mmio_pio_out_line_cfg0,line)
/* MMIO 32-48 */
#define NX51_Pio_GetIn1()         PEEK(Adr_NX51_mmio_ctrl_mmio_in_line_status1)
#define NX51_Pio_GetOut1()        PEEK(Adr_NX51_mmio_pio_out_line_cfg1)
#define NX51_Pio_SetOut1(line)    POKE(Adr_NX51_mmio_pio_out_line_cfg1,line)

#endif /* __NX51_PIO_H */
