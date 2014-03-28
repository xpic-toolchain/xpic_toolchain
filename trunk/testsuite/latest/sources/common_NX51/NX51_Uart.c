#include "NX51_UART.h"
#include "defines.h"
#include "netx51_regdef.h"
#include <stdarg.h>

/*****************************************************************************/
/* Definitions                                                               */
/*****************************************************************************/

const unsigned int aNX51UartBaseAdr[3] = {
  Addr_NX51_uart0,
  Addr_NX51_uart1,
  Addr_NX51_uart2
};

/* The standard output UART number
 *  NX10_Uart_UPrintf() and NX10_Uart_UPrintfDec() use this UART for output
 */
unsigned int NX51_UART_STD = 0;

/*****************************************************************************/
/*! Init serial Port
* \description
*   Initialize serial port.
* \class
*   UART
* \params
*   ulUartNr              [in]  Number of Uart
* \return
*                                                                            */
/*****************************************************************************/
void NX51_Uart_InitSerial(unsigned int ulUartNr)
{
  unsigned int ulUartBaseAdr;

  // get uart base adr
  ulUartBaseAdr = aNX51UartBaseAdr[ulUartNr];

  // First of all disable everything
  POKE(ulUartBaseAdr+REL_Adr_NX51_uartcr, 0);

  // Set the bit for the second baudrate mode
  POKE(ulUartBaseAdr+REL_Adr_NX51_uartcr_2, MSK_NX51_uartcr_2_Baud_Rate_Mode);

  // Adjust the baudrate register
  POKE(ulUartBaseAdr+REL_Adr_NX51_uartlcr_l, NX51_DEV_BAUDRATE_DIV_LO);
  POKE(ulUartBaseAdr+REL_Adr_NX51_uartlcr_m, NX51_DEV_BAUDRATE_DIV_HI);

  // set UART to 8 bits, 1 stop bit, no parity, fifo enabled
  POKE( ulUartBaseAdr+REL_Adr_NX51_uartlcr_h, (MSK_NX51_uartlcr_h_WLEN | MSK_NX51_uartlcr_h_FEN));
  // Set TX-Driver to enabled
  POKE( ulUartBaseAdr+REL_Adr_NX51_uartdrvout, MSK_NX51_uartdrvout_DRVTX);
  // Finally enable the UART
  POKE( ulUartBaseAdr+REL_Adr_NX51_uartcr, MSK_NX51_uartcr_uartEN);
}

/*****************************************************************************/
/*! Close Serial Port
* \description
*   Closes serial port.
* \class
*   UART
* \params
*   ulUartNr              [in]  Number of Uart
* \return
*                                                                            */
/*****************************************************************************/
void NX51_Uart_CloseSerial(unsigned int ulUartNr)
{
   unsigned int ulUartBaseAdr;
   volatile unsigned int uFoo;
   unsigned int uCnt;

   // get uart base adr
   ulUartBaseAdr = aNX51UartBaseAdr[ulUartNr];

   while((PEEK(ulUartBaseAdr+REL_Adr_NX51_uartfr) & MSK_NX51_uartfr_BUSY)!=0); /* UART busy ? */
   POKE(ulUartBaseAdr+REL_Adr_NX51_uartcr,0);   /* First of all disable everything */
   POKE(ulUartBaseAdr+REL_Adr_NX51_uartlcr_m,0); /* Adjust the baudrate register */
   POKE(ulUartBaseAdr+REL_Adr_NX51_uartlcr_l,0);
   POKE(ulUartBaseAdr+REL_Adr_NX51_uartlcr_h,0);  /* UART to be 8 bits, 1 stop bit, no parity, fifo enabled */
   POKE(ulUartBaseAdr+REL_Adr_NX51_uartdrvout,0); /* Set TX-Driver to disabled */
   for(uCnt=0;uCnt<5;uCnt++) {
     uFoo = PEEK(ulUartBaseAdr+REL_Adr_NX51_uartdr); /* Read out the reception register if there are still bytes in it */
   }
}

/*****************************************************************************/
/*! Put Character
* \description
*   Writes a character into uart fifo.
* \class
*   UART
* \params
*   ulUartNr              [in]  Number of Uart
*   bChar                 [in]  Charcter Content
* \return
*                                                                            */
/*****************************************************************************/
void NX51_Uart_PutCharacter(unsigned int ulUartNr, const unsigned char bChar)
{
  unsigned int ulUartBaseAdr;

  // get uart base adr
  ulUartBaseAdr = aNX51UartBaseAdr[ulUartNr];

  // Wait until there is space in the FIFO
  while( (PEEK(ulUartBaseAdr+REL_Adr_NX51_uartfr)&MSK_NX51_uartfr_TXFF)!=0 ) {};
  POKE( ulUartBaseAdr+REL_Adr_NX51_uartdr, bChar);
}

