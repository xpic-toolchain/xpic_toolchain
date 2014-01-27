#ifndef _HAL_RESOURCES_DEFINES_NETX10_H
#define _HAL_RESOURCES_DEFINES_NETX10_H

#include "regdef_netx10.h"
#include "common_func_netx10.h" // used for function names 

/*****************************************************************************/
/*  NOTE: These addresses must be replaced if a non 1:1 MMU mapping is used. */
/*****************************************************************************/

typedef int(*NX10_PFN_MIIMU_READ_PHY_REG) ( unsigned int uMiimuPreamble, unsigned int uMiimuMdcFreq, unsigned int uMiimuRtaField, unsigned int uMiimuPhyAddr, unsigned int uMiimuReqAddr, unsigned short* pusData, void* pvUser );
typedef int(*NX10_PFN_MIIMU_WRITE_PHY_REG)( unsigned int uMiimuPreamble, unsigned int uMiimuMdcFreq, unsigned int uMiimuPhyAddr, unsigned int uMiimuReqAddr, unsigned short usData, void* pvUser );

// *** ASIC Control area ***
#define __USE_ASICCTRL static NX10_ASIC_CTRL_AREA_T* s_ptAsicCtrl = (NX10_ASIC_CTRL_AREA_T*) Addr_NX10_asic_ctrl;

// *** GPIO area ***
#define __USE_GPIO static NX10_PIO_AREA_T* s_pulGpio = (NX10_GPIO_AREA_T*) Addr_NX10_gpio;

// *** MMIO_CTRL area ***
#define __USE_MMIO_CTRL  static NX10_MMIO_CTRL_AREA_T* s_ptMmioCtrl = (NX10_MMIO_CTRL_AREA_T*) Addr_NX10_mmio_ctrl;

// *** MIIMU area ***
#define __USE_MIIMU  static NX10_MIIMU_AREA_T* s_ptMiimu = (NX10_MIIMU_AREA_T*)Addr_NX10_miimu;

#define __USE_MIIMU_FUNC_READ_PHY   static NX10_PFN_MIIMU_READ_PHY_REG  pfnMiimuReadPhyReg  = NX10_MIIMU_ReadPhyReg;
#define __USE_MIIMU_FUNC_WRITE_PHY  static NX10_PFN_MIIMU_WRITE_PHY_REG pfnMiimuWritePhyReg = NX10_MIIMU_WritePhyReg;

// *** SYSTIME area ***
typedef void(*NX10_PFN_SYSTIME_SET_BORDER)  ( unsigned long ulBorder, void* pvUser );
typedef void(*NX10_PFN_SYSTIME_SET_SPEED)   ( unsigned long ulSpeed, void* pvUser );
typedef void(*NX10_PFN_SYSTIME_SET_SYSTIME) ( unsigned long ulSystime_s, unsigned long ulSystime_ns, void* pvUser );
typedef void(*NX10_PFN_SYSTIME_GET_SYSTIME) ( unsigned long* pulSystime_s, unsigned long* pulSystime_ns, void* pvUser );

#define __USE_SYSTIME static NX10_SYSTIME_AREA_T* s_ptSystime = (NX10_SYSTIME_AREA_T*)Addr_NX10_systime;
#define __USE_SYSTIME_FUNC_SET_BORDER  static NX10_PFN_SYSTIME_SET_BORDER  pfnSystimeSetBorder   = NX10_SYSTIME_SetBorder;
#define __USE_SYSTIME_FUNC_SET_SPEED   static NX10_PFN_SYSTIME_SET_SPEED   pfnSystimeSetSpeed    = NX10_SYSTIME_SetSpeed;
#define __USE_SYSTIME_FUNC_SET_SYSTIME static NX10_PFN_SYSTIME_SET_SYSTIME pfnSystimeSetSystime  = NX10_SYSTIME_SetSystime;
#define __USE_SYSTIME_FUNC_GET_SYSTIME static NX10_PFN_SYSTIME_GET_SYSTIME pfnSystimeGetSystime  = NX10_SYSTIME_GetSystime;

// *** xPEC Register area ***
#define __USE_XPEC_REGS                                 \
  static NX10_XPEC_AREA_T* s_ptXpecRegArea[] =          \
  {                                                     \
    (NX10_XPEC_AREA_T*) Addr_NX10_xpec0                 \
  };

