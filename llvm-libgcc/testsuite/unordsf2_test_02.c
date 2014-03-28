/*
 * unordsf2_test_02.c
 *
 * — Runtime Function: int __unordsf2 (float a, float b)
 *
 * These functions return a nonzero value if either argument is NaN,
 * otherwise 0.
 *
 */

int __unordsf2 (float a, float b);

long *pa; // For debug purpose only
long *pb; // For debug purpose only

int main ( void )
{
  union fl_a {
    float fvalue;
    long lvalue;
  } a;
  union fl_b {
    float fvalue;
    long lvalue;
  } b;
  int ret;

  a.fvalue = 3.40282e+038f; // Max. float value
  b.fvalue = 3.40282346638528860e+38f; // Max. float value
  ret = __unordsf2 ( a.fvalue, b.fvalue );
  if ( ret != 0 )
    return ( 1 );

  a.fvalue = 1.40129e-45f; // Min. float value
  b.fvalue = 1.40129846432481707e-45f; // Min. float value
  ret = __unordsf2 ( a.fvalue, b.fvalue );
  if ( ret != 0 )
    return ( 2 );

  a.fvalue = 1.25f;
  b.fvalue = 4.87f;
  ret = __unordsf2 ( a.fvalue, b.fvalue );
  if ( ret != 0 )
    return ( 3 );

  a.fvalue = -1.25f;
  b.fvalue = -4.87f;
  ret = __unordsf2 ( a.fvalue, b.fvalue );
  if ( ret != 0 )
    return ( 4 );

  a.fvalue = 0.0f;
  b.fvalue = 0.0f;
  ret = __unordsf2 ( a.fvalue, b.fvalue );
  if ( ret != 0 )
    return ( 5 );

  a.lvalue = 0xFFFFFFFF; // -NaN (according to IEEE-754 Floating-Point Conversion)
  b.fvalue = 0.0f;
  ret = __unordsf2 ( a.fvalue, b.fvalue );
  if ( ret == 0 )
    return ( 6 );

  a.lvalue = 0x7FFFFFFF; // NaN (according to IEEE-754 Floating-Point Conversion)
  b.fvalue = 0.0f;
  ret = __unordsf2 ( a.fvalue, b.fvalue );
  if ( ret == 0 )
    return ( 7 );

  a.lvalue = 0xFFFFFFFF; // -NaN (according to IEEE-754 Floating-Point Conversion)
  b.lvalue = 0xFFFFFFFF; // -NaN (according to IEEE-754 Floating-Point Conversion)
  ret = __unordsf2 ( a.fvalue, b.fvalue );
  if ( ret == 0 )
    return ( 8 );

  a.lvalue = 0x7FFFFFFF; // NaN (according to IEEE-754 Floating-Point Conversion)
  b.lvalue = 0x7FFFFFFF; // NaN (according to IEEE-754 Floating-Point Conversion)
  ret = __unordsf2 ( a.fvalue, b.fvalue );
  if ( ret == 0 )
    return ( 9 );

  return ( 0 );
}
