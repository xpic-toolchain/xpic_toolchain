/*
 * muldf3_test_02.c
 *
 *  Created on: 16.08.2010
 *      Author: Alexander
 *
 * — Runtime Function: double __muldf3 (double a, double b)
 *
 * These functions return the product of a and b.
 *
 */
#define epsilon 0.001

double __muldf3 (double a, double b);

int main ( void )
{
  union dll {
    double dvalue;
    long long llvalue;
  } a, b, c;

  a.dvalue = 0.0;
  b.dvalue = 0.0;
  c.dvalue = __muldf3(a.dvalue, b.dvalue);
  if ( c.dvalue != 0.0 )
    return ( 1 );

  a.dvalue = 1.0;
  b.dvalue = 0.0;
  c.dvalue = __muldf3(a.dvalue, b.dvalue);
  if ( c.dvalue != 0.0 )
    return ( 2 );

  a.dvalue = 0.0;
  b.dvalue = 1.0;
  c.dvalue = __muldf3(a.dvalue, b.dvalue);
  if ( c.dvalue != 0.0 )
    return ( 3 );

  a.dvalue = 1.0;
  b.dvalue = 1.0;
  c.dvalue = __muldf3(a.dvalue, b.dvalue);
  if ( !(c.dvalue <= 1.0+epsilon && c.dvalue >= 1.0-epsilon) )
    return ( 4 );

  a.dvalue = 34423e+04;
  b.dvalue = 10.0;
  c.dvalue = __muldf3(a.dvalue, b.dvalue);
   if ( !(c.dvalue <= 34423e+05+1e+05 && c.dvalue >= 34423e+05-1e+05) )
    return ( 5 );

  a.dvalue = -34423e+04;
  b.dvalue = 10.0;
  c.dvalue = __muldf3(a.dvalue, b.dvalue);
  if ( !(c.dvalue <= -34423e+05+1e+05 && c.dvalue >= -34423e+05-1e+05) )
    return ( 6 );

  a.dvalue = 34423e+04;
  b.dvalue = -10.0;
  c.dvalue = __muldf3(a.dvalue, b.dvalue);
  if ( !(c.dvalue <= -34423e+04+1e+05 && c.dvalue >= -34423e+05-1e+05) )
    return ( 7 );

  a.dvalue = -34423e+04;
  b.dvalue = -10.0;
  c.dvalue = __muldf3(a.dvalue, b.dvalue);
  if ( !(c.dvalue <= 34423e+05+1e+06 && c.dvalue >= 34423e+05-1e+06) )
    return ( 8 );

  a.dvalue = 34423e+04;
  b.dvalue = 0.1;
  c.dvalue = __muldf3(a.dvalue, b.dvalue);
  if ( !(c.dvalue <= (34423e+03)+1000 && c.dvalue >= (34423e+03)-1000) )
    return ( 9 );

  a.dvalue = -34423e+04;
  b.dvalue = 0.1;
  c.dvalue = __muldf3(a.dvalue, b.dvalue);
  if ( !(c.dvalue <= (-34423e+03)+1000 && c.dvalue >= (-34423e+03)-1000) )
    return ( 10 );

  a.dvalue = 34423e+04;
  b.dvalue = 0.001;
  c.dvalue = __muldf3(a.dvalue, b.dvalue);
  if ( !(c.dvalue <= (34423e+01)+1000 && c.dvalue >= (34423e+01)-1000) )
    return ( 11 );

  a.dvalue = 34423e+04;
  b.dvalue = 0.0001;
  c.dvalue = __muldf3(a.dvalue, b.dvalue);
  if ( !(c.dvalue <= (34423e+00)+1 && c.dvalue >= (34423e+00)-1) )
    return ( 12 );

  a.dvalue = 34423e+04;
  b.dvalue = 0.00001;
  c.dvalue = __muldf3(a.dvalue, b.dvalue);
  if ( !(c.dvalue <= 3442.3+0.1 && c.dvalue >= 3442.3-0.1) )
    return ( 13 );

  a.dvalue = 34423e+04;
  b.dvalue = 0.000001;
  c.dvalue = __muldf3(a.dvalue, b.dvalue);
  if ( !(c.dvalue <= 344.23+0.1 && c.dvalue >= 344.23-0.1) )
    return ( 14 );

  a.dvalue = 34423e+04;
  b.dvalue = 0.0000001;
  c.dvalue = __muldf3(a.dvalue, b.dvalue);
  if ( !(c.dvalue <= 34.423+0.1 && c.dvalue >= 34.423-0.1) )
    return ( 15 );

  a.dvalue = 34423e+04;
  b.dvalue = 0.00000001;
  c.dvalue = __muldf3(a.dvalue, b.dvalue);
  if ( !(c.dvalue <= 3.4423+epsilon && c.dvalue >= 3.4423-epsilon) )
    return ( 16 );

  a.dvalue = -34423e+04;
  b.dvalue = 0.000001;
  c.dvalue = __muldf3(a.dvalue, b.dvalue);
  if ( !(c.dvalue <= -344.23+0.1 && c.dvalue >= -344.23-0.1) )
    return ( 17 );

  a.dvalue = -34423e+04;
  b.dvalue = 0.0000001;
  c.dvalue = __muldf3(a.dvalue, b.dvalue);
  if ( !(c.dvalue <= -34.423+0.1 && c.dvalue >= -34.423-0.1) )
    return ( 18 );

  a.dvalue = -34423e+04;
  b.dvalue = 0.00000001;
  c.dvalue = __muldf3(a.dvalue, b.dvalue);
  if ( !(c.dvalue <= -3.4423+0.1 && c.dvalue >= -3.4423-0.1) )
    return ( 19 );

  a.dvalue = 1.0;
  b.dvalue = 2.0;
  c.dvalue = __muldf3(a.dvalue, b.dvalue);
  if ( !(c.dvalue <=2.0+epsilon && c.dvalue >= 2.0-epsilon) )
    return ( 20 );

  return ( 0 );
}
