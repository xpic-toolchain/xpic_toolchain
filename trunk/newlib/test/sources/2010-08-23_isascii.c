/*
 * isascii---ASCII character predicate
 *
 * Synopsis
 *
 * #include <ctype.h>
 * int isascii(int c);
 *
 *
 * Description
 * isascii is a macro which returns non-zero when c is an ASCII character,
 * and 0 otherwise. It is defined for all integer values.
 *
 * You can use a compiled subroutine instead of the macro definition by
 *  undefining the macro using `#undef isascii'.
 *
 * Returns
 * isascii returns non-zero if the low order byte of c is in the range 0 to 127
 * (0x00--0x7F).
 *
 * Portability
 * isascii is ANSI C.
 *
 * No supporting OS subroutines are required.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

int main ( void )
{
  char asciibuffer[256];
  int i = 0;
  while ( i != 256 )
  {
    asciibuffer[i] = (char)i;
    i++;
  }
  i = 0;
  while (isascii(asciibuffer[i]))
  {
    i++;
  }
  return (i -128);
}
