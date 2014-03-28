/*
 * bzero---initialize memory to zero
 *
 * Synopsis
 * #include <string.h>
 * void bzero(char *b, size_t length);
 *
 * Description
 * bzero initializes length bytes of memory, starting at address b, to zero.
 *
 * Returns
 * bzero does not return a result.
 *
 * Portability
 * bzero is in the Berkeley Software Distribution. Neither ANSI C nor the System V
 *  Interface Definition (Issue 2) require bzero.
 *
 * bzero requires no supporting OS subroutines.
 *
 */

#include <stdio.h>
#include <string.h>

#define str_length 16

int main ( void )
{
  char str[str_length]="ABCDEF";
  bzero(str, str_length);

  int i = 0;
  while ( i != str_length )
  {
    if ( str[i] != 0 )
      return ( -1 );
    i++;
  }

  return ( 0 );
}
