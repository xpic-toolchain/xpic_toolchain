#include <stdarg.h> // uprintf
#include "common_func_netx10.h" // function prototypes and defines

static NX10_ASIC_CTRL_AREA_T*      s_ptAsicCtrl    = (NX10_ASIC_CTRL_AREA_T*) Addr_NX10_asic_ctrl;
static NX10_GPIO_AREA_T*           s_ptGpio        = (NX10_GPIO_AREA_T*) Addr_NX10_gpio;
static NX10_POINTER_FIFO_AREA_T*   s_ptPFifo       = (NX10_POINTER_FIFO_AREA_T*) Addr_NX10_pointer_fifo;
static NX10_VIC_AREA_T*            s_ptVic         = (NX10_VIC_AREA_T*) Addr_NX10_vic;
static NX10_MMIO_CTRL_AREA_T*      s_ptMmioCtrl    = (NX10_MMIO_CTRL_AREA_T*) Addr_NX10_mmio_ctrl;
static NX10_MPWM_AREA_T*           s_ptPwm         = (NX10_MPWM_AREA_T*) Addr_NX10_mpwm;
static NX10_MENC_AREA_T*           s_ptEnc         = (NX10_MENC_AREA_T*) Addr_NX10_menc;
static NX10_ADC_CTRL_AREA_T*       s_ptAdc         = (NX10_ADC_CTRL_AREA_T*) Addr_NX10_adc_ctrl;
static NX10_DMAC_REG_AREA_T*       s_ptDmacReg     = (NX10_DMAC_REG_AREA_T*) Addr_NX10_dmac_reg;
static NX10_ARM_TIMER_AREA_T*      s_ptArmTimer    = (NX10_ARM_TIMER_AREA_T*) Addr_NX10_arm_timer;
static NX10_XPIC_TIMER_AREA_T*     s_ptXpicTimer   = (NX10_XPIC_TIMER_AREA_T*) Addr_NX10_xpic_timer;
static NX10_DPM_AREA_T*            s_ptDpm         = (NX10_DPM_AREA_T*) Addr_NX10_dpm;
static NX10_HANDSHAKE_CTRL_AREA_T* s_ptHsCtrl      = (NX10_HANDSHAKE_CTRL_AREA_T*) Addr_NX10_handshake_ctrl;
static NX10_XPIC_VIC_AREA_T*       s_ptXpicVic     = (NX10_XPIC_VIC_AREA_T*) Addr_NX10_xpic_vic;

/*
  _   _      _      ____    _____ 
 | | | |    / \    |  _ \  |_   _|
 | | | |   / _ \   | |_) |   | |  
 | |_| |  / ___ \  |  _ <    | |  
  \___/  /_/   \_\ |_| \_\   |_|  

*/

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
void NX10_UART_InitSerial( unsigned int ulUartNr )
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
void NX10_UART_CloseSerial(unsigned int ulUartNr)
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
void NX10_UART_PutCharacter(unsigned int ulUartNr, const unsigned char bChar)
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
*   Read a character from uart fifo.
* \class 
*   UART 
* \params
*   ulUartNr              [in]  Number of Uart
* \return
*   reveived character
*   -1 on erroneous                                                          */
/*****************************************************************************/
int NX10_UART_GetCharacter(unsigned int ulUartNr)
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
*   -
* \class 
*   UART 
* \params
*   ulUartNr              [in]  Number of Uart
* \return
*   0
*   1                                                                        */
/*****************************************************************************/
int NX10_UART_Peek(unsigned int ulUartNr)
{
  unsigned int ulUartBaseAdr;
  
  // get uart base adr
  ulUartBaseAdr = aNX10UartBaseAdr[ulUartNr];
  return ( (PEEK(ulUartBaseAdr+REL_Adr_NX10_uartfr)&MSK_NX10_uartfr_RXFE)==0 );
}

/*****************************************************************************/
/*! Uart Printf
* \description
*   This version of printf is very limited compared to the ANSI version.
*   It can only handle the following conversions:
*    �{length}   :  Interpret the argument as 'unsigned integer' and print it in hexadecimal.
*                   The 'length' parameter is optional. It must be one single decimal digit
*                   starting with '1' (i.e. '0' is no valid length)
*   Notes:
*    Passing a NULL pointer as the format string results in the output "NULL\n"
*    The routine does not know escaping the format identifier. This means it is not possible
*    to print '�' characters.
*    The default length for hexadecimal output is 8 digits.
* \class 
*   UART 
* \params
*   pchFmt              [in]   Pointer of Character to put out
* \return
*                                                                            */
/*****************************************************************************/
void NX10_UART_UPrintf(const char* pchFmt, ...)
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
    if( cChar!='�') {
      // no format command -> direct output
      NX10_UART_PutCharacter(0, cChar);
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
        NX10_UART_PutCharacter(0, uDigit);
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
void NX10_UART_UPrintfDec(const char* pchFmt, ...)
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
    if( cChar!='�') {
      // no format command -> direct output
      NX10_UART_PutCharacter(0, cChar);
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
         NX10_UART_PutCharacter(0, uDigit);
         ulArgument = ulArgument % iDivider;
        iDivider = iDivider / 10;
      } while( iDivider>0 );
    }
  }
}




/*
 __     __  ___    ____ 
 \ \   / / |_ _|  / ___|
  \ \ / /   | |  | |    
   \ V /    | |  | |___ 
    \_/    |___|  \____|
                        
*/

/*****************************************************************************/
/*! Install Isr Vector
* \description
*   Installs the ISR vector.
* \class 
*   VIC 
* \params
*   ulVector              [in]  Isr Vector Address
* \return
*                                                                            */
/*****************************************************************************/
void NX10_VIC_InstallIsrVector( NX10_pp_func ulVector )
{
  volatile unsigned long* pulIsrAddress;
  volatile unsigned long* pulArm9_irq_vector;
  
  /* set the address of the isr */
  pulIsrAddress = (volatile unsigned long*) 0x00000020;
  *pulIsrAddress = (unsigned long) ulVector;

  /* set the jump command in the vector table */
  /*
  ** Jump command to set the pc from the irq vector;
  ** this is the assembler instruction:
  **
  ** ldr pc, [pc, #8h] ; 20h / will be placed on address 0x18
  */
  pulArm9_irq_vector = (volatile unsigned long*) ARM9_Vect_IRQ;
  *pulArm9_irq_vector = 0xe59ff000; /* arm926ejs irq vector */

}

/*****************************************************************************/
/*! Install FIQ Isr Vector
* \description
*   Installs the FIQ ISR vector.
* \class
*   VIC
* \params
*   ulVector              [in]  FIQ Isr Address
* \return
*                                                                            */
/*****************************************************************************/
void NX10_VIC_InstallFiqIsrVector( NX10_pp_func ulVector )
{
  volatile unsigned long* pulIsrAddress;
  volatile unsigned long* pulArm9_fiq_vector;

  /* set the address of the isr */
  pulIsrAddress = (volatile unsigned long*) 0x00000024;
  *pulIsrAddress = (unsigned long) ulVector;

  /* set the jump command in the vector table */
  /*
  ** Jump command to set the pc from the irq vector;
  ** this is the assembler instruction:
  **
  ** ldr pc, [pc, #8h] ; 24h / will be placed on address 0x1c
  */
  pulArm9_fiq_vector = (volatile unsigned long*) ARM9_Vect_FIQ;
  *pulArm9_fiq_vector = 0xe59ff000; /* arm926ejs irq vector */

}

/*****************************************************************************/
/*! IRQ Vector Handler
* \description
*   This function reads the interrupt vector to find out where to jump to.
*   Jump to appropriate function and then a new interrupt is possible.
* \class 
*   VIC 
* \params
* \return
*                                                                            */
/*****************************************************************************/
void INTERRUPT NX10_VIC_IRQVectorHandler_C( void ) 
{
  NX10_pp_func fncPtr;

  /* read interrupt vector to find out where to jump to */
  fncPtr = (NX10_pp_func)s_ptVic->ulVic_vect_addr;

  /* jump to appropiate function */
  fncPtr();

  /* end of interrupt to irq controller, new interrupt possible */
  s_ptVic->ulVic_vect_addr = 0;
}

/*****************************************************************************/
/*! Clear VIC
* \description
*   Clear all VIC vectors.
* \class 
*   VIC 
* \params
* \return
*                                                                            */
/*****************************************************************************/
void NX10_VIC_ClearVic( void )
{
  /* no firqs */
  s_ptVic->ulVic_int_select  = 0;
  s_ptVic->ulVic_int_enclear = 0xffffffff;

  /* clear all vectors */
  s_ptVic->aulVic_vect_addr[0] = 0; s_ptVic->aulVic_vect_cntl[0] = 0;
  s_ptVic->aulVic_vect_addr[1] = 0; s_ptVic->aulVic_vect_cntl[1] = 0;
  s_ptVic->aulVic_vect_addr[2] = 0; s_ptVic->aulVic_vect_cntl[2] = 0;
  s_ptVic->aulVic_vect_addr[3] = 0; s_ptVic->aulVic_vect_cntl[3] = 0;
  s_ptVic->aulVic_vect_addr[4] = 0; s_ptVic->aulVic_vect_cntl[4] = 0;
  s_ptVic->aulVic_vect_addr[5] = 0; s_ptVic->aulVic_vect_cntl[5] = 0;
  s_ptVic->aulVic_vect_addr[6] = 0; s_ptVic->aulVic_vect_cntl[6] = 0;
  s_ptVic->aulVic_vect_addr[7] = 0; s_ptVic->aulVic_vect_cntl[7] = 0;
  s_ptVic->aulVic_vect_addr[8] = 0; s_ptVic->aulVic_vect_cntl[8] = 0;
  s_ptVic->aulVic_vect_addr[9] = 0; s_ptVic->aulVic_vect_cntl[9] = 0;
  s_ptVic->aulVic_vect_addr[10] = 0; s_ptVic->aulVic_vect_cntl[10] = 0;
  s_ptVic->aulVic_vect_addr[11] = 0; s_ptVic->aulVic_vect_cntl[11] = 0;
  s_ptVic->aulVic_vect_addr[12] = 0; s_ptVic->aulVic_vect_cntl[12] = 0;
  s_ptVic->aulVic_vect_addr[13] = 0; s_ptVic->aulVic_vect_cntl[13] = 0;
  s_ptVic->aulVic_vect_addr[14] = 0; s_ptVic->aulVic_vect_cntl[14] = 0;
  s_ptVic->aulVic_vect_addr[15] = 0; s_ptVic->aulVic_vect_cntl[15] = 0;
}

/*****************************************************************************/
/*! VIC Default ISR
* \description
*   Set Default ISR.
* \class 
*   VIC 
* \params
* \return
*                                                                            */
/*****************************************************************************/
void NX10_VIC_ISR_Default( void )
{
  unsigned int uIrqSource;
  uIrqSource = s_ptVic->ulVic_irq_status;

  // default handling ...
}

/*****************************************************************************/
/*! VIC Set Default Vector
* \description
*   This function sets the default vector address.
* \class 
*   VIC 
* \params
*   default_handler  [in]   Default Vector Address 
* \return
*                                                                            */
/*****************************************************************************/
void NX10_VIC_SetDefaultVector( NX10_pp_func default_handler )
{
  s_ptVic->ulVic_def_vect_addr = (unsigned long) default_handler;
}

/*****************************************************************************/
/*! VIC Set IRQ Vector
* \description
*   Set Interrupt vector.
* \class 
*   VIC 
* \params
*   vec          [in]   Vector Number
*   src          [in]   Source 
*   irq_handler  [in]   Irq Handler
* \return
*                                                                            */
/*****************************************************************************/
void NX10_VIC_SetIrqVector( unsigned int vec, unsigned long src, NX10_pp_func irq_handler )
{
  /* No action, if vector number out of range */
  if ( vec<MAX_IRQ_VECTORS ) {
    s_ptVic->aulVic_vect_addr[vec] = (unsigned long) irq_handler;
    s_ptVic->aulVic_vect_cntl[vec] = src;
  }
}

/*****************************************************************************/
/*! Enable interrupt sources
* \description
*   Enable Interrupt source.
* \class 
*   VIC 
* \params
*   src          [in]   Source 
* \return
*                                                                            */
/*****************************************************************************/
void NX10_VIC_EnableIrqSources( unsigned long src )
{
  s_ptVic->ulVic_int_enable = src;
}

/*****************************************************************************/
/*! Disable interrupt sources
* \description
*   Clear Interrupt Enable source.
* \class 
*   VIC 
* \params
*   src          [in]   Source 
* \return
*                                                                            */
/*****************************************************************************/
void NX10_VIC_DisableIrqSources( unsigned long src )
{
  s_ptVic->ulVic_int_enclear = src;
}


/*****************************************************************************/
/*! Get raw interrupt
* \description
*  Read the RAW interrupt register.
* \class 
*   VIC 
* \params
* \return
*   ulVic_int_fiq_status value                                               */
/*****************************************************************************/
unsigned long NX10_VIC_GetIrqRaw( void )
{
  return ( s_ptVic->ulVic_raw_intr );
}

/*****************************************************************************/
/*! Get irq status
* \description
*  Read the IRQ status register.
* \class 
*   VIC 
* \params
* \return
*   ulVic_int_irq_status value                                               */
/*****************************************************************************/
unsigned long NX10_VIC_GetIrqStatus( void )
{
  return ( s_ptVic->ulVic_irq_status );
}

/*****************************************************************************/
/*! Get fiq status
* \description
*  Read the FIQ status register.
* \class 
*   VIC 
* \params
* \return
*   ulVic_int_fiq_status value                                               */
/*****************************************************************************/
unsigned long NX10_VIC_GetFiqStatus( void )
{
  return ( s_ptVic->ulVic_fiq_status );
}

/*****************************************************************************/
/*! Set SW interrupt
* \description
*   Set software Interrupt.
* \class 
*   VIC 
* \params
*   src          [in]   Source 
* \return
*                                                                            */
/*****************************************************************************/
void NX10_VIC_SetSoftInterrupt( unsigned long src )
{
  s_ptVic->ulVic_softint = src;
}

/*****************************************************************************/
/*! Clear SW interrupt
* \description
*   Clear software Interrupt.
* \class 
*   VIC 
* \params
*   src          [in]   Source 
* \return
*                                                                            */
/*****************************************************************************/
void NX10_VIC_ClrSoftInterrupt( unsigned long src )
{
  s_ptVic->ulVic_softint_clear = src;
}

/*****************************************************************************/
/*! Set Protection
* \description
*   Enables/disables protected register access.
* \class 
*   VIC 
* \params
*   val          [in]   1/0: Protection enabled/disabled 
* \return
*                                                                            */
/*****************************************************************************/
void NX10_VIC_SetProtection( unsigned long val )
{
  s_ptVic->ulVic_protection = val;
}


/*
   ____   ____    ___    ___  
  / ___| |  _ \  |_ _|  / _ \ 
 | |  _  | |_) |  | |  | | | |
 | |_| | |  __/   | |  | |_| |
  \____| |_|     |___|  \___/ 
                              
*/

/*****************************************************************************/
/*! GPIO Setup Mode
* \description
*   Configure mode for the selected GPIO.
* \class 
*   GPIO 
* \params
*   ulGpioNum          [in]   Selected GPIO 
*   ulMode             [in]   GPIO_MODE_OUTPUT      = Set the selected GPIO to Output mode 
*                             GPIO_MODE_INPUT_READ  = Set the selected GPIO to Input mode
*   ulInvert           [in]   GPIO_NOINVERT         = Does not invert the selected GPIO
*                             GPIO_INVERT           = Inverts the selected GPIO
* \return
*                                                                            */
/*****************************************************************************/
void NX10_GPIO_SetupMode( unsigned long ulGpioNum, unsigned long ulMode, unsigned long ulInvert )
{
  s_ptGpio->aulGpio_cfg[ulGpioNum] = (unsigned long) (ulMode | ulInvert);
}

/*****************************************************************************/
/*! GPIO Set Output
* \description
*   Enables/disable output for the selected GPIO.
* \class 
*   GPIO 
* \params
*   ulGpioNum          [in]   Selected GPIO 
*   fEnable            [in]   0 = Disable the Output
*                             1 = Enable the Output
* \return
*                                                                            */
/*****************************************************************************/
void NX10_GPIO_SetOutput( unsigned long ulGpioNum, int fEnable )
{
  if( fEnable )
    s_ptGpio->ulGpio_line |= 1 << ulGpioNum;
  else
    s_ptGpio->ulGpio_line &= ~(1 << ulGpioNum);  
}

/*****************************************************************************/
/*! GPIO Get Line
* \description
*  Read the GPIO line register.
* \class 
*   GPIO 
* \params
* \return
*   Gpio_line value                                                          */
/*****************************************************************************/
unsigned long NX10_GPIO_GetLine( void )
{
  return ( s_ptGpio->ulGpio_line );
}

/*****************************************************************************/
/*! GPIO Set Line
* \description
*   Set GPIO line.
* \class 
*   GPIO 
* \params
*   ulVal          [in]   Value of GPIO Line 
* \return
*                                                                            */
/*****************************************************************************/
void NX10_GPIO_SetLine( unsigned long ulVal )
{
  s_ptGpio->ulGpio_line = ulVal;
}

/*****************************************************************************/
/*! GPIO Get Input
* \description
*   Read selected GPIO.
* \class 
*   GPIO 
* \params
*   ulGpioNum          [in]   Selected GPIO
* \return
*                                                                            */
/*****************************************************************************/
int NX10_GPIO_GetInput( unsigned long ulGpioNum )
{
  return ( s_ptGpio->ulGpio_in & (1<<ulGpioNum) ) ? 1 : 0;
}

/*****************************************************************************/
/*! GPIO Get In Register
* \description
*  Read the GPIO IN register.
* \class 
*   GPIO 
* \params
* \return
*   Gpio_in value                                                            */
/*****************************************************************************/
unsigned long NX10_GPIO_GetIn( void )
{
  return ( s_ptGpio->ulGpio_in );
}

/*****************************************************************************/
/*! GPIO Irq Enable
* \description
*   Enable interrupt request for the selected GPIO.
* \class 
*   GPIO 
* \params
*   ulGpioNum          [in]   Selected GPIO
* \return
*                                                                            */
/*****************************************************************************/
void NX10_GPIO_IrqEnable( unsigned long ulGpioNum )
{
  s_ptGpio->ulGpio_irq_mask_set |= 1 << ulGpioNum;
}

/*****************************************************************************/
/*! GPIO Get Irq
* \description
*   Read interrupt status of the selected GPIO.
* \class 
*   GPIO 
* \params
*   ulGpioNum          [in]   Selected GPIO
* \return
*  0
*  1                                                                         */
/*****************************************************************************/
int NX10_GPIO_GetIrq( unsigned long ulGpioNum )
{
  return (s_ptGpio->ulGpio_irq_masked & (1 << ulGpioNum)) ? 1 : 0;
}

/*****************************************************************************/
/*! GPIO Irq Reset
* \description
*   Writes 1 to reset the irq of the selected GPIO.
* \class 
*   GPIO 
* \params
*   ulGpioNum          [in]   Selected GPIO
* \return
*                                                                            */
/*****************************************************************************/
void NX10_GPIO_IrqReset( unsigned long ulGpioNum )
{
  s_ptGpio->ulGpio_irq_raw = 1 << ulGpioNum;
}

/*****************************************************************************/
/*! GPIO Sleep
* \description
*   Sleep a while using a GPIO timer.
* \class 
*   GPIO 
* \params
*   ulGpioNum          [in]   Selected GPIO
*   uiTimeout          [out]  The Time to wait in 탎
* \return
*                                                                            */
/*****************************************************************************/
void NX10_GPIO_Sleep( unsigned long ulCounter, unsigned int uiTimeout )
{
  unsigned int uiVal;

  /* Convert counter value from 탎 to ns */
  uiTimeout = uiTimeout * (NX10_DEV_FREQUENCY/1000000);
  
  s_ptGpio->aulGpio_counter_ctrl[ulCounter]  = 0;          /* Clear the timer register         */
  s_ptGpio->aulGpio_counter_cnt[ulCounter]   = 0;          /* Clear the current counter value  */
  s_ptGpio->aulGpio_counter_max[ulCounter]   = uiTimeout;  /* Set the counter value            */
  s_ptGpio->aulGpio_counter_ctrl[ulCounter] |= (MSK_NX10_gpio_counter0_ctrl_run | MSK_NX10_gpio_counter0_ctrl_once  ); /* Enable the timer to one shot */
                                              
  /* poll timer ctrl for 'run' bit */
  do {
    uiVal  = s_ptGpio->aulGpio_counter_ctrl[ulCounter];
    uiVal &= MSK_NX10_gpio_counter0_ctrl_run;
  } while ( uiVal!=0 );
}


/*****************************************************************************/
/*! GPIO Setup Timer
* \description
*   Setup a GPIO timer.
* \class 
*   GPIO 
* \params
*   ulGpioNum          [in]   Selected GPIO
*   uiTimeout          [out]  The Time to wait in 탎
* \return
*                                                                            */
/*****************************************************************************/
void NX10_GPIO_SetupTimer( unsigned long ulCounter, unsigned int uiTimeout )
{
  /* Convert counter value from 탎 to ns */
  uiTimeout = uiTimeout * (NX10_DEV_FREQUENCY/1000000);
  
  s_ptGpio->aulGpio_counter_ctrl[ulCounter]  = 0;          /* Clear the timer register         */
  s_ptGpio->aulGpio_counter_cnt[ulCounter]   = 0;          /* Clear the current counter value  */
  s_ptGpio->aulGpio_counter_max[ulCounter]   = uiTimeout;  /* Set the counter value            */
  s_ptGpio->aulGpio_counter_ctrl[ulCounter] |= (MSK_NX10_gpio_counter0_ctrl_run | MSK_NX10_gpio_counter0_ctrl_once  ); /* Enable the timer to one shot */

}

/*****************************************************************************/
/*! GPIO IsTimerRunning
* \description
*   Checks for GPIO timer expired.
* \class 
*   GPIO 
* \params
* \return
*    0
*    1                                                                       */
/*****************************************************************************/
int NX10_GPIO_IsTimerRunning( unsigned long ulCounter )
{
  /* gets timer ctrl 'run' bit */
  if( (s_ptGpio->aulGpio_counter_ctrl[ulCounter] & MSK_NX10_gpio_counter0_ctrl_run) == 0 )
    return 0;
  else
    return 1;
}



/*
  __  __   __  __   ___    ___  
 |  \/  | |  \/  | |_ _|  / _ \ 
 | |\/| | | |\/| |  | |  | | | |
 | |  | | | |  | |  | |  | |_| |
 |_|  |_| |_|  |_| |___|  \___/ 
                                
*/

/*****************************************************************************/
/*! MMIO Set Configuration
* \description
*   Configures a MMIO pin.
* \class 
*   MMIO 
* \params
*   uMmioNum          [in]  Selected MMIO
*   bFunction         [in]  Signal Selection
*   fInvertOut        [in]  Invert Output Signal
*   fInvertIn         [in]  Invert Input Signal
* \return
*                                                                            */
/*****************************************************************************/
void NX10_MMIO_SetCfg( unsigned int uMmioNum, unsigned char bFunction, int fInvertOut, int fInvertIn )
{
  // read access key, write back access key
  s_ptAsicCtrl->ulAsic_ctrl_access_key = s_ptAsicCtrl->ulAsic_ctrl_access_key;
    
  s_ptMmioCtrl->aulMmio_cfg[uMmioNum] =  bFunction << SRT_NX10_mmio0_cfg_mmio_sel 
                                        |(fInvertOut ? MSK_NX10_mmio0_cfg_mmio_out_inv : 0) 
                                        |(fInvertIn  ? MSK_NX10_mmio0_cfg_mmio_in_inv  : 0);
  
}

/*****************************************************************************/
/*! MMIO Get Configuration
* \description
*   Get configuration of a MMIO pin.
* \class 
*   MMIO 
* \params
*   uMmioNum          [in]  Selected MMIO
*   pbFunction        [out]  Pointer to Signal Selection
*   pfInvertOut       [out]  Pointer to Invert Output Signal
*   pfInvertIn        [out]  Pointer to Invert Input Signal
* \return
*                                                                            */
/*****************************************************************************/
void NX10_MMIO_GetCfg( unsigned int uMmioNum, unsigned char* pbFunction, int* pfInvertOut, int* pfInvertIn )
{
  unsigned long ulTemp = s_ptMmioCtrl->aulMmio_cfg[uMmioNum];
  
  *pbFunction   = (ulTemp & MSK_NX10_mmio0_cfg_mmio_sel)     >> SRT_NX10_mmio0_cfg_mmio_sel;
  *pfInvertOut  = (ulTemp & MSK_NX10_mmio0_cfg_mmio_out_inv) >> SRT_NX10_mmio0_cfg_mmio_out_inv;
  *pfInvertIn   = (ulTemp & MSK_NX10_mmio0_cfg_mmio_in_inv)  >> SRT_NX10_mmio0_cfg_mmio_in_inv;
}

/*****************************************************************************/
/*! MMIO Set Mode
* \description
*   Enables/disable output for the selected MMIO. MMIO must be in PIO mode.
* \class 
*   PIO 
* \params
*   ulMmioNum         [in]  Selected MMIO
*   ulMode            [in]  NX10_MMIO_MODE_IN  = Set the MMIO to input mode
*                           NX10_MMIO_MODE_OUT = Set the MMIO to output mode
* \return
*                                                                            */
/*****************************************************************************/
void NX10_MMIO_SetMode( unsigned long ulMmioNum, unsigned long ulMode )
{
  if( NX10_MMIO_MODE_OUT == ulMode)
    s_ptMmioCtrl->ulMmio_pio_oe_line_cfg |= 1 << ulMmioNum;
  else
    s_ptMmioCtrl->ulMmio_pio_oe_line_cfg &= ~(1 << ulMmioNum);  
}

/*****************************************************************************/
/*! MMIO Get Input
* \description
*   Read current value of selected MMIO within in_line_status register. MMIO must be in PIO mode.
* \class 
*   PIO 
* \params
*   ulMmioNum         [in]  Selected MMIO
* \return
*    0
*    1                                                                       */
/*****************************************************************************/
int NX10_MMIO_GetInput( unsigned long ulMmioNum )
{
  return ( s_ptMmioCtrl->ulMmio_in_line_status & (1 << ulMmioNum)) ? 1 : 0;
}

/*****************************************************************************/
/*! MMIO Get In Register
* \description
*  Read the in_line_status register.
* \class 
*   PIO 
* \params
* \return
*   Mmio_in_line_status                                                      */
/*****************************************************************************/
unsigned long NX10_MMIO_GetIn( void )
{
  return ( s_ptMmioCtrl->ulMmio_in_line_status );
}

/*****************************************************************************/
/*! MMIO Get Output
* \description
*   Read current value of selected MMIO within out_line_cfg register. MMIO must be in PIO mode.
* \class 
*   PIO 
* \params
*   ulMmioNum          [in]  Selected MMIO
* \return
*    0
*    1                                                                       */
/*****************************************************************************/
int NX10_MMIO_GetOutput( unsigned long ulMmioNum )
{
  return ( s_ptMmioCtrl->ulMmio_pio_out_line_cfg & (1 << ulMmioNum)) ? 1 : 0;
}

/*****************************************************************************/
/*! PIO Set Output
* \description
*   Set output for the selected MMIO within out_line_cfg register. MMIO must be in PIO mode.
* \class 
*   PIO 
* \params
*   ulMmioNum         [in]  Selected MMIO
*   fEnable           [in]  0 = Disable the Output
*                           1 = Enable the Output
* \return
*                                                                            */
/*****************************************************************************/
void NX10_MMIO_SetOutput( unsigned long ulMmioNum, int fEnable )
{
  if(fEnable) {
    s_ptMmioCtrl->ulMmio_pio_out_line_cfg |= 1 << ulMmioNum;
  } else {
    s_ptMmioCtrl->ulMmio_pio_out_line_cfg &= ~(1 << ulMmioNum);  
  }
}

/*****************************************************************************/
/*! MMIO Get Line
* \description
*  Read the MMIO output level line. MMIOs must be in PIO mode.
* \class 
*   PIO 
* \params
* \return
*   Pio_line value                                                          */
/*****************************************************************************/
unsigned long NX10_MMIO_GetLine( void )
{
  return ( s_ptMmioCtrl->ulMmio_pio_out_line_cfg );
}

/*****************************************************************************/
/*! MMIO Set Line
* \description
*   Set MMIO output level line. MMIOs must be in PIO mode.
* \class 
*   PIO 
* \params
*   ulVal          [in]   Value of PIO Line 
* \return
*                                                                            */
/*****************************************************************************/
void NX10_MMIO_SetLine( unsigned long ulVal )
{
  s_ptMmioCtrl->ulMmio_pio_out_line_cfg = ulVal;
}


/*
  ____        _           ____                  
 |  _ \    __| |  _   _  |  _ \   _   _   _ __  
 | |_) |  / _` | | | | | | |_) | | | | | | '_ \ 
 |  _ <  | (_| | | |_| | |  _ <  | |_| | | | | |
 |_| \_\  \__,_|  \__, | |_| \_\  \__,_| |_| |_|
                  |___/                         
*/

/*****************************************************************************/
/*! Set RDYRUN Led
* \description
*   Set RDY/RUN led depending of mode.
* \class 
*   RDYRUN 
* \params
*   tMode          [in]  Ready/Run Mode (off, green, red, inv) to set
* \return
*                                                                            */
/*****************************************************************************/
void NX10_RDYRUN_SetRdyRunLed( NX10_RDYRUN_LED_E tMode )
{
  switch( tMode ) {
  case NX10_RDYRUN_LED_OFF:
    s_ptAsicCtrl->ulRdy_run_cfg =   MSK_NX10_rdy_run_cfg_RDY_DRV
                                  | MSK_NX10_rdy_run_cfg_RUN_DRV
                                  | MSK_NX10_rdy_run_cfg_RDY_POL // low-active signal
                                  | MSK_NX10_rdy_run_cfg_RUN_POL; // low-active signal
    break;
  case NX10_RDYRUN_LED_GREEN:
    s_ptAsicCtrl->ulRdy_run_cfg =   MSK_NX10_rdy_run_cfg_RDY_DRV
                                  | MSK_NX10_rdy_run_cfg_RUN_DRV
                                  | MSK_NX10_rdy_run_cfg_RDY_POL // low-active signal
                                  | MSK_NX10_rdy_run_cfg_RUN_POL // low-active signal
                                  | MSK_NX10_rdy_run_cfg_RUN;
    break;
  case NX10_RDYRUN_LED_RED:
    s_ptAsicCtrl->ulRdy_run_cfg =   MSK_NX10_rdy_run_cfg_RDY_DRV
                                  | MSK_NX10_rdy_run_cfg_RUN_DRV
                                  | MSK_NX10_rdy_run_cfg_RDY_POL // low-active signal
                                  | MSK_NX10_rdy_run_cfg_RUN_POL // low-active signal
                                  | MSK_NX10_rdy_run_cfg_RDY;
    break;
  case NX10_RDYRUN_LED_INV:
    s_ptAsicCtrl->ulRdy_run_cfg =   MSK_NX10_rdy_run_cfg_RDY_DRV
                                  | MSK_NX10_rdy_run_cfg_RUN_DRV
                                  | MSK_NX10_rdy_run_cfg_RDY_POL // low-active signal
                                  | MSK_NX10_rdy_run_cfg_RUN_POL // low-active signal
                                  | (s_ptAsicCtrl->ulRdy_run_cfg ^ MSK_NX10_rdy_run_cfg_RDY)
                                  | (s_ptAsicCtrl->ulRdy_run_cfg ^ MSK_NX10_rdy_run_cfg_RUN);
    break;
  }
}
      
