/* BI: The fiq handler has the wrong prologue/epilogue
 */
#ifdef __NETX51
#include "NX51_XpicVic.h"
#define XpicVic_Reset            NX51_XpicVic_Reset
#define XpicVic_ClearSw0Irq      NX51_XpicVic_ClearSw0Irq
#define XpicVic_SetFiqHandler    NX51_XpicVic_SetFiqHandler
#define XpicVic_SetFiqSrc        NX51_XpicVic_SetFiqSrc
#define XpicVic_Enable           NX51_XpicVic_Enable
#define XpicVic_SetSw0Irq        NX51_XpicVic_SetSw0Irq
#define XPICVIC_INTSRC_SOFTWARE  NX51_XPICVIC_INTSRC_SOFTWARE
#endif

#ifdef __NETX10
#include "NX10_XpicVic.h"
#define XpicVic_Reset            NX10_XpicVic_Reset
#define XpicVic_ClearSw0Irq      NX10_XpicVic_ClearSw0Irq
#define XpicVic_SetFiqHandler    NX10_XpicVic_SetFiqHandler
#define XpicVic_SetFiqSrc        NX10_XpicVic_SetFiqSrc
#define XpicVic_Enable           NX10_XpicVic_Enable
#define XpicVic_SetSw0Irq        NX10_XpicVic_SetSw0Irq
#define XPICVIC_INTSRC_SOFTWARE  NX10_XPICVIC_INTSRC_SOFTWARE
#endif

#define MSK_SW_IRQ 0x00000001UL

static volatile int s_iCnt;

void FIQ isr_count(void)
{
  s_iCnt++;
  XpicVic_ClearSw0Irq(MSK_SW_IRQ);
}

int main(void)
{
  int i;

  s_iCnt = 0;

  XpicVic_Reset();

  XpicVic_ClearSw0Irq(MSK_SW_IRQ);

  XpicVic_SetFiqHandler(isr_count);
  XpicVic_SetFiqSrc(XPICVIC_INTSRC_SOFTWARE, 1);

  XpicVic_Enable(1);
#ifdef __NETX51
  /* netx51 has xpic version 2 with additional irq-commands  */
  xpic2_enable_int(3);
#endif

  XpicVic_SetSw0Irq(MSK_SW_IRQ);

  for( i = 0; i < 1000; ++i )
  {
    if( s_iCnt != 0 )
      break;
  }

  if( s_iCnt == 0 )
    return 1;

  return 0;
}