/*****************************************************************************/
/*! Get Character
* \description
*   Wait until uart receive fifo contains a character.
*   Read a character from uart fifo.
* \class
*   UART
* \params
*   ulUartNr              [in]  Number of Uart
* \return
*   reveived character
*   -1 on errors                                                             */
/*****************************************************************************/
int NX51_Uart_GetCharacter(unsigned int ulUartNr)
{
  unsigned int uDat;
  unsigned int ulUartBaseAdr;


  // get uart base adr
  ulUartBaseAdr = aNX51UartBaseAdr[ulUartNr];

  // We need to wait a defined time for a character
  while((PEEK(ulUartBaseAdr+REL_Adr_NX51_uartfr)&MSK_NX51_uartfr_RXFE)!=0); // Wait infinte for new data in the FIFO

  uDat = PEEK(ulUartBaseAdr+REL_Adr_NX51_uartdr);  // Get the received byte
  if( (uDat&0xFFFFFF00)!=0 ) { // we had a receive error
    POKE(ulUartBaseAdr+REL_Adr_NX51_uartrsr, 0); // Clear the error
    return -1; // return error
  }  else {
    return (int)uDat;
  }
}

/*****************************************************************************/
/*! Uart Peek
* \description
*   Check if a character is available in the receive fifo.
* \class
*   UART
* \params
*   ulUartNr              [in]  Number of Uart
* \return
*   0   No character available
*   1   Character can be read from the fifo (with NX51_Uart_GetCharacter())  */
/*****************************************************************************/
int NX51_Uart_Peek(unsigned int ulUartNr)
{
  unsigned int ulUartBaseAdr;

  // get uart base adr
  ulUartBaseAdr = aNX51UartBaseAdr[ulUartNr];
  // check fifo_empty bit of the fifo status register
  return ( (PEEK(ulUartBaseAdr+REL_Adr_NX51_uartfr)&MSK_NX51_uartfr_RXFE)==0 );
}

/* The standard output UART number
 *  NX51_Uart_UPrintf() and NX51_Uart_UPrintfDec() use this UART for output
 */
/*****************************************************************************/
/*! Uart Set_STD
* \description
*   Changes standard output UART number, that functions
*   NX51_Uart_UPrintf() and NX51_Uart_UPrintfDec() use for output
* \class
*   UART
* \params
*   ulUartNr              [in]  Number of Uart
* \return
*                                                                            */
/*****************************************************************************/
void NX51_Uart_Set_STD(unsigned int ulUartNr)
{
	NX51_UART_STD = ulUartNr;
}

/*****************************************************************************/
/*! Uart Printf
* \description
*   This version of printf is very limited compared to the ANSI version.
*   It can only handle the following conversions:
*    §{length}   :  Interpret the argument as 'unsigned integer' and print it in hexadecimal.
*                   The 'length' parameter is optional. It must be one single decimal digit
*                   starting with '1' (i.e. '0' is no valid length)
*   Notes:
*    Passing a NULL pointer as the format string results in the output "NULL\n"
*    The routine does not know escaping the format identifier. This means it is not possible
*    to print '§' characters.
*    The default length for hexadecimal output is 8 digits.
* \class
*   UART
* \params
*   pchFmt              [in]   Pointer of Character to put out
* \return
*                                                                            */
/*****************************************************************************/
void NX51_Uart_UPrintf(const char* pchFmt, ...)
{
  int iLen;
  int iLenParam;
  unsigned int uDigit;
  char cChar;
  unsigned int ulArgument;
  va_list ptArgument;

  // Get initial pointer to first argument
  va_start(ptArgument, pchFmt);

  // Is it a NULL Pointer ?
  if( pchFmt==(char*)0 ) {
    pchFmt = "NULL\n";
  }

  while( (cChar=*(pchFmt++))!=0 ) {
    // is there a format command?
    if( cChar!='§') {
      // no format command -> direct output
      NX51_Uart_PutCharacter(NX51_UART_STD, cChar);
    } else {
      // get the argument
      ulArgument = va_arg((ptArgument),unsigned int);

      // default length is 8 digits
      iLen = 28;

      // get the optional length
      cChar = *pchFmt;
      iLenParam = cChar-'1';
      if( iLenParam>=0 && iLenParam<9 ) {
        iLen = iLenParam<<2;
        ++pchFmt;
      }

      do {
        uDigit = ulArgument>>iLen;
        uDigit &= 0x0f;
        if( uDigit>9 ) {
          uDigit += 'A'-('9'+1);
        }
        uDigit += '0';
        NX51_Uart_PutCharacter(NX51_UART_STD, uDigit);
        iLen -= 4;
      } while( iLen>=0 );
    }
  }
}

