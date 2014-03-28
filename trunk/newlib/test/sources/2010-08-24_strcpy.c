/*
 * strcpy---copy string
 *
 * Synopsis
 *
 * #include <string.h>
 * char *strcpy(char *dst, const char *src);
 *
 * Description
 * strcpy copies the string pointed to by src (including the terminating
 * null character) to the array pointed to by dst.
 *
 * Returns
 * This function returns the initial value of dst.
 *
 * Portability
 * strcpy is ANSI C.
 *
 * strcpy requires no supporting OS subroutines.
 *
 */

#include <stdio.h>
#include <string.h>

#define str_length 16

int main ( void )
{
  char str1[str_length];
  bzero ( str1, str_length );
  char str2[str_length];
  bzero ( str2, str_length );
  strcat (str1,"Hello World\0");
  strcpy ( str2, str1 );
  return ( strcmp( str1, str2 ) );
}
