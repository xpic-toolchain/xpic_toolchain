/*
 * eqdf2_test_02.c
 *
 * — Runtime Function: int __eqdf2 (double a, double b)
 *
 * These functions return zero if neither argument is NaN, and a and b are equal.
 *
 */

int __eqdf2 (double a, double b);

int main ( void )
{
  union dll {
    double dvalue;
    long long llvalue;
  } a, b;
  int ret;

  a.dvalue = 0.0;
  b.dvalue = 0.0;
  ret = __eqdf2 ( a.dvalue, b.dvalue );
  if ( ret != 0)
    return -1;

  a.dvalue = 34213213.32132123133213;
  b.dvalue = 34213213.32132123133213;
  ret = __eqdf2 ( a.dvalue, b.dvalue );
  if ( ret != 0)
    return -1;

  a.dvalue = -34213213.32132123133213;
  b.dvalue = -34213213.32132123133213;
  ret = __eqdf2 ( a.dvalue, b.dvalue );
  if ( ret != 0)
    return -1;

  a.dvalue = 34213213.32132123133213;
  b.dvalue = -34213213.32132123133213;
  ret = __eqdf2 ( a.dvalue, b.dvalue );
  if ( ret == 0)
    return -1;

  a.dvalue = -34213213.32132123133213;
  b.dvalue = 34213213.32132123133213;
  ret = __eqdf2 ( a.dvalue, b.dvalue );
  if ( ret == 0)
    return -1;

  a.dvalue = 12313312.23213214555;
  b.dvalue = 0.0;
  ret = __eqdf2 ( a.dvalue, b.dvalue );
  if ( ret == 0)
    return -1;

  a.dvalue = 0.0;
  b.dvalue = 12313312.212133213123;
  ret = __eqdf2 ( a.dvalue, b.dvalue );
  if ( ret == 0)
    return -1;

  return ( 0 );
}
