/*
 * isalpha---alphabetic character predicate
 *
 * Synopsis
 * #include <ctype.h>
 * int isalpha(int c);
 *
 * Description
 * isalpha is a macro which classifies ASCII integer values by table lookup.
 * It is a predicate returning non-zero when c represents an alphabetic ASCII
 * character, and 0 otherwise. It is defined only when isascii(c) is true or c
 * is EOF.
 *
 * You can use a compiled subroutine instead of the macro definition by
 * undefining the macro using `#undef isalpha'.
 *
 * Returns
 * isalpha returns non-zero if c is a letter (A--Z or a--z).
 *
 * Portability
 * isalpha is ANSI C.
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
  char str[]="AABBeeff...";
  while (isalnum(str[i])) i++; // The first 8 characters are alphanumeric
  return ( i-8 );
}
