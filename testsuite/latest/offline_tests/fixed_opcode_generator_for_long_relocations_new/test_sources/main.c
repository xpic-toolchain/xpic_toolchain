// define a global variable
// use a size bigger than 64kByte
unsigned long longarray1[68000] = { [0 ... 67999] = 0x12345678 };
unsigned long longarray2[68000] = { [0 ... 67999] = 0x22345678 };
unsigned long longarray3[68000] = { [0 ... 67999] = 0x32345678 };

int caller (void);

int main( void )
{
  unsigned long ulTemp = 0;
  longarray1[24] = longarray1[67950] + longarray2[67950] + longarray3[67950];
  ulTemp = longarray2[67933] + longarray3[6792];
  return caller();
}