/*
  ___           _                                   _           ____    _   _  __   __
 |_ _|  _ __   | |_    ___   _ __   _ __     __ _  | |         |  _ \  | | | | \ \ / /
  | |  | '_ \  | __|  / _ \ | '__| | '_ \   / _` | | |         | |_) | | |_| |  \ V / 
  | |  | | | | | |_  |  __/ | |    | | | | | (_| | | |         |  __/  |  _  |   | |  
 |___| |_| |_|  \__|  \___| |_|    |_| |_|  \__,_| |_|  _____  |_|     |_| |_|   |_|  
                                                       |_____|                        

*/
/*****************************************************************************/
/*! Init Internal PHY
* \description
*   Initialize internal PHY.
* \class 
*   INTPHY 
* \params
*   ptPhyCtrlInit          [in]  Pointer of PHY Control Init Value
* \return
*                                                                            */
/*****************************************************************************/
void NX10_INTPHY_Init( NX10_PHY_CONTROL_T *ptPhyCtrlInit )
{
  unsigned long ulTmp;
  unsigned short usMiimuData;
  unsigned int ulDelayCnt;
  
  // read access key, write back access key
  s_ptAsicCtrl->ulAsic_ctrl_access_key = s_ptAsicCtrl->ulAsic_ctrl_access_key;
  // write value
  s_ptAsicCtrl->ulPhy_control = ptPhyCtrlInit->val | MSK_NX10_phy_control_phy_reset;

  // delay for 100us -> do 5 MII transfers of 24us each
  ulDelayCnt = 5;
  do
  {
    NX10_MIIMU_ReadPhyReg( NX10_MIIMU_SEND_PREAMBLE,
                           NX10_MIIMU_MDCFREQ_HIGH,
                           NX10_MIIMU_RTA_0_BITS,
                           NX10_INT_PHY1_ADDR,
                           NX10_DRV_CB12_CONTROL,
                           &usMiimuData,
                           0
                          );
  } while( ulDelayCnt--!=0 );

  /* read access key, write back access key */
  s_ptAsicCtrl->ulAsic_ctrl_access_key = s_ptAsicCtrl->ulAsic_ctrl_access_key;
  /* write value */
  s_ptAsicCtrl->ulPhy_control = ptPhyCtrlInit->val;

  // wait until MIIMU access to PHY 1 successfully
  do {
    NX10_MIIMU_ReadPhyReg( NX10_MIIMU_SEND_PREAMBLE,
                         NX10_MIIMU_MDCFREQ_HIGH,
                         NX10_MIIMU_RTA_0_BITS,
                         NX10_INT_PHY1_ADDR,
                         NX10_DRV_CB12_SILICON_REVISION,
                         &usMiimuData,
                         0
                        );
  } while( (usMiimuData==0) || (usMiimuData==0xffff) );

  // Workaround sequence for correct Auto-MDIX function in forced 100 MBit-Mode
  if(   ptPhyCtrlInit->bf.phy0_automdix==NX10_PHY_CTRL_PHY_AUTOMDIX
      && (  (ptPhyCtrlInit->bf.phy0_mode==NX10_PHYCTRL_100BASE_TXFX_HD_NOAUTONEG_CRSTXRX)
          ||(ptPhyCtrlInit->bf.phy0_mode==NX10_PHYCTRL_100BASE_TXFX_FD_NOAUTONEG_CRSRX)
         )
    )
  {
    NX10_INTPHY_CorrectAutoMdix100MBit(NX10_INT_PHY1_ADDR);
  }

  /* enable PHY clk for xMAC */
  ulTmp = s_ptAsicCtrl->ulIo_config;
  s_ptAsicCtrl->ulAsic_ctrl_access_key = s_ptAsicCtrl->ulAsic_ctrl_access_key;
  s_ptAsicCtrl->ulIo_config = ulTmp | MSK_NX10_io_config_sel_rxclk_from_intphy;
}

/*****************************************************************************/
/*! Correct AutoMDIX for 100 MBit forced modes
* \description
*   This function is a workaround to enable the AutoMDIX function in forced 100 MBit mode (HD or FD).
*   This sequence is necessary because after forcing the internal PHY into 100 MBit mode the requested AutoMDIX function does not work correctly.
*   Note: This function may only be called when requesting forced 100 MBit mode in conjunction with AutoMDIX functionality.
* \class
*   INTPHY
* \params
*   uMiimuPhyAddr          [in] MIIMU PHY Address
* \return
*                                                                            */
/*****************************************************************************/
void NX10_INTPHY_CorrectAutoMdix100MBit( unsigned int uMiimuPhyAddr )
{
  NX10_MIIMU_WritePhyReg( NX10_MIIMU_SEND_PREAMBLE, NX10_MIIMU_MDCFREQ_HIGH, uMiimuPhyAddr, 0x14, 0x400, 0 );
  NX10_MIIMU_WritePhyReg( NX10_MIIMU_SEND_PREAMBLE, NX10_MIIMU_MDCFREQ_HIGH, uMiimuPhyAddr, 0x14, 0x0, 0 );
  NX10_MIIMU_WritePhyReg( NX10_MIIMU_SEND_PREAMBLE, NX10_MIIMU_MDCFREQ_HIGH, uMiimuPhyAddr, 0x14, 0x400, 0 );
  NX10_MIIMU_WritePhyReg( NX10_MIIMU_SEND_PREAMBLE, NX10_MIIMU_MDCFREQ_HIGH, uMiimuPhyAddr, 0x17, 0xa1, 0 );
  NX10_MIIMU_WritePhyReg( NX10_MIIMU_SEND_PREAMBLE, NX10_MIIMU_MDCFREQ_HIGH, uMiimuPhyAddr, 0x14, 0x441b, 0 );
  NX10_MIIMU_WritePhyReg( NX10_MIIMU_SEND_PREAMBLE, NX10_MIIMU_MDCFREQ_HIGH, uMiimuPhyAddr, 0x14, 0x0, 0 );
}

/*
  ____            _           _                           _____   _    __         
 |  _ \    ___   (_)  _ __   | |_    ___   _ __          |  ___| (_)  / _|   ___  
 | |_) |  / _ \  | | | '_ \  | __|  / _ \ | '__|         | |_    | | | |_   / _ \ 
 |  __/  | (_) | | | | | | | | |_  |  __/ | |            |  _|   | | |  _| | (_) |
 |_|      \___/  |_| |_| |_|  \__|  \___| |_|     _____  |_|     |_| |_|    \___/ 
                                                 |_____|                          
*/

/*****************************************************************************/
/*! Reset Pointer Fifo
* \description
*   Resets pointer fifo to default values.
* \class 
*   PFIFO 
* \params
* \return
*                                                                            */
/*****************************************************************************/
void NX10_PFIFO_Reset( void )
{
  unsigned int uCnt;
  
  // set reset flag of all fifos
  s_ptPFifo->ulPfifo_reset = 0xffffffff;

  // reset pointer fifo borders
  for( uCnt = 0; uCnt < 16; uCnt++ ) {
    s_ptPFifo->aulPfifo_border[uCnt] = ((uCnt+1)* 64)-1 ;
  }

  // clear reset flag of all fifos
  s_ptPFifo->ulPfifo_reset = 0;
}

/*****************************************************************************/
/*! Set Pointer Fifo Borders
* \description
*   Set pointer fifo borders to given values.
* \class 
*   PFIFO 
* \params
* \return
*                                                                            */
/*****************************************************************************/
int NX10_PFIFO_SetBorders(const unsigned int *auPFifoDepth)
{
  int iResult;
  unsigned int uBorderTmp;
  unsigned int uFifoNum;

  // set reset bit for all pointer fifos
  s_ptPFifo->ulPfifo_reset = 0xffffffff;

  // define pointer fifo borders
  uBorderTmp = 0;
  for(uFifoNum=0; uFifoNum<16; uFifoNum++)
  {
    uBorderTmp += auPFifoDepth[uFifoNum];
    s_ptPFifo->aulPfifo_border[uFifoNum] = uBorderTmp-1;
  }

  if( uBorderTmp>1024 ) {
    // sum of all fifo elements exceeds the limit
    iResult = -1;
  } else {
    // ok!
    iResult = 0;

    // clear reset bit for all pointer fifos
    s_ptPFifo->ulPfifo_reset = 0x00000000;
  }

  return iResult;
}

/*****************************************************************************/
/*! Get Fifo Fill Level
* \description
*   Returns the fill level of the fifo
* \class 
*   PFIFO 
* \params
* \return
*                                                                            */
/*****************************************************************************/
unsigned long NX10_PFIFO_GetFillLevel( unsigned int uFifoNum )
{
  // set reset flag of all fifos
  if( uFifoNum<16 ) 
    return s_ptPFifo->aulPfifo_fill_level[uFifoNum];
  else 
    return -1;
}

/*****************************************************************************/
/*! Get FIFO Reset Vector
* \description
*   Retrieves the reset vector of pointer fifo.
* \class 
*   PFIFO 
* \params
* \return
    reset vector
                                                                             */
/*****************************************************************************/
unsigned long NX10_PFIFO_GetFifoResetVector( void )
{
  return s_ptPFifo->ulPfifo_reset;
}

/*****************************************************************************/
/*! Get FIFO Full Vector
* \description
*   Retrieves the full vector of pointer fifo.
* \class 
*   PFIFO 
* \params
* \return
    full vector
                                                                             */
/*****************************************************************************/
unsigned long NX10_PFIFO_GetFifoFullVector( void )
{
  return s_ptPFifo->ulPfifo_full;
}

/*****************************************************************************/
/*! Get FIFO Empty Vector
* \description
*   Retrieves the empty vector of pointer fifo.
* \class 
*   PFIFO 
* \params
* \return
    empty vector
                                                                             */
/*****************************************************************************/
unsigned long NX10_PFIFO_GetFifoEmptyVector( void )
{
  return s_ptPFifo->ulPfifo_empty;
}

/*****************************************************************************/
/*! Get FIFO Overflow Vector
* \description
*   Retrieves the overflow vector of pointer fifo.
* \class 
*   PFIFO 
* \params
* \return
    overflow vector
                                                                             */
/*****************************************************************************/
unsigned long NX10_PFIFO_GetFifoOverflowVector( void )
{
  return s_ptPFifo->ulPfifo_overflow;
}

/*****************************************************************************/
/*! Get FIFO Underrun Vector
* \description
*   Retrieves the underrun vector of pointer fifo.
* \class 
*   PFIFO 
* \params
* \return
    underrun vector
                                                                             */
/*****************************************************************************/
unsigned long NX10_PFIFO_GetFifoUnderrunVector( void )
{
  return s_ptPFifo->ulPfifo_underrun;
}

/*
  ____    ____    __  __ 
 |  _ \  |  _ \  |  \/  |
 | | | | | |_) | | |\/| |
 | |_| | |  __/  | |  | |
 |____/  |_|     |_|  |_|
                         
*/

/*****************************************************************************/
/*! Configure DPM window
* \description
*   This function configures the general DPM parameters.
* \class 
*   DPM 
* \params
*   tMode             [in] Mode
*   tEndianess        [in] Endianess
*   tAddrRange        [in] Address Range
*   tCfgWinAddrCfg    [in] Config Window location
* \return
*   0 on success
*                                                                            */
/*****************************************************************************/
void NX10_DPM_CfgDpm( NX10_DPM_MODE tMode, NX10_DPM_ENDIANESS tEndianess, NX10_DPM_ADDRRANGE tAddrRange, NX10_DPM_CFGWINADDRCFG tCfgWinAddrCfg )
{

  s_ptDpm->ulDpm_cfg0x0   =  (tMode<<SRT_NX10_dpm_cfg0x0_mode)
                            |(tEndianess<<SRT_NX10_dpm_cfg0x0_endian);

  s_ptDpm->ulDpm_addr_cfg =  (tAddrRange<<SRT_NX10_dpm_addr_cfg_addr_range)
                            |(tCfgWinAddrCfg<<SRT_NX10_dpm_addr_cfg_cfg_win_addr_cfg);
}

/*****************************************************************************/
/*! Configure DPM timing
* \description
*   This function configures the DPM timing.
* \class 
*   DPM 
* \params
*   uTosa       [in] address setup time
*   fFilter     [in] 1/0: filter en/dis
*   tRds        [in] Read data setup time
*   fRdBurstEn  [in] 1/0: Read burst en/dis
* \return
*   0 on success
*                                                                            */
/*****************************************************************************/
int NX10_DPM_CfgTiming( unsigned int uTosa, int fFilter, unsigned int tRds, int fRdBurstEn )
{
  // plausibility check
  if( uTosa > 3 ) return -1;
  if( tRds  > 7 ) return -1;
  
  s_ptDpm->ulDpm_timing_cfg = (uTosa<<SRT_NX10_dpm_timing_cfg_t_osa)
                             |(fFilter<<SRT_NX10_dpm_timing_cfg_filter)
                             |(tRds<<SRT_NX10_dpm_timing_cfg_t_rds)
                             |(fRdBurstEn<<SRT_NX10_dpm_timing_cfg_rd_burst_en);
  return 0;
}

/*****************************************************************************/
/*! Configure DPM RDY signal
* \description
*   This function configures the RDY behaviour.
* \class 
*   DPM 
* \params
*   fRdypol         [in] Rdy polarity
*   tRdyDrv         [in] Rdy driver
*   tRdyMode        [in] Rdy signal Mode
* \return
*   0 on success
*                                                                            */
/*****************************************************************************/
void NX10_DPM_CfgRdy( int fRdypol, NX10_DPM_RDYDRV tRdyDrv, NX10_DPM_RDYMODE tRdyMode )
{
  s_ptDpm->ulDpm_rdy_cfg = (fRdypol<<SRT_NX10_dpm_rdy_cfg_rdy_pol)
                          |(tRdyMode<<SRT_NX10_dpm_rdy_cfg_rdy_sig_mode)
                          |(tRdyDrv<<SRT_NX10_dpm_rdy_cfg_rdy_drv_mode);
}

/*****************************************************************************/
/*! Configure misc
* \description
*   This function configures miscellanious parameters.
* \class 
*   DPM 
* \params
*   ulMiscCfg         [in] Misc configuration
*   ulIoMiscCfg       [in] IO misc configuration
* \return
*   0 on success
*                                                                            */
/*****************************************************************************/
void NX10_DPM_CfgMisc( unsigned long ulMiscCfg, unsigned long ulIoMiscCfg )
{
  s_ptDpm->ulDpm_misc_cfg = ulMiscCfg;
  s_ptDpm->ulDpm_io_cfg_misc = ulIoMiscCfg;
}

/*****************************************************************************/
/*! Configure DPM window
* \description
*   This function configures a DPM window.
* \class 
*   DPM 
* \params
*   uWinNum             [in] DPM window number 1..4
*   uSize               [in] DPM Size
*   ulMapStartAddr      [in] netX internal start address
*   tAltMapping         [in] Alternative mapping
* \return
*   0 on success
*   -1 on erroneous 
*                                                                            */
/*****************************************************************************/
int NX10_DPM_CfgDpmWindow( unsigned int uWinNum, unsigned int uSize, unsigned long ulMapStartAddr, NX10_DPM_WINMAP_ALT tAltMapping )
{
  // check 128 byte granularity
  if( (uSize&0x7f)!= 0 ) return -1;
  if( (ulMapStartAddr&0x7f)!= 0 ) return -1;
  
  switch( uWinNum ) 
  {
    case 1:  
      s_ptDpm->asDpm_win[0].ulEnd = uSize;
      s_ptDpm->asDpm_win[0].ulMap =  (ulMapStartAddr&0xfffff)
                                    |((ulMapStartAddr>>20)<<SRT_NX10_dpm_win1_map_win_page)
                                    |(tAltMapping<<SRT_NX10_dpm_win1_map_win_map_alt);
    break;
  
    case 2:
      s_ptDpm->asDpm_win[1].ulEnd = uSize + s_ptDpm->asDpm_win[0].ulEnd;
      s_ptDpm->asDpm_win[1].ulMap =  ((ulMapStartAddr&0xfffff) - s_ptDpm->asDpm_win[0].ulEnd)
                                    |(((ulMapStartAddr-s_ptDpm->asDpm_win[0].ulEnd)>>20)<<SRT_NX10_dpm_win2_map_win_page)
                                    |(tAltMapping<<SRT_NX10_dpm_win2_map_win_map_alt);
    break;
    
    case 3:  
      s_ptDpm->asDpm_win[2].ulEnd = uSize + s_ptDpm->asDpm_win[1].ulEnd;
      s_ptDpm->asDpm_win[2].ulMap =  ((ulMapStartAddr&0xfffff)-s_ptDpm->asDpm_win[1].ulEnd)
                                    |(((ulMapStartAddr-s_ptDpm->asDpm_win[1].ulEnd)>>20)<<SRT_NX10_dpm_win3_map_win_page)
                                    |(tAltMapping<<SRT_NX10_dpm_win3_map_win_map_alt);
    break;
  
    case 4:
      s_ptDpm->asDpm_win[3].ulEnd = uSize + s_ptDpm->asDpm_win[2].ulEnd;
      s_ptDpm->asDpm_win[3].ulMap =  ((ulMapStartAddr&0xfffff)-s_ptDpm->asDpm_win[2].ulEnd) 
                                    |(((ulMapStartAddr-s_ptDpm->asDpm_win[2].ulEnd)>>20)<<SRT_NX10_dpm_win4_map_win_page)
                                    |(tAltMapping<<SRT_NX10_dpm_win4_map_win_map_alt);
    break;

    default:
      return -1; // invalid instance
    break;    
  } // end switch  
  return 0;
}

/*****************************************************************************/
/*! Setup Byte Access
* \description
*   Enable/Disable byte access within the selected DPM window.
* \class 
*   DPM 
* \params
*   uWinNum               [in]  DPM window number
*   fByteAccess           [in]  Byte Access Value
* \return
*                                                                            */
/*****************************************************************************/
int NX10_DPM_SetupByteAccess( int uWinNum, int fByteAccess )
{
  switch( uWinNum ) {
    case 1:
      if( fByteAccess ) s_ptDpm->asDpm_win[0].ulMap |= MSK_NX10_dpm_win1_map_byte_area;
      else s_ptDpm->asDpm_win[0].ulMap &= ~MSK_NX10_dpm_win1_map_byte_area;
    break;
  
    case 2:
      if( fByteAccess ) s_ptDpm->asDpm_win[1].ulMap |= MSK_NX10_dpm_win2_map_byte_area;
      else s_ptDpm->asDpm_win[1].ulMap &= ~MSK_NX10_dpm_win1_map_byte_area;
    break;

    case 3:
      if( fByteAccess ) s_ptDpm->asDpm_win[2].ulMap |= MSK_NX10_dpm_win3_map_byte_area;
      else s_ptDpm->asDpm_win[2].ulMap &= ~MSK_NX10_dpm_win1_map_byte_area;
    break;

    case 4:
      if( fByteAccess ) s_ptDpm->asDpm_win[3].ulMap |= MSK_NX10_dpm_win4_map_byte_area;
      else s_ptDpm->asDpm_win[3].ulMap &= ~MSK_NX10_dpm_win1_map_byte_area;
    break;

    default:
      return -1; // invalid instance
    break;
  } // end switch 
  return 0;
}

/*****************************************************************************/
/*! Setup Read Ahead
* \description
*   Enable/Disable read ahead function within the selected DPM window.
* \class 
*   DPM 
* \params
*   iWindow               [in]  Number of DPM Window
*   fReadAhead            [in]  Read Ahead value
* \return
*                                                                            */
/*****************************************************************************/
int NX10_DPM_SetupReadAhead( int uWinNum, int fReadAhead )
{
  switch( uWinNum ) {
    case 1:
      if( fReadAhead ) s_ptDpm->asDpm_win[0].ulMap |= MSK_NX10_dpm_win1_map_read_ahead;
      else s_ptDpm->asDpm_win[0].ulMap &= ~MSK_NX10_dpm_win1_map_read_ahead;
    break;
  
    case 2:
      if( fReadAhead ) s_ptDpm->asDpm_win[1].ulMap |= MSK_NX10_dpm_win2_map_read_ahead;
      else s_ptDpm->asDpm_win[1].ulMap &= ~MSK_NX10_dpm_win2_map_read_ahead;
    break;

    case 3:
      if( fReadAhead ) s_ptDpm->asDpm_win[2].ulMap |= MSK_NX10_dpm_win3_map_read_ahead;
      else s_ptDpm->asDpm_win[2].ulMap &= ~MSK_NX10_dpm_win3_map_read_ahead;
    break;

    case 4:
      if( fReadAhead ) s_ptDpm->asDpm_win[3].ulMap |= MSK_NX10_dpm_win4_map_read_ahead;
      else s_ptDpm->asDpm_win[3].ulMap &= ~MSK_NX10_dpm_win4_map_read_ahead;
    break;

    default:
      return -1; // invalid instance
    break;
  } // end switch 
  return 0;
}

/*****************************************************************************/
/*! Setup IRQ
* \description
*   This function configurs all interrupt enable masks.
* \class 
*   DPM 
* \params
*   ulIrqMsk    [in]  IRQ Mask
*   ulFiqMsk    [in]  FIQ Mask
*   ulFiqMsk    [in]  ARM Mask
*   ulFiqMsk    [in]  xPIC Mask
* \return
*                                                                            */
/*****************************************************************************/
void NX10_DPM_SetupIrq( unsigned long ulIrqMsk, unsigned long ulFiqMsk, unsigned long ulArmMsk, unsigned long ulXpicMsk, unsigned long ulFwIrqMask )
{
  // clear all interrupt enable
  s_ptDpm->ulDpm_irq_arm_mask_reset  = 0xffffffff;
  s_ptDpm->ulDpm_irq_xpic_mask_reset = 0xffffffff;
  s_ptDpm->ulDpm_irq_fiq_mask_reset  = 0xffffffff;
  s_ptDpm->ulDpm_irq_irq_mask_reset  = 0xffffffff;
  
  // set dedicated interrupt enables
  s_ptDpm->ulDpm_irq_arm_mask_set  = ulArmMsk;
  s_ptDpm->ulDpm_irq_xpic_mask_set = ulXpicMsk;
  s_ptDpm->ulDpm_irq_fiq_mask_set  = ulFiqMsk;
  s_ptDpm->ulDpm_irq_irq_mask_set  = ulIrqMsk;
  s_ptDpm->ulDpm_firmware_irq_mask = ulFwIrqMask;
}

/*****************************************************************************/
/*! Get Status/Error
* \description
*   Get current error events.
* \class 
*   DPM 
* \params
*   tIrqType  [in] Interrupt type
*   pulErr    [out] Pointer of IRQ value
* \return
*                                                                            */
/*****************************************************************************/
void NX10_DPM_GetStatusErr( unsigned long *pulStatErr, unsigned long *pulStatErrAddr )
{
    *pulStatErr     = s_ptDpm->ulDpm_status;
    *pulStatErrAddr = s_ptDpm->ulDpm_status_err_addr;
}

/*****************************************************************************/
/*! Clear Error
* \description
*   Clear error events
* \class 
*   DPM 
* \params
*   ulClearMask  [in] Reset mask
* \return
*                                                                            */
/*****************************************************************************/
void NX10_DPM_ClearErr( unsigned long ulClearMask )
{
    s_ptDpm->ulDpm_status_err_reset = ulClearMask;
}

/*****************************************************************************/
/*! Get Interrupt
* \description
*   Get current interrupt events.
* \class 
*   DPM 
* \params
*   tIrqType  [in] Interrupt type
*   pulIrq    [out] Pointer of IRQ value
* \return
*                                                                            */
/*****************************************************************************/
int NX10_DPM_GetIrq( NX10_DPM_IRQ_STATUS tIrqType, unsigned long *pulIrq )
{
  switch( tIrqType ) {
    case NX10_DPM_IRQ_STATUS_RAW:         *pulIrq = s_ptDpm->ulDpm_irq_raw; break;
    case NX10_DPM_IRQ_STATUS_ARM_MASKED:  *pulIrq = s_ptDpm->ulDpm_irq_arm_masked; break;
    case NX10_DPM_IRQ_STATUS_XPEC_MASKED: *pulIrq = s_ptDpm->ulDpm_irq_xpic_masked; break;
    case NX10_DPM_IRQ_STATUS_FIQ_MASKED:  *pulIrq = s_ptDpm->ulDpm_irq_fiq_masked; break;
    case NX10_DPM_IRQ_STATUS_IRQ_MASKED:  *pulIrq = s_ptDpm->ulDpm_irq_irq_masked; break;
    case NX10_DPM_IRQ_STATUS_FW_IRQ_RAW:  *pulIrq = s_ptDpm->ulDpm_firmware_irq_raw; break;
    default: return -1; break; // invalid instance
  } // end switch 

  return 0;
}

/*****************************************************************************/
/*! Reset
* \description
*   This function resets the DPM interface.
*   Note: This function may not be interrupted by other processes.
* \class 
*   DPM 
* \params
* \return
*                                                                            */
/*****************************************************************************/
void NX10_DPM_Reset( void )
{
    s_ptDpm->ulDpm_reset_request = 0x00;
    s_ptDpm->ulDpm_reset_request = 0x01;
    s_ptDpm->ulDpm_reset_request = 0x03;
    s_ptDpm->ulDpm_reset_request = 0x07;
    s_ptDpm->ulDpm_reset_request = 0x0f;
    s_ptDpm->ulDpm_reset_request = 0x1f;
    s_ptDpm->ulDpm_reset_request = 0x3f;
    s_ptDpm->ulDpm_reset_request = 0x7f;
    s_ptDpm->ulDpm_reset_request = 0xff;
}

/*****************************************************************************/
/*! Get System Status Information
* \description
*   Get sys_sta register.
* \class 
*   DPM 
* \params
*   pulSysSta         [out] System Status Information
*   pulSysStaBigEnd16 [out] System Status Information Big Endian 16
* \return
*                                                                            */
/*****************************************************************************/
void NX10_DPM_GetSysSta( unsigned long *pulSysSta, unsigned long *pulSysStaBigEnd16 )
{
    *pulSysSta         = s_ptDpm->ulDpm_sys_sta;
    *pulSysStaBigEnd16 = s_ptDpm->ulDpm_sys_sta_bigend16;
}

/*****************************************************************************/
/*! Get netX Version
* \description
*   Get netX version.
* \class 
*   DPM 
* \params
*   pulNetxVersion         [out] NetX Version
*   pulNetxVersionBigEnd16 [out] NetX Version Big Endian 16
* \return
*                                                                            */
/*****************************************************************************/
void NX10_DPM_GetNetxVersion( unsigned long *pulNetxVersion, unsigned long *pulNetxVersionBigEnd16 )
{
    *pulNetxVersion     = s_ptDpm->ulDpm_netx_version;
    *pulNetxVersionBigEnd16 = s_ptDpm->ulDpm_netx_version_bigend16;
}

//.##.....##....###....##....##.########...######..##.....##....###....##....##.########..........######..########.########..##......
//.##.....##...##.##...###...##.##.....##.##....##.##.....##...##.##...##...##..##...............##....##....##....##.....##.##......
//.##.....##..##...##..####..##.##.....##.##.......##.....##..##...##..##..##...##...............##..........##....##.....##.##......
//.#########.##.....##.##.##.##.##.....##..######..#########.##.....##.#####....######...........##..........##....########..##......
//.##.....##.#########.##..####.##.....##.......##.##.....##.#########.##..##...##...............##..........##....##...##...##......
//.##.....##.##.....##.##...###.##.....##.##....##.##.....##.##.....##.##...##..##...............##....##....##....##....##..##......
//.##.....##.##.....##.##....##.########...######..##.....##.##.....##.##....##.########.#######..######.....##....##.....##.########

/*****************************************************************************/
/*! Configure Handshake cells base address
* \description
*   This function configures the handshake base address within INTRAM 5.
* \class 
*   HSCTRL 
* \params
*   ulHsBaseAddr     [in] Base address
* \return
*   0 on success
*                                                                            */
/*****************************************************************************/
int NX10_HSCTRL_CfgHsBaseAddr( unsigned long ulHsBaseAddr )
{
  // check 128 byte granularity
  if( (ulHsBaseAddr&MSK_NX10_handshake_base_addr_zero_ro)!= 0 ) return -1;

  s_ptHsCtrl->ulHandshake_base_addr = ulHsBaseAddr;

  return 0;
}

/*****************************************************************************/
/*! Enable Interrupt(s)
* \description
*   This function enables a set of interrupts.
* \class 
*   HSCTRL
* \params
*  tSrc     [in] Source
*  ulIrqEn  [in] interrupt enable mask
* \return
*   0                                                                        */
/*****************************************************************************/
int NX10_HSCTRL_EnIrq( NX10_HSCTRL_IRQ_SRC tSrc, unsigned long ulIrqEn )
{
  switch( tSrc ) {
    case NX10_HSCTRL_IRQ_SRC_DPM:  s_ptHsCtrl->ulHandshake_dpm_irq_msk_set = ulIrqEn; break;
    case NX10_HSCTRL_IRQ_SRC_ARM:  s_ptHsCtrl->ulHandshake_arm_irq_msk_set = ulIrqEn; break;
    case NX10_HSCTRL_IRQ_SRC_XPIC: s_ptHsCtrl->ulHandshake_xpic_irq_msk_set = ulIrqEn; break;
    default: return -1; break; // invalid source
  } // end switch 
  return 0;
}

