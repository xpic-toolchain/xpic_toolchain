/* BI: The fiq handler has the wrong prologue/epilogue
 */

#include "NX10_XpicVic.h"

#define MSK_SW_IRQ 0x00000001UL

static volatile int s_iCnt;

void FIQ isr_count(void)
{
  s_iCnt++;
  NX10_XpicVic_ClearSw0Irq(MSK_SW_IRQ);
}

int main(void)
{
  int i;

  s_iCnt = 0;

  NX10_XpicVic_Reset();

  NX10_XpicVic_ClearSw0Irq(MSK_SW_IRQ);

  NX10_XpicVic_SetFiqHandler(isr_count);
  NX10_XpicVic_SetFiqSrc(NX10_XPICVIC_INTSRC_SOFTWARE, 1);

  NX10_XpicVic_Enable(1);

  NX10_XpicVic_SetSw0Irq(MSK_SW_IRQ);

  for( i = 0; i < 1000; ++i )
  {
    if( s_iCnt != 0 )
      break;
  }

  if( s_iCnt == 0 )
    return 1;

  return 0;
}
