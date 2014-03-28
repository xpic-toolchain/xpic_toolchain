/*
 * isdigit---decimal digit predicate
 *
 * Synopsis
 * #include <ctype.h>
 * int isdigit(int c);
 *
 * Description
 * isdigit is a macro which classifies ASCII integer values by table lookup.
 * It is a predicate returning non-zero for decimal digits, and 0 for other
 * characters. It is defined only when isascii(c) is true or c is EOF.
 *
 * You can use a compiled subroutine instead of the macro definition by
 * undefining the macro using `#undef isdigit'.
 *
 * Returns
 * isdigit returns non-zero if c is a decimal digit (0--9).
 *
 * Portability
 * isdigit is ANSI C.
 *
 * No supporting OS subroutines are required.
 *
 */

#include <stdio.h>
#include <stdlib.h>

int main ( void )
{
  char str[]="1023465897asdadadd";
  int number;
  if (isdigit(str[0]))
  {
    number = atoi (str);
  }

  if ( number != 1023465897 )
    return ( -1 );

  return ( 0 );
}