/*****************************************************************************/
/*! Disable Interrupt(s)
* \description
*   This function disables a set of interrupts.
* \class 
*   HSCTRL
* \params
*  tSrc      [in] Source
*  ulIrqDis  [in] interrupt disable mask
* \return
*   0                                                                        */
/*****************************************************************************/
int NX10_HSCTRL_DisIrq( NX10_HSCTRL_IRQ_SRC tSrc, unsigned long ulIrqDis )
{
  switch( tSrc ) {
    case NX10_HSCTRL_IRQ_SRC_DPM:  s_ptHsCtrl->ulHandshake_dpm_irq_msk_reset = ulIrqDis; break;
    case NX10_HSCTRL_IRQ_SRC_ARM:  s_ptHsCtrl->ulHandshake_arm_irq_msk_reset = ulIrqDis; break;
    case NX10_HSCTRL_IRQ_SRC_XPIC: s_ptHsCtrl->ulHandshake_xpic_irq_msk_reset = ulIrqDis; break;
    default: return -1; break; // invalid source
  } // end switch 
  return 0;
}

/*****************************************************************************/
/*! Get Interrupt(s)
* \description
*   This function retrieves the current interrupt masked requests.
* \class 
*   HSCTRL
* \params
*  tSrc      [in]  Source
*  pulIrq    [out] Pointer to interrupt events
* \return
*   0                                                                        */
/*****************************************************************************/
int NX10_HSCTRL_GetIrq( NX10_HSCTRL_IRQ_SRC tSrc, unsigned long *pulIrq )
{
  switch( tSrc ) {
    case NX10_HSCTRL_IRQ_SRC_DPM:  *pulIrq = s_ptHsCtrl->ulHandshake_dpm_irq_masked; break;
    case NX10_HSCTRL_IRQ_SRC_ARM:  *pulIrq = s_ptHsCtrl->ulHandshake_arm_irq_masked; break;
    case NX10_HSCTRL_IRQ_SRC_XPIC: *pulIrq = s_ptHsCtrl->ulHandshake_xpic_irq_masked; break;
    default: return -1; break; // invalid source
  } // end switch 
  return 0;
}

/*****************************************************************************/
/*! Get Raw Interrupt(s)
* \description
*   This function retrieves the current interrupt raw requests.
* \class 
*   HSCTRL
* \params
*  tSrc      [in]  Source
*  pulIrq    [out] Pointer to interrupt events
* \return
*   0                                                                        */
/*****************************************************************************/
int NX10_HSCTRL_GetIrqRaw( NX10_HSCTRL_IRQ_SRC tSrc, unsigned long *pulIrq )
{
  switch( tSrc ) {
    case NX10_HSCTRL_IRQ_SRC_DPM:  *pulIrq = s_ptHsCtrl->ulHandshake_dpm_irq_raw_clear; break;
    case NX10_HSCTRL_IRQ_SRC_ARM:  *pulIrq = s_ptHsCtrl->ulHandshake_arm_irq_raw_clear; break;
    case NX10_HSCTRL_IRQ_SRC_XPIC: *pulIrq = s_ptHsCtrl->ulHandshake_xpic_irq_raw_clear; break;
    default: return -1; break; // invalid source
  } // end switch 
  return 0;
}

/*****************************************************************************/
/*! Confirm Interrupt(s)
* \description
*   This function confirms a set of interrupts.
* \class 
*   HSCTRL
* \params
*  tSrc             [in] Source
*  ulConfirmIrqMask [in] Mask to confirm interrupt events
* \return
*   0                                                                        */
/*****************************************************************************/
int NX10_HSCTRL_ConfirmIrq( NX10_HSCTRL_IRQ_SRC tSrc, unsigned long ulConfirmIrqMask )
{
  switch( tSrc ) {
    case NX10_HSCTRL_IRQ_SRC_DPM:  s_ptHsCtrl->ulHandshake_dpm_irq_raw_clear = ulConfirmIrqMask; break;
    case NX10_HSCTRL_IRQ_SRC_ARM:  s_ptHsCtrl->ulHandshake_arm_irq_raw_clear = ulConfirmIrqMask; break;
    case NX10_HSCTRL_IRQ_SRC_XPIC: s_ptHsCtrl->ulHandshake_xpic_irq_raw_clear = ulConfirmIrqMask; break;
    default: return -1; break; // invalid source
  } // end switch 
  return 0;
}

/*****************************************************************************/
/*! Configure Handshake cell mode
* \description
*   This function configures the mode of a handshake cell.
* \class 
*   HSCTRL
* \params
*  uInst            [in] Handshake cell number
*  tHsCellmode      [in] Handshake cell mode
* \return
*   0                                                                        */
/*****************************************************************************/
int NX10_HSCTRL_CfgHsCell( unsigned int uInst, NX10_HSCTRL_CELL_MODE tHsCellmode, NX10_HSCTRL_HSCELL_WIDTH tHsCellWidth )
{
  // check number of instances
  if( uInst >= NX10_NUM_HS_CELL ) return -1;

  s_ptHsCtrl->aulHandshake_hsc_ctrl[uInst] =  (tHsCellmode<<SRT_NX10_handshake_hsc0_ctrl_mode)
                                             |(tHsCellWidth<<SRT_NX10_handshake_hsc0_ctrl_width);

  return 0;
}

/*****************************************************************************/
/*! Configure Handshake BufMan0
* \description
*   This function configures the PdOut buffer manager of the handshake control unit.
* \class 
*   HSCTRL
* \params
*  tAddrmapCfg      [in] Address mapping configuration
*  fAutoMode        [in] 1/0: Automatic mode enabled/disabled
*  ulWinMapBuf1     [in] Alternate mapping buffer 1
*  ulWinMapBuf2     [in] Alternate mapping buffer 2
* \return
*   0                                                                        */
/*****************************************************************************/
void NX10_HSCTRL_CfgBufmanPdOut( NX10_HSCTRL_BUFMAN_ADDRMAPCFG tAddrmapCfg, int fAutoMode, unsigned long ulWinMapBuf1, unsigned long ulWinMapBuf2 )
{
  // configure alternative window mappings
  s_ptHsCtrl->asHandshake_buf_man[0].ulWin_map =  ( (((ulWinMapBuf1&0xfffff)- s_ptDpm->asDpm_win[0].ulEnd)>>7)<<SRT_NX10_handshake_buf_man0_win_map_win_map_buf1)
                                                 |( (((ulWinMapBuf2&0xfffff)- s_ptDpm->asDpm_win[0].ulEnd)>>7)<<SRT_NX10_handshake_buf_man0_win_map_win_map_buf2);

  // configure AutoMode
  s_ptHsCtrl->asHandshake_buf_man[0].ulCtrl    =  ((fAutoMode<<SRT_NX10_handshake_buf_man0_ctrl_hsc2_auto_PD_OUT)&MSK_NX10_handshake_buf_man0_ctrl_hsc2_auto_PD_OUT)
                                                  |(tAddrmapCfg<<SRT_NX10_handshake_buf_man0_ctrl_buf_dam_cfg);

  // Reset BufMan (self-clearing)
  s_ptHsCtrl->asHandshake_buf_man[0].ulCtrl   |=  MSK_NX10_handshake_buf_man0_ctrl_reset; 

  // supply first write buffer for host if BufMan enabled
  if( tAddrmapCfg == NX10_HSCTRL_BUFMAN_ADDRMAPCFG_CURBUF ) {
    // request write buffer to get first buffer
    s_ptHsCtrl->asHandshake_buf_man[0].ulStatus_ctrl_host = NX10_HSCTRL_BUFMAN_CMD_REQ_WRITE_BUF<<SRT_NX10_handshake_buf_man0_status_ctrl_host_cmd;
    // request write buffer to release first buffer and to get first buffer for cyclic writing
    s_ptHsCtrl->asHandshake_buf_man[0].ulStatus_ctrl_host = NX10_HSCTRL_BUFMAN_CMD_REQ_WRITE_BUF<<SRT_NX10_handshake_buf_man0_status_ctrl_host_cmd;
  }
}

/*****************************************************************************/
/*! Configure Handshake BufMan1
* \description
*   This function configures the PdIn buffer manager of the handshake control unit.
* \class 
*   HSCTRL
* \params
*  tAddrmapCfg      [in] Address mapping configuration
*  fAutoMode        [in] 1/0: Automatic mode enabled/disabled
*  ulWinMapBuf1     [in] Alternate mapping buffer 1
*  ulWinMapBuf2     [in] Alternate mapping buffer 2
* \return
*   0                                                                        */
/*****************************************************************************/
void NX10_HSCTRL_CfgBufmanPdIn( NX10_HSCTRL_BUFMAN_ADDRMAPCFG tAddrmapCfg, int fAutoMode, unsigned long ulWinMapBuf1, unsigned long ulWinMapBuf2 )
{
  // configure alternative window mappings
  s_ptHsCtrl->asHandshake_buf_man[1].ulWin_map =  ( (((ulWinMapBuf1&0xfffff)- s_ptDpm->asDpm_win[1].ulEnd)>>7)<<SRT_NX10_handshake_buf_man1_win_map_win_map_buf1)
                                                 |( (((ulWinMapBuf2&0xfffff)- s_ptDpm->asDpm_win[1].ulEnd)>>7)<<SRT_NX10_handshake_buf_man1_win_map_win_map_buf2);
  // configure AutoMode
  s_ptHsCtrl->asHandshake_buf_man[1].ulCtrl    =  ((fAutoMode<<SRT_NX10_handshake_buf_man1_ctrl_hsc2_auto_PD_IN)&MSK_NX10_handshake_buf_man1_ctrl_hsc2_auto_PD_IN)
                                                 |(tAddrmapCfg<<SRT_NX10_handshake_buf_man1_ctrl_buf_dam_cfg);

  // Reset BufMan (self-clearing)
  s_ptHsCtrl->asHandshake_buf_man[1].ulCtrl   |=  MSK_NX10_handshake_buf_man1_ctrl_reset; 

  // supply first buffer for netX if BufMan enabled
  if( tAddrmapCfg == NX10_HSCTRL_BUFMAN_ADDRMAPCFG_CURBUF ) {
    // request write buffer to get first buffer
    s_ptHsCtrl->asHandshake_buf_man[1].ulStatus_ctrl_netx = NX10_HSCTRL_BUFMAN_CMD_REQ_WRITE_BUF<<SRT_NX10_handshake_buf_man1_status_ctrl_netx_cmd;
    // request write buffer to release first buffer and to get first buffer for cyclic writing
    s_ptHsCtrl->asHandshake_buf_man[1].ulStatus_ctrl_netx = NX10_HSCTRL_BUFMAN_CMD_REQ_WRITE_BUF<<SRT_NX10_handshake_buf_man1_status_ctrl_netx_cmd;
  }

}

/*****************************************************************************/
/*! Reset bufMan
* \description
*   This function resets a BufMan unit.
* \class 
*   HSCTRL
* \params
*  uInst            [in] 0/1: BufMan PdOut/PdIn
* \return
*   0                                                                        */
/*****************************************************************************/
int NX10_HSCTRL_ResetBufMan( unsigned int uInst )
{
  // check number of instances
  if( uInst >= NX10_NUM_HS_BUFMAN ) return -1;

  // Reset BufMan (self-clearing)
  s_ptHsCtrl->asHandshake_buf_man[uInst].ulCtrl  |=  MSK_NX10_handshake_buf_man1_ctrl_reset; 

  return 0;
}

/*****************************************************************************/
/*! Set BufMan Command
* \description
*   This function setups a command to a buffer manager.
* \class 
*   HSCTRL
* \params
*  uInst          [in] Buffer manager number
*  tCmd           [in] Command
*  tCmdMaster     [in] Command initiated by Host/netX
* \return
*   0                                                                        */
/*****************************************************************************/
int NX10_HSCTRL_SetBufManCmd( unsigned int uInst, NX10_HSCTRL_BUFMAN_CMDM tCmdMaster, NX10_HSCTRL_BUFMAN_CMD tCmd )
{
  // check number of instances
  if( uInst >= NX10_NUM_HS_BUFMAN ) return -1;
  
  switch( tCmdMaster ) {
    case NX10_HSCTRL_BUFMAN_CMDM_NETX: 
      s_ptHsCtrl->asHandshake_buf_man[uInst].ulStatus_ctrl_netx = tCmd<<SRT_NX10_handshake_buf_man0_status_ctrl_netx_cmd;
    break;
    
    case NX10_HSCTRL_BUFMAN_CMDM_HOST: 
      s_ptHsCtrl->asHandshake_buf_man[uInst].ulStatus_ctrl_host = tCmd<<SRT_NX10_handshake_buf_man0_status_ctrl_host_cmd;
    break;
     
    default: 
      // invalid command master
      return -1; 
    break; 
  } // end switch 
  return 0;
}

/*****************************************************************************/
/*! Get BufMan Status
* \description
*   This function retrieves the current buffer manager status.
* \class 
*   HSCTRL
* \params
*  uInst             [in]  Buffer manager instance
*  ptBufManStatNetx  [out] Pointer to netX status
*  ptBufManStatHost  [out] Pointer to host status
* \return
*   0                                                                        */
/*****************************************************************************/
int NX10_HSCTRL_GetBufManStat( unsigned int uInst, NX10_HSCTRL_BUFMAN_STAT* ptBufManStatNetx, NX10_HSCTRL_BUFMAN_STAT* ptBufManStatHost )
{
  // check number of instances
  if( uInst >= NX10_NUM_HS_BUFMAN ) return -1;

  *ptBufManStatNetx = s_ptHsCtrl->asHandshake_buf_man[uInst].ulStatus_ctrl_netx & MSK_NX10_handshake_buf_man0_status_ctrl_netx_buf_ro;
  *ptBufManStatHost = s_ptHsCtrl->asHandshake_buf_man[uInst].ulStatus_ctrl_host & MSK_NX10_handshake_buf_man0_status_ctrl_host_buf_ro;

  return 0;
}

/*****************************************************************************/
/*! Read Handshake Cell
* \description
*   This function retrieves the current handshake cell value.
* \class 
*   HSCTRL
* \params
*  uInst             [in]  Handshake cell instance
*  pulHandshakeVal    [out] Pointer to handshake cell value
* \return
*   0                                                                        */
/*****************************************************************************/
int NX10_HSCTRL_ReadHsCell( unsigned int uInst, unsigned long* pulHandshakeVal )
{
  unsigned long* pulHsCellAddr;
  
  // check number of instances
  if( uInst >= NX10_NUM_HS_CELL ) return -1;

  // calc address of handshake cell
  pulHsCellAddr = (unsigned long*) ( Addr_NX10_intram5_arm_mirror+NX10_OFFSET_CH_HS+(uInst*sizeof(unsigned long)) );

  // read value
  *pulHandshakeVal = *pulHsCellAddr;

  return 0;
}

/*****************************************************************************/
/*! Write Handshake Cell
* \description
*   This function write the a handshake cell value.
* \class 
*   HSCTRL
* \params
*  uInst             [in]  Handshake cell instance
*  ulHandshakeVal    [in]  Handshake cell value
* \return
*   0                                                                        */
/*****************************************************************************/
int NX10_HSCTRL_WriteHsCell( unsigned int uInst, unsigned long ulHandshakeVal )
{
  unsigned long* pulHsCellAddr;
  
  // check number of instances
  if( uInst >= NX10_NUM_HS_CELL ) return -1;

  // calc address of handshake cell
  pulHsCellAddr = (unsigned long*) ( Addr_NX10_intram5_arm_mirror+NX10_OFFSET_CH_HS+(uInst*sizeof(unsigned long)) );

  // write value
  *pulHsCellAddr = ulHandshakeVal;

  return 0;
}

/*****************************************************************************/
/*! Write Handshake Cell
* \description
*   This function write the a handshake cell value.
* \class 
*   HSCTRL
* \params
*  uInst             [in]  Handshake cell instance
*  ulHandshakeVal    [in]  Handshake cell value
* \return
*   0                                                                        */
/*****************************************************************************/
int NX10_HSCTRL_ToggleHsCellFlag( unsigned int uInst, unsigned long ulHsCellBitNum )
{
  unsigned long* pulHsCellAddr;
  
  // check number of instances
  if( uInst >= NX10_NUM_HS_CELL ) return -1;

  // calc address of handshake cell
  pulHsCellAddr = (unsigned long*) ( Addr_NX10_intram5_arm_mirror+NX10_OFFSET_CH_HS+(uInst*sizeof(unsigned long)) );

  // toggle value
  *pulHsCellAddr ^= (1<<ulHsCellBitNum);

  return 0;
}

/*****************************************************************************/
/*! Check for handshake flags equal
* \description
*   This function checks for equality of two handshake flags.
* \class 
*   HSCTRL
* \params
*  uInst             [in]  Handshake cell instance
*  ulHsFlagNum1      [in]  Handshake cell flag 1
*  ulHsFlagNum2      [in]  Handshake cell flag 2
* \return
*   0                                                                        */
/*****************************************************************************/
int NX10_HSCTRL_AreHsFlagsEqual( unsigned int uInst, unsigned long ulHsFlagNum1, unsigned long ulHsFlagNum2 )
{
  volatile unsigned long* pulHsCellAddr;
  
  // calc address of handshake cell
  pulHsCellAddr = (volatile unsigned long*) ( Addr_NX10_intram5_arm_mirror+NX10_OFFSET_CH_HS+(uInst * sizeof(unsigned long)) );

  // return euqality value
  if( ((*pulHsCellAddr>>ulHsFlagNum1)&1)==((*pulHsCellAddr>>ulHsFlagNum2)&1) ) return 1; else return 0;
}

/*
.########..##......##.##.....##
.##.....##.##..##..##.###...###
.##.....##.##..##..##.####.####
.########..##..##..##.##.###.##
.##........##..##..##.##.....##
.##........##..##..##.##.....##
.##.........###..###..##.....##
*/

/*****************************************************************************/
/*! PWM Configure counter
* \description
*   This function configures the waveform and the dual edge mode of a PWM counter.
* \class 
*   PWM
* \params
*   uInst             [in] Counter instance
*   fWaveForm         [in] 1/0: Sawtooth/Triangle
*   fDualEdge         [in] 1/0: update shadow registers at (mpwm_cnt==0 and mpwm_cnt==max)/(mpwm_cnt==0)
* \return
*   NX10_MOTION_OKAY                                                  
*   NX10_MOTION_ERR_INVAL_PARAM                                                   */
/*****************************************************************************/
NX10_MOTION_RESULT NX10_PWM_ConfigCnt( unsigned int uInst, int fWaveForm, int fDualEdge )
{
  unsigned long ulVal;
  
  switch( uInst ) 
  {
    case 0:  
      ulVal = s_ptPwm->ulMpwm_config_counter & ~(MSK_NX10_mpwm_config_counter_waveform_cnt0|MSK_NX10_mpwm_config_counter_dual_edge0);
      s_ptPwm->ulMpwm_config_counter = ulVal | (fWaveForm<<SRT_NX10_mpwm_config_counter_waveform_cnt0) |(fDualEdge<<SRT_NX10_mpwm_config_counter_dual_edge0);
    break;
  
    case 1:
      ulVal = s_ptPwm->ulMpwm_config_counter & ~(MSK_NX10_mpwm_config_counter_waveform_cnt1|MSK_NX10_mpwm_config_counter_dual_edge1);
      s_ptPwm->ulMpwm_config_counter = ulVal | (fWaveForm<<SRT_NX10_mpwm_config_counter_waveform_cnt1) |(fDualEdge<<SRT_NX10_mpwm_config_counter_dual_edge1);
    break;
    
    default:
      // invalid instance
      return NX10_MOTION_ERR_INVAL_PARAM;
    break;    
  } // end switch  

  return NX10_MOTION_OKAY;  
}

/*****************************************************************************/
/*! PWM Configure failure
* \description
*   This function configures the failure behaviour.
* \class 
*   PWM
* \params
*   fEn         [in] 1/0: failure input sets failure bit (all PWM pins set to defined state) / failure input disabled (but might generate an interrupt)
* \return
*   NX10_MOTION_OKAY                                                  
*   NX10_MOTION_ERR_INVAL_PARAM                                                   */
/*****************************************************************************/
NX10_MOTION_RESULT NX10_PWM_ConfigFailure( int fEn )
{
  if( fEn ) {
    s_ptPwm->ulMpwm_config_failure |= MSK_NX10_mpwm_config_failure_enable;
  } else {
    s_ptPwm->ulMpwm_config_failure &= ~MSK_NX10_mpwm_config_failure_enable;
  }

  return NX10_MOTION_OKAY;  
}

/*****************************************************************************/
/*! PWM Set SW failure
* \description
*   This function sets the SW failure signal.
* \class 
*   PWM
* \params
*   fEn         [in] 1/0: SW failure on/off
* \return
*   NX10_MOTION_OKAY                                                  
*   NX10_MOTION_ERR_INVAL_PARAM                                                   */
/*****************************************************************************/
NX10_MOTION_RESULT NX10_PWM_SetSwFailure( int fEn )
{
  if( fEn ) {
    s_ptPwm->ulMpwm_config_failure |= MSK_NX10_mpwm_config_failure_failure;
  } else {
    s_ptPwm->ulMpwm_config_failure &= ~MSK_NX10_mpwm_config_failure_failure;
  }

  return NX10_MOTION_OKAY;  
}

/*****************************************************************************/
/*! PWM Configure channel
* \description
*   This function configures a PWM output channel.
* \class 
*   PWM
* \params
*   uInst             [in] Channel instance
*   ePinCfg           [in] PWM pin configuration
*   fShadowEn         [in] 1/0: shadow enabled/disabled
*   eFailureCfg       [in] Failure configuration
* \return
*   NX10_MOTION_OKAY                                                  
*   NX10_MOTION_ERR_INVAL_PARAM                                                   */
/*****************************************************************************/
NX10_MOTION_RESULT NX10_PWM_ConfigCh( unsigned int uInst, NX10_PWM_PINCFG ePinCfg, int fShadowEn, NX10_PWM_FAILCFG eFailureCfg )
{
  unsigned long ulVal;

  if( uInst>=NX10_PWM_CHANNELS )
    return NX10_MOTION_ERR_INVAL_PARAM;
  
  switch( uInst ) 
  {
    case 0:  
      ulVal = s_ptPwm->ulMpwm_config_pins & ~(MSK_NX10_mpwm_config_pins_cfg_0|MSK_NX10_mpwm_config_pins_shadow0);
      s_ptPwm->ulMpwm_config_pins = ulVal | (ePinCfg<<SRT_NX10_mpwm_config_pins_cfg_0) | (fShadowEn<<SRT_NX10_mpwm_config_pins_shadow0);
      
      ulVal = s_ptPwm->ulMpwm_config_failure & (~MSK_NX10_mpwm_config_failure_cfg_0);
      s_ptPwm->ulMpwm_config_failure = ulVal | (eFailureCfg<<SRT_NX10_mpwm_config_failure_cfg_0);
    break;
  
    case 1:  
      ulVal = s_ptPwm->ulMpwm_config_pins & ~(MSK_NX10_mpwm_config_pins_cfg_1|MSK_NX10_mpwm_config_pins_shadow1);
      s_ptPwm->ulMpwm_config_pins = ulVal | (ePinCfg<<SRT_NX10_mpwm_config_pins_cfg_1) | (fShadowEn<<SRT_NX10_mpwm_config_pins_shadow1);
      
      ulVal = s_ptPwm->ulMpwm_config_failure & ~MSK_NX10_mpwm_config_failure_cfg_1;
      s_ptPwm->ulMpwm_config_failure = ulVal | (eFailureCfg<<SRT_NX10_mpwm_config_failure_cfg_1);
    break;

    case 2:  
      ulVal = s_ptPwm->ulMpwm_config_pins & ~(MSK_NX10_mpwm_config_pins_cfg_2|MSK_NX10_mpwm_config_pins_shadow2);
      s_ptPwm->ulMpwm_config_pins = ulVal | (ePinCfg<<SRT_NX10_mpwm_config_pins_cfg_2) | (fShadowEn<<SRT_NX10_mpwm_config_pins_shadow2);
      
      ulVal = s_ptPwm->ulMpwm_config_failure & (~MSK_NX10_mpwm_config_failure_cfg_2);
      s_ptPwm->ulMpwm_config_failure = ulVal | (eFailureCfg<<SRT_NX10_mpwm_config_failure_cfg_2);
    break;

    case 3:  
      ulVal = s_ptPwm->ulMpwm_config_pins & ~(MSK_NX10_mpwm_config_pins_cfg_3|MSK_NX10_mpwm_config_pins_shadow3);
      s_ptPwm->ulMpwm_config_pins = ulVal | (ePinCfg<<SRT_NX10_mpwm_config_pins_cfg_3) | (fShadowEn<<SRT_NX10_mpwm_config_pins_shadow3);
      
      ulVal = s_ptPwm->ulMpwm_config_failure & (~MSK_NX10_mpwm_config_failure_cfg_3);
      s_ptPwm->ulMpwm_config_failure = ulVal | (eFailureCfg<<SRT_NX10_mpwm_config_failure_cfg_3);
    break;

    case 4:  
      ulVal = s_ptPwm->ulMpwm_config_pins & ~(MSK_NX10_mpwm_config_pins_cfg_4|MSK_NX10_mpwm_config_pins_shadow4);
      s_ptPwm->ulMpwm_config_pins = ulVal | (ePinCfg<<SRT_NX10_mpwm_config_pins_cfg_4) | (fShadowEn<<SRT_NX10_mpwm_config_pins_shadow4);
      
      ulVal = s_ptPwm->ulMpwm_config_failure & (~MSK_NX10_mpwm_config_failure_cfg_4);
      s_ptPwm->ulMpwm_config_failure = ulVal | (eFailureCfg<<SRT_NX10_mpwm_config_failure_cfg_4);
    break;

    case 5:  
      ulVal = s_ptPwm->ulMpwm_config_pins & ~(MSK_NX10_mpwm_config_pins_cfg_5|MSK_NX10_mpwm_config_pins_shadow5);
      s_ptPwm->ulMpwm_config_pins = ulVal | (ePinCfg<<SRT_NX10_mpwm_config_pins_cfg_5) | (fShadowEn<<SRT_NX10_mpwm_config_pins_shadow5);
      
      ulVal = s_ptPwm->ulMpwm_config_failure & (~MSK_NX10_mpwm_config_failure_cfg_5);
      s_ptPwm->ulMpwm_config_failure = ulVal | (eFailureCfg<<SRT_NX10_mpwm_config_failure_cfg_5);
    break;
    
    case 6:  
      ulVal = s_ptPwm->ulMpwm_config_pins & ~(MSK_NX10_mpwm_config_pins_cfg_6|MSK_NX10_mpwm_config_pins_shadow6);
      s_ptPwm->ulMpwm_config_pins = ulVal | (ePinCfg<<SRT_NX10_mpwm_config_pins_cfg_6) | (fShadowEn<<SRT_NX10_mpwm_config_pins_shadow6);
      
      ulVal = s_ptPwm->ulMpwm_config_failure & (~MSK_NX10_mpwm_config_failure_cfg_6);
      s_ptPwm->ulMpwm_config_failure = ulVal | (eFailureCfg<<SRT_NX10_mpwm_config_failure_cfg_6);
    break;

    case 7:  
      ulVal = s_ptPwm->ulMpwm_config_pins & ~(MSK_NX10_mpwm_config_pins_cfg_7|MSK_NX10_mpwm_config_pins_shadow7);
      s_ptPwm->ulMpwm_config_pins = ulVal | (ePinCfg<<SRT_NX10_mpwm_config_pins_cfg_7) | (fShadowEn<<SRT_NX10_mpwm_config_pins_shadow7);
      
      ulVal = s_ptPwm->ulMpwm_config_failure & (~MSK_NX10_mpwm_config_failure_cfg_7);
      s_ptPwm->ulMpwm_config_failure = ulVal | (eFailureCfg<<SRT_NX10_mpwm_config_failure_cfg_7);
    break;

    default:
      // invalid instance
      return NX10_MOTION_ERR_INVAL_PARAM;
    break;    
  } // end switch  

  return NX10_MOTION_OKAY;  
}

/*****************************************************************************/
/*! PWM Set counter period
* \description
*   This function sets the period of a counter.
*   Note: The counter period must be set after the counter waveform was configured, because it uses the waveform to correct the period value.
* \class 
*   PWM
* \params
*   uInst             [in] Counter instance
*   ulPeriod          [in] Period
* \return
*   NX10_MOTION_OKAY                                                  
*   NX10_MOTION_ERR_INVAL_PARAM                                                   */
/*****************************************************************************/
NX10_MOTION_RESULT NX10_PWM_SetCntPeriod( unsigned int uInst, unsigned long ulPeriod )
{
  if( ulPeriod>0x000fffff )
    return NX10_MOTION_ERR_INVAL_PARAM;
  if( uInst>=NX10_PWM_NUM_CNT )
    return NX10_MOTION_ERR_INVAL_PARAM;

  switch( uInst ) {
    case 0:
      if( (s_ptPwm->ulMpwm_config_counter&MSK_NX10_mpwm_config_counter_waveform_cnt0)==0 ) 
        s_ptPwm->asMpwm_cnt[uInst].ulPeriod = ulPeriod; // triangle
      else 
        s_ptPwm->asMpwm_cnt[uInst].ulPeriod = ulPeriod-1; // waveform
    break;
    
    case 1:
      if( (s_ptPwm->ulMpwm_config_counter&MSK_NX10_mpwm_config_counter_waveform_cnt1)==0 ) 
        s_ptPwm->asMpwm_cnt[uInst].ulPeriod = ulPeriod; // triangle
      else 
        s_ptPwm->asMpwm_cnt[uInst].ulPeriod = ulPeriod-1; // waveform
    break;

  } // end switch
  
  return NX10_MOTION_OKAY;  
}


/*****************************************************************************/
/*! PWM Retigger counter watchdog
* \description
*   This function re-triggers the watchdog of a PWM counter.
*   Note: 0 disables the watchdog.
* \class 
*   PWM
* \params
*   uInst             [in] Counter instance
*   ulWdgVal          [in] 0: disable 1..15: set number of cycles until watchdog expires
* \return
*   NX10_MOTION_OKAY                                                  
*   NX10_MOTION_ERR_INVAL_PARAM                                                   */
/*****************************************************************************/
NX10_MOTION_RESULT NX10_PWM_RetriggerCnt( unsigned int uInst, unsigned long ulWdgVal )
{
  if( ulWdgVal>15 )
    return NX10_MOTION_ERR_INVAL_PARAM;
  if( uInst>=NX10_PWM_NUM_CNT )
    return NX10_MOTION_ERR_INVAL_PARAM;

  s_ptPwm->asMpwm_cnt[uInst].ulWatchdog = ulWdgVal;
  
  return NX10_MOTION_OKAY;  
}

