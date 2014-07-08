#include "main.h"

int caller( void )
{
  int temp = 0x0;
  temp = longarray2[68000] | 4711;
  return temp;
}