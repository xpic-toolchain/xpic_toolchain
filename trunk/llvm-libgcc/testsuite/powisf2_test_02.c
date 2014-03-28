/*
 * powisf2_test_02.c
 *
 * — Runtime Function: float __powisf2 (float a, int b)
 *
 * These functions convert raise a to the power b. [ a^b ]
 *
 */

float *pc;

#define epsilon 0.1f

float __powisf2 (float a, int b);

int main ( void )
{
  float a, c;
  int b;
  pc = &c;

  a = 0.0f;
  b = 0;
  c = __powisf2( a, b );
  if ( c != 1.0 )
    return ( -1 );

  a = 2.0f;
  b = 10;
  c = __powisf2( a, b );
  if ( c != 1024.0f )
    return ( -1 );

  a = 2.0f;
  b = 20;
  c = __powisf2( a, b );
  if ( c != 1048576.0f )
    return ( -1 );

  a = 2.5f;
  b = 10;
  c = __powisf2( a, b );
  if ( !(c <= 9536.7431f+epsilon && c >= 9536.7431f-epsilon) )
    return ( -1 );

  a = 2.0f;
  b = -10;
  c = __powisf2( a, b );
  if ( c != 0.0009765625f )
    return ( -1 );

  a = 1.0f;
  b = 0;
  c = __powisf2( a, b );
  if ( c != 1.0f )
    return ( -1 );

  a = 6.9f;
  b = 5;
  c = __powisf2( a, b );
  if ( !(c <= 15640.3135f+epsilon && c >= 15640.3135f-epsilon) )
    return ( -1 );

  return ( 0 );
}
