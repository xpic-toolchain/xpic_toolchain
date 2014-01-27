/**************************************************************************************
 
   Copyright (c) Hilscher GmbH. All Rights Reserved.
 
 **************************************************************************************
 
   Filename:
    xpicgdb.c
   Last Modification:
    $Author: bilsen $
    $Modtime: 17.02.06 14:54 $
    $Revision: 19 $
   
   Targets:
    Win32/ANSI   : no
    Win32/Unicode: no (define _UNICODE)
    WinCE        : yes
    rcX          : yes
 
   Description:
       
    XpicGdb
    Server Functions
    
   Changes:
 
     Version    Date        Author
        Description
     ----------------------------------------------------------------------------------
              
**************************************************************************************/

/**************************************************************************************/
/* includes */
#include "xpicgdb.h"   /* XpicGdb includes */

#include "common_func_netx10.h"

#include <string.h>

/* A remote command handler called with "monitor xyz" on gdb */
typedef struct XPICGDB_RCMD_HANDLER_Ttag {
  const char* pszQuery;
  unsigned long ulQueryLen;
  void (*pfnHandleRcmd)(XPICGDB_T* ptXpicGdb, char* pszPacket);
} XPICGDB_RCMD_HANDLER_T;

#define DEBUG_HALT while(1)

#define XPICGDB_CNF_POS       1
#define XPICGDB_CNF_NEG       0
#define XPICGDB_SWBP_USED     1
#define XPICGDB_SWBP_UNUSED   0
#define SW_BP_INST            0xc4000010

#define XPICGDB_MAX_RCMD_LEN  32

static const char hexchars[]="0123456789abcdef";

static int
hex(char cVal)
{
  if (cVal >= 'a' && cVal <= 'f')
    return cVal-'a'+10;
  if (cVal >= '0' && cVal <= '9')
    return cVal-'0';
  if (cVal >= 'A' && cVal <= 'F')
    return cVal-'A'+10;
  return -1;
}

void xpicgdb_reset(XPICGDB_T* ptXpicGdb)
{
  int i;

  NX10_XPIC_Reset(NULL);
  ptXpicGdb->abState[XPICGDB_STATE_XPIC] = XPICGDB_STATE_XPIC_TRAP;

  for(i = 0; i < XPICGDB_SWBP_CNT; ++i)
  {
    ptXpicGdb->atSwBp[i].ui_used = XPICGDB_SWBP_UNUSED;
  }
}

XPICGDB_RESULT_E xpicgdb_init(XPICGDB_T* ptXpicGdb)
{
  memset(ptXpicGdb, 0, sizeof(XPICGDB_T));
  xpicgdb_reset(ptXpicGdb);
  return XPICGDB_OK;
}

void xpicgdb_handlePacketCnf(XPICGDB_T* ptXpicGdb, int fCnf)
{

}

void xpicgdb_putChar(XPICGDB_T* ptXpicGdb, char c)
{
  if( ptXpicGdb->ulOutputLen == XPICGDB_OUTBUF_SIZE )
  {
    ptXpicGdb->abState[XPICGDB_STATE_MAIN] = XPICGDB_STATE_MAIN_ERROR;
    DEBUG_HALT;
  }
  else
  {
    ptXpicGdb->abOutputBuffer[ptXpicGdb->ulOutputLen++] = c;
    if( ptXpicGdb->fOutputPacket )
    {
      ptXpicGdb->bOutputChecksum += c;
    }
  }
}

void xpicgdb_startOutputPacket(XPICGDB_T* ptXpicGdb)
{
  xpicgdb_putChar(ptXpicGdb, '$');
  ptXpicGdb->bOutputChecksum = 0;
  ptXpicGdb->fOutputPacket = 1;
}

void xpicgdb_finishOutputPacket(XPICGDB_T* ptXpicGdb)
{
  ptXpicGdb->fOutputPacket = 0;
  xpicgdb_putChar(ptXpicGdb, '#');
  xpicgdb_putChar(ptXpicGdb, hexchars[ptXpicGdb->bOutputChecksum >> 4]);
  xpicgdb_putChar(ptXpicGdb, hexchars[ptXpicGdb->bOutputChecksum & 0xf]);
}

