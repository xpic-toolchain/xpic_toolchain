#ifndef __NX10_PFIFO_H
#define __NX10_PFIFO_H

#include "regdef_netx10.h"
#include "defines.h"

#define NX10_Pfifo_IsEmpty(fifoNo) ((PEEK(Adr_NX10_pointer_fifo_motion_pfifo_empty) & (1 << fifoNo)) != 0)
#define NX10_Pfifo_IsFull(fifoNo) ((PEEK(Adr_NX10_pointer_fifo_motion_pfifo_full) & (1 << fifoNo)) != 0)

#define NX10_Pfifo_Read(fifoNo) PEEK(Adr_NX10_pointer_fifo_motion_pfifo_base + (fifoNo<<2))
#define NX10_Pfifo_Write(fifoNo,elem) POKE(Adr_NX10_pointer_fifo_motion_pfifo_base + (fifoNo<<2),(unsigned long)(elem))

#endif /* __NX10_PFIFO_H */
