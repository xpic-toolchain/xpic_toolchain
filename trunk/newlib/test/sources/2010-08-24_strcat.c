/*
 * strcat---concatenate strings
 *
 * Synopsis
 * #include <string.h>
 * char *strcat(char *dst, const char *src);
 *
 * Description
 * strcat appends a copy of the string pointed to by src (including the
 * terminating null character) to the end of the string pointed to by dst.
 * The initial character of src overwrites the null character at the end of dst.
 *
 * Returns
 * This function returns the initial value of dst
 *
 * Portability
 * strcat is ANSI C.
 *
 * strcat requires no supporting OS subroutines.
 *
 */


#include <stdio.h>
#include <string.h>

int main ( void )
{
  char str[80];
  char strt4[3] = "T4";
  strcpy (str,"T1");
  strcat (str,"T2");
  strcat (str,"T3");
  strcat (str,strt4);
  if ( strcmp (str,"T1T2T3T4") != 0)
    return ( -1 );
  return 0;
}
