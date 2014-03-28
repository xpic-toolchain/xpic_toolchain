/*
 * abs---integer absolute value (magnitude)
 *
 * Synopsis
 * #include <stdlib.h>
 * int abs(int i);
 *
 * Description
 * abs returns the absolute value of i (also called the magnitude of i).
 * That is, if i is negative, the result is the opposite of i, but if i
 * is nonnegative the result is i.
 *
 * The similar function labs uses and returns long rather than int values.
 *
 * Returns
 * The result is a nonnegative integer.
 *
 * Portability
 * abs is ANSI.
 *
 * No supporting OS subroutines are required.
 *
 */

#include <stdlib.h>

int main ( void )
{
  int ret;

  ret = abs(0);
  if ( ret != 0 )
    return ( -1 );

  ret = abs(1);
  if ( ret != 1 )
    return ( -1 );

  ret = abs(255);
  if ( ret != 255 )
    return ( -1 );

  ret = abs(65535);
  if ( ret != 65535 )
    return ( -1 );

  ret = abs(16777215);
  if ( ret != 16777215 )
    return ( -1 );

  ret = abs(2147483647);
  if ( ret != 2147483647 )
    return ( -1 );

  ret = abs(-2147483647);
  if ( ret != 2147483647 )
    return ( -1 );

  ret = abs(-16777215);
  if ( ret != 16777215 )
    return ( -1 );

  ret = abs(-65535);
  if ( ret != 65535 )
    return ( -1 );

  ret = abs(-255);
  if ( ret != 255 )
    return ( -1 );

  ret = abs(-1);
  if ( ret != 1 )
    return ( -1 );

  ret = abs(-0);
  if ( ret != 0 )
    return ( -1 );

  return 0;
}
