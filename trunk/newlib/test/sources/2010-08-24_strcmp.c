/*
 * strcmp---character string compare
 *
 * Synopsis
 * #include <string.h>
 * int strcmp(const char *a, const char *b);
 *
 * Description
 * strcmp compares the string at a to the string at b.
 *
 * Returns
 * If *a sorts lexicographically after *b, strcmp returns a number greater
 * than zero. If the two strings match, strcmp returns zero. If *a sorts
 * lexicographically before *b, strcmp returns a number less than zero.
 *
 * Portability
 * strcmp is ANSI C.
 *
 * strcmp requires no supporting OS subroutines.
 *
 */

#include <stdio.h>
#include <string.h>

int main ( void )
{
  char aKey[] = "aaabbb";
  char bKey[] = "bbbccc";
  char cKey[] = "cccddd";

  if ( !(strcmp ( aKey, bKey ) < 0) )
    return ( -1 );
  if ( !(strcmp ( bKey, aKey ) > 0) )
    return ( -1 );
  if ( !(strcmp ( aKey, aKey ) == 0) )
    return ( -1 );
  if ( !(strcmp ( bKey, bKey ) == 0) )
    return ( -1 );
  if ( !(strcmp ( aKey, cKey ) < 0) )
    return ( -1 );
  if ( !(strcmp ( cKey, aKey ) > 0) )
    return ( -1 );

  return ( 0 );
}
