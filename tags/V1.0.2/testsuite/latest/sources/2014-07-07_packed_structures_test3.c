#define __PACKED_PRE
#define __PACKED_POST __attribute__ ((__packed__))

#define PEEK(addr)      (*(unsigned int *)(addr))

#if 0
#define WORKAROUND_PRE 
#define WORKAROUND_POST
#else
#define WORKAROUND_PRE __PACKED_PRE
#define WORKAROUND_POST __PACKED_POST
#endif

#include <stdint.h>


//new layout zone 2
typedef __PACKED_PRE struct T_AT_CM_AREA2_Ttag
{
    unsigned short usPciVendorID;          /*PCI Settings*/
    unsigned short usPciDeviceID;
    unsigned char  bPciSubClassCode;
    unsigned char  bPciClassCode;
    unsigned short usPciSubSystemVendorID;
    unsigned short usPciSubSystemDeviceID;
    
    union T_PCI_TARGET_SIZE_Utag
    {
      struct WORKAROUND_PRE T_PCI_TARGET_SIZE_BF_Ttag
      {
        unsigned int uiPciSizeTarget:24;  /* 2nd target size */
        unsigned int uiPciSizeIO:8;       /* I/O target size */
      } WORKAROUND_POST tTarget;
      unsigned long ulTarget;
    } uTarget;

    unsigned char  bZone2Revision;          /* Revision Structure of Zone 2 */
    unsigned char  bZone2Checksum;          /* CRC8-ATM Byte 0 till 30 */
} __PACKED_POST T_AT_CM_AREA2_T, *T_PAT_CM_AREA2_T;


int main( void )
{
  volatile T_AT_CM_AREA2_T
    tCmArea2;
  
  
  tCmArea2.usPciVendorID = 0x1111;
  tCmArea2.usPciVendorID = 0x2222;
  tCmArea2.usPciDeviceID = 0x3333;
  tCmArea2.bPciSubClassCode = 0x44;
  tCmArea2.bPciClassCode = 0x55;
  tCmArea2.usPciSubSystemVendorID = 0x6666;
  tCmArea2.usPciSubSystemDeviceID = 0x7777;
  
  tCmArea2.uTarget.tTarget.uiPciSizeTarget = tCmArea2.bPciClassCode + tCmArea2.usPciSubSystemVendorID;
  
  tCmArea2.uTarget.tTarget.uiPciSizeIO = tCmArea2.uTarget.tTarget.uiPciSizeTarget & 0xFF;
  
  return PEEK(0x1018c4bc);
}
