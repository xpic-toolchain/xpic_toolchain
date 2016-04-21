
extern unsigned long longarray2[68000];

int caller( void )
{
  int temp = 0x0;
  // access a variable, which is located far away from source code
  temp = longarray2[67999] | 4711;
  return temp;
}