/*****************************************************************************/
/*! PWM Set channel width
* \description
*   This function sets the width of a PWM channel.
* \class 
*   PWM
* \params
*   uInst             [in] Channel instance
*   ulWidth           [in] Width in steps of 2.5ns
* \return
*   NX10_MOTION_OKAY                                                  
*   NX10_MOTION_ERR_INVAL_PARAM                                                   */
/*****************************************************************************/
NX10_MOTION_RESULT NX10_PWM_SetChWidth( unsigned int uInst, unsigned long ulWidth )
{
  if( ulWidth>0x000fffff )
    return NX10_MOTION_ERR_INVAL_PARAM;
  if( uInst>=NX10_PWM_CHANNELS )
    return NX10_MOTION_ERR_INVAL_PARAM;

  s_ptPwm->aulMpwm_t[uInst] = ulWidth;
  
  return NX10_MOTION_OKAY;  
}

/*****************************************************************************/
/*! PWM Configure interrupt source
* \description
*   This function configures the mode and the delay of an interrupt source.
* \class 
*   PWM
* \params
*   uInst             [in] Source instance
*   eIrqCfg           [in] Select PWM pin for interrupt
*   uDelay            [in] Delay from event to generation interrupt in steps of 20ns
* \return
*   NX10_MOTION_OKAY                                                  
*   NX10_MOTION_ERR_INVAL_PARAM                                                   */
/*****************************************************************************/
NX10_MOTION_RESULT NX10_PWM_CfgIrqSrc( unsigned int uInst, NX10_PWM_IRQCFG eIrqCfg, unsigned int uDelay )
{
  unsigned long ulVal;
  
  if( uDelay>0x000003ff )
    return NX10_MOTION_ERR_INVAL_PARAM;
  if( uInst>=NX10_PWM_IRQ_SRC_NUM )
    return NX10_MOTION_ERR_INVAL_PARAM;

  switch( uInst ) {
    case 0: 
      ulVal = s_ptPwm->ulMpwm_irq_config & ~(MSK_NX10_mpwm_irq_config_nr0|MSK_NX10_mpwm_irq_config_delay0);
      s_ptPwm->ulMpwm_irq_config = ulVal | (eIrqCfg<<SRT_NX10_mpwm_irq_config_nr0) | (uDelay<<SRT_NX10_mpwm_irq_config_delay0);
    break;
    case 1: 
      ulVal = s_ptPwm->ulMpwm_irq_config & ~( MSK_NX10_mpwm_irq_config_nr1|MSK_NX10_mpwm_irq_config_delay1);
      s_ptPwm->ulMpwm_irq_config = ulVal | (eIrqCfg<<SRT_NX10_mpwm_irq_config_nr1) | (uDelay<<SRT_NX10_mpwm_irq_config_delay1);
    break;
    default: 
      return NX10_MOTION_ERR_INVAL_PARAM; // invalid instance
    break; // invalid instance
  } // end switch  

  return NX10_MOTION_OKAY;  
}

/*****************************************************************************/
/*! PWM Get counter status
* \description
*   This function reads the actual counter value and the captures systime at counter==0.
* \class 
*   PWM
* \params
*   uInst             [in]  Counter instance
*   ulValue           [out] Actual value
*   ulSystime         [out] Captured systime_ns at counter=0
* \return
*   NX10_MOTION_OKAY                                                  
*   NX10_MOTION_ERR_INVAL_PARAM                                                   */
/*****************************************************************************/
NX10_MOTION_RESULT NX10_PWM_GetCntStat( unsigned int uInst, unsigned long* pulValue, unsigned int* pulSystime )
{
  if( uInst>=NX10_PWM_NUM_CNT )
    return NX10_MOTION_ERR_INVAL_PARAM;

  *pulValue   = s_ptPwm->asMpwm_cnt[uInst].ulCnt;
  *pulSystime = s_ptPwm->asMpwm_cnt[uInst].ulSystime;

  return NX10_MOTION_OKAY;  
}

/*****************************************************************************/
/*! PWM Start
* \description
*   This function starts the PWM unit. Both counters can be started individually.
* \class 
*   PWM
* \params
* \return
*   NX10_MOTION_OKAY                                                              */
/*****************************************************************************/
NX10_MOTION_RESULT NX10_PWM_Start( int fCnt0En, int fCnt1En )
{
  unsigned int ulVal;
  
  /* get current config */
  ulVal = s_ptPwm->ulMpwm_config_counter;
  
  /* include CntRun if enabled */
  if( fCnt0En ) {
    ulVal |= MSK_NX10_mpwm_config_counter_run_cnt0;
  }  
  if( fCnt1En ) {
    ulVal |= MSK_NX10_mpwm_config_counter_run_cnt1;
  }  
  /* write config back */
  s_ptPwm->ulMpwm_config_counter = ulVal;
  
  return NX10_MOTION_OKAY;  
}

/*****************************************************************************/
/*! PWM Stop
* \description
*   This function stops the PWM unit. Both counters can be stopped individually.
* \class 
*   PWM
* \params
* \return
*   NX10_MOTION_OKAY                                                              */
/*****************************************************************************/
NX10_MOTION_RESULT NX10_PWM_Stop( int fCnt0Dis, int fCnt1Dis )
{
  unsigned int ulVal;
  
  /* get current config */
  ulVal = s_ptPwm->ulMpwm_config_counter;
  
  /* exclude CntRun if disabled */
  if( fCnt0Dis ) {
    ulVal &= ~MSK_NX10_mpwm_config_counter_run_cnt0;
  }  
  if( fCnt1Dis ) {
    ulVal &= ~MSK_NX10_mpwm_config_counter_run_cnt1;
  }  
  /* write config back */
  s_ptPwm->ulMpwm_config_counter = ulVal;
  
  return NX10_MOTION_OKAY;  
}

/*****************************************************************************/
/*! PWM Enable Interrupt(s)
* \description
*   This function enables a set of interrupts.
* \class 
*   PWM
* \params
*  fSrc0IrqEn     [in] 1: enable src0 interrupt
*  fSrc1IrqEn     [in] 1: enable src1 interrupt
*  fFailureIrqEn  [in] 1: enable failure interrupt
* \return
*   NX10_MOTION_OKAY                                                              */
/*****************************************************************************/
NX10_MOTION_RESULT NX10_PWM_EnIrq( int fSrc0IrqEn, int fSrc1IrqEn, int fFailureIrqEn )
{
  s_ptPwm->ulMpwm_irq_msk_set =  (fSrc0IrqEn<<SRT_NX10_mpwm_irq_msk_set_interrupt0)
                                |(fSrc1IrqEn<<SRT_NX10_mpwm_irq_msk_set_interrupt1)
                                |(fFailureIrqEn<<SRT_NX10_mpwm_irq_msk_set_failure);
  return NX10_MOTION_OKAY;
}

/*****************************************************************************/
/*! PWM Disable Interrupt(s)
* \description
*   This function disables a set of interrupts.
* \class 
*   PWM
* \params
*  fSrc0IrqDis    [in] 1: disable src0 interrupt
*  fSrc1IrqDis    [in] 1: disable src1 interrupt
*  fFailureIrqDis [in] 1: disable failure interrupt
* \return
*   NX10_MOTION_OKAY                                                              */
/*****************************************************************************/
NX10_MOTION_RESULT NX10_PWM_DisIrq( int fSrc0IrqDis, int fSrc1IrqDis, int fFailureIrqDis )
{
  s_ptPwm->ulMpwm_irq_msk_reset =  (fSrc0IrqDis<<SRT_NX10_mpwm_irq_msk_set_interrupt0)
                                  |(fSrc1IrqDis<<SRT_NX10_mpwm_irq_msk_set_interrupt1)
                                  |(fFailureIrqDis<<SRT_NX10_mpwm_irq_msk_set_failure);
  return NX10_MOTION_OKAY;
}

/*****************************************************************************/
/*! PWM Get Interrupt(s)
* \description
*   This function retrieves the current interrupt masked requests of the PWM unit.
* \class 
*   PWM
* \params
   pulIrq     [out] Pointer to interrupt events
* \return
*   NX10_MOTION_OKAY                                                              */
/*****************************************************************************/
NX10_MOTION_RESULT NX10_PWM_GetIrq( unsigned long *pulIrq )
{
  /* get irq requests */
  *pulIrq = s_ptPwm->ulMpwm_irq_masked;
  
  return NX10_MOTION_OKAY;
}

/*****************************************************************************/
/*! PWM Get Interrupt(s)
* \description
*   This function retrieves the current interrupt raw requests of the PWM unit.
* \class 
*   PWM
* \params
   pulIrq     [out] Pointer to interrupt events
* \return
*   NX10_MOTION_OKAY                                                              */
/*****************************************************************************/
NX10_MOTION_RESULT NX10_PWM_GetIrqRaw( unsigned long *pulIrq )
{
  /* get irq requests */
  *pulIrq = s_ptPwm->ulMpwm_irq_raw;
  
  return NX10_MOTION_OKAY;
}

/*****************************************************************************/
/*! PWM Confirm interrupt(s)
* \description
*   This function confirms a set of interrupts that were requested by the PWM unit.
* \class 
*   PWM
* \params
*   ulConfirmIrqMask  [in] Mask to confirm interrupt events
* \return
*   NX10_MOTION_OKAY                                                              */
/*****************************************************************************/
NX10_MOTION_RESULT NX10_PWM_ConfirmIrq( unsigned long ulConfirmIrqMask )
{
  /* confirm the set of irqs */
  s_ptPwm->ulMpwm_irq_raw = ulConfirmIrqMask;
  
  return NX10_MOTION_OKAY;
}

/*****************************************************************************/
/*! PWM Set Parameter
* \description
*   This function writes parameter.
* \class 
*   PWM
* \params
*   ulPrmID          [in]  Parameter ID
*   ulPrmVal         [in]  Parameter value
* \return
*   NX10_MOTION_OKAY                                                             
*   NX10_MOTION_ERR_INVAL_PARAM                                                   */
/*****************************************************************************/
NX10_MOTION_RESULT NX10_PWM_SetPrm( unsigned long ulPrmID, unsigned long ulPrmVal )
{
  switch( ulPrmID )
  { 
  /* Config_Counter */
    case NX10_PWM_PRM_WAVEFORM_CNT0:
      s_ptPwm->ulMpwm_config_counter &= ~MSK_NX10_mpwm_config_counter_waveform_cnt0;
      s_ptPwm->ulMpwm_config_counter |= (ulPrmVal<<SRT_NX10_mpwm_config_counter_waveform_cnt0);
    break;
    case NX10_PWM_PRM_DUAL_EDGE0:
      s_ptPwm->ulMpwm_config_counter &= ~MSK_NX10_mpwm_config_counter_dual_edge0;
      s_ptPwm->ulMpwm_config_counter |= (ulPrmVal<<SRT_NX10_mpwm_config_counter_dual_edge0);
    break;
    case NX10_PWM_PRM_WAVEFORM_CNT1:
      s_ptPwm->ulMpwm_config_counter &= ~MSK_NX10_mpwm_config_counter_waveform_cnt1;
      s_ptPwm->ulMpwm_config_counter |= (ulPrmVal<<SRT_NX10_mpwm_config_counter_waveform_cnt1);
    break;
    case NX10_PWM_PRM_DUAL_EDGE1:
      s_ptPwm->ulMpwm_config_counter &= ~MSK_NX10_mpwm_config_counter_dual_edge1;
      s_ptPwm->ulMpwm_config_counter |= (ulPrmVal<<SRT_NX10_mpwm_config_counter_dual_edge1);
    break;

  /* Config_Pins */
    case NX10_PWM_PRM_CFG_PIN0:
      s_ptPwm->ulMpwm_config_pins &= ~MSK_NX10_mpwm_config_pins_cfg_0;
      s_ptPwm->ulMpwm_config_pins |= (ulPrmVal<<SRT_NX10_mpwm_config_pins_cfg_0);
    break;
    case NX10_PWM_PRM_SHADOW0:
      s_ptPwm->ulMpwm_config_pins &= ~MSK_NX10_mpwm_config_pins_shadow0;
      s_ptPwm->ulMpwm_config_pins |= (ulPrmVal<<SRT_NX10_mpwm_config_pins_shadow0);
    break;
    case NX10_PWM_PRM_CFG_PIN1:
      s_ptPwm->ulMpwm_config_pins &= ~MSK_NX10_mpwm_config_pins_cfg_1;
      s_ptPwm->ulMpwm_config_pins |= (ulPrmVal<<SRT_NX10_mpwm_config_pins_cfg_1);
    break;
    case NX10_PWM_PRM_SHADOW1:
      s_ptPwm->ulMpwm_config_pins &= ~MSK_NX10_mpwm_config_pins_shadow1;
      s_ptPwm->ulMpwm_config_pins |= (ulPrmVal<<SRT_NX10_mpwm_config_pins_shadow1);
    break;
    case NX10_PWM_PRM_CFG_PIN2:
      s_ptPwm->ulMpwm_config_pins &= ~MSK_NX10_mpwm_config_pins_cfg_2;
      s_ptPwm->ulMpwm_config_pins |= (ulPrmVal<<SRT_NX10_mpwm_config_pins_cfg_2);
    break;
    case NX10_PWM_PRM_SHADOW2:
      s_ptPwm->ulMpwm_config_pins &= ~MSK_NX10_mpwm_config_pins_shadow2;
      s_ptPwm->ulMpwm_config_pins |= (ulPrmVal<<SRT_NX10_mpwm_config_pins_shadow2);
    break;
    case NX10_PWM_PRM_CFG_PIN3:
      s_ptPwm->ulMpwm_config_pins &= ~MSK_NX10_mpwm_config_pins_cfg_3;
      s_ptPwm->ulMpwm_config_pins |= (ulPrmVal<<SRT_NX10_mpwm_config_pins_cfg_3);
    break;
    case NX10_PWM_PRM_SHADOW3:
      s_ptPwm->ulMpwm_config_pins &= ~MSK_NX10_mpwm_config_pins_shadow3;
      s_ptPwm->ulMpwm_config_pins |= (ulPrmVal<<SRT_NX10_mpwm_config_pins_shadow3);
    break;
    case NX10_PWM_PRM_CFG_PIN4:
      s_ptPwm->ulMpwm_config_pins &= ~MSK_NX10_mpwm_config_pins_cfg_4;
      s_ptPwm->ulMpwm_config_pins |= (ulPrmVal<<SRT_NX10_mpwm_config_pins_cfg_4);
    break;
    case NX10_PWM_PRM_SHADOW4:
      s_ptPwm->ulMpwm_config_pins &= ~MSK_NX10_mpwm_config_pins_shadow4;
      s_ptPwm->ulMpwm_config_pins |= (ulPrmVal<<SRT_NX10_mpwm_config_pins_shadow4);
    break;
    case NX10_PWM_PRM_CFG_PIN5:
      s_ptPwm->ulMpwm_config_pins &= ~MSK_NX10_mpwm_config_pins_cfg_5;
      s_ptPwm->ulMpwm_config_pins |= (ulPrmVal<<SRT_NX10_mpwm_config_pins_cfg_5);
    break;
    case NX10_PWM_PRM_SHADOW5:
      s_ptPwm->ulMpwm_config_pins &= ~MSK_NX10_mpwm_config_pins_shadow5;
      s_ptPwm->ulMpwm_config_pins |= (ulPrmVal<<SRT_NX10_mpwm_config_pins_shadow5);
    break;
    case NX10_PWM_PRM_CFG_PIN6:
      s_ptPwm->ulMpwm_config_pins &= ~MSK_NX10_mpwm_config_pins_cfg_6;
      s_ptPwm->ulMpwm_config_pins |= (ulPrmVal<<SRT_NX10_mpwm_config_pins_cfg_6);
    break;
    case NX10_PWM_PRM_SHADOW6:
      s_ptPwm->ulMpwm_config_pins &= ~MSK_NX10_mpwm_config_pins_shadow6;
      s_ptPwm->ulMpwm_config_pins |= (ulPrmVal<<SRT_NX10_mpwm_config_pins_shadow6);
    break;
    case NX10_PWM_PRM_CFG_PIN7:
      s_ptPwm->ulMpwm_config_pins &= ~MSK_NX10_mpwm_config_pins_cfg_7;
      s_ptPwm->ulMpwm_config_pins |= (ulPrmVal<<SRT_NX10_mpwm_config_pins_cfg_7);
    break;
    case NX10_PWM_PRM_SHADOW7:
      s_ptPwm->ulMpwm_config_pins &= ~MSK_NX10_mpwm_config_pins_shadow7;
      s_ptPwm->ulMpwm_config_pins |= (ulPrmVal<<SRT_NX10_mpwm_config_pins_shadow7);
    break;

  /* Config_Failure */
    case NX10_PWM_PRM_CFG_FAIL0:
      s_ptPwm->ulMpwm_config_failure &= ~MSK_NX10_mpwm_config_failure_cfg_0;
      s_ptPwm->ulMpwm_config_failure |= (ulPrmVal<<SRT_NX10_mpwm_config_failure_cfg_0);
    break;
    case NX10_PWM_PRM_CFG_FAIL1:
      s_ptPwm->ulMpwm_config_failure &= ~MSK_NX10_mpwm_config_failure_cfg_1;
      s_ptPwm->ulMpwm_config_failure |= (ulPrmVal<<SRT_NX10_mpwm_config_failure_cfg_1);
    break;
    case NX10_PWM_PRM_CFG_FAIL2:
      s_ptPwm->ulMpwm_config_failure &= ~MSK_NX10_mpwm_config_failure_cfg_2;
      s_ptPwm->ulMpwm_config_failure |= (ulPrmVal<<SRT_NX10_mpwm_config_failure_cfg_2);
    break;
    case NX10_PWM_PRM_CFG_FAIL3:
      s_ptPwm->ulMpwm_config_failure &= ~MSK_NX10_mpwm_config_failure_cfg_3;
      s_ptPwm->ulMpwm_config_failure |= (ulPrmVal<<SRT_NX10_mpwm_config_failure_cfg_3);
    break;
    case NX10_PWM_PRM_CFG_FAIL4:
      s_ptPwm->ulMpwm_config_failure &= ~MSK_NX10_mpwm_config_failure_cfg_4;
      s_ptPwm->ulMpwm_config_failure |= (ulPrmVal<<SRT_NX10_mpwm_config_failure_cfg_4);
    break;
    case NX10_PWM_PRM_CFG_FAIL5:
      s_ptPwm->ulMpwm_config_failure &= ~MSK_NX10_mpwm_config_failure_cfg_5;
      s_ptPwm->ulMpwm_config_failure |= (ulPrmVal<<SRT_NX10_mpwm_config_failure_cfg_5);
    break;
    case NX10_PWM_PRM_CFG_FAIL6:
      s_ptPwm->ulMpwm_config_failure &= ~MSK_NX10_mpwm_config_failure_cfg_6;
      s_ptPwm->ulMpwm_config_failure |= (ulPrmVal<<SRT_NX10_mpwm_config_failure_cfg_6);
    break;
    case NX10_PWM_PRM_CFG_FAIL7:
      s_ptPwm->ulMpwm_config_failure &= ~MSK_NX10_mpwm_config_failure_cfg_7;
      s_ptPwm->ulMpwm_config_failure |= (ulPrmVal<<SRT_NX10_mpwm_config_failure_cfg_7);
    break;
    case NX10_PWM_PRM_FAIL_INPUT_EN:
      s_ptPwm->ulMpwm_config_failure &= ~MSK_NX10_mpwm_config_failure_enable;
      s_ptPwm->ulMpwm_config_failure |= (ulPrmVal<<SRT_NX10_mpwm_config_failure_enable);
    break;
    case NX10_PWM_PRM_FAIL_FAILURE:
      s_ptPwm->ulMpwm_config_failure &= ~MSK_NX10_mpwm_config_failure_failure;
      s_ptPwm->ulMpwm_config_failure |= (ulPrmVal<<SRT_NX10_mpwm_config_failure_failure);
    break;

    /* Irq_Config */
    case NX10_PWM_PRM_IRQCFG0:
      s_ptPwm->ulMpwm_irq_config &= ~MSK_NX10_mpwm_irq_config_nr0;
      s_ptPwm->ulMpwm_irq_config |= (ulPrmVal<<SRT_NX10_mpwm_irq_config_nr0);
    break;
    case NX10_PWM_PRM_IRQDELAY0:
      s_ptPwm->ulMpwm_irq_config &= ~MSK_NX10_mpwm_irq_config_delay0;
      s_ptPwm->ulMpwm_irq_config |= (ulPrmVal<<SRT_NX10_mpwm_irq_config_delay0);
    break;
    case NX10_PWM_PRM_IRQCFG1:
      s_ptPwm->ulMpwm_irq_config &= ~MSK_NX10_mpwm_irq_config_nr1;
      s_ptPwm->ulMpwm_irq_config |= (ulPrmVal<<SRT_NX10_mpwm_irq_config_nr1);
    break;
    case NX10_PWM_PRM_IRQDELAY1:
      s_ptPwm->ulMpwm_irq_config &= ~MSK_NX10_mpwm_irq_config_delay1;
      s_ptPwm->ulMpwm_irq_config |= (ulPrmVal<<SRT_NX10_mpwm_irq_config_delay1);
    break;

    /* Interrupts */
    case NX10_PWM_PRM_IRQMSKSET: s_ptPwm->ulMpwm_irq_msk_set = ulPrmVal; break;
    case NX10_PWM_PRM_IRQMSKRST: s_ptPwm->ulMpwm_irq_msk_reset = ulPrmVal; break;

    /* Counters */
    case NX10_PWM_PRM_CNT0PERIOD: s_ptPwm->asMpwm_cnt[0].ulPeriod = ulPrmVal; break;
    case NX10_PWM_PRM_CNT1PERIOD: s_ptPwm->asMpwm_cnt[1].ulPeriod = ulPrmVal; break;

    /* T0..T8 */
    case NX10_PWM_PRM_T0: s_ptPwm->aulMpwm_t[0] = ulPrmVal; break;
    case NX10_PWM_PRM_T1: s_ptPwm->aulMpwm_t[1] = ulPrmVal; break;
    case NX10_PWM_PRM_T2: s_ptPwm->aulMpwm_t[2] = ulPrmVal; break;
    case NX10_PWM_PRM_T3: s_ptPwm->aulMpwm_t[3] = ulPrmVal; break;
    case NX10_PWM_PRM_T4: s_ptPwm->aulMpwm_t[4] = ulPrmVal; break;
    case NX10_PWM_PRM_T5: s_ptPwm->aulMpwm_t[5] = ulPrmVal; break;
    case NX10_PWM_PRM_T6: s_ptPwm->aulMpwm_t[6] = ulPrmVal; break;
    case NX10_PWM_PRM_T7: s_ptPwm->aulMpwm_t[7] = ulPrmVal; break;

    /* Default */
    default: return NX10_MOTION_ERR_INVAL_PARAM; break;
  } /* end switch */

  return NX10_MOTION_OKAY;
}


/*****************************************************************************/
/*! PWM Get Parameter
* \description
*   This function reads parameter.
* \class 
*   PWM
* \params
*   ulPrmID          [in]  Parameter ID
*   pulPrmVal        [in]  Pointer to parameter value
* \return
*   NX10_MOTION_OKAY                                                             
*   NX10_MOTION_ERR_INVAL_PARAM                                                   */
/*****************************************************************************/
NX10_MOTION_RESULT NX10_PWM_GetPrm( unsigned long ulPrmID, unsigned long* pulPrmVal )
{
  switch( ulPrmID )
  { 
  /* Config_Counter */
    case NX10_PWM_PRM_RUN_CNT0:
      *pulPrmVal = (s_ptPwm->ulMpwm_config_counter & MSK_NX10_mpwm_config_counter_run_cnt0)>>SRT_NX10_mpwm_config_counter_run_cnt0;
    break;
    case NX10_PWM_PRM_WAVEFORM_CNT0:
      *pulPrmVal = (s_ptPwm->ulMpwm_config_counter & MSK_NX10_mpwm_config_counter_waveform_cnt0)>>SRT_NX10_mpwm_config_counter_waveform_cnt0;
    break;
    case NX10_PWM_PRM_DUAL_EDGE0:
      *pulPrmVal = (s_ptPwm->ulMpwm_config_counter & MSK_NX10_mpwm_config_counter_dual_edge0)>>SRT_NX10_mpwm_config_counter_dual_edge0;
    break;
    case NX10_PWM_PRM_RUN_CNT1:
      *pulPrmVal = (s_ptPwm->ulMpwm_config_counter & MSK_NX10_mpwm_config_counter_run_cnt1)>>SRT_NX10_mpwm_config_counter_run_cnt1;
    break;
    case NX10_PWM_PRM_WAVEFORM_CNT1:
      *pulPrmVal = (s_ptPwm->ulMpwm_config_counter & MSK_NX10_mpwm_config_counter_waveform_cnt1)>>SRT_NX10_mpwm_config_counter_waveform_cnt1;
    break;
    case NX10_PWM_PRM_DUAL_EDGE1:
      *pulPrmVal = (s_ptPwm->ulMpwm_config_counter & MSK_NX10_mpwm_config_counter_dual_edge1)>>SRT_NX10_mpwm_config_counter_dual_edge1;
    break;

  /* Config_Pins */
    case NX10_PWM_PRM_CFG_PIN0:
      *pulPrmVal = (s_ptPwm->ulMpwm_config_pins & MSK_NX10_mpwm_config_pins_cfg_0)>>SRT_NX10_mpwm_config_pins_cfg_0;
    break;
    case NX10_PWM_PRM_SHADOW0:
      *pulPrmVal = (s_ptPwm->ulMpwm_config_pins & MSK_NX10_mpwm_config_pins_shadow0)>>SRT_NX10_mpwm_config_pins_shadow0;
    break;
    case NX10_PWM_PRM_CFG_PIN1:
      *pulPrmVal = (s_ptPwm->ulMpwm_config_pins & MSK_NX10_mpwm_config_pins_cfg_1)>>SRT_NX10_mpwm_config_pins_cfg_1;
    break;
    case NX10_PWM_PRM_SHADOW1:
      *pulPrmVal = (s_ptPwm->ulMpwm_config_pins & MSK_NX10_mpwm_config_pins_shadow1)>>SRT_NX10_mpwm_config_pins_shadow1;
    break;
    case NX10_PWM_PRM_CFG_PIN2:
      *pulPrmVal = (s_ptPwm->ulMpwm_config_pins & MSK_NX10_mpwm_config_pins_cfg_2)>>SRT_NX10_mpwm_config_pins_cfg_2;
    break;
    case NX10_PWM_PRM_SHADOW2:
      *pulPrmVal = (s_ptPwm->ulMpwm_config_pins & MSK_NX10_mpwm_config_pins_shadow2)>>SRT_NX10_mpwm_config_pins_shadow2;
    break;
    case NX10_PWM_PRM_CFG_PIN3:
      *pulPrmVal = (s_ptPwm->ulMpwm_config_pins & MSK_NX10_mpwm_config_pins_cfg_3)>>SRT_NX10_mpwm_config_pins_cfg_3;
    break;
    case NX10_PWM_PRM_SHADOW3:
      *pulPrmVal = (s_ptPwm->ulMpwm_config_pins & MSK_NX10_mpwm_config_pins_shadow3)>>SRT_NX10_mpwm_config_pins_shadow3;
    break;
    case NX10_PWM_PRM_CFG_PIN4:
      *pulPrmVal = (s_ptPwm->ulMpwm_config_pins & MSK_NX10_mpwm_config_pins_cfg_4)>>SRT_NX10_mpwm_config_pins_cfg_4;
    break;
    case NX10_PWM_PRM_SHADOW4:
      *pulPrmVal = (s_ptPwm->ulMpwm_config_pins & MSK_NX10_mpwm_config_pins_shadow4)>>SRT_NX10_mpwm_config_pins_shadow4;
    break;
    case NX10_PWM_PRM_CFG_PIN5:
      *pulPrmVal = (s_ptPwm->ulMpwm_config_pins & MSK_NX10_mpwm_config_pins_cfg_5)>>SRT_NX10_mpwm_config_pins_cfg_5;
    break;
    case NX10_PWM_PRM_SHADOW5:
      *pulPrmVal = (s_ptPwm->ulMpwm_config_pins & MSK_NX10_mpwm_config_pins_shadow5)>>SRT_NX10_mpwm_config_pins_shadow5;
    break;
    case NX10_PWM_PRM_CFG_PIN6:
      *pulPrmVal = (s_ptPwm->ulMpwm_config_pins & MSK_NX10_mpwm_config_pins_cfg_6)>>SRT_NX10_mpwm_config_pins_cfg_6;
    break;
    case NX10_PWM_PRM_SHADOW6:
      *pulPrmVal = (s_ptPwm->ulMpwm_config_pins & MSK_NX10_mpwm_config_pins_shadow6)>>SRT_NX10_mpwm_config_pins_shadow6;
    break;
    case NX10_PWM_PRM_CFG_PIN7:
      *pulPrmVal = (s_ptPwm->ulMpwm_config_pins & MSK_NX10_mpwm_config_pins_cfg_7)>>SRT_NX10_mpwm_config_pins_cfg_7;
    break;
    case NX10_PWM_PRM_SHADOW7:
      *pulPrmVal = (s_ptPwm->ulMpwm_config_pins & MSK_NX10_mpwm_config_pins_shadow7)>>SRT_NX10_mpwm_config_pins_shadow7;
    break;

  /* Config_Failure */
    case NX10_PWM_PRM_CFG_FAIL0:
      *pulPrmVal = (s_ptPwm->ulMpwm_config_failure & MSK_NX10_mpwm_config_failure_cfg_0)>>SRT_NX10_mpwm_config_failure_cfg_0;
    break;
    case NX10_PWM_PRM_CFG_FAIL1:
      *pulPrmVal = (s_ptPwm->ulMpwm_config_failure & MSK_NX10_mpwm_config_failure_cfg_1)>>SRT_NX10_mpwm_config_failure_cfg_1;
    break;
    case NX10_PWM_PRM_CFG_FAIL2:
      *pulPrmVal = (s_ptPwm->ulMpwm_config_failure & MSK_NX10_mpwm_config_failure_cfg_2)>>SRT_NX10_mpwm_config_failure_cfg_2;
    break;
    case NX10_PWM_PRM_CFG_FAIL3:
      *pulPrmVal = (s_ptPwm->ulMpwm_config_failure & MSK_NX10_mpwm_config_failure_cfg_3)>>SRT_NX10_mpwm_config_failure_cfg_3;
    break;
    case NX10_PWM_PRM_CFG_FAIL4:
      *pulPrmVal = (s_ptPwm->ulMpwm_config_failure & MSK_NX10_mpwm_config_failure_cfg_4)>>SRT_NX10_mpwm_config_failure_cfg_4;
    break;
    case NX10_PWM_PRM_CFG_FAIL5:
      *pulPrmVal = (s_ptPwm->ulMpwm_config_failure & MSK_NX10_mpwm_config_failure_cfg_5)>>SRT_NX10_mpwm_config_failure_cfg_5;
    break;
    case NX10_PWM_PRM_CFG_FAIL6:
      *pulPrmVal = (s_ptPwm->ulMpwm_config_failure & MSK_NX10_mpwm_config_failure_cfg_6)>>SRT_NX10_mpwm_config_failure_cfg_6;
    break;
    case NX10_PWM_PRM_CFG_FAIL7:
      *pulPrmVal = (s_ptPwm->ulMpwm_config_failure & MSK_NX10_mpwm_config_failure_cfg_7)>>SRT_NX10_mpwm_config_failure_cfg_7;
    break;
    case NX10_PWM_PRM_FAIL_INPUT_EN:
      *pulPrmVal = (s_ptPwm->ulMpwm_config_failure & MSK_NX10_mpwm_config_failure_enable)>>SRT_NX10_mpwm_config_failure_enable;
    break;
    case NX10_PWM_PRM_FAIL_FAILURE:
      *pulPrmVal = (s_ptPwm->ulMpwm_config_failure & MSK_NX10_mpwm_config_failure_failure)>>SRT_NX10_mpwm_config_failure_failure;
    break;

  /* Irq_Config */
    case NX10_PWM_PRM_IRQCFG0:
      *pulPrmVal = (s_ptPwm->ulMpwm_irq_config & MSK_NX10_mpwm_irq_config_nr0)>>SRT_NX10_mpwm_irq_config_nr0;
    break;
    case NX10_PWM_PRM_IRQDELAY0:
      *pulPrmVal = (s_ptPwm->ulMpwm_irq_config & MSK_NX10_mpwm_irq_config_delay0)>>SRT_NX10_mpwm_irq_config_delay0;
    break;
    case NX10_PWM_PRM_IRQCFG1:
      *pulPrmVal = (s_ptPwm->ulMpwm_irq_config & MSK_NX10_mpwm_irq_config_nr1)>>SRT_NX10_mpwm_irq_config_nr1;
    break;
    case NX10_PWM_PRM_IRQDELAY1:
      *pulPrmVal = (s_ptPwm->ulMpwm_irq_config & MSK_NX10_mpwm_irq_config_delay1)>>SRT_NX10_mpwm_irq_config_delay1;
    break;

    /* Interrupt */
    case NX10_PWM_PRM_IRQMSKSET: *pulPrmVal = s_ptPwm->ulMpwm_irq_msk_set; break;
    case NX10_PWM_PRM_IRQMSKRST: *pulPrmVal = s_ptPwm->ulMpwm_irq_msk_reset; break;

    /* Counters */
    case NX10_PWM_PRM_CNT0PERIOD:  *pulPrmVal = s_ptPwm->asMpwm_cnt[0].ulPeriod; break;
    case NX10_PWM_PRM_CNT0:        *pulPrmVal = s_ptPwm->asMpwm_cnt[0].ulCnt; break;
    case NX10_PWM_PRM_CNT0SYSTIME: *pulPrmVal = s_ptPwm->asMpwm_cnt[0].ulSystime; break;
    case NX10_PWM_PRM_CNT0WDG:     *pulPrmVal = s_ptPwm->asMpwm_cnt[0].ulWatchdog; break;
    case NX10_PWM_PRM_CNT1PERIOD:  *pulPrmVal = s_ptPwm->asMpwm_cnt[1].ulPeriod; break;
    case NX10_PWM_PRM_CNT1:        *pulPrmVal = s_ptPwm->asMpwm_cnt[1].ulCnt; break;
    case NX10_PWM_PRM_CNT1SYSTIME: *pulPrmVal = s_ptPwm->asMpwm_cnt[1].ulSystime; break;
    case NX10_PWM_PRM_CNT1WDG:     *pulPrmVal = s_ptPwm->asMpwm_cnt[1].ulWatchdog; break;

    /* T0..T8 */
    case NX10_PWM_PRM_T0: *pulPrmVal = s_ptPwm->aulMpwm_t[0]; break;
    case NX10_PWM_PRM_T1: *pulPrmVal = s_ptPwm->aulMpwm_t[1];  break;
    case NX10_PWM_PRM_T2: *pulPrmVal = s_ptPwm->aulMpwm_t[2];  break;
    case NX10_PWM_PRM_T3: *pulPrmVal = s_ptPwm->aulMpwm_t[3];  break;
    case NX10_PWM_PRM_T4: *pulPrmVal = s_ptPwm->aulMpwm_t[4];  break;
    case NX10_PWM_PRM_T5: *pulPrmVal = s_ptPwm->aulMpwm_t[5];  break;
    case NX10_PWM_PRM_T6: *pulPrmVal = s_ptPwm->aulMpwm_t[6];  break;
    case NX10_PWM_PRM_T7: *pulPrmVal = s_ptPwm->aulMpwm_t[7];  break;

  /* T0_Shadow..T8_Shadow */
    case NX10_PWM_PRM_T0_SHADOW: *pulPrmVal = s_ptPwm->aulMpwm_t_shadow[0]; break;
    case NX10_PWM_PRM_T1_SHADOW: *pulPrmVal = s_ptPwm->aulMpwm_t_shadow[1];  break;
    case NX10_PWM_PRM_T2_SHADOW: *pulPrmVal = s_ptPwm->aulMpwm_t_shadow[2];  break;
    case NX10_PWM_PRM_T3_SHADOW: *pulPrmVal = s_ptPwm->aulMpwm_t_shadow[3];  break;
    case NX10_PWM_PRM_T4_SHADOW: *pulPrmVal = s_ptPwm->aulMpwm_t_shadow[4];  break;
    case NX10_PWM_PRM_T5_SHADOW: *pulPrmVal = s_ptPwm->aulMpwm_t_shadow[5];  break;
    case NX10_PWM_PRM_T6_SHADOW: *pulPrmVal = s_ptPwm->aulMpwm_t_shadow[6];  break;
    case NX10_PWM_PRM_T7_SHADOW: *pulPrmVal = s_ptPwm->aulMpwm_t_shadow[7];  break;

  /* Default */
    default:
      return NX10_MOTION_ERR_INVAL_PARAM;
    break;
  } /* end switch */

  return NX10_MOTION_OKAY;
}

