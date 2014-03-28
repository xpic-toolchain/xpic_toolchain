#ifndef __NX10_USB_H
#define __NX10_USB_H

#include "regdef_netx10.h"
#include "defines.h"


/*****************************************************************************/
/* Definitions                                                               */
/*****************************************************************************/

#define NX10_Usb_EnIrq(mask)      POKE(Adr_NX10_usb_dev_ctrl_usb_dev_irq_mask,PEEK(Adr_NX10_usb_dev_ctrl_usb_dev_irq_mask)|mask)
#define NX10_Usb_DisIrq(mask)     POKE(Adr_NX10_usb_dev_ctrl_usb_dev_irq_mask,PEEK(Adr_NX10_usb_dev_ctrl_usb_dev_irq_mask)&~mask)
#define NX10_Usb_ConfirmIrq(mask) POKE(Adr_NX10_usb_dev_ctrl_usb_dev_irq_raw,mask)
#define NX10_Usb_GetIrq()         PEEK(Adr_NX10_usb_dev_ctrl_usb_dev_irq_masked)
#define NX10_Usb_GetIrqRaw()      PEEK(Adr_NX10_usb_dev_ctrl_usb_dev_irq_raw)


/*****************************************************************************/
/* Function prototypes                                                       */
/*****************************************************************************/

void          NX10_Usb_Init(unsigned short usVID,
                            unsigned short usPID,
                            unsigned short usPREL,
                            unsigned short usCHPOW,
                            unsigned char* pbVendorName,
                            unsigned char* pbProductName,
                            unsigned char* pbSerialStr);

void          NX10_Usb_Deinit(void);

int           NX10_Usb_PollConnection(void);

unsigned long NX10_Usb_CdcRecv(unsigned char* pbBuffer);

unsigned long NX10_Usb_CdcSend(unsigned char* pbBuffer, unsigned long ulNumBytes);

unsigned long NX10_Usb_CdcBufRecPeek(void);

void          NX10_Usb_Reset(void);


#endif /* __NX10_USB_H */
