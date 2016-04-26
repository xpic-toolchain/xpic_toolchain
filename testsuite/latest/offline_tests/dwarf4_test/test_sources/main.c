// any test case is fine

// define a global variable
unsigned long longarray1[68] = { [0 ... 67] = 0x12345678 };
unsigned long longarray2[68] = { [0 ... 67] = 0x22345678 };
unsigned long longarray3[68] = { [0 ... 67] = 0x32345678 };

int caller (void);

int main( void )
{
  unsigned long ulTemp = 0;
  longarray1[24] = longarray1[67] + longarray2[67] + longarray3[67];
  ulTemp = longarray2[62] + longarray3[63];
  return caller();
}