/*
.########.##....##..######...#######..########..########.########.
.##.......###...##.##....##.##.....##.##.....##.##.......##.....##
.##.......####..##.##.......##.....##.##.....##.##.......##.....##
.######...##.##.##.##.......##.....##.##.....##.######...########.
.##.......##..####.##.......##.....##.##.....##.##.......##...##..
.##.......##...###.##....##.##.....##.##.....##.##.......##....##.
.########.##....##..######...#######..########..########.##.....##
*/


/*****************************************************************************/
/*! ENC Start
* \description
*   This function starts the encoder and MP channels. 
*   All channels can be started individually.
* \class 
*   ENC
* \params
*   fEnc0En     [in] 1: Start encoder channel 0
*   fEnc1En     [in] 1: Start encoder channel 1
*   fMp0En      [in] 1: Start MP channel 0
*   fMp1En      [in] 1: Start MP channel 1
* \return
*   NX10_MOTION_OKAY                                                              */
/*****************************************************************************/
NX10_MOTION_RESULT NX10_Enc_Start( int fEnc0En, int fEnc1En, int fMp0En, int fMp1En )
{
  unsigned int ulVal;
  
  /* get current config */
  ulVal = s_ptEnc->ulMenc_config;
  
  /* include EncEn if enabled */
  if( fEnc0En ) {
    ulVal |= MSK_NX10_menc_config_enc0_en;
  }  
  if( fEnc1En ) {
    ulVal |= MSK_NX10_menc_config_enc1_en;
  }  
  /* include MpEn if enabled */
  if( fMp0En ) {
    ulVal |= MSK_NX10_menc_config_mp0_en;
  }  
  if( fMp1En ) {
    ulVal |= MSK_NX10_menc_config_mp1_en;
  }  
  /* write config back */
  s_ptEnc->ulMenc_config = ulVal;
  
  return NX10_MOTION_OKAY;  
}

/*****************************************************************************/
/*! ENC Stop
* \description
*   This function stops the encoder and MP channels. 
*   All channels can be stopped individually.
* \class 
*   ENC
* \params
*   fEnc0Dis     [in] 1: Stop encoder channel 0
*   fEnc1Dis     [in] 1: Stop encoder channel 1
*   fMp0En       [in] 1: Stop MP channel 0
*   fMp1En       [in] 1: Stop MP channel 1
* \return
*   NX10_MOTION_OKAY                                                              */
/*****************************************************************************/
NX10_MOTION_RESULT NX10_Enc_Stop( int fEnc0Dis, int fEnc1Dis, int fMp0En, int fMp1En )
{
  unsigned int ulVal;
  
  /* get current config */
  ulVal = s_ptEnc->ulMenc_config;
  
  /* include EncEn if enabled */
  if( fEnc0Dis ) {
    ulVal &= ~MSK_NX10_menc_config_enc0_en;
  }  
  if( fEnc1Dis ) {
    ulVal &= ~MSK_NX10_menc_config_enc1_en;
  }  
  /* include MpEn if enabled */
  if( fMp0En ) {
    ulVal &= ~MSK_NX10_menc_config_mp0_en;
  }  
  if( fMp1En ) {
    ulVal &= ~MSK_NX10_menc_config_mp1_en;
  }  
  /* write config back */
  s_ptEnc->ulMenc_config = ulVal;
  
  return NX10_MOTION_OKAY;  
}

/*****************************************************************************/
/*! ENC Configure encoder channel
* \description
*   This function configures a encoder channel. 
* \class 
*   ENC
* \params
*   uEncCh        [in] Encoder channel
*   ptEncChCfg    [in] Pointer to encoder configuration
* \return
*   NX10_MOTION_OKAY
*   NX10_MOTION_ERR_INVAL_PARAM                                                   */
/*****************************************************************************/
NX10_MOTION_RESULT NX10_Enc_ConfigEncCh( unsigned int uEncCh, NX10_MENC_ENCCHCFG* ptEncChCfg )
{
  switch( uEncCh ) {
    case 0:  
      // set config
      s_ptEnc->ulMenc_config      &= ~(MSK_NX10_menc_config_enc0_filter_sample_rate | MSK_NX10_menc_config_enc0_count_dir);
      s_ptEnc->ulMenc_config      |=  (ptEncChCfg->eFilterSampleRate<<SRT_NX10_menc_config_enc0_filter_sample_rate)
                                    |(ptEncChCfg->fCountDir<<SRT_NX10_menc_config_enc0_count_dir);

      // disable all encoder channel interrupts
      s_ptEnc->ulMenc_irq_msk_reset = MSK_NX10_menc_irq_msk_reset_enc0_n
                                     |MSK_NX10_menc_irq_msk_reset_enc0_phase_error
                                     |MSK_NX10_menc_irq_msk_reset_enc0_edge 
                                     |MSK_NX10_menc_irq_msk_reset_enc0_ovfl_pos 
                                     |MSK_NX10_menc_irq_msk_reset_enc0_ovfl_neg;

      // enable encoder channel interrupts
      s_ptEnc->ulMenc_irq_msk_set =  (ptEncChCfg->fIrqEnEdge     << SRT_NX10_menc_irq_msk_set_enc0_edge)
                                    |(ptEncChCfg->fIrqEnPhaseErr << SRT_NX10_menc_irq_msk_set_enc0_phase_error)
                                    |(ptEncChCfg->fIrqEnOvflPos  << SRT_NX10_menc_irq_msk_set_enc0_ovfl_pos)
                                    |(ptEncChCfg->fIrqEnOvflNeg  << SRT_NX10_menc_irq_msk_set_enc0_ovfl_neg)
                                    |(ptEncChCfg->fIrqEnN        << SRT_NX10_menc_irq_msk_set_enc0_n);

      // init position
      s_ptEnc->aulMenc_enc_position[0] = ptEncChCfg->ulInitPos;
    
    break;
  
    case 1:
      // set config
      s_ptEnc->ulMenc_config      &= ~( MSK_NX10_menc_config_enc1_filter_sample_rate | MSK_NX10_menc_config_enc1_count_dir);
      s_ptEnc->ulMenc_config      |=  (ptEncChCfg->eFilterSampleRate<<SRT_NX10_menc_config_enc1_filter_sample_rate)
                                     |( ptEncChCfg->fCountDir<<SRT_NX10_menc_config_enc1_count_dir);

      // disable all encoder channel interrupts
      s_ptEnc->ulMenc_irq_msk_reset = MSK_NX10_menc_irq_msk_reset_enc1_n
                                     |MSK_NX10_menc_irq_msk_reset_enc1_phase_error
                                     |MSK_NX10_menc_irq_msk_reset_enc1_edge 
                                     |MSK_NX10_menc_irq_msk_reset_enc1_ovfl_pos 
                                     |MSK_NX10_menc_irq_msk_reset_enc1_ovfl_neg;

      // enable encoder channel interrupts
      s_ptEnc->ulMenc_irq_msk_set =  (ptEncChCfg->fIrqEnEdge     << SRT_NX10_menc_irq_msk_set_enc1_edge)
                                    |(ptEncChCfg->fIrqEnPhaseErr << SRT_NX10_menc_irq_msk_set_enc1_phase_error)
                                    |(ptEncChCfg->fIrqEnOvflPos  << SRT_NX10_menc_irq_msk_set_enc1_ovfl_pos)
                                    |(ptEncChCfg->fIrqEnOvflNeg  << SRT_NX10_menc_irq_msk_set_enc1_ovfl_neg)
                                    |(ptEncChCfg->fIrqEnN        << SRT_NX10_menc_irq_msk_set_enc1_n);

      // init position
      s_ptEnc->aulMenc_enc_position[1] = ptEncChCfg->ulInitPos;
    
    break;
    
    default:
      // invalid channel number
      return NX10_MOTION_ERR_INVAL_PARAM;
    break;    
  } // end switch  

  return NX10_MOTION_OKAY;  
}

/*****************************************************************************/
/*! ENC Configure MP channel
* \description
*   This function configures a MP channel. 
* \class 
*   ENC
* \params
*   uMpCh        [in] Encoder channel
*   ptMpChCfg    [in] Pointer to MP configuration
* \return
*   NX10_MOTION_OKAY
*   NX10_MOTION_ERR_INVAL_PARAM                                                   */
/*****************************************************************************/
NX10_MOTION_RESULT NX10_Enc_ConfigMpCh( unsigned int uMpCh, NX10_MENC_MPCHCFG* ptMpChCfg )
{
  switch( uMpCh ) {
    
    case 0:  
      // set config
      s_ptEnc->ulMenc_config      &= ~MSK_NX10_menc_config_mp0_filter_sample_rate;
      s_ptEnc->ulMenc_config      |=  ptMpChCfg->eFilterSampleRate<<SRT_NX10_menc_config_mp0_filter_sample_rate;

      // disable MP channel interrupt
      s_ptEnc->ulMenc_irq_msk_reset = MSK_NX10_menc_irq_msk_reset_mp0;

      // enable MP channel interrupt
      s_ptEnc->ulMenc_irq_msk_set =  (ptMpChCfg->fIrqEnMP << SRT_NX10_menc_irq_msk_set_mp0);

    break;
  
    case 1:
      // set config
      s_ptEnc->ulMenc_config      &= ~MSK_NX10_menc_config_mp1_filter_sample_rate;
      s_ptEnc->ulMenc_config      |=  ptMpChCfg->eFilterSampleRate<<SRT_NX10_menc_config_mp1_filter_sample_rate;

      // disable MP channel interrupt
      s_ptEnc->ulMenc_irq_msk_reset = MSK_NX10_menc_irq_msk_reset_mp1;

      // enable MP channel interrupt
      s_ptEnc->ulMenc_irq_msk_set =  (ptMpChCfg->fIrqEnMP << SRT_NX10_menc_irq_msk_set_mp1);

    break;
    
    default:
      // invalid channel number
      return NX10_MOTION_ERR_INVAL_PARAM;
    break;    
  } // end switch  

  return NX10_MOTION_OKAY;  
}

/*****************************************************************************/
/*! ENC Configure capture channel
* \description
*   This function configures a capture channel. 
* \class 
*   ENC
* \params
*   uCapCh        [in] Capture channel
*   ptCapChCfg    [in] Pointer to capture configuration
* \return
*   NX10_MOTION_OKAY
*   NX10_MOTION_ERR_INVAL_PARAM                                                   */
/*****************************************************************************/
NX10_MOTION_RESULT NX10_Enc_ConfigCapCh( unsigned int uCapCh, NX10_MENC_CAPCHCFG* ptCapChCfg )
{
  switch( uCapCh ) {
    
    case 0:  
      // set config
      s_ptEnc->asMenc_capture[0].ulConfig = (ptCapChCfg->eCapSrc     << SRT_NX10_menc_capture0_config_src)
                                           |(ptCapChCfg->eCapSel     << SRT_NX10_menc_capture0_config_sel)
                                           |(ptCapChCfg->fCapInv     << SRT_NX10_menc_capture0_config_inv)
                                           |(ptCapChCfg->fSrcNum     << SRT_NX10_menc_capture0_config_src_nr)
                                           |(ptCapChCfg->fCapOnce    << SRT_NX10_menc_capture0_config_conce);

      // disable capture channel interrupt
      s_ptEnc->ulMenc_irq_msk_reset = MSK_NX10_menc_irq_msk_reset_cap0;

      // enable capture channel interrupt
      s_ptEnc->ulMenc_irq_msk_set =  (ptCapChCfg->fIrqEnCap << SRT_NX10_menc_irq_msk_set_cap0);

    break;
    
    case 1:  
      // set config
      s_ptEnc->asMenc_capture[1].ulConfig = (ptCapChCfg->eCapSrc  << SRT_NX10_menc_capture1_config_src)
                                           |(ptCapChCfg->eCapSel  << SRT_NX10_menc_capture1_config_sel)
                                           |(ptCapChCfg->fCapInv  << SRT_NX10_menc_capture1_config_inv)
                                           |(ptCapChCfg->fSrcNum  << SRT_NX10_menc_capture1_config_src_nr)
                                           |(ptCapChCfg->fCapOnce << SRT_NX10_menc_capture1_config_conce);

      // disable capture channel interrupt
      s_ptEnc->ulMenc_irq_msk_reset = MSK_NX10_menc_irq_msk_reset_cap1;

      // enable capture channel interrupt
      s_ptEnc->ulMenc_irq_msk_set =  (ptCapChCfg->fIrqEnCap << SRT_NX10_menc_irq_msk_set_cap1);

    break;
   
    case 2:  
      // set config
      s_ptEnc->asMenc_capture[2].ulConfig = (ptCapChCfg->eCapSrc  << SRT_NX10_menc_capture2_config_src)
                                           |(ptCapChCfg->eCapSel  << SRT_NX10_menc_capture2_config_sel)
                                           |(ptCapChCfg->fCapInv  << SRT_NX10_menc_capture2_config_inv)
                                           |(ptCapChCfg->fSrcNum  << SRT_NX10_menc_capture2_config_src_nr)
                                           |(ptCapChCfg->fCapOnce << SRT_NX10_menc_capture2_config_conce);

      // disable capture channel interrupt
      s_ptEnc->ulMenc_irq_msk_reset = MSK_NX10_menc_irq_msk_reset_cap2;

      // enable capture channel interrupt
      s_ptEnc->ulMenc_irq_msk_set =  (ptCapChCfg->fIrqEnCap << SRT_NX10_menc_irq_msk_set_cap2);

    break;

    case 3:  
      // set config
      s_ptEnc->asMenc_capture[3].ulConfig = (ptCapChCfg->eCapSrc  << SRT_NX10_menc_capture3_config_src)
                                           |(ptCapChCfg->eCapSel  << SRT_NX10_menc_capture3_config_sel)
                                           |(ptCapChCfg->fCapInv  << SRT_NX10_menc_capture3_config_inv)
                                           |(ptCapChCfg->fSrcNum  << SRT_NX10_menc_capture3_config_src_nr)
                                           |(ptCapChCfg->fCapOnce << SRT_NX10_menc_capture3_config_conce);

      // disable capture channel interrupt
      s_ptEnc->ulMenc_irq_msk_reset = MSK_NX10_menc_irq_msk_reset_cap3;

      // enable capture channel interrupt
      s_ptEnc->ulMenc_irq_msk_set =  (ptCapChCfg->fIrqEnCap << SRT_NX10_menc_irq_msk_set_cap3);

    break;
    
    default:
      // invalid channel number
      return NX10_MOTION_ERR_INVAL_PARAM;
    break;    
  } // end switch  

  return NX10_MOTION_OKAY;  
}

/*****************************************************************************/
/*! ENC Reset encoder channel
* \description
*   This function resets a encoder channel to default values. 
*   Before reset the encoder channel must be stopped.
* \class 
*   ENC
* \params
*   unsigned int uEncCh    [in] Encoder channel
* \return
*   NX10_MOTION_OKAY
*   NX10_MOTION_ERR_INVAL_PARAM                                                   */
/*****************************************************************************/
NX10_MOTION_RESULT NX10_Enc_ResetEncCh( unsigned int uEncCh )
{

  switch( uEncCh ) {
    
    case 0:  
      // disable interrupts
      s_ptEnc->ulMenc_irq_msk_reset = MSK_NX10_menc_irq_msk_reset_enc0_n
                                     |MSK_NX10_menc_irq_msk_reset_enc0_edge 
                                     |MSK_NX10_menc_irq_msk_reset_enc0_ovfl_pos 
                                     |MSK_NX10_menc_irq_msk_reset_enc0_ovfl_neg 
                                     |MSK_NX10_menc_irq_msk_reset_enc0_phase_error;

      // reset config to default values
      s_ptEnc->ulMenc_config        = s_ptEnc->ulMenc_config & ~( MSK_NX10_menc_config_enc0_filter_sample_rate
                                                                 |MSK_NX10_menc_config_enc0_count_dir
                                                                );
      // confirm and reset all stati
      s_ptEnc->ulMenc_status        = MSK_NX10_menc_status_enc0_n
                                     |MSK_NX10_menc_status_enc0_edge 
                                     |MSK_NX10_menc_status_enc0_ovfl_pos 
                                     |MSK_NX10_menc_status_enc0_ovfl_neg 
                                     |MSK_NX10_menc_status_enc0_phase_error;
  
      // clear position
      s_ptEnc->aulMenc_enc_position[0] = 0;
    break;
  
    case 1:
      // disable interrupts
      s_ptEnc->ulMenc_irq_msk_reset = MSK_NX10_menc_irq_msk_reset_enc1_n
                                     |MSK_NX10_menc_irq_msk_reset_enc1_edge 
                                     |MSK_NX10_menc_irq_msk_reset_enc1_ovfl_pos 
                                     |MSK_NX10_menc_irq_msk_reset_enc1_ovfl_neg 
                                     |MSK_NX10_menc_irq_msk_reset_enc1_phase_error;
    
      // reset config to default values
      s_ptEnc->ulMenc_config        = s_ptEnc->ulMenc_config & ~( MSK_NX10_menc_config_enc1_filter_sample_rate
                                                                 |MSK_NX10_menc_config_enc1_count_dir
                                                                );
      // clear all stati
      s_ptEnc->ulMenc_status        = MSK_NX10_menc_status_enc1_n
                                     |MSK_NX10_menc_status_enc1_edge 
                                     |MSK_NX10_menc_status_enc1_ovfl_pos 
                                     |MSK_NX10_menc_status_enc1_ovfl_neg 
                                     |MSK_NX10_menc_status_enc1_phase_error;
    
      // clear position
      s_ptEnc->aulMenc_enc_position[1] = 0;
    break;
    
    default:
      // invalid channel number
      return NX10_MOTION_ERR_INVAL_PARAM;
    break;    
  } // end switch  

  return NX10_MOTION_OKAY;  
}

/*****************************************************************************/
/*! ENC Reset MP channel
* \description
*   This function resets a MP channel to default values. 
*   Before reset the MP channel must be stopped.
* \class 
*   ENC
* \params
*   unsigned int uMpCh    [in] MP channel
* \return
*   NX10_MOTION_OKAY
*   NX10_MOTION_ERR_INVAL_PARAM                                                   */
/*****************************************************************************/
NX10_MOTION_RESULT NX10_Enc_ResetMpCh( unsigned int uMpCh )
{

  switch( uMpCh ) {
    
    case 0:  
      // set config
      s_ptEnc->ulMenc_config      &= ~MSK_NX10_menc_config_mp0_filter_sample_rate;

      // disable MP channel interrupt
      s_ptEnc->ulMenc_irq_msk_reset = MSK_NX10_menc_irq_msk_reset_mp0;

    break;
  
    case 1:
      // set config
      s_ptEnc->ulMenc_config      &= ~MSK_NX10_menc_config_mp1_filter_sample_rate;

      // disable MP channel interrupt
      s_ptEnc->ulMenc_irq_msk_reset = MSK_NX10_menc_irq_msk_reset_mp1;
    break;
    
    default:
      // invalid channel number
      return NX10_MOTION_ERR_INVAL_PARAM;
    break;    
  } // end switch  

  return NX10_MOTION_OKAY;  
}


/*****************************************************************************/
/*! ENC Reset capture channel
* \description
*   This function resets a capture channel. 
* \class 
*   ENC
* \params
*   unsigned int uCapCh  [in] Capture channel
* \return
*   NX10_MOTION_OKAY
*   NX10_MOTION_ERR_INVAL_PARAM                                                   */
/*****************************************************************************/
NX10_MOTION_RESULT NX10_Enc_ResetCapCh( unsigned int uCapCh )
{
  switch( uCapCh ) {
    case 0:  
      s_ptEnc->asMenc_capture[0].ulConfig = 0; // reset config to default values
      s_ptEnc->ulMenc_irq_msk_reset       = MSK_NX10_menc_irq_msk_reset_cap0; // disable interrupts
      s_ptEnc->ulMenc_status              = MSK_NX10_menc_status_cap0; // confirm and reset all stati
    break;
  
    case 1:  
      s_ptEnc->asMenc_capture[1].ulConfig = 0; // reset config to default values
      s_ptEnc->ulMenc_irq_msk_reset       = MSK_NX10_menc_irq_msk_reset_cap1; // disable interrupts
      s_ptEnc->ulMenc_status              = MSK_NX10_menc_status_cap1; // confirm and reset all stati
    break;

    case 2:  
      s_ptEnc->asMenc_capture[2].ulConfig = 0; // reset config to default values
      s_ptEnc->ulMenc_irq_msk_reset       = MSK_NX10_menc_irq_msk_reset_cap2; // disable interrupts
      s_ptEnc->ulMenc_status              = MSK_NX10_menc_status_cap2; // confirm and reset all stati
    break;

    case 3:  
      s_ptEnc->asMenc_capture[3].ulConfig = 0; // reset config to default values
      s_ptEnc->ulMenc_irq_msk_reset       = MSK_NX10_menc_irq_msk_reset_cap3; // disable interrupts
      s_ptEnc->ulMenc_status              = MSK_NX10_menc_status_cap3; // confirm and reset all stati
    break;
    
    default:
      // invalid channel number
      return NX10_MOTION_ERR_INVAL_PARAM;
    break;    
  } // end switch  

  return NX10_MOTION_OKAY;  
}

/*****************************************************************************/
/*! ENC Get Stati
* \description
*   This function retrieves the current stati of the Encoder.
* \class 
*   ENC
* \params
   pulStat     [out] Pointer to stati
* \return
*   NX10_MOTION_OKAY                                                              */
/*****************************************************************************/
NX10_MOTION_RESULT NX10_Enc_GetStat( unsigned long *pulStat )
{
  /* get stati */
  *pulStat = s_ptEnc->ulMenc_status;
  
  return NX10_MOTION_OKAY;
}

/*****************************************************************************/
/*! ENC Confirm interrupt(s)
* \description
*   This function confirms a set of stati and interrupts that were requested by the Encoder.
* \class 
*   ENC
* \params
*   ulConfirmIrqMask  [in] Mask to confirm interrupt events
* \return
*   NX10_MOTION_OKAY                                                              */
/*****************************************************************************/
NX10_MOTION_RESULT NX10_Enc_ConfirmStatIrq( unsigned long ulConfirmStatIrqMask )
{
  /* confirm the set of irqs */
  s_ptEnc->ulMenc_status = ulConfirmStatIrqMask;
  
  return NX10_MOTION_OKAY;
}

/*****************************************************************************/
/*! ENC Get Interrupt(s)
* \description
*   This function retrieves the current interrupt masked requests of the Encoder.
* \class 
*   ENC
* \params
   pulIrq     [out] Pointer to interrupt events
* \return
*   NX10_MOTION_OKAY                                                              */
/*****************************************************************************/
NX10_MOTION_RESULT NX10_Enc_GetIrq( unsigned long *pulIrq )
{
  /* get irq requests */
  *pulIrq = s_ptEnc->ulMenc_irq_masked;
  
  return NX10_MOTION_OKAY;
}

/*****************************************************************************/
/*! ENC Software capture now request
* \description
*   This function requests capturing by SW.
*   For all channels can be captured individually or together.
* \class 
*   ENC
* \params
*   uCaptureNum  [in] Capture channel
* \return
*   NX10_MOTION_OKAY                                                              */
/*****************************************************************************/
NX10_MOTION_RESULT NX10_Enc_CaptureNowReq( int fCh0En, int fCh1En, int fCh2En, int fCh3En )
{
  unsigned int ulVal;
  
  ulVal=0;
  
  /* include fChEn if enabled */
  if( fCh0En ) ulVal |= MSK_NX10_menc_capture_now_cap0_now;
  if( fCh1En ) ulVal |= MSK_NX10_menc_capture_now_cap1_now;
  if( fCh2En ) ulVal |= MSK_NX10_menc_capture_now_cap2_now;
  if( fCh3En ) ulVal |= MSK_NX10_menc_capture_now_cap3_now;

  /* set capture now request */
  s_ptEnc->ulMenc_capture_now = ulVal;
  return NX10_MOTION_OKAY;
}

