/*
 * index---search for character in string
 *
 * Synopsis
 * #include <string.h>
 * char * index(const char *string, int c);
 *
 * Description
 * This function finds the first occurence of c (converted to a char) in
 * the string pointed to by string (including the terminating null character).
 *
 * This function is identical to strchr.
 *
 * Returns
 * Returns a pointer to the located character, or a null pointer if c does not
 * occur in string.
 *
 * Portability
 * index requires no supporting OS subroutines.
 *
 */

#include <stdio.h>
#include <string.h>

int main ( void )
{
  char str[] = "This is a sample string";
  char * pch;
  pch=index ( str, 's' );
  int check = 0;
  while (pch!=NULL)
  {
    check++; // s is four times in the sample string
    pch=index ( pch+1, 's' );
  }
  return ( (check-4) );
}
