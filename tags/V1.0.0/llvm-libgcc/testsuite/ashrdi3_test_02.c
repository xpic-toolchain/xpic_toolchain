/*
 * ashrdi3_test_02.c
 *
 * Returns: arithmetic a >> b
 *
 * Example:
 * a = 4;
 * b = a >> 1;   // b == 2;
 * c = b >> 1;   // c == 1;
 * d = 8 >> 3;   // d == 1;
 *
 * Precondition:  0 <= b < bits_in_dword
 *
 */

long long __ashrdi3 ( long long a, int b );

long long *pvar;

int main ( void )
{
  long long var = 0;
  pvar = &var;

  if ((__ashrdi3(1, 1)) != 0)
      return ( -1 );
  if ((__ashrdi3(1, 0)) != 1)
      return ( -1 );

  var = __ashrdi3(0x8000000000000000uLL, 0);
  if (var != 0x8000000000000000uLL)
      return ( -1 );

  var = __ashrdi3(0x8000000000000000uLL, 1);
  if (var != 0xC000000000000000uLL)
     return ( -1 );

  var = __ashrdi3(0x8000000000000000uLL, 2);
  if (var != 0xE000000000000000uLL)
     return ( -1 );

  var = __ashrdi3(0x8000000000000000uLL, 3);
  if (var != 0xF000000000000000uLL)
     return ( -1 );

  var = __ashrdi3(0x8000000000000000uLL, 4);
  if (var != 0xF800000000000000uLL)
     return ( -1 );

  var = __ashrdi3(0x0000000000000002uLL, 1);
  if (var != 0x0000000000000001uLL)
     return ( -1 );

  var = __ashrdi3(0x0000000000000004uLL, 2);
  if (var != 0x0000000000000001uLL)
     return ( -1 );

  var = __ashrdi3(0x0000000000000008uLL, 3);
  if (var != 0x0000000000000001uLL)
     return ( -1 );










  return ( 0 );
}