/*****************************************************************************/
/*! ENC Set Parameter
* \description
*   This function writes parameter.
* \class 
*   ENC
* \params
*   ulPrmID          [in]  Parameter ID
*   ulPrmVal         [in]  Parameter value
* \return
*   NX10_MOTION_OKAY                                                             
*   NX10_MOTION_ERR_INVAL_PARAM                                                   */
/*****************************************************************************/
NX10_MOTION_RESULT NX10_Enc_SetPrm( unsigned long ulPrmID, unsigned long ulPrmVal )
{
  switch( ulPrmID )
  { 

  /* Config */
    case NX10_ENC_PRM_ENC0_FILT_SAMPLE_RATE:
      s_ptEnc->ulMenc_config &= ~MSK_NX10_menc_config_enc0_filter_sample_rate;
      s_ptEnc->ulMenc_config |= (ulPrmVal<<SRT_NX10_menc_config_enc0_filter_sample_rate);
    break;
    case NX10_ENC_PRM_ENC0_CNT_DIR:
      s_ptEnc->ulMenc_config &= ~MSK_NX10_menc_config_enc0_count_dir;
      s_ptEnc->ulMenc_config |= (ulPrmVal<<SRT_NX10_menc_config_enc0_count_dir);
    break;
    case NX10_ENC_PRM_ENC1_FILT_SAMPLE_RATE:
      s_ptEnc->ulMenc_config &= ~MSK_NX10_menc_config_enc1_filter_sample_rate;
      s_ptEnc->ulMenc_config |= (ulPrmVal<<SRT_NX10_menc_config_enc1_filter_sample_rate);
    break;
    case NX10_ENC_PRM_ENC1_CNT_DIR:
      s_ptEnc->ulMenc_config &= ~MSK_NX10_menc_config_enc1_count_dir;
      s_ptEnc->ulMenc_config |= (ulPrmVal<<SRT_NX10_menc_config_enc1_count_dir);
    break;
    case NX10_ENC_PRM_MP0_FILT_SAMPLE_RATE:
      s_ptEnc->ulMenc_config &= ~MSK_NX10_menc_config_mp0_filter_sample_rate;
      s_ptEnc->ulMenc_config |= (ulPrmVal<<SRT_NX10_menc_config_mp0_filter_sample_rate);
    break;
    case NX10_ENC_PRM_MP1_FILT_SAMPLE_RATE:
      s_ptEnc->ulMenc_config &= ~MSK_NX10_menc_config_mp1_filter_sample_rate;
      s_ptEnc->ulMenc_config |= (ulPrmVal<<SRT_NX10_menc_config_mp1_filter_sample_rate);
    break;

    /* Position */
    case NX10_ENC_PRM_ENC0_POS: s_ptEnc->aulMenc_enc_position[0] = ulPrmVal; break;
    case NX10_ENC_PRM_ENC1_POS: s_ptEnc->aulMenc_enc_position[1] = ulPrmVal; break;

    /* Capture_config */
    case NX10_ENC_PRM_CAPTURE0_CFG: s_ptEnc->asMenc_capture[0].ulConfig = ulPrmVal; break;
    case NX10_ENC_PRM_CAPTURE1_CFG: s_ptEnc->asMenc_capture[1].ulConfig = ulPrmVal; break;
    case NX10_ENC_PRM_CAPTURE2_CFG: s_ptEnc->asMenc_capture[2].ulConfig = ulPrmVal; break;
    case NX10_ENC_PRM_CAPTURE3_CFG: s_ptEnc->asMenc_capture[3].ulConfig = ulPrmVal; break;

    /* Interrupt */
    case NX10_ENC_PRM_IRQMSKSET:    s_ptEnc->ulMenc_irq_msk_set = ulPrmVal; break;
    case NX10_ENC_PRM_IRQMSKRST:    s_ptEnc->ulMenc_irq_msk_reset = ulPrmVal; break;

    /* Default */
    default:
      return NX10_MOTION_ERR_INVAL_PARAM;
    break;
  } /* end switch */

  return NX10_MOTION_OKAY;
}


/*****************************************************************************/
/*! ENC Get Parameter
* \description
*   This function reads parameter.
* \class 
*   ENC
* \params
*   ulPrmID          [in]  Parameter ID
*   pulPrmVal        [in]  Pointer to parameter value
* \return
*   NX10_MOTION_OKAY                                                             
*   NX10_MOTION_ERR_INVAL_PARAM                                                   */
/*****************************************************************************/
NX10_MOTION_RESULT NX10_Enc_GetPrm( unsigned long ulPrmID, unsigned long* pulPrmVal )
{
  switch( ulPrmID )
  { 
    /* Config */
    case NX10_ENC_PRM_ENC0_EN:               *pulPrmVal = (s_ptEnc->ulMenc_config & MSK_NX10_menc_config_enc0_en)>>SRT_NX10_menc_config_enc0_en; break;
    case NX10_ENC_PRM_ENC0_FILT_SAMPLE_RATE: *pulPrmVal = (s_ptEnc->ulMenc_config & MSK_NX10_menc_config_enc0_filter_sample_rate)>>SRT_NX10_menc_config_enc0_filter_sample_rate; break;
    case NX10_ENC_PRM_ENC0_CNT_DIR:          *pulPrmVal = (s_ptEnc->ulMenc_config & MSK_NX10_menc_config_enc0_count_dir)>>SRT_NX10_menc_config_enc0_count_dir; break;
    case NX10_ENC_PRM_ENC1_EN:               *pulPrmVal = (s_ptEnc->ulMenc_config & MSK_NX10_menc_config_enc1_en)>>SRT_NX10_menc_config_enc1_en; break;
    case NX10_ENC_PRM_ENC1_FILT_SAMPLE_RATE: *pulPrmVal = (s_ptEnc->ulMenc_config & MSK_NX10_menc_config_enc1_filter_sample_rate)>>SRT_NX10_menc_config_enc1_filter_sample_rate; break;
    case NX10_ENC_PRM_ENC1_CNT_DIR:          *pulPrmVal = (s_ptEnc->ulMenc_config & MSK_NX10_menc_config_enc1_count_dir)>>SRT_NX10_menc_config_enc1_count_dir; break;
    case NX10_ENC_PRM_MP0_EN:                *pulPrmVal = (s_ptEnc->ulMenc_config & MSK_NX10_menc_config_mp0_en)>>SRT_NX10_menc_config_mp0_en; break;
    case NX10_ENC_PRM_MP0_FILT_SAMPLE_RATE:  *pulPrmVal = (s_ptEnc->ulMenc_config & MSK_NX10_menc_config_mp0_filter_sample_rate)>>SRT_NX10_menc_config_mp0_filter_sample_rate; break;
    case NX10_ENC_PRM_MP1_EN:                *pulPrmVal = (s_ptEnc->ulMenc_config & MSK_NX10_menc_config_mp1_en)>>SRT_NX10_menc_config_mp1_en; break;
    case NX10_ENC_PRM_MP1_FILT_SAMPLE_RATE:  *pulPrmVal = (s_ptEnc->ulMenc_config & MSK_NX10_menc_config_mp1_filter_sample_rate)>>SRT_NX10_menc_config_mp1_filter_sample_rate; break;

    /* Position */
    case NX10_ENC_PRM_ENC0_POS: *pulPrmVal = s_ptEnc->aulMenc_enc_position[0]; break;
    case NX10_ENC_PRM_ENC1_POS: *pulPrmVal = s_ptEnc->aulMenc_enc_position[1]; break;

    /* Capture */

    case NX10_ENC_PRM_CAPTURE0_CFG: *pulPrmVal = s_ptEnc->asMenc_capture[0].ulConfig; break;
    case NX10_ENC_PRM_CAPTURE0_VAL: *pulPrmVal = s_ptEnc->asMenc_capture[0].ulVal;    break;
    case NX10_ENC_PRM_CAPTURE0_TA:  *pulPrmVal = s_ptEnc->asMenc_capture[0].ulTa;     break;
    case NX10_ENC_PRM_CAPTURE0_TE:  *pulPrmVal = s_ptEnc->asMenc_capture[0].ulTe;     break;

    case NX10_ENC_PRM_CAPTURE1_CFG: *pulPrmVal = s_ptEnc->asMenc_capture[1].ulConfig; break;
    case NX10_ENC_PRM_CAPTURE1_VAL: *pulPrmVal = s_ptEnc->asMenc_capture[1].ulVal;    break;
    case NX10_ENC_PRM_CAPTURE1_TA:  *pulPrmVal = s_ptEnc->asMenc_capture[1].ulTa;     break;
    case NX10_ENC_PRM_CAPTURE1_TE:  *pulPrmVal = s_ptEnc->asMenc_capture[1].ulTe;     break;

    case NX10_ENC_PRM_CAPTURE2_CFG: *pulPrmVal = s_ptEnc->asMenc_capture[2].ulConfig; break;
    case NX10_ENC_PRM_CAPTURE2_VAL: *pulPrmVal = s_ptEnc->asMenc_capture[2].ulVal;    break;
    case NX10_ENC_PRM_CAPTURE2_TA:  *pulPrmVal = s_ptEnc->asMenc_capture[2].ulTa;     break;
    case NX10_ENC_PRM_CAPTURE2_TE:  *pulPrmVal = s_ptEnc->asMenc_capture[2].ulTe;     break;
    
    case NX10_ENC_PRM_CAPTURE3_CFG: *pulPrmVal = s_ptEnc->asMenc_capture[3].ulConfig; break;
    case NX10_ENC_PRM_CAPTURE3_VAL: *pulPrmVal = s_ptEnc->asMenc_capture[3].ulVal;    break;
    case NX10_ENC_PRM_CAPTURE3_TA:  *pulPrmVal = s_ptEnc->asMenc_capture[3].ulTa;     break;
    case NX10_ENC_PRM_CAPTURE3_TE:  *pulPrmVal = s_ptEnc->asMenc_capture[3].ulTe;     break;

    /* Interrupt */
    case NX10_ENC_PRM_IRQMASKED: *pulPrmVal = s_ptEnc->ulMenc_irq_masked; break;
    case NX10_ENC_PRM_IRQMSKSET: *pulPrmVal = s_ptEnc->ulMenc_irq_msk_set; break;
    case NX10_ENC_PRM_IRQMSKRST: *pulPrmVal = s_ptEnc->ulMenc_irq_msk_reset; break;
  
    /* Default */
    default:
      return NX10_MOTION_ERR_INVAL_PARAM;
    break;
  } /* end switch */

  return NX10_MOTION_OKAY;
}





/*
....###....########...######.
...##.##...##.....##.##....##
..##...##..##.....##.##......
.##.....##.##.....##.##......
.#########.##.....##.##......
.##.....##.##.....##.##....##
.##.....##.########...######.
*/

/*****************************************************************************/
/*! ADC Set Operational/PowerDown
* \description
*   This function sets the ADC into operational or power down mode.
* \class 
*   ADC
* \params
*  fOpMode   [in] 1/0: operational/PowerDown
* \return
*   NX10_MOTION_OKAY                                                              */
/*****************************************************************************/
NX10_MOTION_RESULT NX10_Adc_SetOpmode( unsigned int uAdcNum, int fOpMode )
{
  unsigned int uVal=0;
  
  switch( uAdcNum )
  {
    /* ADC0 */
    case 0:
      if( fOpMode ) {
        // PowerUp
        s_ptAdc->aulAdc_ctrl_mansample_config[0] &= ~MSK_NX10_adc_ctrl_mansample_config0_conv;
        s_ptAdc->aulAdc_ctrl_mansample_config[0] |= MSK_NX10_adc_ctrl_mansample_config0_pdb;
        // delay
        do{ uVal++; } while( uVal<NX10_ADC_POWERUP_DELAY);       
        // Dummy Sample to clear X in simu
        s_ptAdc->aulAdc_ctrl_mansample_config[0] |= MSK_NX10_adc_ctrl_mansample_config0_conv;
        s_ptAdc->aulAdc_ctrl_mansample_config[0] &= ~MSK_NX10_adc_ctrl_mansample_config0_conv;

      } else {
        // PowerDown
        s_ptAdc->aulAdc_ctrl_mansample_config[0] &= ~MSK_NX10_adc_ctrl_mansample_config0_pdb;
      }
    break;

    /* ADC1 */
    case 1:
      if( fOpMode ) {
        // PowerUp
        s_ptAdc->aulAdc_ctrl_mansample_config[1] &= ~MSK_NX10_adc_ctrl_mansample_config1_conv;
        s_ptAdc->aulAdc_ctrl_mansample_config[1] |= MSK_NX10_adc_ctrl_mansample_config1_pdb;
        // delay
        do{ uVal++; } while( uVal<NX10_ADC_POWERUP_DELAY);       
        // Dummy sample to clear X in simu
        s_ptAdc->aulAdc_ctrl_mansample_config[1] |= MSK_NX10_adc_ctrl_mansample_config1_conv;
        s_ptAdc->aulAdc_ctrl_mansample_config[1] &= ~MSK_NX10_adc_ctrl_mansample_config1_conv;
      } else {
        // PowerDown
        s_ptAdc->aulAdc_ctrl_mansample_config[1] &= ~MSK_NX10_adc_ctrl_mansample_config1_pdb;
      }
    break;
  
    /* Default */
    default:
      return NX10_MOTION_ERR_INVAL_PARAM;
    break;
  } /* end switch */
  
  return NX10_MOTION_OKAY;  
}

/*****************************************************************************/
/*! ADC Start
* \description
*   This function starts the ADC units for single or dual request.
*   Both unit can be started individually.
* \class 
*   ADC
* \params
* \return
*   NX10_MOTION_OKAY                                                              */
/*****************************************************************************/
NX10_MOTION_RESULT NX10_Adc_StartAuto( int fStartAdc0, int fStartAdc1, int fStart2ndAdc0, int fStart2ndAdc1 )
{
  s_ptAdc->ulAdc_ctrl_start =  (fStartAdc0<<SRT_NX10_adc_ctrl_start_start_adc0)
                              |(fStartAdc1<<SRT_NX10_adc_ctrl_start_start_adc1);
  
  s_ptAdc->ulAdc_ctrl_start =  (fStart2ndAdc0<<SRT_NX10_adc_ctrl_start_start_adc0)
                              |(fStart2ndAdc1<<SRT_NX10_adc_ctrl_start_start_adc1);
  return NX10_MOTION_OKAY;  
}

/*****************************************************************************/
/*! ADC Config Auto
* \description
*   This function configures an ADC unit for autosample state machine.
* \class 
*   ADC
* \params
* \return
*   NX10_MOTION_OKAY
*   NX10_MOTION_ERR_INVAL_PARAM                                                   */
/*****************************************************************************/
NX10_MOTION_RESULT NX10_Adc_ConfigAuto( unsigned int uAdcNum, unsigned int uChannel, unsigned int uPwrDown, unsigned int uInv, NX10_ADC_AUTOCFG_EVENTSEL eEventSel )
{
  switch( uAdcNum )
  {
  /* ADC0 */
    case 0:
    s_ptAdc->aulAdc_ctrl_autosample_config[0] =  (uChannel<<SRT_NX10_adc_ctrl_autosample_config0_sel)
                                               | (uPwrDown<<SRT_NX10_adc_ctrl_autosample_config0_power_down)
                                               | (uInv<<SRT_NX10_adc_ctrl_autosample_config0_event_inv)
                                               | (eEventSel<<SRT_NX10_adc_ctrl_autosample_config0_event_sel);
    break;

  /* ADC1 */
    case 1:
    s_ptAdc->aulAdc_ctrl_autosample_config[1] =  (uChannel<<SRT_NX10_adc_ctrl_autosample_config1_sel)
                                               | (uPwrDown<<SRT_NX10_adc_ctrl_autosample_config1_power_down)
                                               | (uInv<<SRT_NX10_adc_ctrl_autosample_config1_event_inv)
                                               | (eEventSel<<SRT_NX10_adc_ctrl_autosample_config1_event_sel);
    break;
  
  /* Default */
    default:
      return NX10_MOTION_ERR_INVAL_PARAM;
    break;
  } /* end switch */
  
  return NX10_MOTION_OKAY;  
}


/*****************************************************************************/
/*! ADC Start Manual
* \description
*   This function starts an AD-conversion manually.
* \class 
*   ADC
* \params
*   uAdcNum          [in]  ADC number (0..1)
*   uChannel         [in]  ADC channel (0..7)
* \return
*   NX10_MOTION_OKAY
*   NX10_MOTION_ERR_INVAL_PARAM
*   NX10_MOTION_ERR_UNIT_NOT_READY                                                */
/*****************************************************************************/
NX10_MOTION_RESULT NX10_Adc_StartMan( unsigned int uAdcNum, unsigned int uChannel )
{
  unsigned int uVal=0;
  
  if( uChannel>=NX10_ADC_CHANNELS )
    return NX10_MOTION_ERR_INVAL_PARAM;

  switch( uAdcNum )
  {
    // ADC0
    case 0:
      if( (s_ptAdc->aulAdc_ctrl_mansample_config[0] & MSK_NX10_adc_ctrl_mansample_config0_conv)!=0 )
        return NX10_MOTION_ERR_NOT_READY;
      if( (s_ptAdc->aulAdc_ctrl_mansample_config[0] & MSK_NX10_adc_ctrl_mansample_config0_pdb)==0 )
        return NX10_MOTION_ERR_NOT_READY;

      // set SEL incl. PDB=1 and CONV=0
      s_ptAdc->aulAdc_ctrl_mansample_config[0] = (uChannel<<SRT_NX10_adc_ctrl_mansample_config0_sel) | MSK_NX10_adc_ctrl_mansample_config0_pdb;
      // SEL2CONV delay
      do{ uVal++; } while( uVal<NX10_ADC_SEL2CONV_DELAY);       
      // start ADC 
      s_ptAdc->aulAdc_ctrl_mansample_config[0] = (uChannel<<SRT_NX10_adc_ctrl_mansample_config0_sel) | MSK_NX10_adc_ctrl_mansample_config0_pdb | MSK_NX10_adc_ctrl_mansample_config0_conv;
      s_ptAdc->aulAdc_ctrl_mansample_config[0] = (uChannel<<SRT_NX10_adc_ctrl_mansample_config0_sel) | MSK_NX10_adc_ctrl_mansample_config0_pdb;

    break;

    // ADC1
    case 1:
      if( (s_ptAdc->aulAdc_ctrl_mansample_config[1] & MSK_NX10_adc_ctrl_mansample_config1_conv)!=0 )
        return NX10_MOTION_ERR_NOT_READY;
      if( (s_ptAdc->aulAdc_ctrl_mansample_config[1] & MSK_NX10_adc_ctrl_mansample_config1_pdb)==0 )
        return NX10_MOTION_ERR_NOT_READY;

      // set SEL incl. PDB=1 and CONV=0
      s_ptAdc->aulAdc_ctrl_mansample_config[1] = (uChannel<<SRT_NX10_adc_ctrl_mansample_config1_sel) | MSK_NX10_adc_ctrl_mansample_config1_pdb;
      // SEL2CONV delay
      do{ uVal++; } while( uVal<NX10_ADC_SEL2CONV_DELAY);       
      // start ADC 
      s_ptAdc->aulAdc_ctrl_mansample_config[1] = (uChannel<<SRT_NX10_adc_ctrl_mansample_config1_sel) | MSK_NX10_adc_ctrl_mansample_config1_pdb | MSK_NX10_adc_ctrl_mansample_config1_conv;
      s_ptAdc->aulAdc_ctrl_mansample_config[1] = (uChannel<<SRT_NX10_adc_ctrl_mansample_config1_sel) | MSK_NX10_adc_ctrl_mansample_config1_pdb;
    break;
  
  /* Default */
    default:
      return NX10_MOTION_ERR_INVAL_PARAM;
    break;
  } /* end switch */
  
  return NX10_MOTION_OKAY;  
}

/*****************************************************************************/
/*! ADC Get status
* \description
*   This function reads the status of an ADC unit.
* \class 
*   ADC
*   uAdcNum          [in]   ADC number (0..1)
*   pfFin            [out] Pointer to Fin Flag
*   pfEocb           [out] Pointer to "End Of Conversion" Flag
* \params
* \return
*   NX10_MOTION_OKAY
*   NX10_MOTION_ERR_INVAL_PARAM                                                   */
/*****************************************************************************/
NX10_MOTION_RESULT NX10_Adc_GetStatus( unsigned int uAdcNum, int* pfFin, int* pfEocb )
{
  if( uAdcNum>=NX10_ADC_NUM )
    return NX10_MOTION_ERR_INVAL_PARAM;

  switch( uAdcNum )
  {
  /* ADC0 */
    case 0:
      *pfFin  = (s_ptAdc->ulAdc_ctrl_status & MSK_NX10_adc_ctrl_status_adc0_finish) ? 1 : 0;
      *pfEocb = (s_ptAdc->ulAdc_ctrl_status & MSK_NX10_adc_ctrl_status_adc0_eocb)   ? 1 : 0;
    break;

  /* ADC1 */
    case 1:
      *pfFin  = (s_ptAdc->ulAdc_ctrl_status & MSK_NX10_adc_ctrl_status_adc1_finish) ? 1 : 0;
      *pfEocb = (s_ptAdc->ulAdc_ctrl_status & MSK_NX10_adc_ctrl_status_adc1_eocb)   ? 1 : 0;
    break;
  
  /* Default */
    default:
      return NX10_MOTION_ERR_INVAL_PARAM;
    break;
  } /* end switch */
  
  return NX10_MOTION_OKAY;  
}
/*****************************************************************************/
/*! ADC Get result
* \description
*   This function reads an AD-conversion result.
* \class 
*   ADC
* \params
*   uAdcNum          [in]   ADC number (0..1)
*   pusVal           [out] Pointer to sampled value
* \return
*   NX10_MOTION_OKAY
*   NX10_MOTION_ERR_INVAL_PARAM                                                   */
/*****************************************************************************/
NX10_MOTION_RESULT NX10_Adc_GetAdcVal( unsigned int uAdcNum, unsigned short* pusVal )
{
  if( uAdcNum>=NX10_ADC_NUM )
    return NX10_MOTION_ERR_INVAL_PARAM;
  
  *pusVal = s_ptAdc->aulAdc_ctrl_adc_val[uAdcNum];
  
  return NX10_MOTION_OKAY;  
}

/*****************************************************************************/
/*! ADC Enable Interrupt(s)
* \description
*   This function enables interrupts of the ADC.
* \class 
*   ADC
* \params
   ulIrqMask     [in] Interrupt enable mask
* \return
*   NX10_MOTION_OKAY                                                              */
/*****************************************************************************/
NX10_MOTION_RESULT NX10_Adc_EnableIrq( unsigned long ulIrqMask )
{
  /* enable interrupts */
  s_ptAdc->ulAdc_ctrl_irq_msk_set = ulIrqMask;
  
  return NX10_MOTION_OKAY;
}

/*****************************************************************************/
/*! ADC Disable Interrupt(s)
* \description
*   This function disables interrupts of the ADC.
* \class 
*   ADC
* \params
   ulIrqMask     [in] Interrupt disable mask
* \return
*   NX10_MOTION_OKAY                                                              */
/*****************************************************************************/
NX10_MOTION_RESULT NX10_Adc_DisableIrq( unsigned long ulIrqMask )
{
  /* disable interrupts */
  s_ptAdc->ulAdc_ctrl_irq_msk_reset = ulIrqMask;
  
  return NX10_MOTION_OKAY;
}

/*****************************************************************************/
/*! ADC Get masked Interrupt(s)
* \description
*   This function retrieves the current interrupt masked requests of the ADC.
* \class 
*   ADC
* \params
   pulIrq     [out] Pointer to interrupt events
* \return
*   NX10_MOTION_OKAY                                                              */
/*****************************************************************************/
NX10_MOTION_RESULT NX10_Adc_GetIrq( unsigned long *pulIrq )
{
  /* get irq requests */
  *pulIrq = s_ptAdc->ulAdc_ctrl_irq_masked;
  
  return NX10_MOTION_OKAY;
}

/*****************************************************************************/
/*! ADC Get raw Interrupt(s)
* \description
*   This function retrieves the current interrupt raw requests of the ADC.
* \class 
*   ADC
* \params
   pulIrq     [out] Pointer to interrupt events
* \return
*   NX10_MOTION_OKAY                                                              */
/*****************************************************************************/
NX10_MOTION_RESULT NX10_Adc_GetIrqRaw( unsigned long *pulIrq )
{
  /* get irq requests */
  *pulIrq = s_ptAdc->ulAdc_ctrl_irq_raw;
  
  return NX10_MOTION_OKAY;
}

/*****************************************************************************/
/*! ADC Confirm interrupt(s)
* \description
*   This function confirms a set of interrupts that were requested by the ADCoder.
* \class 
*   ADC
* \params
*   ulConfirmIrqMask  [in] Mask to confirm interrupt events
* \return
*   NX10_MOTION_OKAY                                                              */
/*****************************************************************************/
NX10_MOTION_RESULT NX10_Adc_ConfirmIrq( unsigned long ulConfirmIrqMask )
{
  
  /* confirm the set of irqs */
  s_ptAdc->ulAdc_ctrl_irq_raw = ulConfirmIrqMask;
  
  return NX10_MOTION_OKAY;
}






/*
.########..##.....##....###.....######.
.##.....##.###...###...##.##...##....##
.##.....##.####.####..##...##..##......
.##.....##.##.###.##.##.....##.##......
.##.....##.##.....##.#########.##......
.##.....##.##.....##.##.....##.##....##
.########..##.....##.##.....##..######.
*/

#define NX10_NUM_DMA_CH 3

/*****************************************************************************/
/*! DMAC Configuration
* \description
*   This function configures the DMA controller.
* \class 
*   DMAC
* \params
*   fEnable  [in]  1: enable, 0: disable the DMA conroller
* \return
*   NX10_DMAC_OKAY                                                                */
/*****************************************************************************/
NX10_DMAC_RESULT NX10_Dmac_Config( int fEnable )
{
  /* set config */
  s_ptDmacReg->ulDmac_config = (fEnable == 1) ? MSK_NX10_dmac_config_DMACENABLE : 0;
  
  return NX10_DMAC_OKAY;  
}


/*****************************************************************************/
/*! DMAC Get Interrupt(s)
* \description
*   This function retrieves the current interrupt masked requests of the DMAC.
*   The interrupt mask is specified in the appropriate DMA channel configuration.
* \class 
*   DMAC
* \params
*   pulIrqTc     [out] Pointer to interrupt terminal count events
*   pulIrqErr    [out] Pointer to interrupt error events
* \return
*   NX10_DMAC_OKAY                                                              */
/*****************************************************************************/
NX10_DMAC_RESULT NX10_Dmac_GetIrq( unsigned long* pulIrqTc, unsigned long* pulIrqErr )
{
  /* get irq requests */
  *pulIrqTc = s_ptDmacReg->ulDmac_inttc_status;
  *pulIrqErr = s_ptDmacReg->ulDmac_interr_status;
  
  return NX10_DMAC_OKAY;
}


/*****************************************************************************/
/*! DMAC Confirm Interrupt(s)
* \description
*   This function confirms a set of interrupts that were requested by the DMAC.
* \class 
*   DMAC
* \params
*   ulConfirmIrqTcMask  [in] Mask to confirm interrupt terminal count events
*   ulConfirmIrqErrMask  [in] Mask to confirm interrupt error events
* \return
*   NX10_DMAC_OKAY                                                              */
/*****************************************************************************/
NX10_DMAC_RESULT NX10_Dmac_ConfirmIrq( unsigned long ulConfirmIrqTcMask, unsigned long ulConfirmIrqErrMask )
{
  /* get irq requests */
  s_ptDmacReg->ulDmac_inttc_clear = ulConfirmIrqTcMask;
  s_ptDmacReg->ulDmac_interr_clear = ulConfirmIrqErrMask;
  
  return NX10_DMAC_OKAY;
}


/*****************************************************************************/
/*! DMAC Configure Channel
* \description
*   This function configures one channel of the DMA controller.
*   The bits "Halt" and "Enable" of the channel configuration register
*   are ignored within this routine.
* \class 
*   DMAC
* \params
*   ulChannel           [in] DMA channel number
*   ptDmaChCfg           [in] Pointer to DMA channel configuration structure
* \return
*   NX10_DMAC_OKAY
*   NX10_DMAC_ERR_INVAL_CHANNEL                                                   */
/*****************************************************************************/
NX10_DMAC_RESULT NX10_Dmac_ConfigChannel( unsigned long ulChannel, NX10_DMAC_CHCFG* ptDmaChCfg )
{
  NX10_DMAC_CH_AREA_T* ptDmaCh;
  
  /* check for valid channel number */
  if (ulChannel >= NX10_NUM_DMA_CH)
    return NX10_DMAC_ERR_INVAL_CHANNEL;
  
  /* get current channel address */
  ptDmaCh = (NX10_DMAC_CH_AREA_T*) (Addr_NX10_dmac_ch0 + (ulChannel * 0x20));
  
  ptDmaCh->ulDmac_chsrc_ad = ptDmaChCfg->ulDmacChSrcAd;
  ptDmaCh->ulDmac_chdest_ad = ptDmaChCfg->ulDmacChDestAd;
  ptDmaCh->ulDmac_chlink = ptDmaChCfg->ulDmacChLink; 
  ptDmaCh->ulDmac_chctrl = ptDmaChCfg->ulDmacChCtrl; 
  ptDmaCh->ulDmac_chcfg = ptDmaChCfg->ulDmacChCfg & ~(MSK_NX10_dmac_chcfg_E | MSK_NX10_dmac_chcfg_H);

  return NX10_DMAC_OKAY;
}


/*****************************************************************************/
/*! DMAC Enable Channel
* \description
*   This function enables one channel of the DMA controller.
*   The channel configuration must be written before the channel is enabled.
* \class 
*   DMAC
* \params
*   ulChannel           [in] DMA channel number
* \return
*   NX10_DMAC_OKAY
*   NX10_DMAC_ERR_INVAL_CHANNEL                                                   */
/*****************************************************************************/
NX10_DMAC_RESULT NX10_Dmac_EnableChannel( unsigned long ulChannel )
{
  NX10_DMAC_CH_AREA_T* ptDmaCh;
  
  /* check for valid channel number */
  if (ulChannel >= NX10_NUM_DMA_CH)
    return NX10_DMAC_ERR_INVAL_CHANNEL;
  
  /* get current channel address */
  ptDmaCh = (NX10_DMAC_CH_AREA_T*) (Addr_NX10_dmac_ch0 + (ulChannel * 0x20));

  ptDmaCh->ulDmac_chcfg |= MSK_NX10_dmac_chcfg_E;
  
  return NX10_DMAC_OKAY;
}


