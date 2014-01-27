/*
 * subsf3_test_02.c
 *
 * — Runtime Function: float __subsf3 (float a, float b)
 *
 * These functions return the difference between b and a; that is, a - b.
 *
 */

float __subsf3 (float a, float b);

int main ( void )
{
  float a, b ,c;

  a = 0.0f;
  b = 0.0f;
  c = __subsf3( a, b );
  if ( c != 0.0f )
    return ( -1 );

  a = -0.0f;
  b = 0.0f;
  c = __subsf3( a, b );
  if ( c != 0.0f )
    return ( -1 );

  a = 0.0f;
  b = -0.0f;
  c = __subsf3( a, b );
  if ( c != 0.0f )
    return ( -1 );

  a = -0.0f;
  b = -0.0f;
  c = __subsf3( a, b );
  if ( c != 0.0f )
    return ( -1 );

  a = 2.0f;
  b = 2.0f;
  c = __subsf3 ( a, b );
  if ( c != 0.0f )
    return ( -1 );

  a = -2.0f;
  b = 2.0f;
  c = __subsf3 ( a, b );
  if ( c != -4.0f )
    return ( -1 );

  a = 2.0f;
  b = -2.0f;
  c = __subsf3 ( a, b );
  if ( c != 4.0f )
    return ( -1 );

  a = -2.0f;
  b = -2.0f;
  c = __subsf3 ( a, b );
  if ( c != 0.0f )
    return ( -1 );

  a = 322.35f;
  b = 2334.25f;
  c = __subsf3 ( a, b );
  if ( c != -2011.9f )
    return ( -1 );

  a = -322.35f;
  b = 2334.25f;
  c = __subsf3 ( a, b );
  if ( c != -2656.6f )
    return ( -1 );

  a = 1.0f;
  b = 1.0f;
  c = __subsf3 ( a, b );
  if ( c != 0.0f )
    return ( -1 );

  a = -1.0f;
  b = 1.0f;
  c = __subsf3 ( a, b );
  if ( c != -2.0f )
    return ( -1 );

  a = 1.0f;
  b = -1.0f;
  c = __subsf3 ( a, b );
  if ( c != 2.0f )
    return ( -1 );

  a = -1.0f;
  b = -1.0f;
  c = __subsf3 ( a, b );
  if ( c != 0.0f )
    return ( -1 );

  a = -1.2587f;
  b = -1.2587f;
  c = __subsf3 ( a, b );
  if ( c != 0.0f )
    return ( -1 );

  a = 1.2587f;
  b = 1.2587f;
  c = __subsf3 ( a, b );
  if ( c != 0.0f )
    return ( -1 );

  a = 3.40282e+038f;
  b = 0.0f;
  c = __subsf3 ( a, b );
  if ( c != 3.40282e+038f )
    return ( -1 );

  a = 3.40282e+038f;
  b = 3.40282e+038f;
  c = __subsf3 ( a, b );
  if ( c != 0.0f )
    return ( -1 );

  a = 1.40129e-45f;
  b = 0.0f;
  c = __subsf3 ( a, b );
  if ( c != 1.40129e-45f )
    return ( -1 );

  a = 1.40129e-45f;
  b = 1.40129e-45f;
  c = __subsf3 ( a, b );
  if ( c != 0.0f )
    return ( -1 );

  return ( 0 );
}
