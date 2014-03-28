/*
 * ledf2_test_02.c
 *
 * — Runtime Function: int __ledf2 (double a, double b)
 *
 * These functions return a value less than or equal to zero if neither argument is NaN,
 * and a is less than or equal to b.
 *
 */

int __ledf2 (double a, double b);

int main ( void )
{
  union dll {
    double dvalue;
    long long llvalue;
  } a, b;
  int ret;

  a.dvalue = 1.0;
  b.dvalue = 3.0;
  ret = __ledf2 ( a.dvalue, b.dvalue );
  if ( !( ret <= 0 ) )
    return ( -1 );

  a.dvalue = -1.0;
  b.dvalue = 3.0;
  ret = __ledf2 ( a.dvalue, b.dvalue );
  if ( !( ret <= 0 ) )
    return ( -1 );

  a.dvalue = 1.0;
  b.dvalue = -3.0;
  ret = __ledf2 ( a.dvalue, b.dvalue );
  if ( ( ret <= 0 ) )
    return ( -1 );

  a.dvalue = -1.0;
  b.dvalue = -3.0;
  ret = __ledf2 ( a.dvalue, b.dvalue );
  if ( ( ret <= 0 ) )
    return ( -1 );

  a.dvalue = 0.0;
  b.dvalue = 0.0;
  ret = __ledf2 ( a.dvalue, b.dvalue );
  if ( !( ret == 0 ) )
    return ( -1 );

  a.dvalue = 231232133.234444;
  b.dvalue = 231232133.234444;
  ret = __ledf2 ( a.dvalue, b.dvalue );
  if ( !( ret == 0 ) )
    return ( -1 );

  a.dvalue = -231232133.234444;
  b.dvalue = -231232133.234444;
  ret = __ledf2 ( a.dvalue, b.dvalue );
  if ( !( ret == 0 ) )
    return ( -1 );

  a.dvalue = 231232133.234444;
  b.dvalue = -231232133.234444;
  ret = __ledf2 ( a.dvalue, b.dvalue );
  if ( ( ret <= 0 ) )
    return ( -1 );

  a.dvalue = -231232133.234444;
  b.dvalue = 231232133.234444;
  ret = __ledf2 ( a.dvalue, b.dvalue );
  if ( !( ret <= 0 ) )
    return ( -1 );

  return ( 0 );
}