void xpicgdb_waitForSinglestep(XPICGDB_T* ptXpicGdb)
{
  unsigned long ulVal;

  if( ptXpicGdb->fSwBpHappened ) {
    ptXpicGdb->fSwBpHappened = 0;
    // rewind pc
    POKE(Adr_NX10_xpic_pc, PEEK(Adr_NX10_xpic_pc)-4);

    // set singlestep mode on and start xPIC
    POKE( Adr_NX10_xpic_hold_pc, MSK_NX10_xpic_hold_pc_single_step );

    // wait for singlestep
    do
    {
      ulVal = PEEK(Adr_NX10_xpic_break_status);
    } while( ulVal != (MSK_NX10_xpic_break_status_single_step+MSK_NX10_xpic_break_status_xpic_in_hold) );

    // next step
    POKE(Adr_NX10_xpic_break_irq_raw, MSK_NX10_xpic_break_irq_raw_single_step_irq);
  }
  else
  {
    // set singlestep mode on and start xPIC
    POKE( Adr_NX10_xpic_hold_pc, MSK_NX10_xpic_hold_pc_single_step );
  }

  // wait for singlestep
  do
  {
    ulVal = PEEK(Adr_NX10_xpic_break_status);
  } while( (ulVal & MSK_NX10_xpic_break_status_xpic_in_hold) == 0 );

  // HACK stepping over break instruction
  // the gdb removes the breakpoint after it was fetched! We have to rerun this command.
  // This should be corrected in xpic-gdb
  if( ulVal & MSK_NX10_xpic_break_status_soft_break )
  {

    // rewind pc
    POKE(Adr_NX10_xpic_pc, PEEK(Adr_NX10_xpic_pc)-8);
    POKE(Adr_NX10_xpic_break_irq_raw, MSK_NX10_xpic_break_irq_raw_soft_break_irq+MSK_NX10_xpic_break_irq_raw_single_step_irq);
    // wait for singlestep
    do
    {
      ulVal = PEEK(Adr_NX10_xpic_break_status);
    } while( (ulVal & MSK_NX10_xpic_break_status_xpic_in_hold) == 0 );

    // next step
    POKE(Adr_NX10_xpic_break_irq_raw, MSK_NX10_xpic_break_irq_raw_single_step_irq);
  }

  // set singlestep mode off
  POKE( Adr_NX10_xpic_hold_pc, MSK_NX10_xpic_hold_pc_hold );

  NX10_XPIC_Stop(NULL);
  // release xPIC break
  POKE(Adr_NX10_xpic_break_irq_raw, MSK_NX10_xpic_break_irq_raw_single_step_irq);


  // set signal SIGTRAP
  ptXpicGdb->abState[XPICGDB_STATE_XPIC] = XPICGDB_STATE_XPIC_TRAP;

}

/* Remote command "r"eset */
#define XPICGDB_RCMD_HEX_R "72"
#define XPICGDB_RCMD_LEN_R 2
#define XPICGDB_RCMD_HEX_RESET "7265736574"
#define XPICGDB_RCMD_LEN_RESET 10
static void xpicgdb_rcmd_r(XPICGDB_T* ptXpicGdb, char* pszPacket)
{
  /* Perform a soft reset
   * Reset pc, disable VIC
   * leave all breakpoints
   */
  NX10_XPIC_Reset(NULL);
  ptXpicGdb->abState[XPICGDB_STATE_XPIC] = XPICGDB_STATE_XPIC_TRAP;

  xpicgdb_putChar(ptXpicGdb, 'O');
  xpicgdb_putChar(ptXpicGdb, 'K');
}

static XPICGDB_RCMD_HANDLER_T s_atRcmdHandlers[] = {
  {XPICGDB_RCMD_HEX_R,     XPICGDB_RCMD_LEN_R,     xpicgdb_rcmd_r}, // "r"
  {XPICGDB_RCMD_HEX_RESET, XPICGDB_RCMD_LEN_RESET, xpicgdb_rcmd_r}, // "reset"
};

