/*
 * islower---lower-case character predicate
 *
 * Synopsis
 * #include <ctype.h>
 * int islower(int c);
 *
 * Description
 * islower is a macro which classifies ASCII integer values by table lookup.
 * It is a predicate returning non-zero for minuscules (lower-case alphabetic characters),
 *  and 0 for other characters. It is defined only when isascii(c) is true or c is EOF.
 *
 * You can use a compiled subroutine instead of the macro definition by undefining the
 * macro using `#undef islower'.
 *
 * Returns
 * islower returns non-zero if c is a lower case letter (a--z).
 *
 * Portability
 * islower is ANSI C.
 *
 * No supporting OS subroutines are required.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

int main ( void )
{
  int i=0;
  char str[]="Test String.\0";
  char cp_str[14] = "\0";
  char c;
  while (str[i])
  {
    c=str[i];
    if (islower(c)) c=toupper(c);
    cp_str[i] = c;
    i++;
  }
  if ( cp_str[0] != 'T' || cp_str[1] != 'E' || cp_str[2] != 'S' || cp_str[3] != 'T' || cp_str[4] != ' ' || cp_str[5] != 'S' || cp_str[6] != 'T' || cp_str[7] != 'R' || cp_str[8] != 'I' || cp_str[9] != 'N' || cp_str[10] != 'G' || cp_str[11] != '.' )
      return -1;

  return ( 0 );
}