// *** xPEC PRAM area ***
#define __USE_XPEC_PRAM                                 \
  static volatile unsigned long* s_pulXpecPramArea[] =  \
  {                                                     \
    (volatile unsigned long*) Adr_NX10_xpec0_pram_start \
  };

// *** xPEC DRAM area ***
#define __USE_XPEC_DRAM                                 \
  static volatile unsigned long* s_pulXpecDramArea[] =  \
  {                                                     \
    (volatile unsigned long*) Adr_NX10_xpec0_dram_start \
  };

// *** xMAC area ***
#define __USE_XMAC                              \
  static NX10_XMAC_AREA_T* s_ptXmacArea[] =     \
  {                                             \
    (NX10_XMAC_AREA_T*) Addr_NX10_xmac0         \
  };

// *** XC loader ***
typedef int(*NX10_PFN_XC_RESET)(unsigned int uPortNo, void* pvUser);
typedef int(*NX10_PFN_XC_LOAD) (unsigned int uPortNo, NX10_XC_TYPE_E eXcType, const unsigned long* pulXcPrg, void* pvUser);
typedef int(*NX10_PFN_XC_START)(unsigned int uPortNo, void* pvUser);

#define __USE_XC_LOADER_FUNC_RESET static NX10_PFN_XC_RESET pfnXcReset = NX10_XC_Reset;
#define __USE_XC_LOADER_FUNC_LOAD  static NX10_PFN_XC_LOAD  pfnXcLoad  = NX10_XC_Load;
#define __USE_XC_LOADER_FUNC_START static NX10_PFN_XC_START pfnXcStart = NX10_XC_Start;

// *** Pointer FIFO area ***
#define __USE_POINTERFIFO static NX10_POINTER_FIFO_AREA_T*  s_ptPFifo = (NX10_POINTER_FIFO_AREA_T*) Addr_NX10_pointer_fifo;

// *** xPEC IRQ area ***
#define __USE_XPEC_IRQS  static NX10_XPEC_IRQ_REGISTERS_AREA_T* s_ptXpecIrqs = (NX10_XPEC_IRQ_REGISTERS_AREA_T*) Addr_NX10_xpec_irq_registers;

// *** Buffer Manager area ***
#define __USE_BUFMAN static NX10_BUF_MAN_AREA_T* s_ptBufMan = (NX10_BUF_MAN_AREA_T*) Addr_NX10_buf_man;

// *** INTRAM area ***
#define __USE_INTRAM    \
  static volatile unsigned long* s_pulIntRamStart[] =           \
  {                                                             \
    (volatile unsigned long*) Adr_NX10_intram4_intram4_base     \
  };

// *** xPIC Control/Debug area ***
#define __USE_XPIC  static NX10_XPIC_AREA_T*           s_ptXpic        = (NX10_XPIC_AREA_T*) Addr_NX10_xpic;
#define __USE_XPIC_DEBUG  static NX10_XPIC_DEBUG_AREA_T*     s_ptXpicDebug   = (NX10_XPIC_DEBUG_AREA_T*) Addr_NX10_xpic_debug;

typedef void(*NX10_PFN_XPIC_RESET)(void* pvUser);
typedef void(*NX10_PFN_XPIC_LOAD) (const unsigned long* pulXpicPrg, void* pvUser);
typedef void(*NX10_PFN_XPIC_START)(void* pvUser);

// *** xPIC loader ***
#define __USE_XPIC_LOADER_FUNC_RESET static NX10_PFN_XPIC_RESET pfnXpicReset = NX10_XPIC_Reset;
#define __USE_XPIC_LOADER_FUNC_LOAD  static NX10_PFN_XPIC_LOAD  pfnXpicLoad  = NX10_XPIC_Load;
#define __USE_XPIC_LOADER_FUNC_START static NX10_PFN_XPIC_START pfnXpicStart = NX10_XPIC_Start;

// *** xPIC DRAM area ***
#define __USE_XPICDRAM  volatile unsigned long* pulXPicDpm = (volatile unsigned long*) Adr_NX10_xpic_dram_start;


#endif /* _HAL_RESOURCES_DEFINES_NETX10_H */
