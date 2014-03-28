/*
 * isxdigit---hexadecimal digit predicate
 *
 * Synopsis
 * #include <ctype.h>
 * int isxdigit(int c);
 *
 * Description
 * isxdigit is a macro which classifies ASCII integer values by table lookup.
 * It is a predicate returning non-zero for hexadecimal digits, and 0 for other
 * characters. It is defined only when isascii(c) is true or c is EOF.
 *
 * You can use a compiled subroutine instead of the macro definition by undefining
 * the macro using `#undef isxdigit'.
 *
 * Returns
 * isxdigit returns non-zero if c is a hexadecimal digit (0--9, a--f, or A--F).
 *
 * Portability
 * isxdigit is ANSI C.
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

  while( i != 0x30 )
  {
    if ( isxdigit(i) != 0 )
      return ( -1 );
    i++;
  }

  while( i != 0x3A )
  {
    if ( isxdigit(i) == 0 )
      return ( -1 );
    i++;
  }

  while( i != 0x41 )
  {
    if ( isxdigit(i) != 0 )
      return ( -1 );
    i++;
  }

  while( i != 0x47 )
  {
    if ( isxdigit(i) == 0 )
      return ( -1 );
    i++;
  }

  while( i != 0x61 )
  {
    if ( isxdigit(i) != 0 )
      return ( -1 );
    i++;
  }

  while( i != 0x67 )
  {
    if ( isxdigit(i) == 0 )
      return ( -1 );
    i++;
  }

  while( i != 0x80 )
  {
    if ( isxdigit(i) != 0 )
      return ( -1 );
    i++;
  }

  return ( 0 );
}
