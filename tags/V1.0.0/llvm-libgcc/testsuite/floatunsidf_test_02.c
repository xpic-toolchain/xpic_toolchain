/*
 * floatunsidf_test_02.c
 *
 * — Runtime Function: double __floatunsidf (unsigned int i)
 *
 * These functions convert i, an unsigned integer, to floating point.
 *
 */

double __floatunsidf (unsigned int i);

int main ( void )
{
  unsigned int i;
  double ret;

  i = 0;
  ret = __floatunsidf ( i );
  if ( ret != 0.0 )
    return -1;

  i = 1;
  ret = __floatunsidf ( i );
  if ( ret != 1.0 )
    return -1;

  i = 0x0000FFFF;
  ret = __floatunsidf ( i );
  if ( ret != 65535.0 )
    return -1;

  i = 0xFFFFFFFF;
  ret = __floatunsidf ( i );
  if ( ret != 4294967295.0 )
    return -1;

  return ( 0 );
}
