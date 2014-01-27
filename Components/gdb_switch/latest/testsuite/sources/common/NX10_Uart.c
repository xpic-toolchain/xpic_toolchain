#include "NX10_UART.h" // function prototypes and defines
#include "defines.h"
#include "regdef_netx10.h"
#include <stdarg.h>

/*****************************************************************************/
/* Definitions                                                               */
/*****************************************************************************/

const unsigned int aNX10UartBaseAdr[2] = {
  Addr_NX10_uart0,
  Addr_NX10_uart1
};

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
void NX10_Uart_InitSerial(unsigned int ulUartNr)
{
  unsigned int ulUartBaseAdr;

  // get uart base adr
  ulUartBaseAdr = aNX10UartBaseAdr[ulUartNr];

  // First of all disable everything
  POKE(ulUartBaseAdr+REL_Adr_NX10_uartcr, 0);

  // Set the bit for the second baudrate mode
  POKE(ulUartBaseAdr+REL_Adr_NX10_uartcr_2, MSK_NX10_uartcr_2_Baud_Rate_Mode);

  // Adjust the baudrate register
  POKE(ulUartBaseAdr+REL_Adr_NX10_uartlcr_l, NX10_DEV_BAUDRATE_DIV_LO);
  POKE(ulUartBaseAdr+REL_Adr_NX10_uartlcr_m, NX10_DEV_BAUDRATE_DIV_HI);

  // set UART to 8 bits, 1 stop bit, no parity, fifo enabled
  POKE( ulUartBaseAdr+REL_Adr_NX10_uartlcr_h, (MSK_NX10_uartlcr_h_WLEN | MSK_NX10_uartlcr_h_FEN));
  // Set TX-Driver to enabled
  POKE( ulUartBaseAdr+REL_Adr_NX10_uartdrvout, MSK_NX10_uartdrvout_DRVTX);
  // Finally enable the UART
  POKE( ulUartBaseAdr+REL_Adr_NX10_uartcr, MSK_NX10_uartcr_uartEN);
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
void NX10_Uart_CloseSerial(unsigned int ulUartNr)
{
   unsigned int ulUartBaseAdr;
   volatile unsigned int uFoo;
   unsigned int uCnt;

   // get uart base adr
   ulUartBaseAdr = aNX10UartBaseAdr[ulUartNr];

   while((PEEK(ulUartBaseAdr+REL_Adr_NX10_uartfr) & MSK_NX10_uartfr_BUSY)!=0); /* UART busy ? */
   POKE(ulUartBaseAdr+REL_Adr_NX10_uartcr,0);   /* First of all disable everything */
   POKE(ulUartBaseAdr+REL_Adr_NX10_uartlcr_m,0); /* Adjust the baudrate register */
   POKE(ulUartBaseAdr+REL_Adr_NX10_uartlcr_l,0);
   POKE(ulUartBaseAdr+REL_Adr_NX10_uartlcr_h,0);  /* UART to be 8 bits, 1 stop bit, no parity, fifo enabled */
   POKE(ulUartBaseAdr+REL_Adr_NX10_uartdrvout,0); /* Set TX-Driver to disabled */
   for(uCnt=0;uCnt<5;uCnt++) {
     uFoo = PEEK(ulUartBaseAdr+REL_Adr_NX10_uartdr); /* Read out the reception register if there are still bytes in it */
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
void NX10_Uart_PutCharacter(unsigned int ulUartNr, const unsigned char bChar)
{
  unsigned int ulUartBaseAdr;

  // get uart base adr
  ulUartBaseAdr = aNX10UartBaseAdr[ulUartNr];

  // Wait until there is space in the FIFO
  while( (PEEK(ulUartBaseAdr+REL_Adr_NX10_uartfr)&MSK_NX10_uartfr_TXFF)!=0 ) {};
  POKE( ulUartBaseAdr+REL_Adr_NX10_uartdr, bChar);
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
int NX10_Uart_GetCharacter(unsigned int ulUartNr)
{
  unsigned int uDat;
  unsigned int ulUartBaseAdr;


  // get uart base adr
  ulUartBaseAdr = aNX10UartBaseAdr[ulUartNr];

  // We need to wait a defined time for a character
  while((PEEK(ulUartBaseAdr+REL_Adr_NX10_uartfr)&MSK_NX10_uartfr_RXFE)!=0); // Wait infinte for new data in the FIFO

  uDat = PEEK(ulUartBaseAdr+REL_Adr_NX10_uartdr);  // Get the received byte
  if( (uDat&0xFFFFFF00)!=0 ) { // we had a receive error
    POKE(ulUartBaseAdr+REL_Adr_NX10_uartrsr, 0); // Clear the error
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
*   1   Character can be read from the fifo (with NX10_Uart_GetCharacter())  */
/*****************************************************************************/
int NX10_Uart_Peek(unsigned int ulUartNr)
{
  unsigned int ulUartBaseAdr;

  // get uart base adr
  ulUartBaseAdr = aNX10UartBaseAdr[ulUartNr];
  // check fifo_empty bit of the fifo status register
  return ( (PEEK(ulUartBaseAdr+REL_Adr_NX10_uartfr)&MSK_NX10_uartfr_RXFE)==0 );
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
void NX10_Uart_UPrintf(const char* pchFmt, ...)
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
      NX10_Uart_PutCharacter(NX10_UART_STD, cChar);
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
        NX10_Uart_PutCharacter(NX10_UART_STD, uDigit);
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
void NX10_Uart_UPrintfDec(const char* pchFmt, ...)
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
      NX10_Uart_PutCharacter(NX10_UART_STD, cChar);
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
         NX10_Uart_PutCharacter(NX10_UART_STD, uDigit);
         ulArgument = ulArgument % iDivider;
        iDivider = iDivider / 10;
      } while( iDivider>0 );
    }
  }
}
