/*
 * addsf3_test_02.c
 *
 * — Runtime Function: float __addsf3 (float a, float b)
 *
 * These functions return the sum of a and b.
 *
 */

float __addsf3 (float a, float b);

int main ( void )
{
  float a, b, c;
  a = 0.0f;
  b = 0.0f;
  c = __addsf3( a, b );
  if ( c != 0.0f )
    return ( -1 );

  a = 1.0f;
  b = 0.0f;
  c = __addsf3( a, b );
  if ( c != 1.0f )
    return ( -1 );

  a = 0.0f;
  b = 1.0f;
  c = __addsf3( a, b );
  if ( c != 1.0f )
    return ( -1 );

  a = 1.0f;
  b = 1.0f;
  c = __addsf3( a, b );
  if ( c != 2.0f )
    return ( -1 );

  a = -1.0f;
  b = -1.0f;
  c = __addsf3( a, b );
  if ( c != -2.0f )
    return ( -1 );

  a = -1.0f;
  b = 0.0f;
  c = __addsf3( a, b );
  if ( c != -1.0f )
    return ( -1 );

  a = 0.0f;
  b = -1.0f;
  c = __addsf3( a, b );
  if ( c != -1.0f )
    return ( -1 );

  a = 1.0f;
  b = -1.0f;
  c = __addsf3( a, b );
  if ( c != -0.0f )
    return ( -1 );

  a = -1.0f;
  b = 1.0f;
  c = __addsf3( a, b );
  if ( c != -0.0f )
    return ( -1 );

  a = -1233233.222f;
  b = 22323331.232f;
  c = __addsf3( a, b );
  if ( c != 2.1090098e+07f )
    return ( -1 );

  a = 1233233.222f;
  b = -22323331.232f;
  c = __addsf3( a, b );
  if ( c != -2.1090098e+07f )
    return ( -1 );

  return ( 0 );
}
