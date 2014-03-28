/*
 * strupr---force string to uppercase
 *
 * Synopsis
 * #include <string.h>
 * char *strupr(char *a);
 *
 * Description
 * strupr converts each characters in the string at a to upper case.
 *
 * Returns
 * strupr returns its argument, a.
 *
 * Portability
 * strupr is not widely portable.
 *
 * strupr requires no supporting OS subroutines.
 *
 */

#include <stdio.h>
#include <string.h>

int main ( void )
{
  char str1[12] = "JUST A TEST";
  char str2[12] = "just a test";
  strupr( str2 );
  if ( strcmp( str1, str2 ) != 0 )
    return ( -1 );

  char str3[16] = "12JUST A TEST34";
  char str4[16] = "12just a test34";
  strupr( str4 );
  if ( strcmp( str4, str4 ) != 0 )
    return ( -1 );

  return ( 0 );
}
