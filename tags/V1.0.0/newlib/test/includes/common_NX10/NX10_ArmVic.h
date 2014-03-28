#ifndef __NX10_ARMVIC_H
#define __NX10_ARMVIC_H

#include "regdef_netx10.h"
#include "defines.h"

#define NX10_ArmVic_SetSwIrq(mask)   POKE(Adr_NX10_vic_softint,mask)
#define NX10_ArmVic_GetSwIrq()       PEEK(Adr_NX10_vic_softint)
#define NX10_ArmVic_ClearSwIrq(mask) POKE(Adr_NX10_vic_softint_clear,mask)

#endif /* __NX10_ARMVIC_H */
