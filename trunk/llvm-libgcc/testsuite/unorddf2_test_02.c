/*
 * unorddf2_test_02.c
 *
 * — Runtime Function: int __unorddf2 (double a, double b)
 *
 *  These functions return a nonzero value if either argument is NaN,
 *  otherwise 0.
 *
 */

int __unorddf2 (double a, double b);

long long *pa; // For debug purpose only
long long *pb; // For debug purpose only

int main ( void )
{
  union dll_a {
    double dvalue;
    long long llvalue;
  } a;
  union dll_b {
    double dvalue;
    long long llvalue;
  } b;
  int ret;
  pa = &a;
  pb = &b;
  a.dvalue = 1.25;
  b.dvalue = 4.87;
  ret = __unorddf2 ( a.dvalue, b.dvalue );
  if ( ret != 0 )
    return ( 1 );

  a.dvalue = -1.25;
  b.dvalue = -4.87;
  ret = __unorddf2 ( a.dvalue, b.dvalue );
  if ( ret != 0 )
    return ( 2 );

  a.dvalue = 0.0;
  b.dvalue = 0.0;
  ret = __unorddf2 ( a.dvalue, b.dvalue );
  if ( ret != 0 )
    return ( 3 );

  a.dvalue = 1.7976931348623158e+308 ; // Max. double value
  b.dvalue = 2.2250738585072014e-308; // Min. double value
  ret = __unorddf2 ( a.dvalue, b.dvalue );
  if ( ret != 0 )
    return ( 4 );

  a.llvalue = 0xFFFFFFFFFFFFFFFFLL; // -NaN (according to IEEE-754 Floating-Point Conversion)
  b.dvalue = 0.0;
  ret = __unorddf2 ( a.dvalue, b.dvalue );
  if ( ret == 0 )
    return ( 5 );

  a.llvalue = 0x7FFFFFFFFFFFFFFFLL; // NaN (according to IEEE-754 Floating-Point Conversion)
  b.dvalue = 0.0;
  ret = __unorddf2 ( a.dvalue, b.dvalue );
  if ( ret == 0 )
    return ( 6 );

  a.llvalue = 0x7FFFFFFFFFFFFFFFLL; // -NaN (according to IEEE-754 Floating-Point Conversion)
  b.llvalue = 0x7FFFFFFFFFFFFFFFLL; // -NaN (according to IEEE-754 Floating-Point Conversion)
  ret = __unorddf2 ( a.dvalue, b.dvalue );
  if ( ret == 0 )
    return ( 7 );

  a.llvalue = 0xFFFFFFFFFFFFFFFFLL; // NaN (according to IEEE-754 Floating-Point Conversion)
  b.llvalue = 0xFFFFFFFFFFFFFFFFLL; // NaN (according to IEEE-754 Floating-Point Conversion)
  ret = __unorddf2 ( a.dvalue, b.dvalue );
  if ( ret == 0 )
    return ( 8 );

return ( 0 );
}
