/*
 * iscntrl---control character predicate
 *
 * Synopsis
 * #include <ctype.h>
 * int iscntrl(int c);
 *
 * Description
 * iscntrl is a macro which classifies ASCII integer values by table lookup.
 * It is a predicate returning non-zero for control characters, and 0 for other
 * characters. It is defined only when isascii(c) is true or c is EOF.
 *
 * You can use a compiled subroutine instead of the macro definition by undefining
 * the macro using `#undef iscntrl'.
 *
 * Returns
 * iscntrl returns non-zero if c is a delete character or ordinary control character
 * (0x7F or 0x00--0x1F).
 *
 * Portability
 * iscntrl is ANSI C.
 *
 * No supporting OS subroutines are required.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

int main ( void )
{
  int ret = 0;
  int i = 0;
  while ( i != 32 )
  {
    ret = iscntrl(i);
    if ( ret == 0 )
      return ( -1 );
    i++;
  }
  if ( iscntrl(127) == 0 )
    return ( -1 );

  return ( 0 );
}