void xpicgdb_handleRcmd(XPICGDB_T* ptXpicGdb, char* pszCmd)
{
  unsigned int uiCnt;

  for(uiCnt = 0; uiCnt < ARRAY_LEN(s_atRcmdHandlers); ++uiCnt)
  {
    if( memcmp(s_atRcmdHandlers[uiCnt].pszQuery, pszCmd, s_atRcmdHandlers[uiCnt].ulQueryLen) == 0 )
    {
      s_atRcmdHandlers[uiCnt].pfnHandleRcmd(ptXpicGdb, &pszCmd[s_atRcmdHandlers[uiCnt].ulQueryLen]);
      break;
    }
  }
}

/**
 * Handle GDB Packet
 */
int xpicgdb_handlePacket(XPICGDB_T* ptXpicGdb)
{
  char *ptr;
  unsigned int uiRegisterValue;
  unsigned int uiCnt;
  unsigned int uiOldCnt;
  unsigned int uiStartAdr;
  unsigned int uiByteCnt;
  unsigned int fSwBpAlreadySet;
  unsigned int i;
  unsigned int uiBAddr,uiOutByteCnt;
  union{
    unsigned int  uiDWORD;
    unsigned char ucBYTEs[4];
  } tUnion;

  if( ptXpicGdb->abState[XPICGDB_STATE_MAIN] == 0 )
  {
    char input;
    unsigned int uiSwBpIndex;
    unsigned int uiSwBpAddr;

    xpicgdb_startOutputPacket(ptXpicGdb);
    ptr = ptXpicGdb->abInputBuffer;
    input = *ptr++;
    switch (input)
    {
      case '?' :
        xpicgdb_putChar(ptXpicGdb, 'S');
        xpicgdb_putChar(ptXpicGdb, '0');
        xpicgdb_putChar(ptXpicGdb, ptXpicGdb->abState[XPICGDB_STATE_XPIC] + '0');
        xpicgdb_finishOutputPacket(ptXpicGdb);
        break;

      case 'c':  // cAA..AA    Continue at address AA..AA(optional)
        if( ptXpicGdb->fSwBpHappened ) {
          ptXpicGdb->fSwBpHappened = 0;
          // rewind pc
          POKE(Adr_NX10_xpic_pc, PEEK(Adr_NX10_xpic_pc)-4);
        }
        NX10_XPIC_Start(NULL);
        ptXpicGdb->ulOutputLen -= 1;
        ptXpicGdb->fOutputPacket = 0;
        ptXpicGdb->abState[XPICGDB_STATE_XPIC] = XPICGDB_STATE_XPIC_RUN;
        return 1;
        break;

      // XXX: irq Register fehlen noch!
      case 'g':  // return the value of the CPU registers

        /* BI: Why do we need this? */
        // hold xPIC and select bank 0
        //ptXpicDebug->ulXpic_hold_pc = MSK_NX10_xpic_hold_pc_hold | MSK_NX10_xpic_hold_pc_bank_control;

        //r0..7, usr0..4, pc, stat, zero
        for (uiCnt = 0; uiCnt < 16; uiCnt++){
          uiRegisterValue = PEEK(Adr_NX10_xpic_r0 + (uiCnt << 2));

          xpicgdb_putChar(ptXpicGdb, hexchars[(uiRegisterValue >> 4) & 0xf]);
          xpicgdb_putChar(ptXpicGdb, hexchars[uiRegisterValue & 0xf]);
          xpicgdb_putChar(ptXpicGdb, hexchars[(uiRegisterValue >> 12) & 0xf]);
          xpicgdb_putChar(ptXpicGdb, hexchars[(uiRegisterValue >> 8) & 0xf]);
          xpicgdb_putChar(ptXpicGdb, hexchars[(uiRegisterValue >> 20) & 0xf]);
          xpicgdb_putChar(ptXpicGdb, hexchars[(uiRegisterValue >> 16) & 0xf]);
          xpicgdb_putChar(ptXpicGdb, hexchars[(uiRegisterValue >> 28) & 0xf]);
          xpicgdb_putChar(ptXpicGdb, hexchars[(uiRegisterValue >> 24) & 0xf]);
        }

        xpicgdb_finishOutputPacket(ptXpicGdb);

        // engage bank control, select bank 1
        //ptXpicDebug->ulXpic_hold_pc = MSK_NX10_xpic_hold_pc_hold | MSK_NX10_xpic_hold_pc_bank_control | MSK_NX10_xpic_hold_pc_bank_select;

        // release bank control
        //ptXpicDebug->ulXpic_hold_pc = MSK_NX10_xpic_hold_pc_hold;
        break;


      case 'G':  // set the value of the CPU registers - return OK

        for (uiCnt = 0; uiCnt < 16; uiCnt++){
          uiRegisterValue = 0;
          uiRegisterValue =
              ( hex(ptr[0]) <<  4 ) +
              ( hex(ptr[1])       ) +
              ( hex(ptr[2]) << 12 ) +
              ( hex(ptr[3]) <<  8 ) +
              ( hex(ptr[4]) << 20 ) +
              ( hex(ptr[5]) << 16 ) +
              ( hex(ptr[6]) << 28 ) +
              ( hex(ptr[7]) << 24 );

          POKE(Adr_NX10_xpic_r0 + (uiCnt << 2), uiRegisterValue);
          ptr += 8;
        }

        xpicgdb_putChar(ptXpicGdb, 'O');
        xpicgdb_putChar(ptXpicGdb, 'K');
        xpicgdb_finishOutputPacket(ptXpicGdb);
        break;

      case 'm':  // mAA..AA,LLLL  Read LLLL (from 1 till BUFMAX/2-1) bytes at address AA..AA (align 1)
        uiStartAdr = 0;
        uiByteCnt = 0;
        // read start address AA..AA:
        for ( uiCnt = 1; uiCnt < 9; uiCnt++)
        {
          if ( ptXpicGdb->abInputBuffer[uiCnt] == ',' ) break;
          uiStartAdr = ( uiStartAdr << 4  );
          uiStartAdr += ( hex( ptXpicGdb->abInputBuffer[uiCnt] ) );
        }
        // read bytes count LLLL
        for (uiCnt++,i=uiCnt+4; uiCnt < i; uiCnt++)
        {
          if ( ptXpicGdb->abInputBuffer[uiCnt] == 0 ) break;
          uiByteCnt = ( uiByteCnt << 4 );
          uiByteCnt += ( hex( ptXpicGdb->abInputBuffer[uiCnt] ) );
        }
        // Address correction:
        if(uiStartAdr<0x00002000)
          uiStartAdr+=Adr_NX10_xpic_pram_start;
        // Read byte-after-byte
        for(uiOutByteCnt=0; uiOutByteCnt < uiByteCnt;uiStartAdr+=4)
        {
          uiBAddr    = uiStartAdr &   3 ;
          uiStartAdr = uiStartAdr & (~3);
          tUnion.uiDWORD = PEEK(uiStartAdr);
          while(uiBAddr<4)
          {
            xpicgdb_putChar(ptXpicGdb, hexchars[(tUnion.ucBYTEs[uiBAddr]& 0xf0)>>4]);
            xpicgdb_putChar(ptXpicGdb, hexchars[ tUnion.ucBYTEs[uiBAddr++]& 0x0f  ]);
            uiOutByteCnt++;
            if(uiOutByteCnt >= uiByteCnt) break;
          }
        }
        xpicgdb_finishOutputPacket(ptXpicGdb);
        break;

      case 'M':  // MAA..AA,LLLL: Write LLLL bytes at address AA.AA return OK
        uiStartAdr = 0;
        uiByteCnt  = 0;
        uiOldCnt   = 0;
        uiRegisterValue = 0;

        // read start address AA..AA:
        for ( uiCnt = 1; uiCnt < 9; uiCnt++)
        {
          if ( ptXpicGdb->abInputBuffer[uiCnt] == ',' ) break;
          uiStartAdr = ( uiStartAdr << 4  );
          uiStartAdr += ( hex( ptXpicGdb->abInputBuffer[uiCnt] ) );
        }
        // read bytes count LLLL
        for (uiCnt++,i=uiCnt+4; uiCnt < i; uiCnt++)
        {
          if ( ptXpicGdb->abInputBuffer[uiCnt] == ':' ) break;
          uiByteCnt = ( uiByteCnt << 4 );
          uiByteCnt += ( hex( ptXpicGdb->abInputBuffer[uiCnt] ) );
        }
        uiCnt++;
        // Address correction:
        if(uiStartAdr<0x00002000)
          uiStartAdr+=Adr_NX10_xpic_pram_start;
        //Write data byte-after-byte
        for(uiOutByteCnt=0; uiOutByteCnt < uiByteCnt;uiStartAdr+=4)
        {
          uiBAddr    = uiStartAdr &   3 ;
          uiStartAdr = uiStartAdr & (~3);
          tUnion.uiDWORD = PEEK(uiStartAdr);
          while(uiBAddr<4)
          {
            tUnion.ucBYTEs[uiBAddr]=( hex(ptXpicGdb->abInputBuffer[uiCnt++] )<< 4 );
            tUnion.ucBYTEs[uiBAddr++]+=( hex(ptXpicGdb->abInputBuffer[uiCnt++]) );
            uiOutByteCnt++;
            if(uiOutByteCnt >= uiByteCnt) break;
          }
          POKE(uiStartAdr, tUnion.uiDWORD);
        }
        xpicgdb_putChar(ptXpicGdb, 'O');
        xpicgdb_putChar(ptXpicGdb, 'K');
        xpicgdb_finishOutputPacket(ptXpicGdb);
        break;

      case 'q':  // Query packets
        if( strncmp("Rcmd,", ptr, 5) == 0 )
          xpicgdb_handleRcmd(ptXpicGdb, &ptr[5]);
        xpicgdb_finishOutputPacket(ptXpicGdb);
        break;

      case 's':  // Single Step
        xpicgdb_waitForSinglestep(ptXpicGdb);
        xpicgdb_putChar(ptXpicGdb, 'S');
        xpicgdb_putChar(ptXpicGdb, '0');
        xpicgdb_putChar(ptXpicGdb, '5');
        xpicgdb_finishOutputPacket(ptXpicGdb);
        break;

      case 'X':  // `X addr,length:XX...' write code to memory
        uiStartAdr = 0;
        uiByteCnt  = 0;
        uiOldCnt   = 0;
        uiRegisterValue = 0;

        // read start address AA..AA:
        for ( uiCnt = 1; uiCnt < 9; uiCnt++)
        {
          if ( ptXpicGdb->abInputBuffer[uiCnt] == ',' ) break;
          uiStartAdr = ( uiStartAdr << 4  );
          uiStartAdr += ( hex( ptXpicGdb->abInputBuffer[uiCnt] ) );
        }
        // read bytes count LLLL
        for (uiCnt++,i=uiCnt+4; uiCnt < i; uiCnt++)
        {
          if ( ptXpicGdb->abInputBuffer[uiCnt] == ':' ) break;
          uiByteCnt = ( uiByteCnt << 4 );
          uiByteCnt += ( hex( ptXpicGdb->abInputBuffer[uiCnt] ) );
        }
        uiCnt++;
        // Address correction:
        if(uiStartAdr<0x00002000)
          uiStartAdr+=Adr_NX10_xpic_pram_start;

        //Write data byte-after-byte
        for(uiOutByteCnt=0; uiOutByteCnt < uiByteCnt; uiStartAdr+=4)
        {
          uiBAddr    = uiStartAdr &   3 ;
          uiStartAdr = uiStartAdr & (~3);
          tUnion.uiDWORD = PEEK(uiStartAdr);
          while(uiBAddr<4)
          {
            char c = ptXpicGdb->abInputBuffer[uiCnt++];
            if( c == '}' )
              c = 0x20 ^ ptXpicGdb->abInputBuffer[uiCnt++];
            tUnion.ucBYTEs[uiBAddr++] = c;
            uiOutByteCnt++;
            if(uiOutByteCnt >= uiByteCnt) break;
          }
          POKE(uiStartAdr, tUnion.uiDWORD);
        }
        xpicgdb_putChar(ptXpicGdb, 'O');
        xpicgdb_putChar(ptXpicGdb, 'K');
        xpicgdb_finishOutputPacket(ptXpicGdb);
        break;


      // XXX length not implemented! needed?
      // XXX send ERR or OK if bp already set?
      case 'Z':  // Z0,addr,length  Insert (`Z0') a memory breakpoint at address addr of size length

        // reset search index pointer
        uiSwBpIndex = 0;


        // check if breakpoint is software breakpoint
        if ( ptXpicGdb->abInputBuffer[1] == '0')
        {
          // read address
          uiSwBpAddr = 0;
          for ( uiCnt = 3; uiCnt < 11; uiCnt++)
          {
            if ( ptXpicGdb->abInputBuffer[uiCnt] == ',')
              break;
            uiSwBpAddr = ( uiSwBpAddr << 4  );
            uiSwBpAddr += ( hex( ptXpicGdb->abInputBuffer[uiCnt] ) );
          }
        }
        else
        {
          xpicgdb_finishOutputPacket(ptXpicGdb);
          break;
        }

// ...Artem.  Address correction:
        if(uiSwBpAddr<0x00002000)
          uiSwBpAddr+=Adr_NX10_xpic_pram_start;

        // search if breakpoint is already set in list 'ptXpicGdb->atSwBp'
        fSwBpAlreadySet = 0;
        for ( uiSwBpIndex = 0; uiSwBpIndex < XPICGDB_SWBP_CNT; uiSwBpIndex++)
        {
          if ( (ptXpicGdb->atSwBp[uiSwBpIndex].ui_addr == uiSwBpAddr) && (ptXpicGdb->atSwBp[uiSwBpIndex].ui_used == XPICGDB_SWBP_USED) )
          {
            fSwBpAlreadySet = 1;
            break;
          }
        }

        // search for free entry in list 'ptXpicGdb->atSwBp'
        if ( fSwBpAlreadySet == 0 )
        {
          uiSwBpIndex = 0;
          while ( (uiSwBpIndex < XPICGDB_SWBP_CNT) && (ptXpicGdb->atSwBp[uiSwBpIndex].ui_used == XPICGDB_SWBP_USED) )
            uiSwBpIndex++;

          // if no free entry found send error to GDB
          if ( uiSwBpIndex >= XPICGDB_SWBP_CNT )
          {
            xpicgdb_putChar(ptXpicGdb, 'E');
            xpicgdb_putChar(ptXpicGdb, '0');
            xpicgdb_putChar(ptXpicGdb, '1');
            xpicgdb_finishOutputPacket(ptXpicGdb);
            break;
          }

          // read xpic instruction at ui_sw_breakpoint_addr and put it into list 'ptXpicGdb->atSwBp'
          ptXpicGdb->atSwBp[uiSwBpIndex].ui_addr = uiSwBpAddr;
          ptXpicGdb->atSwBp[uiSwBpIndex].ui_inst = PEEK(uiSwBpAddr);
          ptXpicGdb->atSwBp[uiSwBpIndex].ui_used = XPICGDB_SWBP_USED;

          // write xpic breakpoint instruction 'break' into memory
          POKE(uiSwBpAddr, SW_BP_INST);

        }
        else
        {
          fSwBpAlreadySet = 0;
        }

        // send OK to GDB
        xpicgdb_putChar(ptXpicGdb, 'O');
        xpicgdb_putChar(ptXpicGdb, 'K');
        xpicgdb_finishOutputPacket(ptXpicGdb);

        break;


      case 'z':

        // reset search index pointer
        uiSwBpIndex = 0;

        // check if breakpoint is software breakpoint
        if ( ptXpicGdb->abInputBuffer[1] == '0')
        {
          // read address
          uiSwBpAddr = 0;
          for ( uiCnt = 3; uiCnt < 11; uiCnt++)
          {
            if ( ptXpicGdb->abInputBuffer[uiCnt] == ',')
              break;
            uiSwBpAddr = ( uiSwBpAddr << 4  );
            uiSwBpAddr += ( hex( ptXpicGdb->abInputBuffer[uiCnt] ) );
          }
        }
        else
        {
          xpicgdb_finishOutputPacket(ptXpicGdb);
          break;
        }

// ...Artem.  Address correction:
        if(uiSwBpAddr<0x00002000)
          uiSwBpAddr+=Adr_NX10_xpic_pram_start;

        // search for breakpoint in list 'ptXpicGdb->atSwBp'
        for ( uiSwBpIndex = 0; uiSwBpIndex < XPICGDB_SWBP_CNT; uiSwBpIndex++)
        {
          if ( (ptXpicGdb->atSwBp[uiSwBpIndex].ui_addr == uiSwBpAddr) && (ptXpicGdb->atSwBp[uiSwBpIndex].ui_used == XPICGDB_SWBP_USED) )
            break;
        }

          // if breakpoint not found send error to GDB
        if ( uiSwBpIndex >= XPICGDB_SWBP_CNT )
        {
          xpicgdb_putChar(ptXpicGdb, 'E');
          xpicgdb_putChar(ptXpicGdb, '0');
          xpicgdb_putChar(ptXpicGdb, '1');
          xpicgdb_finishOutputPacket(ptXpicGdb);
          break;
        }

        // remove breakpoint from list 'ptXpicGdb->atSwBp' and write old instruction to memory
        POKE(uiSwBpAddr, ptXpicGdb->atSwBp[uiSwBpIndex].ui_inst);
        ptXpicGdb->atSwBp[uiSwBpIndex].ui_used = XPICGDB_SWBP_UNUSED;

        // send OK to GDB
        xpicgdb_putChar(ptXpicGdb, 'O');
        xpicgdb_putChar(ptXpicGdb, 'K');
        xpicgdb_finishOutputPacket(ptXpicGdb);

        break;


      case 'k':
        ptXpicGdb->abState[XPICGDB_STATE_XPIC] = XPICGDB_STATE_XPIC_KILL;
        return 0;
      default:
        xpicgdb_finishOutputPacket(ptXpicGdb);



    } // end switch

  } // end if

  return 0;
} // end breakpoint

