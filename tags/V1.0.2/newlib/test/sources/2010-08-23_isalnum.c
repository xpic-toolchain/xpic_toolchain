/*
 * isalnum---alphanumeric character predicate
 *
 * Synopsis
 * #include <ctype.h>
 * int isalnum(int c);
 *
 * Description
 * isalnum is a macro which classifies ASCII integer values by table lookup.
 * It is a predicate returning non-zero for alphabetic or numeric ASCII characters,
 * and 0 for other arguments. It is defined for all integer values.
 *
 * You can use a compiled subroutine instead of the macro definition by undefining
 * the macro using `#undef isalnum'.
 *
 * Returns
 * isalnum returns non-zero if c is a letter (a--z or A--Z) or a digit (0--9).
 *
 * Portability
 * isalnum is ANSI C.
 *
 * No OS subroutines are required.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

int main ( void )
{
  int i = 0;
  char str[]="c3poAB...";
  while (isalnum(str[i])) i++; // The first 6 characters are alphanumeric
  return ( i-6 );
}
