/*
 * labs---long integer absolute value
 *
 * Synopsis
 *
 * #include <stdlib.h>
 * long labs(long i);
 *
 * Description
 * labs returns the absolute value of i (also called the magnitude of i).
 * That is, if i is negative, the result is the opposite of i, but if i is
 * nonnegative the result is i.
 *
 * The similar function abs uses and returns int rather than long values.
 *
 * Returns
 * The result is a nonnegative long integer.
 *
 * Portability
 * labs is ANSI.
 *
 * No supporting OS subroutine calls are required.
 *
 */

#include <stdlib.h>
long labs(long i);

int main ( void )
{
  if ( labs(0L) != 0L )
    return ( -1 );
  if ( labs(1L) != 1L )
    return ( -1 );
  if ( labs(-1L) != 1L )
    return ( -1 );
  if ( labs(2147483647L) != 2147483647L )
    return ( -1 );
  if ( labs(-2147483647L) != 2147483647L )
    return ( -1 );

  return ( 0 );
}
