/*
 * floatsidf_test_02.c
 *
 * — Runtime Function: double __floatsidf (int i)
 *
 * These functions convert i, a signed integer, to floating point.
 *
 */

double __floatsidf (int i);

int main ( void )
{
  int i;
  double ret;

  i = 0;
  ret = __floatsidf ( i );
  if ( ret != 0.0 )
    return -1;

  i = 1;
  ret = __floatsidf ( i );
  if ( ret != 1.0 )
    return -1;

  i = -1;
  ret = __floatsidf ( i );
  if ( ret != -1.0 )
    return -1;

  i = 2147483647;
  ret = __floatsidf ( i );
  if ( ret != 2147483647.0 )
    return -1;

  i = -2147483646;
  ret = __floatsidf ( i );
  if ( ret != -2147483646.0 )
    return -1;

  return ( 0 );
}
