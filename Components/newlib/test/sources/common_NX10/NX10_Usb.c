#include "NX10_Usb.h" // function prototypes and defines
#include "defines.h"
#include "regdef_netx10.h"
#include <string.h>


/*****************************************************************************/
/* Definitions                                                               */
/*****************************************************************************/

#define NX10_USB_FIFO_UART_TX 4
#define NX10_USB_FIFO_UART_RX 5

#define MSK_NX10_usb_dev_fifo_ctrl_mode_fifo4              0x00001000
#define MSK_NX10_usb_dev_fifo_ctrl_out_handshake_ack_fifo4 0x00000010

/* why is this not defined in regdef? */
typedef struct NX10_USB_DEV_ENUM_RAM_AREA_Ttag
{
  volatile unsigned long aulUsb_dev_enum_ram_descriptors[2];
  volatile unsigned long aulUsb_dev_enum_ram_string_descriptors[14];
} NX10_USB_DEV_ENUM_RAM_AREA_T;

#define NX10_USB_STRDESC_OFS_VENDOR       0
#define NX10_USB_STRDESC_OFS_PRODUCT     18
#define NX10_USB_STRDESC_OFS_SERIAL      36

void NX10_Fifo_ReadBytes(unsigned long ulFillLevel, unsigned char* pbBuffer, volatile unsigned long* pulFifo);


/*****************************************************************************/
/* Variables                                                                 */
/*****************************************************************************/

static NX10_USB_DEV_CTRL_AREA_T*      const s_ptUsbCtrl     = (NX10_USB_DEV_CTRL_AREA_T*)Addr_NX10_usb_dev_ctrl;
static NX10_USB_DEV_FIFO_CTRL_AREA_T* const s_ptUsbFifoCtrl = (NX10_USB_DEV_FIFO_CTRL_AREA_T*)Addr_NX10_usb_dev_fifo_ctrl;
static NX10_USB_DEV_FIFO_AREA_T*      const s_ptUsbFifo     = (NX10_USB_DEV_FIFO_AREA_T*)Addr_NX10_usb_dev_fifo;
static NX10_USB_DEV_ENUM_RAM_AREA_T*  const s_ptUsbEnumRam  = (NX10_USB_DEV_ENUM_RAM_AREA_T*)Addr_NX10_usb_dev_enum_ram;


/*****************************************************************************/
/* Functions                                                                 */
/*****************************************************************************/

/*****************************************************************************/
/*! Generate String Descriptor
* \description
*   Copy maximum 16 characters to a specified buffer with a usb descriptor
*   type and length field.
* \class
*   USB
* \params
*   pbBuf              [out]  String Descriptor with type/length field
*   pszString          [in]   Raw zero terminated input string
* \return
*                                                                            */
/*****************************************************************************/
void NX10_Usb_GenStringDescriptor(unsigned char* pbBuf, unsigned char* pszString)
{
  unsigned char bLen;
  int iOfs;

  pbBuf[1] = 3; /* USB String Descriptor Type */

  bLen = 0;
  iOfs = 2;

  if( pszString != NULL )
  {
    do {

      if( pszString[bLen] == 0 )
        break;

      pbBuf[iOfs++] = pszString[bLen];

    } while(++bLen < 16);
  }

  pbBuf[0] = bLen + 2;
}

/*****************************************************************************/
/*! Reset USB State
* \description
*   Soft reset of all usb_dev_fifos.
*   This function may be called during runtime without irq lock or usb_dev disabled.
* \class
*   USB
* \params
* \return
*                                                                            */
/*****************************************************************************/
void NX10_Usb_Reset(void)
{
  unsigned long ulValue;

  /* reset all FIFOs */
  ulValue  = DFLT_VAL_NX10_usb_dev_fifo_ctrl_conf_auto_out_ack;
  ulValue |= DFLT_VAL_NX10_usb_dev_fifo_ctrl_conf_mode;

  ulValue &= ~MSK_NX10_usb_dev_fifo_ctrl_mode_fifo4;

  s_ptUsbFifoCtrl->ulUsb_dev_fifo_ctrl_conf = ulValue | MSK_NX10_usb_dev_fifo_ctrl_conf_reset;

  /* release the reset */
  s_ptUsbFifoCtrl->ulUsb_dev_fifo_ctrl_conf = ulValue;

  //s_ptUsbFifoCtrl->ulUsb_dev_fifo_ctrl_out_handshake = MSK_NX10_usb_dev_fifo_ctrl_out_handshake_ack;
  s_ptUsbFifoCtrl->ulUsb_dev_fifo_ctrl_out_handshake = 0;

  s_ptUsbFifoCtrl->ulUsb_dev_fifo_ctrl_in_handshake  = 0;
}


