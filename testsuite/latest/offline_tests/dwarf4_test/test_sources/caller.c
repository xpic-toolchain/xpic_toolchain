
extern unsigned long longarray2[68];

int caller( void )
{
  int temp = 0x0;
  // access a variable, which is located far away from source code
  temp = longarray2[67] | 4711;
  return temp;
}