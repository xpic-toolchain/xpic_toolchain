/*
 * adddf3_test_02.c
 *
 * — Runtime Function: double __adddf3 (double a, double b)
 *
 * These functions return the sum of a and b.
 *
 */

double *pa;
double *pb;
double *pc;
double __adddf3 (double a, double b);

int main ( void )
{
  double a, b, c;

  pa = &a;
  pb = &b;
  pc = &c;

  a = 0.0;
  b = 0.0;
  c = __adddf3( a, b );
  if ( c != 0.0 )
    return ( -1 );

  a = 1.0;
  b = 0.0;
  c = __adddf3( a, b );
  if ( c != 1.0 )
    return ( -1 );

  a = 0.0;
  b = 1.0;
  c = __adddf3( a, b );
  if ( c != 1.0 )
    return ( -1 );

  a = 1.0;
  b = 1.0;
  c = __adddf3( a, b );
  if ( c != 2.0 )
    return ( -1 );

  a = -1.0;
  b = -1.0;
  c = __adddf3( a, b );
  if ( c != -2.0 )
    return ( -1 );

  a = -1.0;
  b = 0.0;
  c = __adddf3( a, b );
  if ( c != -1.0 )
    return ( -1 );

  a = 0.0;
  b = -1.0;
  c = __adddf3( a, b );
  if ( c != -1.0 )
    return ( -1 );

  a = 1.0;
  b = -1.0;
  c = __adddf3( a, b );
  if ( c != -0.0 )
    return ( -1 );

  a = -1.0;
  b = 1.0;
  c = __adddf3( a, b );
  if ( c != -0.0 )
    return ( -1 );

  a = 1233233.222;
  b = 22323331.232;
  c = __adddf3( a, b );
  if ( !( c <= 23556564.545+0.1 && c >= 23556564.545-0.1 ) )
    return ( -1 );

  a = -1233233.222;
  b = 22323331.232;
  c = __adddf3( a, b );
  if ( c != 21090098.01 )
    return ( -1 );

  a = -1233233.222;
  b = -22323331.232;
  c = __adddf3( a, b );
  if ( c != -23556564.454 )
    return ( -1 );

  return ( 0 );
}