/*****************************************************************************/
/*! Initialize USB Device
* \description
*   Prepare the USB device for packet based communication.
*   This function should be called only once during runtime.
* \class
*   USB
* \params
* \return
*                                                                            */
/*****************************************************************************/
void NX10_Usb_Init(unsigned short usVID,
                   unsigned short usPID,
                   unsigned short usPREL,
                   unsigned short usCHPOW,
                   unsigned char* pbVendorName,
                   unsigned char* pbProductName,
                   unsigned char* pbSerialStr)
{
  unsigned long ulValue;
  union {
    unsigned long aulRegs[14];
    unsigned char abBytes[14*4];
  } tStrDesc;

  /* Reset and disable core */
  s_ptUsbCtrl->ulUsb_dev_cfg  = MSK_NX10_usb_dev_cfg_usb_dev_reset;
  s_ptUsbCtrl->ulUsb_dev_cfg &= ~MSK_NX10_usb_dev_cfg_usb_dev_reset;

  NX10_Usb_Reset();

  /* Device Descriptor */
  ulValue = (usPID << 16) | usVID;
  s_ptUsbEnumRam->aulUsb_dev_enum_ram_descriptors[0] = ulValue;

  ulValue = (usCHPOW << 16)  | usPREL;
  s_ptUsbEnumRam->aulUsb_dev_enum_ram_descriptors[1] = ulValue;

  memset(&tStrDesc, 0, sizeof(tStrDesc));

  NX10_Usb_GenStringDescriptor(&tStrDesc.abBytes[NX10_USB_STRDESC_OFS_VENDOR], pbVendorName);
  NX10_Usb_GenStringDescriptor(&tStrDesc.abBytes[NX10_USB_STRDESC_OFS_PRODUCT], pbProductName);
  NX10_Usb_GenStringDescriptor(&tStrDesc.abBytes[NX10_USB_STRDESC_OFS_SERIAL], pbSerialStr);

  for(ulValue = 0; ulValue < 14; ++ulValue)
  {
    s_ptUsbEnumRam->aulUsb_dev_enum_ram_string_descriptors[ulValue] = tStrDesc.aulRegs[ulValue];
  }

  s_ptUsbCtrl->ulUsb_dev_irq_mask = 0;

  /* Enable Core */
  s_ptUsbCtrl->ulUsb_dev_cfg = MSK_NX10_usb_dev_cfg_usb_core_enable;
}


/*****************************************************************************/
/*! Shutdown USB Device
* \description
*   Disable USB
* \class
*   USB
* \params
* \return
*                                                                            */
/*****************************************************************************/
void NX10_Usb_Deinit(void)
{
  s_ptUsbCtrl->ulUsb_dev_cfg = 0;
}


/*****************************************************************************/
/*! Poll USB Connection
* \description
*   Query the connection state.
* \class
*   USB
* \params
* \return
*   0 no host connected
*   1 connection established
*                                                                            */
/*****************************************************************************/
int NX10_Usb_PollConnection(void)
{
  unsigned long ulValue;
  int     iRet = 0;

  /* check if the device is addressed and configured */
  ulValue  = s_ptUsbCtrl->ulUsb_dev_status;
  ulValue &= (MSK_NX10_usb_dev_status_usb_configured | MSK_NX10_usb_dev_status_usb_addressed);

  if((MSK_NX10_usb_dev_status_usb_configured | MSK_NX10_usb_dev_status_usb_addressed) == ulValue)
  {
    /* connected */
    iRet = 1;
  } else
  {
    iRet = 0;
  }
  return iRet;
}