/*****************************************************************************/
/*! DMAC Disable Channel
* \description
*   This function disables one channel of the DMA controller.
* \class 
*   DMAC
* \params
*   ulChannel           [in] DMA channel number
* \return
*   NX10_DMAC_OKAY
*   NX10_DMAC_ERR_INVAL_CHANNEL                                                   */
/*****************************************************************************/
NX10_DMAC_RESULT NX10_Dmac_DisableChannel( unsigned long ulChannel )
{
  NX10_DMAC_CH_AREA_T* ptDmaCh;
  
  /* check for valid channel number */
  if (ulChannel >= NX10_NUM_DMA_CH)
    return NX10_DMAC_ERR_INVAL_CHANNEL;
  
  /* get current channel address */
  ptDmaCh = (NX10_DMAC_CH_AREA_T*) (Addr_NX10_dmac_ch0 + (ulChannel * 0x20));

  /* halt channel */
  ptDmaCh->ulDmac_chcfg |= MSK_NX10_dmac_chcfg_H;
  
  /* wait for channel to get inactive */
  while ((ptDmaCh->ulDmac_chcfg & MSK_NX10_dmac_chcfg_A) != 0);
  
  /* disable channel */
  ptDmaCh->ulDmac_chcfg &= ~MSK_NX10_dmac_chcfg_E;
  
  return NX10_DMAC_OKAY;
}

/*
....###....########..##.....##.........########.####.##.....##.########.########.
...##.##...##.....##.###...###............##.....##..###...###.##.......##.....##
..##...##..##.....##.####.####............##.....##..####.####.##.......##.....##
.##.....##.########..##.###.##............##.....##..##.###.##.######...########.
.#########.##...##...##.....##............##.....##..##.....##.##.......##...##..
.##.....##.##....##..##.....##............##.....##..##.....##.##.......##....##.
.##.....##.##.....##.##.....##.#######....##....####.##.....##.########.##.....##
*/


/*****************************************************************************/
/*! ARMTIMER Start
* \description
*   This function starts an ARM timer..
* \class 
*   ARMTIMER
* \params
* \return
*   NX10_ARMTIMER_RESULT
*   NX10_ARMTIMER_ERR_INVAL_PARAM                                                 */
/*****************************************************************************/
NX10_ARMTIMER_RESULT NX10_ArmTimer_Start( unsigned int uInst, NX10_ARMTIMER_CFG_MODE eMode, unsigned long ulVal )
{
  if( uInst>=NX10_ARMTIMER_CHANNELS )
    return NX10_ARMTIMER_ERR_INVAL_PARAM;

    switch( eMode ) {
    
    case NX10_ARMTIMER_CFG_MODE_STOP_AT_0:
      s_ptArmTimer->aulArm_timer_config_timer[uInst]  =  eMode<<SRT_NX10_arm_timer_config_timer0_mode;
      s_ptArmTimer->aulArm_timer_timer[uInst]         =  ulVal;
    break;
    
    case NX10_ARMTIMER_CFG_MODE_PRELOAD_AT_0:
      s_ptArmTimer->aulArm_timer_config_timer[uInst]  =  eMode<<SRT_NX10_arm_timer_config_timer0_mode;
      s_ptArmTimer->aulArm_timer_preload_timer[uInst] =  ulVal;
      s_ptArmTimer->aulArm_timer_timer[uInst]         =  ulVal;
    break;
    
    case NX10_ARMTIMER_CFG_MODE_SYSTIME:
      s_ptArmTimer->aulArm_timer_config_timer[uInst]  =  eMode<<SRT_NX10_arm_timer_config_timer0_mode;
      s_ptArmTimer->aulArm_timer_timer[uInst]         =  ulVal;
    break;
 
    default: return NX10_ARMTIMER_ERR_INVAL_PARAM;
    break;
  }

  return NX10_ARMTIMER_OKAY;  
}

/*****************************************************************************/
/*! ARMTIMER Stop
* \description
*   This function stops and resets an ARM timer..
* \class 
*   ARMTIMER
* \params
* \return
*   NX10_ARMTIMER_RESULT
*   NX10_ARMTIMER_ERR_INVAL_PARAM                                                 */
/*****************************************************************************/
NX10_ARMTIMER_RESULT NX10_ArmTimer_Stop( unsigned int uInst  )
{
  if( uInst>=NX10_ARMTIMER_CHANNELS )
    return NX10_ARMTIMER_ERR_INVAL_PARAM;

  /* disable interrupt */
  switch( uInst ) {
    case 0: s_ptArmTimer->ulArm_timer_irq_msk_reset =  1<<SRT_NX10_arm_timer_irq_msk_reset_timer0_irq; break;
    case 1: s_ptArmTimer->ulArm_timer_irq_msk_reset =  1<<SRT_NX10_arm_timer_irq_msk_reset_timer1_irq; break;
    default: return NX10_XPICTIMER_ERR_INVAL_PARAM;
    break;
  }

  /* reset timer registers */
  s_ptArmTimer->aulArm_timer_config_timer[uInst]  =  0;
  s_ptArmTimer->aulArm_timer_preload_timer[uInst] =  0;
  s_ptArmTimer->aulArm_timer_timer[uInst]         =  0;
  s_ptArmTimer->aulArm_timer_timer[uInst]         =  0;

  /* confirm pending interrupt */
  switch( uInst ) {
    case 0: s_ptArmTimer->ulArm_timer_irq_raw =  1<<SRT_NX10_arm_timer_irq_raw_timer0_irq; break;
    case 1: s_ptArmTimer->ulArm_timer_irq_raw =  1<<SRT_NX10_arm_timer_irq_raw_timer1_irq; break;
    default: return NX10_XPICTIMER_ERR_INVAL_PARAM;
    break;
  }

  return NX10_ARMTIMER_OKAY;  
}

/*****************************************************************************/
/*! ARMTIMER Start Systime NS compare
* \description
*   This function starts the systime_ns compare machine.
* \class 
*   ARMTIMER
* \params
* \return
*   NX10_ARMTIMER_RESULT                                                          */
/*****************************************************************************/
NX10_ARMTIMER_RESULT NX10_ArmTimer_StartSystimeNsCmp( unsigned long ulVal )
{
  s_ptArmTimer->ulArm_timer_systime_ns_compare = ulVal;

  return NX10_ARMTIMER_OKAY;  
}

/*****************************************************************************/
/*! ARMTIMER Start Systime S compare
* \description
*   This function starts the systime_s compare machine.
* \class 
*   ARMTIMER
* \params
* \return
*   NX10_ARMTIMER_RESULT                                                          */
/*****************************************************************************/
NX10_ARMTIMER_RESULT NX10_ArmTimer_StartSystimeSecCmp( unsigned long ulVal )
{
  // write value
  s_ptArmTimer->ulArm_timer_systime_s_compare = ulVal;
  
  // reset irq_raw.systime_s_irq to activate compare machine
  s_ptArmTimer->ulArm_timer_irq_raw = MSK_NX10_arm_timer_irq_raw_systime_s_irq;

  return NX10_ARMTIMER_OKAY;  
}

/*****************************************************************************/
/*! ARMTIMER Enable Interrupt(s)
* \description
*   This function enables interrupts of the ARMTIMER.
* \class 
*   ARMTIMER
* \params
   ulIrqMask     [in] Interrupt enable mask
* \return
*   NX10_ARMTIMER_RESULT                                                          */
/*****************************************************************************/
NX10_ARMTIMER_RESULT NX10_ArmTimer_EnIrq( int fTimer0IrqEn, int fTimer1IrqEn, int fSystimeNsIrqEn, int fSystimeSecIrqEn )
{
  /* enable interrupts */
  s_ptArmTimer->ulArm_timer_irq_msk_set =  (fTimer0IrqEn<<SRT_NX10_arm_timer_irq_msk_set_timer0_irq)
                                          |(fTimer1IrqEn<<SRT_NX10_arm_timer_irq_msk_set_timer1_irq)
                                          |(fSystimeNsIrqEn<<SRT_NX10_arm_timer_irq_msk_set_systime_ns_irq)
                                          |(fSystimeSecIrqEn<<SRT_NX10_arm_timer_irq_msk_set_systime_s_irq);
  return NX10_ARMTIMER_OKAY;
}

/*****************************************************************************/
/*! ARMTIMER Disable Interrupt(s)
* \description
*   This function disables interrupts.
* \class 
*   ARMTIMER
* \params
   ulIrqMask     [in] Interrupt disable mask
* \return
*   NX10_ARMTIMER_OKAY                                                            */
/*****************************************************************************/
NX10_ARMTIMER_RESULT NX10_ArmTimer_DisIrq( int fTimer0IrqDis, int fTimer1IrqDis, int fSystimeNsIrqDis, int fSystimeSecIrqDis )
{
  /* disable interrupts */
  s_ptArmTimer->ulArm_timer_irq_msk_reset =  (fTimer0IrqDis<<SRT_NX10_arm_timer_irq_msk_reset_timer0_irq)
                                            |(fTimer1IrqDis<<SRT_NX10_arm_timer_irq_msk_reset_timer1_irq)
                                            |(fSystimeNsIrqDis<<SRT_NX10_arm_timer_irq_msk_reset_systime_ns_irq)
                                            |(fSystimeSecIrqDis<<SRT_NX10_arm_timer_irq_msk_reset_systime_s_irq);
  return NX10_ARMTIMER_OKAY;
}

/*****************************************************************************/
/*! ARMTIMER Get masked Interrupt(s)
* \description
*   This function retrieves the current interrupt masked requests.
* \class 
*   ARMTIMER
* \params
   pulIrq     [out] Pointer to interrupt events
* \return
*   NX10_ARMTIMER_OKAY                                                            */
/*****************************************************************************/
NX10_ARMTIMER_RESULT NX10_ArmTimer_GetIrq( unsigned long *pulIrq )
{
  /* get irq requests */
  *pulIrq = s_ptArmTimer->ulArm_timer_irq_masked;
  
  return NX10_ARMTIMER_OKAY;
}

/*****************************************************************************/
/*! ARMTIMER Get raw Interrupt(s)
* \description
*   This function retrieves the current interrupt raw requests.
* \class 
*   ARMTIMER
* \params
   pulIrq     [out] Pointer to interrupt events
* \return
*   NX10_ARMTIMER_OKAY                                                            */
/*****************************************************************************/
NX10_ARMTIMER_RESULT NX10_ArmTimer_GetIrqRaw( unsigned long *pulIrq )
{
  /* get irq requests */
  *pulIrq = s_ptArmTimer->ulArm_timer_irq_raw;
  
  return NX10_ARMTIMER_OKAY;
}

/*****************************************************************************/
/*! ARMTIMER Confirm interrupt(s)
* \description
*   This function confirms a set of interrupts that were requested.
* \class 
*   ARMTIMER
* \params
*   ulConfirmIrqMask  [in] Mask to confirm interrupt events
* \return
*   NX10_ARMTIMER_OKAY                                                            */
/*****************************************************************************/
NX10_ARMTIMER_RESULT NX10_ArmTimer_ConfirmIrq( unsigned long ulConfirmIrqMask )
{
  /* confirm the set of irqs */
  s_ptArmTimer->ulArm_timer_irq_raw = ulConfirmIrqMask;
  
  return NX10_ARMTIMER_OKAY;
}

/*****************************************************************************/
/*! ARMTIMER Get Parameter
* \description
*   This function reads parameter.
* \class 
*   PWM
* \params
*   ulPrmID          [in]  Parameter ID
*   pulPrmVal        [in]  Pointer to parameter value
* \return
*   NX10_ARMTIMER_OKAY                                                             
*   NX10_ARMTIMER_ERR_INVAL_PARAM                                                 */
/*****************************************************************************/
NX10_ARMTIMER_RESULT NX10_ArmTimer_GetPrm( unsigned long ulPrmID, unsigned long* pulPrmVal )
{
  switch( ulPrmID )
  { 
    case NX10_ARMTIMER_PRM_CONFIG_TIMER0:      *pulPrmVal = s_ptArmTimer->aulArm_timer_config_timer[0];   break;
    case NX10_ARMTIMER_PRM_CONFIG_TIMER1:      *pulPrmVal = s_ptArmTimer->aulArm_timer_config_timer[1];   break;
    case NX10_ARMTIMER_PRM_PRELOAD_TIMER0:     *pulPrmVal = s_ptArmTimer->aulArm_timer_preload_timer[0];  break;
    case NX10_ARMTIMER_PRM_PRELOAD_TIMER1:     *pulPrmVal = s_ptArmTimer->aulArm_timer_preload_timer[1];  break;
    case NX10_ARMTIMER_PRM_TIMER0:             *pulPrmVal = s_ptArmTimer->aulArm_timer_timer[0];          break;
    case NX10_ARMTIMER_PRM_TIMER1:             *pulPrmVal = s_ptArmTimer->aulArm_timer_timer[1];          break;
    case NX10_ARMTIMER_PRM_SYSTIME_NS_COMPARE: *pulPrmVal = s_ptArmTimer->ulArm_timer_systime_ns_compare; break;
    case NX10_ARMTIMER_PRM_SYSTIME_S_COMPARE:  *pulPrmVal = s_ptArmTimer->ulArm_timer_systime_s_compare;  break;
    case NX10_ARMTIMER_PRM_IRQMSKSET:          *pulPrmVal = s_ptArmTimer->ulArm_timer_irq_msk_set;        break;
    case NX10_ARMTIMER_PRM_IRQMSKRST:          *pulPrmVal = s_ptArmTimer->ulArm_timer_irq_msk_reset;      break;

    /* Default */
    default:
      return NX10_ARMTIMER_ERR_INVAL_PARAM;
    break;
  } /* end switch */

  return NX10_ARMTIMER_OKAY;
}

/*
.##.....##.########..####..######..........########.####.##.....##.########.########.
..##...##..##.....##..##..##....##............##.....##..###...###.##.......##.....##
...##.##...##.....##..##..##..................##.....##..####.####.##.......##.....##
....###....########...##..##..................##.....##..##.###.##.######...########.
...##.##...##.........##..##..................##.....##..##.....##.##.......##...##..
..##...##..##.........##..##....##............##.....##..##.....##.##.......##....##.
.##.....##.##........####..######..#######....##....####.##.....##.########.##.....##
*/

/*****************************************************************************/
/*! XPICTIMER Start
* \description
*   This function starts an xPIC timer..
* \class 
*   XPICTIMER
* \params
* \return
*   NX10_XPICTIMER_RESULT
*   NX10_XPICTIMER_ERR_INVAL_PARAM                                                */
/*****************************************************************************/
NX10_XPICTIMER_RESULT NX10_XpicTimer_Start( unsigned int uInst, NX10_XPICTIMER_CFG_MODE eMode, unsigned long ulVal )
{
  if( uInst>=NX10_XPICTIMER_CHANNELS )
    return NX10_XPICTIMER_ERR_INVAL_PARAM;

    switch( eMode ) {
    
    case NX10_XPICTIMER_CFG_MODE_STOP_AT_0:
      s_ptXpicTimer->aulXpic_timer_config_timer[uInst]  =  eMode<<SRT_NX10_xpic_timer_config_timer0_mode;
      s_ptXpicTimer->aulXpic_timer_timer[uInst]         =  ulVal;
    break;
    
    case NX10_XPICTIMER_CFG_MODE_PRELOAD_AT_0:
      s_ptXpicTimer->aulXpic_timer_config_timer[uInst]  =  eMode<<SRT_NX10_xpic_timer_config_timer0_mode;
      s_ptXpicTimer->aulXpic_timer_preload_timer[uInst] =  ulVal;
      s_ptXpicTimer->aulXpic_timer_timer[uInst]         =  ulVal;
    break;
    
    case NX10_XPICTIMER_CFG_MODE_SYSTIME:
      s_ptXpicTimer->aulXpic_timer_config_timer[uInst]  =  eMode<<SRT_NX10_xpic_timer_config_timer0_mode;
      s_ptXpicTimer->aulXpic_timer_timer[uInst]         =  ulVal;
    break;
 
    default: return NX10_XPICTIMER_ERR_INVAL_PARAM;
    break;
  }
    
  return NX10_XPICTIMER_OKAY;  
}

/*****************************************************************************/
/*! XPICTIMER Stop
* \description
*   This function stops and resets an XPIC timer..
* \class 
*   XPICTIMER
* \params
* \return
*   NX10_XPICTIMER_RESULT
*   NX10_XPICTIMER_ERR_INVAL_PARAM                                                 */
/*****************************************************************************/
NX10_XPICTIMER_RESULT NX10_XpicTimer_Stop( unsigned int uInst  )
{
  /* disable interrupt */
  switch( uInst ) {
    case 0: s_ptXpicTimer->ulXpic_timer_irq_msk_reset =  1<<SRT_NX10_xpic_timer_irq_msk_reset_timer0_irq; break;
    case 1: s_ptXpicTimer->ulXpic_timer_irq_msk_reset =  1<<SRT_NX10_xpic_timer_irq_msk_reset_timer1_irq; break;
    case 2: s_ptXpicTimer->ulXpic_timer_irq_msk_reset =  1<<SRT_NX10_xpic_timer_irq_msk_reset_timer2_irq; break;
    default: return NX10_XPICTIMER_ERR_INVAL_PARAM;
    break;
  }

  /* reset timer registers */
  s_ptXpicTimer->aulXpic_timer_config_timer[uInst]  =  0;
  s_ptXpicTimer->aulXpic_timer_preload_timer[uInst] =  0;
  s_ptXpicTimer->aulXpic_timer_timer[uInst]         =  0;
  s_ptXpicTimer->aulXpic_timer_timer[uInst]         =  0;

  /* confirm pending interrupt */
  switch( uInst ) {
    case 0: s_ptXpicTimer->ulXpic_timer_irq_raw =  1<<SRT_NX10_xpic_timer_irq_raw_timer0_irq; break;
    case 1: s_ptXpicTimer->ulXpic_timer_irq_raw =  1<<SRT_NX10_xpic_timer_irq_raw_timer1_irq; break;
    case 2: s_ptXpicTimer->ulXpic_timer_irq_raw =  1<<SRT_NX10_xpic_timer_irq_raw_timer2_irq; break;
    default: return NX10_XPICTIMER_ERR_INVAL_PARAM;
    break;
  }

  return NX10_XPICTIMER_OKAY;  
}

/*****************************************************************************/
/*! XPICTIMER Enable Interrupt(s)
* \description
*   This function enables interrupts of the XPICTIMER.
* \class 
*   XPICTIMER
* \params
   ulIrqMask     [in] Interrupt enable mask
* \return
*   NX10_XPICTIMER_RESULT                                                         */
/*****************************************************************************/
NX10_XPICTIMER_RESULT NX10_XpicTimer_EnIrq( int fTimer0IrqEn, int fTimer1IrqEn, int fTimer2IrqEn )
{
  /* enable interrupts */
  s_ptXpicTimer->ulXpic_timer_irq_msk_set =  (fTimer0IrqEn<<SRT_NX10_xpic_timer_irq_msk_set_timer0_irq)
                                            |(fTimer1IrqEn<<SRT_NX10_xpic_timer_irq_msk_set_timer1_irq)
                                            |(fTimer2IrqEn<<SRT_NX10_xpic_timer_irq_msk_set_timer2_irq);
  return NX10_XPICTIMER_OKAY;
}

/*****************************************************************************/
/*! XPICTIMER Disable Interrupt(s)
* \description
*   This function disables interrupts.
* \class 
*   XPICTIMER
* \params
   ulIrqMask     [in] Interrupt disable mask
* \return
*   NX10_XPICTIMER_OKAY                                                           */
/*****************************************************************************/
NX10_XPICTIMER_RESULT NX10_XpicTimer_DisIrq( int fTimer0IrqDis, int fTimer1IrqDis, int fTimer2IrqDis )
{
  /* disable interrupts */
  s_ptXpicTimer->ulXpic_timer_irq_msk_reset =  (fTimer0IrqDis<<SRT_NX10_xpic_timer_irq_msk_reset_timer0_irq)
                                              |(fTimer1IrqDis<<SRT_NX10_xpic_timer_irq_msk_reset_timer1_irq)
                                              |(fTimer2IrqDis<<SRT_NX10_xpic_timer_irq_msk_reset_timer2_irq);
  return NX10_XPICTIMER_OKAY;
}

/*****************************************************************************/
/*! XPICTIMER Get masked Interrupt(s)
* \description
*   This function retrieves the current interrupt masked requests.
* \class 
*   XPICTIMER
* \params
   pulIrq     [out] Pointer to interrupt events
* \return
*   NX10_XPICTIMER_OKAY                                                           */
/*****************************************************************************/
NX10_XPICTIMER_RESULT NX10_XpicTimer_GetIrq( unsigned long *pulIrq )
{
  /* get irq requests */
  *pulIrq = s_ptXpicTimer->ulXpic_timer_irq_masked;
  
  return NX10_XPICTIMER_OKAY;
}

/*****************************************************************************/
/*! XPICTIMER Get raw Interrupt(s)
* \description
*   This function retrieves the current interrupt raw requests.
* \class 
*   XPICTIMER
* \params
   pulIrq     [out] Pointer to interrupt events
* \return
*   NX10_XPICTIMER_OKAY                                                           */
/*****************************************************************************/
NX10_XPICTIMER_RESULT NX10_XpicTimer_GetIrqRaw( unsigned long *pulIrq )
{
  /* get irq requests */
  *pulIrq = s_ptXpicTimer->ulXpic_timer_irq_raw;
  
  return NX10_XPICTIMER_OKAY;
}

/*****************************************************************************/
/*! XPICTIMER Confirm interrupt(s)
* \description
*   This function confirms a set of interrupts that were requested.
* \class 
*   XPICTIMER
* \params
*   ulConfirmIrqMask  [in] Mask to confirm interrupt events
* \return
*   NX10_XPICTIMER_OKAY                                                              */
/*****************************************************************************/
NX10_XPICTIMER_RESULT NX10_XpicTimer_ConfirmIrq( unsigned long ulConfirmIrqMask )
{
  /* confirm the set of irqs */
  s_ptXpicTimer->ulXpic_timer_irq_masked = ulConfirmIrqMask;
  
  return NX10_XPICTIMER_OKAY;
}

/*****************************************************************************/
/*! XPICTIMER Get Parameter
* \description
*   This function reads parameter.
* \class 
*   XPICTIMER
* \params
*   ulPrmID          [in]  Parameter ID
*   pulPrmVal        [in]  Pointer to parameter value
* \return
*   NX10_XPICTIMER_OKAY                                                             
*   NX10_XPICTIMER_ERR_INVAL_PARAM                                                 */
/*****************************************************************************/
NX10_XPICTIMER_RESULT NX10_XpicTimer_GetPrm( unsigned long ulPrmID, unsigned long* pulPrmVal )
{
  switch( ulPrmID )
  { 
    case NX10_XPICTIMER_PRM_CONFIG_TIMER0:      *pulPrmVal = s_ptXpicTimer->aulXpic_timer_config_timer[0];   break;
    case NX10_XPICTIMER_PRM_CONFIG_TIMER1:      *pulPrmVal = s_ptXpicTimer->aulXpic_timer_config_timer[1];   break;
    case NX10_XPICTIMER_PRM_CONFIG_TIMER2:      *pulPrmVal = s_ptXpicTimer->aulXpic_timer_config_timer[2];   break;
    case NX10_XPICTIMER_PRM_PRELOAD_TIMER0:     *pulPrmVal = s_ptXpicTimer->aulXpic_timer_preload_timer[0];  break;
    case NX10_XPICTIMER_PRM_PRELOAD_TIMER1:     *pulPrmVal = s_ptXpicTimer->aulXpic_timer_preload_timer[1];  break;
    case NX10_XPICTIMER_PRM_PRELOAD_TIMER2:     *pulPrmVal = s_ptXpicTimer->aulXpic_timer_preload_timer[2];  break;
    case NX10_XPICTIMER_PRM_TIMER0:             *pulPrmVal = s_ptXpicTimer->aulXpic_timer_timer[0];          break;
    case NX10_XPICTIMER_PRM_TIMER1:             *pulPrmVal = s_ptXpicTimer->aulXpic_timer_timer[1];          break;
    case NX10_XPICTIMER_PRM_TIMER2:             *pulPrmVal = s_ptXpicTimer->aulXpic_timer_timer[2];          break;
    case NX10_XPICTIMER_PRM_IRQMSKSET:          *pulPrmVal = s_ptXpicTimer->ulXpic_timer_irq_msk_set;        break;
    case NX10_XPICTIMER_PRM_IRQMSKRST:          *pulPrmVal = s_ptXpicTimer->ulXpic_timer_irq_msk_reset;      break;

    /* Default */
    default:
      return NX10_XPICTIMER_ERR_INVAL_PARAM;
    break;
  } /* end switch */

  return NX10_XPICTIMER_OKAY;
}

/*****************************************************************************/
/*! XPICTIMER Get Systime
* \description
*   This function reads the systime completely.
* \class 
*   XPICTIMER
* \params
*   pulSystime_s         [out]  Pointer systime_s value
*   pulSystime_ns        [out]  Pointer systime_ns value
* \return
*   NX10_XPICTIMER_OKAY                                                             
*   NX10_XPICTIMER_ERR_INVAL_PARAM                                                */
/*****************************************************************************/
NX10_XPICTIMER_RESULT NX10_XpicTimer_GetSystime( unsigned long* pulSystime_s, unsigned long* pulSystime_ns )
{
  *pulSystime_s  = s_ptXpicTimer->ulXpic_timer_systime_s;  
  *pulSystime_ns = s_ptXpicTimer->ulXpic_timer_systime_ns;
  return NX10_XPICTIMER_OKAY;
}






//                                                                  
// ######  #    #  #    #   ####   #####  #   ####   #    #   ####  
// #       #    #  ##   #  #    #    #    #  #    #  ##   #  #      
// #####   #    #  # #  #  #         #    #  #    #  # #  #   ####  
// #       #    #  #  # #  #         #    #  #    #  #  # #       # 
// #       #    #  #   ##  #    #    #    #  #    #  #   ##  #    # 
// #        ####   #    #   ####     #    #   ####   #    #   ####  
//                                                                  
//                                                        #     #     #     #       
// #    #   ####   ######  #####       #####   #   #      #     #    # #    #       
// #    #  #       #       #    #      #    #   # #       #     #   #   #   #       
// #    #   ####   #####   #    #      #####     #        #######  #     #  #       
// #    #       #  #       #    #      #    #    #        #     #  #######  #       
// #    #  #    #  #       #    #      #    #    #        #     #  #     #  #       
//  ####    ####   ######  #####       #####     #        #     #  #     #  ####### 
//                                                                                  

/*****************************************************************************/
#include "hal_resources_defines_netx10.h"
__USE_XPEC_PRAM
__USE_XPEC_REGS
__USE_XPEC_DRAM
__USE_XMAC
__USE_MIIMU
__USE_SYSTIME
__USE_XPIC
__USE_XPIC_DEBUG
/*****************************************************************************/

/*
 __  __   ____           _                           _               
 \ \/ /  / ___|         | |       ___     __ _    __| |   ___   _ __ 
  \  /  | |             | |      / _ \   / _` |  / _` |  / _ \ | '__|
  /  \  | |___          | |___  | (_) | | (_| | | (_| | |  __/ | |   
 /_/\_\  \____|  _____  |_____|  \___/   \__,_|  \__,_|  \___| |_|   
                |_____|                                              

*/

#define NX10_NUM_XPECS 1

const unsigned long XcCode_rpu_reset0[27] = {
  0x00000064, // program size
  0x00000000, // trailing loads size
  0x101a0000, 0x01040001, 0xa15fdc81, 0x01080001, 0xa15fdc82, 0x0113fffd, 0xa15fdc83, 0x0117fffd, 
  0xa15fdc84, 0x01180001, 0xa15fdc85, 0x0113fff9, 0xa15fdc86, 0x011bfffd, 0xa15fdc87, 0x01140001, 
  0xa15fdc88, 0x0113fffd, 0xa15fdc89, 0x01180001, 0xa15fdc8a, 0x01080005, 0xa15fdc8b, 0x00e40000, 
  0x001fdc8b, 
  // trailing loads
  
};

const unsigned long XcCode_tpu_reset0[27] = {
  0x00000064, // program size
  0x00000000, // trailing loads size
  0x101a0400, 0x011c0601, 0xa15fdc81, 0x01200001, 0xa15fdc82, 0x012bfffd, 0xa15fdc83, 0x012ffffd, 
  0xa15fdc84, 0x01300001, 0xa15fdc85, 0x012bfff9, 0xa15fdc86, 0x0133fffd, 0xa15fdc87, 0x012c0001, 
  0xa15fdc88, 0x012bfffd, 0xa15fdc89, 0x01300001, 0xa15fdc8a, 0x01200005, 0xa15fdc8b, 0x00e40000, 
  0x001fdc8b, 
  // trailing loads
  
};

static const unsigned long* paulxMacRpuCodes[1]=
{
  XcCode_rpu_reset0,
};

static const unsigned long* paulxMacTpuCodes[1]=
{
  XcCode_tpu_reset0,
};