// ----------------------------------------------------

void xpicgdb_output(XPICGDB_T* ptXpicGdb)
{
  unsigned long ulLen = ptXpicGdb->ulOutputLen - ptXpicGdb->ulOutputPos;

  if( ulLen )
  {
    ptXpicGdb->pfnOutput(ptXpicGdb->pvAppCtx, &ptXpicGdb->abOutputBuffer[ptXpicGdb->ulOutputPos], &ulLen);
    ptXpicGdb->ulOutputPos += ulLen;
    if( ptXpicGdb->ulOutputLen == ptXpicGdb->ulOutputPos )
    {
      ptXpicGdb->ulOutputLen = 0;
      ptXpicGdb->ulOutputPos = 0;
    }
  }
}

void xpicgdb_handleSuspendRequest(XPICGDB_T* ptXpicGdb)
{
  NX10_XPIC_Stop(NULL);
  ptXpicGdb->abState[XPICGDB_STATE_XPIC] = XPICGDB_STATE_XPIC_INT;

  xpicgdb_startOutputPacket(ptXpicGdb);
  xpicgdb_putChar(ptXpicGdb, 'S');
  xpicgdb_putChar(ptXpicGdb, '0');
  xpicgdb_putChar(ptXpicGdb, ptXpicGdb->abState[XPICGDB_STATE_XPIC] + '0');
  xpicgdb_finishOutputPacket(ptXpicGdb);
}

