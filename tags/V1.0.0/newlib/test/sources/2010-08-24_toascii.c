/*
 * toascii---force integers to ASCII range
 *
 * Synopsis
 * #include <ctype.h>
 * int toascii(int c);
 *
 * Description
 * toascii is a macro which coerces integers to the ASCII range (0--127) by zeroing
 * any higher-order bits.
 *
 * You can use a compiled subroutine instead of the macro definition by undefining
 * this macro using `#undef toascii'.
 *
 * Returns
 * toascii returns integers between 0 and 127.
 *
 * Portability
 * toascii is not ANSI C.
 *
 * No supporting OS subroutines are required.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

int main ( void )
{
  int i = 0;

  while( i != 0xFFFF )
  {
    int r = toascii(i);
    if ( r > 127 )
      return ( i );
    i++;
  }

  return ( 0 );
}