/*****************************************************************************/
/*! Receive Data
* \description
*   Read USB receive FIFO and copy the data into a buffer.
* \class
*   USB
* \params
* \return
*   number of bytes received
*                                                                            */
/*****************************************************************************/
unsigned long  NX10_Usb_CdcRecv(unsigned char* pbBuffer)
{
  unsigned long ulFillLevel = s_ptUsbFifoCtrl->aulUsb_dev_fifo_ctrl_status[NX10_USB_FIFO_UART_RX] & MSK_NX10_usb_dev_fifo_ctrl_status0_out_fill_level;

  if( ulFillLevel != 0 )
    NX10_Fifo_ReadBytes(ulFillLevel, pbBuffer, &s_ptUsbFifo->aulUsb_dev_fifo[NX10_USB_FIFO_UART_RX]);

  return ulFillLevel;
}


/*****************************************************************************/
/*! Send Data
* \description
*   Copy as much bytes into the transmit fifo as possible.
*   Wait for the packet to be sent.
* \class
*   USB
* \params
* \return
*                                                                            */
/*****************************************************************************/
unsigned long NX10_Usb_CdcSend(unsigned char* pbBuffer, unsigned long ulNumBytes)
{
  unsigned long ulFillLevel;
  unsigned long ulBytesToSend;
  unsigned long ulVal;

  ulFillLevel   = s_ptUsbFifoCtrl->aulUsb_dev_fifo_ctrl_status[NX10_USB_FIFO_UART_TX];
  ulFillLevel  &= MSK_NX10_usb_dev_fifo_ctrl_status0_in_fill_level;
  ulFillLevel >>= SRT_NX10_usb_dev_fifo_ctrl_status0_in_fill_level;

  ulBytesToSend = 64 - ulFillLevel;

  if( ulNumBytes < ulBytesToSend )
    ulBytesToSend = ulNumBytes;

  for(ulFillLevel = 0; ulFillLevel < ulBytesToSend; ++ulFillLevel)
  {
    s_ptUsbFifo->aulUsb_dev_fifo[NX10_USB_FIFO_UART_TX] = *pbBuffer++;
  }

  /* ack fifo */
  s_ptUsbFifoCtrl->ulUsb_dev_fifo_ctrl_in_handshake = MSK_NX10_usb_dev_fifo_ctrl_out_handshake_ack_fifo4;

  /* wait for packet to be sent */
  do
  {
    ulVal = s_ptUsbCtrl->ulUsb_dev_irq_raw;

    /* detect usb reset from bus */
    if( (ulVal & MSK_NX10_usb_dev_irq_raw_reset_detected) != 0 )
    {
      s_ptUsbFifoCtrl->ulUsb_dev_fifo_ctrl_in_handshake = 0;
      return 0;
    }
  } while( (ulVal & MSK_NX10_usb_dev_irq_raw_uart_tx_packet_sent) == 0 );

  /* confirm irq */
  s_ptUsbCtrl->ulUsb_dev_irq_raw = MSK_NX10_usb_dev_irq_raw_uart_tx_packet_sent;

  s_ptUsbFifoCtrl->ulUsb_dev_fifo_ctrl_in_handshake = 0;

  return ulBytesToSend;
}


/*****************************************************************************/
/*! Get Receive FIFO fill level
* \description
* \class
*   USB
* \params
* \return
*   RX FIFO fill level in bytes
*                                                                            */
/*****************************************************************************/
unsigned long NX10_Usb_CdcBufRecPeek(void)
{
  /* get the Usb RX input fill level */
  unsigned long ulFillLevel = s_ptUsbFifoCtrl->aulUsb_dev_fifo_ctrl_status[NX10_USB_FIFO_UART_RX];
  ulFillLevel  &= MSK_NX10_usb_dev_fifo_ctrl_status0_out_fill_level;
  ulFillLevel >>= SRT_NX10_usb_dev_fifo_ctrl_status0_out_fill_level;

  return ulFillLevel;
}
