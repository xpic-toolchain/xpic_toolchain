/*
 * isprint, isgraph---printable character predicates
 *
 * Synopsis
 *
 * #include <ctype.h>
 * int isprint(int c);
 * int isgraph(int c);
 *
 * Description
 * isprint is a macro which classifies ASCII integer values by table lookup.
 * It is a predicate returning non-zero for printable characters, and 0 for
 * other character arguments. It is defined only when isascii(c) is true or c is EOF.
 *
 * You can use a compiled subroutine instead of the macro definition by undefining
 * either macro using `#undef isprint' or `#undef isgraph'.
 *
 * Returns
 * isprint returns non-zero if c is a printing character, (0x20--0x7E). isgraph behaves
 * identically to isprint, except that the space character (0x20) is excluded.
 *
 * Portability
 * isprint and isgraph are ANSI C.
 *
 * No supporting OS subroutines are required.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

int main ( void )
{
  int ret;
  int i = 0x00;
  while ( i != 0x20 )
  {
    ret = isprint(i);
    if ( ret != 0 )
      return ( -1 );
    i++;
  }
  while ( i != 0x7F )
  {
    ret = isprint(i);
    if ( ret == 0 )
      return ( -1 );
    i++;
  }
  while ( i != 0xFF )
  {
    ret = isprint(i);
    if ( ret != 0 )
      return ( i );
    i++;
  }

  return ( 0 );
}
