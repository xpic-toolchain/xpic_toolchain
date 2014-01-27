/*
 * isupper---uppercase character predicate
 *
 * Synopsis
 * #include <ctype.h>
 * int isupper(int c);
 *
 * Description
 * isupper is a macro which classifies ASCII integer values by table lookup.
 * It is a predicate returning non-zero for upper-case letters (A--Z), and 0
 * for other characters. It is defined only when isascii(c) is true or c is EOF.
 *
 * You can use a compiled subroutine instead of the macro definition by undefining
 *  the macro using `#undef isupper'.
 *
 * Returns
 * isupper returns non-zero if c is a upper case letter (A-Z).
 *
 * Portability
 * isupper is ANSI C.
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

  while( i != 0x41 )
  {
    if ( isupper(i) != 0 )
      return ( -1 );
    i++;
  }

  while( i != 0x5B )
  {
    if ( isupper(i) == 0 )
      return ( -1 );
    i++;
  }

  while( i != 0x80 )
  {
    if ( isupper(i) != 0 )
      return ( -1 );
    i++;
  }

  return ( 0 );
}
