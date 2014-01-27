/**************************************************************************************
 
   Copyright (c) Hilscher GmbH. All Rights Reserved.
 
 **************************************************************************************
 
   Filename:
    xpicgdb.h
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

#ifndef __XPIC_GDB_H__
#define __XPIC_GDB_H__

#define XPICGDB_INBUF_SIZE 512
#define XPICGDB_OUTBUF_SIZE 2048

typedef enum {
  XPICGDB_STATE_MAIN = 0,
  XPICGDB_STATE_PACKET,
  XPICGDB_STATE_XPIC,
  XPICGDB_STATE_RESERVED2,
  XPICGDB_STATE_CNT
} XPICGDB_STATE;

#define XPICGDB_STATE_MAIN_ERROR 0xff

#define XPICGDB_STATE_PACKET_NONE  0
#define XPICGDB_STATE_PACKET_READ  2
#define XPICGDB_STATE_PACKET_FIN1  3
#define XPICGDB_STATE_PACKET_FIN2  4

#define XPICGDB_STATE_XPIC_RUN    0
#define XPICGDB_STATE_XPIC_INT    2
#define XPICGDB_STATE_XPIC_TRAP   5
#define XPICGDB_STATE_XPIC_KILL  0xff


#define XPICGDB_SWBP_CNT 128

typedef struct XPICGDB_SWBP_Ttag {
  unsigned int ui_addr;
  unsigned int ui_inst;
  unsigned int ui_used;
} XPICGDB_SWBP_T;

typedef struct XPICGDB_Ttag {
  unsigned char abState[XPICGDB_STATE_CNT];

  /* set by application */
  void (*pfnOutput)(void* pvAppCtx, const char* pbData, unsigned long* pulCnt);
  void* pvAppCtx;

  char abInputBuffer[XPICGDB_INBUF_SIZE];
  unsigned long ulInputLen;
  unsigned char bInputChecksum;
  char abOutputBuffer[XPICGDB_OUTBUF_SIZE];
  unsigned long ulOutputPos;
  unsigned long ulOutputLen;
  unsigned char bOutputChecksum;
  int fOutputPacket;

  int fSwBpHappened;

  XPICGDB_SWBP_T atSwBp[XPICGDB_SWBP_CNT];
} XPICGDB_T;

typedef enum {
  XPICGDB_OK = 0,
} XPICGDB_RESULT_E;

XPICGDB_RESULT_E xpicgdb_init(XPICGDB_T* ptXpicGdb);

void xpicgdb_process(XPICGDB_T* ptXpicGdb, char* pchInput, unsigned long ulLen);

#endif /* __XPIC_GDB_H__ */

