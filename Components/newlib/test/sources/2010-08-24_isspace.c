/*
 * isspace---whitespace character predicate
 *
 * Synopsis
 * #include <ctype.h>
 * int isspace(int c);
 *
 * Description
 * isspace is a macro which classifies ASCII integer values by table lookup.
 * It is a predicate returning non-zero for whitespace characters, and 0 for
 * other characters. It is defined only when isascii(c) is true or c is EOF.
 *
 * You can use a compiled subroutine instead of the macro definition by undefining
 * the macro using `#undef isspace'.
 *
 * Returns
 * isspace returns non-zero if c is a space, tab, carriage return, new line, vertical
 * tab, or formfeed (0x09--0x0D, 0x20).
 *
 * Portability
 * isspace is ANSI C.
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
  int cmp = 0;

  while( i != 0x80 )
  {
    if ( i == 0x09 || i == 0x0A || i == 0x0B || i ==0x0C || i == 0x0D || i == 0x20 )
    {
      if ( isspace(i) == 0 )
        return ( -1 );
    }
    else
    {
      if ( isspace(i) != 0 )
        return ( -1 );
    }
    i++;
  }

  return ( 0 );
}
