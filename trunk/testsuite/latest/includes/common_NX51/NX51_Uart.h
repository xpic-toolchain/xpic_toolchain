#ifndef __NX51_UART_H
#define __NX51_UART_H

/*****************************************************************************/
/* Definitions                                                               */
/*****************************************************************************/

#define NX51_UART_IRQ_RECEIVE         0x10
#define NX51_UART_IRQ_TRANSMIT        0x20

typedef unsigned long UART_BAUDRATE;
#define NX51_UART_BAUDRATE_300        3
#define NX51_UART_BAUDRATE_600        6
#define NX51_UART_BAUDRATE_1200      12
#define NX51_UART_BAUDRATE_2400      24
#define NX51_UART_BAUDRATE_4800      48
#define NX51_UART_BAUDRATE_9600      96
#define NX51_UART_BAUDRATE_19200    192
#define NX51_UART_BAUDRATE_38400    384
#define NX51_UART_BAUDRATE_57600    576
#define NX51_UART_BAUDRATE_115200  1152

#define NX51_DEV_BAUDRATE NX51_UART_BAUDRATE_115200

// DEV_BAUDRATE is 100 times to small -> multiply with 100 (or divide by DEV_FREQUENCY/100)
#define NX51_DEV_BAUDRATE_DIV_LO (((NX51_DEV_BAUDRATE*16*65536)/(NX51_DEV_FREQUENCY/100))&0xff)
#define NX51_DEV_BAUDRATE_DIV_HI (((NX51_DEV_BAUDRATE*16*65536)/(NX51_DEV_FREQUENCY/100))>>8)


/*****************************************************************************/
/* Function prototypes                                                       */
/*****************************************************************************/

void NX51_Uart_InitSerial(unsigned int ulUartNr);
void NX51_Uart_CloseSerial(unsigned int ulUartNr);
void NX51_Uart_PutCharacter(unsigned int ulUartNr, const unsigned char bChar);
int  NX51_Uart_GetCharacter(unsigned int ulUartNr);
int  NX51_Uart_Peek(unsigned int ulUartNr);
void NX51_Uart_Set_STD(unsigned int ulUartNr);
void NX51_Uart_UPrintf(const char* pchFmt, ...);
void NX51_Uart_UPrintfDec(const char* pchFmt, ...);

/* Function aliases for compatibility reasons */
#define NX51_UART_InitSerial NX51_Uart_InitSerial
#define NX51_UART_CloseSerial NX51_Uart_CloseSerial
#define NX51_UART_PutCharacter NX51_Uart_PutCharacter
#define NX51_UART_GetCharacter NX51_Uart_GetCharacter
#define NX51_UART_Peek NX51_Uart_Peek
#define NX51_UART_UPrintf NX51_Uart_UPrintf
#define NX51_UART_UPrintfDec NX51_Uart_UPrintfDec

void NX51_Uart_SetupInterrupt  (unsigned int uiUartNr, unsigned int uiIRQ_Mask, unsigned int uiFillLevel);
void NX51_Uart_DisableInterrupt(unsigned int uiUartNr, unsigned int uiIRQ_Mask);
#endif /* __NX51_UART_H */