/*****************************************************************************/
/*! Uart Print a Decimal Value
* \description
*   Output a decimal value on uart port.
* \class
*   UART
* \params
*   pchFmt              [in]  Pointer of Character to put out
* \return
*                                                                            */
/*****************************************************************************/
void NX51_Uart_UPrintfDec(const char* pchFmt, ...)
{
  int iDivider;
  unsigned int uDigit;
  char cChar;
  unsigned int ulArgument;
  va_list ptArgument;

  // Get initial pointer to first argument
  va_start(ptArgument, pchFmt);

  // Is it a NULL Pointer ?
  if( pchFmt==(char*)0 ) {
    pchFmt = "NULL\n";
  }

  while( (cChar=*(pchFmt++))!=0 ) {
    // is there a format command?
    if( cChar!='§') {
      // no format command -> direct output
      NX51_Uart_PutCharacter(NX51_UART_STD, cChar);
    } else {
      // get the argument
      ulArgument = va_arg((ptArgument),unsigned int);

      // determine start value
      iDivider = 1000000000;
      if( ulArgument != 0 ) {
        while ((ulArgument / iDivider) == 0)
        {
           iDivider = iDivider / 10;
        }
      } else {
        iDivider = 0;
      }

      // print digits
      do {
        uDigit = (ulArgument / iDivider);
        uDigit += '0';
         NX51_Uart_PutCharacter(NX51_UART_STD, uDigit);
         ulArgument = ulArgument % iDivider;
        iDivider = iDivider / 10;
      } while( iDivider>0 );
    }
  }
}

/*****************************************************************************/
/*! Uart Setup an Interrupt Source
* \description
*   Initialize uart interrupt source.
* \class
*   UART
* \params
*   ulUartNr              [in]  Number of Uart
*   uiIRQ_Mask            [in]  Mask to setup interrupt source. It will be mask with "or" to another uart-interrupt sources
*   uiFillLevel           [in]  Fill level. Receive mask: the IRQ-bit will be set if the number of received bytes in the receive fifo are greater than or equal uiFillLevel.
*                               Transmit mask: the IRQ-bit will be set if the number of transmitted bytes in the transmit fifo are less than uiFillLevel.
* \return
*                                    */
/*****************************************************************************/
void NX51_Uart_SetupInterrupt(unsigned int uiUartNr, unsigned int uiIRQ_Mask, unsigned int uiFillLevel)
{
	  unsigned long ulUartBaseAdr;
	  // get uart base adr
	  ulUartBaseAdr = aNX51UartBaseAdr[uiUartNr];

	  // Finally enable an interrupt
	  POKE( ulUartBaseAdr+REL_Adr_NX51_uartcr, PEEK(ulUartBaseAdr+REL_Adr_NX51_uartcr) | uiIRQ_Mask );

	  /* set fill level for receive interrupt */
	  if(uiIRQ_Mask == NX51_UART_IRQ_RECEIVE)
		  /*  The IRQ-bit will be set if the number of received bytes in the receive fifo are greater than or equal uiFillLevel */
		  POKE(ulUartBaseAdr+REL_Adr_NX51_uartrxiflsel, uiFillLevel);

	  /* or set fill level for transmit interrupt */
	  if(uiIRQ_Mask == NX51_UART_IRQ_TRANSMIT)
		  /*  The IRQ-bit will be set if the number of transmitted bytes in the transmit fifo are less than uiFillLevel         */
		  POKE(ulUartBaseAdr+REL_Adr_NX51_uarttxiflsel, uiFillLevel);
}

/*****************************************************************************/
/*! Uart Disable an Interrupt Source
* \description
*   Initialize uart interrupt source.
* \class
*   UART
* \params
*   ulUartNr              [in]  Number of Uart
*   uiIRQ_Mask            [in]  Mask to disable interrupt source. It will be masked out, preserving another uart-interrupt sources
* \return
*                                    */
/*****************************************************************************/
void NX51_Uart_DisableInterrupt(unsigned int uiUartNr, unsigned int uiIRQ_Mask)
{
	  unsigned long ulUartBaseAdr;
	  // get uart base adr
	  ulUartBaseAdr = aNX51UartBaseAdr[uiUartNr];

	  // Finally mask out an interrupt
	  POKE( ulUartBaseAdr+REL_Adr_NX51_uartcr, PEEK(ulUartBaseAdr+REL_Adr_NX51_uartcr) & ~uiIRQ_Mask );
}