static void xpicgdb_updateXpicState(XPICGDB_T* ptXpicGdb)
{
  unsigned long ulVal;

  if( ptXpicGdb->abState[XPICGDB_STATE_XPIC] == XPICGDB_STATE_XPIC_RUN )
  {
    ulVal = PEEK(Adr_NX10_xpic_break_status);
    if( ulVal & MSK_NX10_xpic_break_status_xpic_in_hold )
    {
      /* TODO: get correct hold reason */
      ptXpicGdb->fSwBpHappened = 1;

      /* rewind pc by 4 (1 instruction) */
      ulVal = PEEK(Adr_NX10_xpic_pc);
      ulVal -= 4;
      POKE(Adr_NX10_xpic_pc, ulVal);

      /* confirm xPIC break irq */
      NX10_XPIC_Stop(NULL);
      POKE(Adr_NX10_xpic_break_irq_raw, MSK_NX10_xpic_break_irq_raw_soft_break_irq);

      /* inform client */
      ptXpicGdb->abState[XPICGDB_STATE_XPIC] = XPICGDB_STATE_XPIC_TRAP;
      xpicgdb_startOutputPacket(ptXpicGdb);
      xpicgdb_putChar(ptXpicGdb, 'S');
      xpicgdb_putChar(ptXpicGdb, '0');
      xpicgdb_putChar(ptXpicGdb, ptXpicGdb->abState[XPICGDB_STATE_XPIC] + '0');
      xpicgdb_finishOutputPacket(ptXpicGdb);
    }
  }
}