/*****************************************************************************/
/*! Reset XC Code
* \description
*   Reset XC port.
* \class 
*   XC 
* \params
*   uPortNo          [in]  XC Port Number
*   pvUser           [in]  User Specific Paramters
* \return
*   0 on success
*   -1 on erroneous                                                          */
/*****************************************************************************/
int NX10_XC_Reset( unsigned int uPortNo, void* pvUser )
{
  NX10_XMAC_AREA_T* ptXmac;
  NX10_XPEC_AREA_T* ptXpec;
  volatile unsigned long* pulXpecPram;
  volatile unsigned long* pulXpecDram;
  unsigned int uIdx;
  
  if( uPortNo >= NX10_NUM_XPECS)
    return -1;

  ptXmac      = s_ptXmacArea[uPortNo];
  ptXpec      = s_ptXpecRegArea[uPortNo];
  pulXpecPram = s_pulXpecPramArea[uPortNo];
  pulXpecDram = s_pulXpecDramArea[uPortNo];

  /* Stop xPEC */
  ptXpec->ulXpu_hold_pc = 1;                                       
  /* Stop xMAC */
  ptXmac->ulXmac_tpu_hold_pc = MSK_NX10_xmac_tpu_hold_pc_tpu_hold;
  ptXmac->ulXmac_rpu_hold_pc = MSK_NX10_xmac_rpu_hold_pc_rpu_hold;
  /* Clear output enable of tx asap */
  ptXmac->ulXmac_config_obu  = 0x00000180;
  /* Clear output enable of io0..5 asap */
  ptXpec->ulStatcfg0 = 0;

  /* load ratemul reset code */
  if( NX10_XC_Load( uPortNo, NX10_XC_TYPE_RPU, (unsigned long*)paulxMacRpuCodes[uPortNo], pvUser)!=0 ) {
    return -1;
  }
  if( NX10_XC_Load( uPortNo, NX10_XC_TYPE_TPU, (unsigned long*)paulxMacTpuCodes[uPortNo], pvUser)!=0 ) {
    return -1;
  }
  
  ptXmac->ulXmac_rpu_pc      = 0; /* Reset PC to 0 */
  ptXmac->ulXmac_tpu_pc      = 0; /* Reset PC to 0 */
  ptXmac->ulXmac_tpu_hold_pc = 0; /* Clear Hold-Bit */
  ptXmac->ulXmac_rpu_hold_pc = 0; /* Clear Hold-Bit */

  /* !!!! ATTENTION: There must be enougth time between starting xMAC and stopping xMAC to execute reset programm */

  pulXpecPram[0] = 0xC0000FFF;                                   /* Use the command wait b000000000000,b111111111111 at Address 0*/

  ptXpec->ulXpec_pc = 0x7ff;                                     /* Reset the Program Counter to 0x7ff */

  ptXpec->ulXpec_statcfg = 0;                                    /* Reset Timer operation and DMA */
  pulXpecDram[0x7FE] = 0x7F;                                     /* Link and Stop DMA */
  pulXpecDram[0x7FF] = 0x00;                                     /* Link and Stop DMA */

  /* let the XC run for at least 10 cycles */
  for (uIdx = 0; uIdx < 10; uIdx++) ptXpec->ulXpu_hold_pc = 0;

  ptXpec->ulXpu_hold_pc = 1;                                     /* Hold the Program Counter */
  ptXpec->ulXpec_pc     = 0x7ff;                                 /* Reset the Program Counter to 0x7ff */

  /* reset all registers */
  ptXpec->aulXpec_r[0]   = 0;
  ptXpec->aulXpec_r[1]   = 0;
  ptXpec->aulXpec_r[2]   = 0;
  ptXpec->aulXpec_r[3]   = 0;
  ptXpec->aulXpec_r[4]   = 0;
  ptXpec->aulXpec_r[5]   = 0;
  ptXpec->aulXpec_r[6]   = 0;
  ptXpec->aulXpec_r[7]   = 0;
  ptXpec->aulUsr[0]      = 0;
  ptXpec->aulUsr[1]      = 0;
  ptXpec->ulRange45      = 0;
  ptXpec->ulRange67      = 0;
  ptXpec->ulUrx_count    = 0;
  ptXpec->ulUtx_count    = 0;
  ptXpec->ulXpec_statcfg = 0;
  
  
  /* Stop all Timers */
  ptXpec->ulTimer4       = 0;
  ptXpec->ulTimer5       = 0;
  ptXpec->aulTimer[0]    = 0; 
  ptXpec->aulTimer[1]    = 0;
  ptXpec->aulTimer[2]    = 0;
  ptXpec->aulTimer[3]    = 0;

  ptXpec->ulIrq          = 0xFFFF0000; /* Clear XPEC side IRQ request lines */

  /* Reset events */
  ptXpec->ulEc_maska     = 0x0000FFFF;
  ptXpec->ulEc_maskb     = 0x0000FFFF;
  ptXpec->aulEc_mask[0]  = 0x0000FFFF; 
  ptXpec->aulEc_mask[1]  = 0x0000FFFF;
  ptXpec->aulEc_mask[2]  = 0x0000FFFF;
  ptXpec->aulEc_mask[3]  = 0x0000FFFF;
  ptXpec->aulEc_mask[4]  = 0x0000FFFF;
  ptXpec->aulEc_mask[5]  = 0x0000FFFF;
  ptXpec->aulEc_mask[6]  = 0x0000FFFF;
  ptXpec->aulEc_mask[7]  = 0x0000FFFF;
  ptXpec->aulEc_mask[8]  = 0x0000FFFF;
  ptXpec->aulEc_mask[9]  = 0x0000FFFF;

  ptXpec->ulXpec_adc     = 0;

  /* Reset shared registers, reset URX/UTX fifos */
  ptXpec->aulXpec_sr[0] = 0;
  ptXpec->aulXpec_sr[1] = 0;
  ptXpec->aulXpec_sr[2] = 0;
  ptXpec->aulXpec_sr[3] = 0;
  ptXpec->aulXpec_sr[4] = 0;
  ptXpec->aulXpec_sr[5] = 0;
  ptXpec->ulStatcfg0 = (MSK_NX10_statcfg0_reset_rx_fifo | MSK_NX10_statcfg0_reset_tx_fifo); // !!! special set/mask
  ptXpec->ulStatcfg0 = 0x00000000;
  POKE(Adr_NX10_irq_xpec0, 0x0000FFFF); // confirm all interrupts from xPEC

  /* hold xMAC */
  ptXmac->ulXmac_tpu_hold_pc = MSK_NX10_xmac_tpu_hold_pc_tpu_hold;
  ptXmac->ulXmac_rpu_hold_pc = MSK_NX10_xmac_rpu_hold_pc_rpu_hold;
      
  /* reset all xMAC registers to default values */
  ptXmac->ulXmac_rx_hw               = 0;
  ptXmac->ulXmac_rx_hw_count         = 0;
  ptXmac->ulXmac_tx                  = 0;
  ptXmac->ulXmac_tx_hw               = 0;
  ptXmac->ulXmac_tx_hw_count         = 0;
  ptXmac->ulXmac_tx_sent             = 0;
  ptXmac->aulXmac_wr[0]              = 0;
  ptXmac->aulXmac_wr[1]              = 0;
  ptXmac->aulXmac_wr[2]              = 0;
  ptXmac->aulXmac_wr[3]              = 0;
  ptXmac->aulXmac_wr[4]              = 0;
  ptXmac->aulXmac_wr[5]              = 0;
  ptXmac->aulXmac_wr[6]              = 0;
  ptXmac->aulXmac_wr[7]              = 0;
  ptXmac->aulXmac_wr[8]              = 0;
  ptXmac->aulXmac_wr[9]              = 0;
  ptXmac->ulXmac_config_mii          = 0;
  ptXmac->ulXmac_config_rx_nibble_fifo  = 0x00001000;
  ptXmac->ulXmac_config_tx_nibble_fifo  = 0;
  ptXmac->ulXmac_rpu_count1          = 0;
  ptXmac->ulXmac_rpu_count1          = 0;
  ptXmac->ulXmac_tpu_count1          = 0;
  ptXmac->ulXmac_tpu_count2          = 0;
  ptXmac->ulXmac_rx_count            = 0;
  ptXmac->ulXmac_tx_count            = 0;
  ptXmac->ulXmac_rpm_mask0           = 0;
  ptXmac->ulXmac_rpm_val0            = 0;
  ptXmac->ulXmac_rpm_mask1           = 0;
  ptXmac->ulXmac_rpm_val1            = 0;
  ptXmac->ulXmac_tpm_mask0           = 0;
  ptXmac->ulXmac_tpm_val0            = 0;
  ptXmac->ulXmac_tpm_mask1           = 0;
  ptXmac->ulXmac_tpm_val1            = 0;
  
  ptXmac->ulXmac_rx_crc_polynomial_l = 0;
  ptXmac->ulXmac_rx_crc_polynomial_h = 0;
  ptXmac->ulXmac_rx_crc_l            = 0;
  ptXmac->ulXmac_rx_crc_h            = 0;
  ptXmac->ulXmac_rx_crc_cfg          = 0;
  ptXmac->ulXmac_tx_crc_polynomial_l = 0;
  ptXmac->ulXmac_tx_crc_polynomial_h = 0;
  ptXmac->ulXmac_tx_crc_l            = 0;
  ptXmac->ulXmac_tx_crc_h            = 0;
  ptXmac->ulXmac_tx_crc_cfg          = 0;

  ptXmac->ulXmac_rx_tx_nof_bits      = 0x00000088;
  ptXmac->ulXmac_rx_crc32_l          = 0;
  ptXmac->ulXmac_rx_crc32_h          = 0;
  ptXmac->ulXmac_rx_crc32_cfg        = 0;
  ptXmac->ulXmac_tx_crc32_l          = 0;
  ptXmac->ulXmac_tx_crc32_h          = 0;
  ptXmac->ulXmac_tx_crc32_cfg        = 0;

  ptXmac->ulXmac_rpu_pc              = 0;
  ptXmac->ulXmac_tpu_pc              = 0;

  return 0;
}

/*****************************************************************************/
/*! Load XC Code
* \description
*   Load XC port.
* \class 
*   XC 
* \params
*   uPortNo              [in]  XC Port Number
*   eXcType              [in]  XC Entity To Download To (RPU, TPU, xPEC)
*   pulXcPrg             [in]  Pointer To Microcode
*   pvUser               [in]  User specific parameter
* \return
*   0 on success
*   -1 on erroneous                                                          */
/*****************************************************************************/
int NX10_XC_Load(unsigned int uPortNo, NX10_XC_TYPE_E eXcType, const unsigned long* pulXcPrg, void* pvUser)
{
  volatile unsigned long *pulDst, *pulDstCnt;
  volatile unsigned long *pulRamStart, *pulRamEnd;
  unsigned long ulPhysicalStart;
  const unsigned long *pulSrcStart, *pulSrcCnt, *pulSrcEnd;
  unsigned int uiElements;
  
  /* check the instance number */
  if(uPortNo>= NX10_NUM_XPECS) return -1;
  
  /* get the start and end address of the ram area, get the physical address */
  switch( eXcType )
  {
    case NX10_XC_TYPE_XPEC: pulRamStart     = (volatile unsigned long*) s_ptXpecRegArea[uPortNo];
                            pulRamEnd       = pulRamStart + (sizeof(NX10_XPEC_AREA_T) / sizeof(unsigned long));
                            ulPhysicalStart = (unsigned long) s_ptXpecRegArea[uPortNo];
                            break;
    case NX10_XC_TYPE_RPU:  /* fall through */
    case NX10_XC_TYPE_TPU:  pulRamStart     = (volatile unsigned long*)s_ptXmacArea[uPortNo];
                            pulRamEnd       = pulRamStart + (sizeof(NX10_XMAC_AREA_T)/sizeof(unsigned long));
                            ulPhysicalStart = (unsigned long) s_ptXmacArea[uPortNo];
                            break;
             
    default:           return -1; /* unknown unit type */
  }
  
  /* get the number of code elements */
  uiElements = pulXcPrg[0] / sizeof(unsigned long) - 1;
  
  /* get the pointer in the xc area */
  /* ram_virtual_start + code_physical_start - ram_physical_start */
  pulDst = pulRamStart + (pulXcPrg[2] - ulPhysicalStart) / sizeof(unsigned long);
  
  /* the code must fit into the ram area */
  if( (pulDst<pulRamStart) || ((pulDst+uiElements)>pulRamEnd) ) {
    /* the code exceeds the xpec ram! */
    return -1;
  }
  
  /* get source start and end pointer */
  pulSrcStart = pulXcPrg + 3;
  pulSrcEnd = pulSrcStart + uiElements;
  
  /* copy the code to xc ram */
  pulSrcCnt = pulSrcStart;
  pulDstCnt = pulDst;
  while( pulSrcCnt<pulSrcEnd ) {
    *pulDstCnt = *pulSrcCnt;
    pulDstCnt++;
    pulSrcCnt++;
  }
  
  /* compare the code */
  pulSrcCnt = pulSrcStart;
  pulDstCnt = pulDst;
  while( pulSrcCnt<pulSrcEnd ) {
    if( *pulDstCnt != *pulSrcCnt ) return -1;
    pulDstCnt++;
    pulSrcCnt++;
  }
 
  /* get the number of trailing loads */
  uiElements = pulXcPrg[1] / sizeof(unsigned long);
  
  /* get source start and end pointer */
  pulSrcCnt = pulXcPrg + 2 + pulXcPrg[0] / sizeof(unsigned long);
  pulSrcEnd = pulSrcCnt + uiElements;
  
  /* write all trailing loads */
  while( pulSrcCnt<pulSrcEnd ) {
    /* get the destination address ( ram_virtual_start + data_physical_start - ram_physical_start) */
    pulDst = pulRamStart + (*pulSrcCnt - ulPhysicalStart) / sizeof(unsigned long);
    pulSrcCnt++;
    
    /* check the destination address must fit into whole xpec area (pram, dram, ragisters)  */
    if( (pulDst<pulRamStart) || (pulDst>=(pulRamEnd + 0x2000)) ) return -1;

    /* write the data */
    *pulDst = *pulSrcCnt;
    pulSrcCnt++;
  }
  
  return 0;
}

/*****************************************************************************/
/*! Start XC Port
* \description
*   Start XC port.
* \class 
*   XC 
* \params
*   uPortNo              [in]  XC Port Number
*   pvUser               [in]  User specific parameter
* \return
*   0 on success
*   -1 on erroneous                                                          */
/*****************************************************************************/
int NX10_XC_Start(unsigned int uPortNo, void* pvUser)
{
  NX10_XMAC_AREA_T* ptXmacArea;
  NX10_XPEC_AREA_T* ptXpecArea;

  if(uPortNo>= NX10_NUM_XPECS) {
    return -1;
  }
  ptXmacArea = s_ptXmacArea[uPortNo];
  ptXpecArea = s_ptXpecRegArea[uPortNo];

  /* reset pc of units */
  ptXmacArea->ulXmac_rpu_pc = 0;
  ptXmacArea->ulXmac_tpu_pc = 0;
  ptXpecArea->ulXpec_pc     = 0x7ff;  

  /* start units */
  ptXmacArea->ulXmac_tpu_hold_pc = 0;
  ptXmacArea->ulXmac_rpu_hold_pc = 0;
  ptXpecArea->ulXpu_hold_pc      = 0;

  return 0;
}


/*
  __  __   ___   ___   __  __   _   _ 
 |  \/  | |_ _| |_ _| |  \/  | | | | |
 | |\/| |  | |   | |  | |\/| | | | | |
 | |  | |  | |   | |  | |  | | | |_| |
 |_|  |_| |___| |___| |_|  |_|  \___/ 
                                      
*/

/*****************************************************************************/
/*! Read PHY Register
* \description
*   Reads PHY register over MDIO.
* \class 
*   MIIMU 
* \params
*   uMiimuPreamble              [in]  Miimu Preamble
*   uMiimuMdcFreq               [in]  Miimu Mdc Frequence
*   uMiimuRtaField              [in]  Miimu Rta Field
*   uMiimuPhyAddr               [in]  Miimu PHY Address
*   uMiimuReqAddr               [in]  Miimu Register Address
*   pusData                     [out] Miimu Data
*   pvUser                      [in]  User specific parameter 
* \return
*   0 on success
*   -1 on erroneous                                                          */
/*****************************************************************************/
int NX10_MIIMU_ReadPhyReg( unsigned int uMiimuPreamble, unsigned int uMiimuMdcFreq, unsigned int uMiimuRtaField, unsigned int uMiimuPhyAddr, unsigned int uMiimuReqAddr, unsigned short* pusData, void* pvUser)
{

  NX10_MIIMU_REG_T tMiiMuReg = {0};

  if( (uMiimuPhyAddr>31) || (uMiimuReqAddr>31) ) return -1; // invalid phy register number
  else {
    tMiiMuReg.bf.miimu_snrdy      = 1;
    tMiiMuReg.bf.phy_nres         = 1;
    tMiiMuReg.bf.miimu_preamble   = uMiimuPreamble;
    tMiiMuReg.bf.miimu_mdc_period = uMiimuMdcFreq;
    tMiiMuReg.bf.miimu_rta        = uMiimuRtaField;
    tMiiMuReg.bf.miimu_regaddr    = uMiimuReqAddr;
    tMiiMuReg.bf.miimu_phyaddr    = uMiimuPhyAddr;
  
    // write command to PHY 
    s_ptMiimu->ulMiimu_reg = tMiiMuReg.val;
  
    // wait until ready
    do {
      tMiiMuReg.val = s_ptMiimu->ulMiimu_reg;
    } while( tMiiMuReg.bf.miimu_snrdy );
  
    *pusData = (unsigned short) tMiiMuReg.bf.miimu_data;
  }

  return 0;
}

/*****************************************************************************/
/*! Write PHY Register
* \description
*   Write PHY register over MDIO.
* \class
*   MIIMU 
* \params
*   uMiimuPreamble              [in]  Miimu Preamble
*   uMiimuMdcFreq               [in]  Miimu Mdc Frequence
*   uMiimuPhyAddr               [in]  Miimu PHY Address
*   uMiimuReqAddr               [in]  Miimu Register Address
*   usData                      [in]  Miimu Data
*   pvUser                      [in]  User specific parameter 
* \return
*   0 on success
*   -1 on erroneous                                                          */
/*****************************************************************************/
int NX10_MIIMU_WritePhyReg( unsigned int uMiimuPreamble, unsigned int uMiimuMdcFreq, unsigned int uMiimuPhyAddr, unsigned int uMiimuReqAddr, unsigned short usData, void* pvUser)
{
  NX10_MIIMU_REG_T tMiiMuReg = {0};
  
  if( (uMiimuPhyAddr>31) || (uMiimuReqAddr>31) ) return -1; // invalid phy register number
  else {
    tMiiMuReg.bf.miimu_snrdy        = 1;
    tMiiMuReg.bf.phy_nres           = 1;
    tMiiMuReg.bf.miimu_opmode       = 1;
    tMiiMuReg.bf.miimu_preamble     = uMiimuPreamble;
    tMiiMuReg.bf.miimu_mdc_period   = uMiimuMdcFreq;
    tMiiMuReg.bf.miimu_regaddr      = uMiimuReqAddr;
    tMiiMuReg.bf.miimu_phyaddr      = uMiimuPhyAddr;
    tMiiMuReg.bf.miimu_data         = usData;
  
    // write command to PHY 
    s_ptMiimu->ulMiimu_reg = tMiiMuReg.val;
  
    // wait until ready
    do {
      tMiiMuReg.val = s_ptMiimu->ulMiimu_reg;
    } while( tMiiMuReg.bf.miimu_snrdy );
  }

  return 0;
}
      
/*
  ____                  _     _                    
 / ___|   _   _   ___  | |_  (_)  _ __ ___     ___ 
 \___ \  | | | | / __| | __| | | | '_ ` _ \   / _ \
  ___) | | |_| | \__ \ | |_  | | | | | | | | |  __/
 |____/   \__, | |___/  \__| |_| |_| |_| |_|  \___|
          |___/                                    

*/

/*****************************************************************************/
/*! Set Systime Border
* \description
*   Set the systime border.
* \class 
*   SYSTIME 
* \params
*   ulBorder              [in]  Systime Border Value
*   pvUser                [in]  User specific parameter 
* \return 
*                                                                            */
/*****************************************************************************/
void NX10_SYSTIME_SetBorder( unsigned long ulBorder, void* pvUser )
{
  s_ptSystime->ulSystime_border = ulBorder;  
}

/*****************************************************************************/
/*! Set Systime Speed
* \description
*   Set the systime speed.
* \class 
*   SYSTIME 
* \params
*   ulSpeed               [in]  Systime Speed Value
*   pvUser                [in]  User specific parameter 
* \return 
*                                                                            */
/*****************************************************************************/
void NX10_SYSTIME_SetSpeed( unsigned long ulSpeed, void* pvUser )
{
  s_ptSystime->ulSystime_count_value = ulSpeed;  
}

/*****************************************************************************/
/*! Set Systime
* \description
*   Sets the systime.
* \class 
*   SYSTIME 
* \params
*   ulSystime_s           [in]  Systime Value in Seconds
*   ulSystime_ns          [in]  Systime Value in Nanoseconds
*   pvUser                [in]  User specific parameter 
* \return 
*                                                                            */
/*****************************************************************************/
void NX10_SYSTIME_SetSystime( unsigned long ulSystime_s, unsigned long ulSystime_ns, void* pvUser )
{
  s_ptSystime->ulSystime_s = ulSystime_s;  
  s_ptSystime->ulSystime_ns = ulSystime_ns;
}

/*****************************************************************************/
/*! Get Systime
* \description
*   Read the systime.
* \class 
*   SYSTIME 
* \params
*   pulSystime_s          [in]  Pointer of Systime Value in Seconds
*   pulSystime_ns         [in]  Pointer of Systime Value in Nanoseconds
*   pvUser                [in]  User specific parameter 
* \return 
*                                                                            */
/*****************************************************************************/
void NX10_SYSTIME_GetSystime( unsigned long* pulSystime_s, unsigned long* pulSystime_ns, void* pvUser )
{
  *pulSystime_s  = s_ptSystime->ulSystime_s;  
  *pulSystime_ns = s_ptSystime->ulSystime_ns;  
}



//.##.....##.########..####..######.
//..##...##..##.....##..##..##....##
//...##.##...##.....##..##..##......
//....###....########...##..##......
//...##.##...##.........##..##......
//..##...##..##.........##..##....##
//.##.....##.##........####..######.


/*****************************************************************************/
/*! Load data/code segments
* \description
*   Load dword aligned data into memory
*   Can be used for xPIC program or any other areas
* \class
*   XPIC
* \params
*   pulData [in]  Pointer to microcode
*                                                                            */
/*****************************************************************************/
#define NX10_XPIC_SEGDEF_HEADER_MAGIC              0x43495058
#define NX10_XPIC_SEGDEF_HEADER_MSK_VERSION_MAJOR  0xffff0000
#define NX10_XPIC_SEGDEF_HEADER_MSK_VERSION_MINOR  0x0000ffff
#define NX10_XPIC_SEGDEF_HEADER_VERSION_MAJOR      0x00010000
#define NX10_XPIC_SEGDEF_HEADER_VERSION_MINOR      0x00000000

static void NX10_XPIC_LoadSegments(const unsigned long *pulData)
{
  const unsigned long *pulCur = &pulData[2];
  unsigned long ulDest;
  unsigned long ulLen;

  // check header magic
  if( *pulCur++ != NX10_XPIC_SEGDEF_HEADER_MAGIC ) {
    // invalid header
    return;
  }
  // check major version
  if( (*pulCur++ & NX10_XPIC_SEGDEF_HEADER_MSK_VERSION_MAJOR) > NX10_XPIC_SEGDEF_HEADER_VERSION_MAJOR ) {
    // version not supported
    return;
  }

  ulDest = *pulCur++;
  ulLen = *pulCur++;
  while( ulLen ) {
    // load segment
    while( ulLen-- ) {
      POKE(ulDest, *pulCur++);
      ulDest += 4;
    }
    // read next segment
    ulDest = *pulCur++;
    ulLen = *pulCur++;
  }
}

/*****************************************************************************/
/*! Load xPIC code
* \description
*   Load xPIC code,
*   Initialize trailing loads
* \class
*   XPIC
* \params
*   xpic_code [in]  Pointer to microcode
*   pvUser    [in] ignored
*                                                                            */
/*****************************************************************************/
void NX10_XPIC_Load(const unsigned long *xpic_code, void* pvUser)
{
  unsigned int programm_size;
  unsigned int tl_size;
  unsigned int programm_start_adr;
  unsigned int  i;
  unsigned int  tl_start;
  unsigned long* dummy;

  if( ( xpic_code[0] == 0 ) && ( xpic_code[1] == 0 ) ) {
    // xPIC Toolchain - segment definitions
    NX10_XPIC_LoadSegments(xpic_code);
  } else {
    // xpicass generated program and trailing loads
    programm_size       = xpic_code[0] / 4;  // in Bytes / 4
    tl_size             = xpic_code[1] / 4;
    programm_start_adr  = xpic_code[2];
    tl_start            = programm_size + 2;

    // programm loader
    for (i = 1; i < programm_size; i++){
      dummy = (unsigned long*) (programm_start_adr + (i - 1)*4);
      *dummy = xpic_code[i + 2];
    }

    // trailing loader
    for (i = 0; i < tl_size; i = i + 2){
      dummy = (unsigned long*) (xpic_code[tl_start + i]);
      *dummy = xpic_code[tl_start + i + 1];
    }
  }
}


/*****************************************************************************/
/*! Start xPIC
* \description
*   Start xPIC
* \class 
*   XPIC
* \params
*   pvUser [in] ignored
*                                                                            */
/*****************************************************************************/
void NX10_XPIC_Start(void* pvUser)
{
  s_ptXpicDebug->ulXpic_hold_pc = 0;
}

/*****************************************************************************/
/*! Stop xPIC
* \description
*   Halt xPIC
* \class 
*   XPIC
* \params
*   pvUser [in] ignored
*                                                                            */
/*****************************************************************************/
void NX10_XPIC_Stop(void* pvUser)
{
  s_ptXpicDebug->ulXpic_hold_pc = MSK_NX10_xpic_hold_pc_hold;
}

/*****************************************************************************/
/*! Reset xPIC
* \description
*   Hold xPIC, reset xPIC state, 
*   clear work registers (both banks), clear user registers
* \class 
*   XPIC
* \params
*   pvUser [in] ignored
*
*                                                                            */
/*****************************************************************************/
void NX10_XPIC_Reset(void* pvUser)
{
  unsigned long ulVal, ulCnt;
  
  // hold xPIC
  s_ptXpicDebug->ulXpic_hold_pc = MSK_NX10_xpic_hold_pc_hold;
  
  // disable xPIC VIC
  s_ptXpicVic->ulXpic_vic_config = 0;
  
  // clear all hold reasons
  s_ptXpicDebug->ulXpic_break_irq_raw = 0x1f;

  // hold xPIC and request reset
  s_ptXpicDebug->ulXpic_hold_pc = MSK_NX10_xpic_hold_pc_hold | MSK_NX10_xpic_hold_pc_reset_xpic;
  
  // wait for reset to be finished
  ulCnt = 0;
  do
  {
    ulVal = s_ptXpicDebug->ulXpic_break_status;
  } while( (ulVal & MSK_NX10_xpic_break_status_xpic_reset_status) == 0 );
  
  // release reset request, engage bank control, select bank 1
  s_ptXpicDebug->ulXpic_hold_pc = MSK_NX10_xpic_hold_pc_hold | MSK_NX10_xpic_hold_pc_bank_control | MSK_NX10_xpic_hold_pc_bank_select;
  
  // reset xPIC user registers
  for( ulVal = 0; ulVal < 5; ulVal++ )
  {
    s_ptXpic->aulXpic_usr[ulVal] = 0;
  }
  
  // reset xPIC work registers (bank 1)
  for( ulVal = 0; ulVal < 8; ulVal++ )
  {
    s_ptXpic->aulXpic_r[ulVal] = 0;
  }
  
  // select bank 0
  s_ptXpicDebug->ulXpic_hold_pc = MSK_NX10_xpic_hold_pc_hold | MSK_NX10_xpic_hold_pc_bank_control;
  
  // reset xPIC work registers (bank 0)
  for( ulVal = 0; ulVal < 8; ulVal++ )
  {
    s_ptXpic->aulXpic_r[ulVal] = 0;
  }
  
  // release bank control
  s_ptXpicDebug->ulXpic_hold_pc = MSK_NX10_xpic_hold_pc_hold;
}

/*
    _        _          ____ _        _
   / \   ___(_) ___    / ___| |_ _ __| |
  / _ \ / __| |/ __|  | |   | __| '__| |
 / ___ \\__ \ | (__   | |___| |_| |  | |
/_/   \_\___/_|\___|___\____|\__|_|  |_|
                  |_____|
*/

/*****************************************************************************/
/*! Set IO_Config
* \description
*   Writes a new value to IO_Config.
* \class 
*   ASIC_CTRL
* \params
*   ulVal  [in]  Value for io_config
* \return
*                                                                            */
/*****************************************************************************/
void NX10_AsicCtrl_SetIoConfig(unsigned long ulVal)
{
  s_ptAsicCtrl->ulAsic_ctrl_access_key = s_ptAsicCtrl->ulAsic_ctrl_access_key;
  s_ptAsicCtrl->ulIo_config = ulVal;  
}

/*****************************************************************************/
/*! Get IO_Config
* \description
*   Read the value of IO_Config.
* \class 
*   ASIC_CTRL
* \params
*   pulVal  [out]  Pointer to value of io_config
* \return
*                                                                            */
/*****************************************************************************/
void NX10_AsicCtrl_GetIoConfig(unsigned long* pulVal)
{
  *pulVal = s_ptAsicCtrl->ulIo_config;  
}

/*****************************************************************************/
/*! Set ClockEnable
* \description
*   Writes a new value clock enable register.
* \class 
*   ASIC_CTRL
* \params
*   ulVal  [in]  Value for clock enable
* \return
*                                                                            */
/*****************************************************************************/
void NX10_AsicCtrl_SetClockEnable(unsigned long ulVal)
{
  s_ptAsicCtrl->ulAsic_ctrl_access_key = s_ptAsicCtrl->ulAsic_ctrl_access_key;
  s_ptAsicCtrl->ulClock_enable = ulVal;  
}

/*****************************************************************************/
/*! Get Clock Enable
* \description
*   Read the value of Clock Enable
* \class 
*   ASIC_CTRL
* \params
*   pulVal  [out]  Pointer to value of clock_enable
* \return
*                                                                            */
/*****************************************************************************/
void NX10_AsicCtrl_GetClockEnable(unsigned long* pulVal)
{
  *pulVal = s_ptAsicCtrl->ulClock_enable;  
}

/*****************************************************************************/
/*! Set Fb0Clk
* \description
*   Configure 400MHz rate multiplier for Fieldbus clock.
* \class 
*   ASIC_CTRL
* \params
*   ulFb0ClkRateMulAdd  [in]  Value for Rate multiplier
*   ulFb0ClkDiv         [in]  Value for Divider
* \return
*                                                                            */
/*****************************************************************************/
void NX10_AsicCtrl_SetFb0Clk( unsigned long ulFb0ClkRateMulAdd, unsigned long ulFb0ClkDiv )
{
  s_ptAsicCtrl->ulAsic_ctrl_access_key = s_ptAsicCtrl->ulAsic_ctrl_access_key;
  s_ptAsicCtrl->ulFb0clk_rate_mul_add = ulFb0ClkRateMulAdd;  

  s_ptAsicCtrl->ulAsic_ctrl_access_key = s_ptAsicCtrl->ulAsic_ctrl_access_key;
  s_ptAsicCtrl->ulFb0clk_div = ulFb0ClkDiv;  

}



