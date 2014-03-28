/*
 * strcoll---locale specific character string compare
 *
 * Synopsis
 *
 * #include <string.h>
 * int strcoll(const char *stra, const char * strb);
 *
 *
 * Description
 * strcoll compares the string pointed to by stra to the string pointed to by strb, using an interpretation appropriate to the current LC_COLLATE state.
 *
 *
 * Returns
 * If the first string is greater than the second string, strcoll returns a number greater than zero. If the two strings are equivalent, strcoll returns zero. If the first string is less than the second string, strcoll returns a number less than zero.
 *
 *
 * Portability
 * strcoll is ANSI C.
 *
 * strcoll requires no supporting OS subroutines.
 *
 */

#include <stdio.h>
#include <string.h>

int main ( void )
{
  char aKey[] = "aaabbb";
  char bKey[] = "bbbccc";
  char cKey[] = "cccddd";

  if ( !(strcoll ( aKey, bKey ) < 0) )
    return ( -1 );
  if ( !(strcoll ( bKey, aKey ) > 0) )
    return ( -1 );
  if ( !(strcoll ( aKey, aKey ) == 0) )
    return ( -1 );
  if ( !(strcoll ( bKey, bKey ) == 0) )
    return ( -1 );
  if ( !(strcoll ( aKey, cKey ) < 0) )
    return ( -1 );
  if ( !(strcoll ( cKey, aKey ) > 0) )
    return ( -1 );

  return ( 0 );
}