static void xpicgdb_input(XPICGDB_T* ptXpicGdb, char* pchInput, unsigned long ulLen)
{
  char input;

  while( ulLen-- )
  {
    input = *pchInput++;
    switch( ptXpicGdb->abState[XPICGDB_STATE_PACKET] )
    {
    case XPICGDB_STATE_PACKET_NONE:
      switch(input)
      {
      case '+':
        xpicgdb_handlePacketCnf(ptXpicGdb, XPICGDB_CNF_POS);
        break;
      case '-':
        xpicgdb_handlePacketCnf(ptXpicGdb, XPICGDB_CNF_NEG);
        break;
      case '$':
        ptXpicGdb->abState[XPICGDB_STATE_PACKET] = XPICGDB_STATE_PACKET_READ;
        ptXpicGdb->bInputChecksum = 0;
        ptXpicGdb->ulInputLen = 0;
        break;
      case 3: /* CTRL+C */
        xpicgdb_handleSuspendRequest(ptXpicGdb);
        break;
      default:
        xpicgdb_putChar(ptXpicGdb, '-');
        ulLen = 0;
      }
      break;

    case XPICGDB_STATE_PACKET_READ:
      if( input == '#' )
      {
        /* end of packet */
        ptXpicGdb->abState[XPICGDB_STATE_PACKET] = XPICGDB_STATE_PACKET_FIN1;
        ptXpicGdb->abInputBuffer[ptXpicGdb->ulInputLen] = '\0';
      }
      else
      {
        /* read packet char */
        ptXpicGdb->abInputBuffer[ptXpicGdb->ulInputLen++] = input;
        ptXpicGdb->bInputChecksum += input;
      }
      break;

    case XPICGDB_STATE_PACKET_FIN1:
      ptXpicGdb->abState[XPICGDB_STATE_PACKET] = XPICGDB_STATE_PACKET_FIN2;
      ptXpicGdb->bInputChecksum -= (hex(input)<<4);
      break;

    case XPICGDB_STATE_PACKET_FIN2:
      ptXpicGdb->bInputChecksum -= hex(input);
      ptXpicGdb->abState[XPICGDB_STATE_PACKET] = XPICGDB_STATE_PACKET_NONE;
      if( ptXpicGdb->bInputChecksum == 0 )
      {
        /* packet recieved successfully -> send positive confirmation and handle packet */
        xpicgdb_putChar(ptXpicGdb, '+');
        xpicgdb_handlePacket(ptXpicGdb);
        if( ptXpicGdb->abState[XPICGDB_STATE_XPIC] == XPICGDB_STATE_XPIC_KILL )
        {
          xpicgdb_reset(ptXpicGdb);
        }
      }
      else
      {
        /* packet checksum mismatch -> send negative confirmation and drop packet */
        xpicgdb_putChar(ptXpicGdb, '-');
      }
      break;

    default:
      ptXpicGdb->abState[XPICGDB_STATE_MAIN] = XPICGDB_STATE_MAIN_ERROR;
      DEBUG_HALT;
    }
  }
}

void xpicgdb_process(XPICGDB_T* ptXpicGdb, char* pchInput, unsigned long ulLen)
{
  /* check xPIC state */
  xpicgdb_updateXpicState(ptXpicGdb);

  /* handle client input */
  xpicgdb_input(ptXpicGdb, pchInput, ulLen);

  /* send buffered output */
  xpicgdb_output(ptXpicGdb);
}

