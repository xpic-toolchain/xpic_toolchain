/*
 * strlwr---force string to lower case
 *
 * Synopsis
 * #include <string.h>
 * char *strlwr(char *a);
 *
 * Description
 * strlwr converts each characters in the string at a to lower case.
 *
 * Returns
 * strlwr returns its argument, a.
 *
 * Portability
 * strlwr is not widely portable.
 *
 * strlwr requires no supporting OS subroutines.
 *
 */

#include <stdio.h>
#include <string.h>

int main ( void )
{
  char str1[12] = "JUST A TEST";
  char str2[12] = "just a test";
  strlwr( str1 );
  if ( strcmp( str1, str2 ) != 0 )
    return ( -1 );

  char str3[16] = "12JUST A TEST34";
  char str4[16] = "12just a test34";
  strlwr( str3 );
  if ( strcmp( str4, str4 ) != 0 )
    return ( -1 );

  return ( 0 );
}
