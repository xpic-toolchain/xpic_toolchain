/*
 * gedf2_test_02.c
 *
 * — Runtime Function: int __gedf2 (double a, double b)
 *
 * These functions return a value greater than or equal to zero if neither argument is NaN,
 * and a is greater than or equal to b.
 *
 */

int __gedf2 (double a, double b);

int main ( void )
{
  union dll {
    double dvalue;
    long long llvalue;
  } a, b;
  int ret;

  a.dvalue = 0.0;
  b.dvalue = 0.0;
  ret = __gedf2 ( a.dvalue, b.dvalue );
  if ( !( ret == 0 ) )
    return -1;

  a.dvalue = 2.0;
  b.dvalue = 1.0;
  ret = __gedf2 ( a.dvalue, b.dvalue );
  if ( !( ret > 0 ) )
    return -1;

  a.dvalue = 1.0;
  b.dvalue = 2.0;
  ret = __gedf2 ( a.dvalue, b.dvalue );
  if ( ( ret > 0 ) )
    return -1;

  a.dvalue = -1.0;
  b.dvalue = 2.0;
  ret = __gedf2 ( a.dvalue, b.dvalue );
  if ( ( ret > 0 ) )
    return -1;

  a.dvalue = 1.0;
  b.dvalue = -2.0;
  ret = __gedf2 ( a.dvalue, b.dvalue );
  if ( !( ret > 0 ) )
    return -1;

  a.dvalue = -1.0;
  b.dvalue = -2.0;
  ret = __gedf2 ( a.dvalue, b.dvalue );
  if ( !( ret > 0 ) )
    return -1;

  a.dvalue = 32132132133.424525215255215;
  b.dvalue = 3123213332.02312321323132;
  ret = __gedf2 ( a.dvalue, b.dvalue );
  if ( !( ret > 0 ) )
    return -1;

  a.dvalue = 322132133.424525215255215;
  b.dvalue = 3123213332.0231232221323132;
  ret = __gedf2 ( a.dvalue, b.dvalue );
  if ( ( ret > 0 ) )
    return -1;

  return ( 0 );
}
