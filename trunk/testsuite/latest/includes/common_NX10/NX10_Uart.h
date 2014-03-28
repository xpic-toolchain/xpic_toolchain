#ifndef __NX10_UART_H
#define __NX10_UART_H

/*****************************************************************************/
/* Definitions                                                               */
/*****************************************************************************/

#define NX10_UART_IRQ_RECEIVE         0x10
#define NX10_UART_IRQ_TRANSMIT        0x20

typedef unsigned long UART_BAUDRATE;
#define NX10_UART_BAUDRATE_300        3
#define NX10_UART_BAUDRATE_600        6
#define NX10_UART_BAUDRATE_1200      12
#define NX10_UART_BAUDRATE_2400      24
#define NX10_UART_BAUDRATE_4800      48
#define NX10_UART_BAUDRATE_9600      96
#define NX10_UART_BAUDRATE_19200    192
#define NX10_UART_BAUDRATE_38400    384
#define NX10_UART_BAUDRATE_57600    576
#define NX10_UART_BAUDRATE_115200  1152

#define NX10_DEV_BAUDRATE NX10_UART_BAUDRATE_115200

// DEV_BAUDRATE is 100 times to small -> multiply with 100 (or divide by DEV_FREQUENCY/100)
#define NX10_DEV_BAUDRATE_DIV_LO (((NX10_DEV_BAUDRATE*16*65536)/(NX10_DEV_FREQUENCY/100))&0xff)
#define NX10_DEV_BAUDRATE_DIV_HI (((NX10_DEV_BAUDRATE*16*65536)/(NX10_DEV_FREQUENCY/100))>>8)

/* The standard output UART number
 *  NX10_Uart_UPrintf() and NX10_Uart_UPrintfDec() use this UART for output
 */
#define NX10_UART_STD 0


/*****************************************************************************/
/* Function prototypes                                                       */
/*****************************************************************************/

void NX10_Uart_InitSerial(unsigned int ulUartNr);
void NX10_Uart_CloseSerial(unsigned int ulUartNr);
void NX10_Uart_PutCharacter(unsigned int ulUartNr, const unsigned char bChar);
int  NX10_Uart_GetCharacter(unsigned int ulUartNr);
int  NX10_Uart_Peek(unsigned int ulUartNr);
void NX10_Uart_UPrintf(const char* pchFmt, ...);
void NX10_Uart_UPrintfDec(const char* pchFmt, ...);

/* Function aliases for compatibility reasons */
#define NX10_UART_InitSerial NX10_Uart_InitSerial
#define NX10_UART_CloseSerial NX10_Uart_CloseSerial
#define NX10_UART_PutCharacter NX10_Uart_PutCharacter
#define NX10_UART_GetCharacter NX10_Uart_GetCharacter
#define NX10_UART_Peek NX10_Uart_Peek
#define NX10_UART_UPrintf NX10_Uart_UPrintf
#define NX10_UART_UPrintfDec NX10_Uart_UPrintfDec

void NX10_Uart_SetupInterrupt  (unsigned int uiUartNr, unsigned int uiIRQ_Mask, unsigned int uiFillLevel);
void NX10_Uart_DisableInterrupt(unsigned int uiUartNr, unsigned int uiIRQ_Mask);
#endif /* __NX10_UART_H */
