/*
 * rindex---reverse search for character in string
 *
 * Synopsis
 * #include <string.h>
 * char * rindex(const char *string, int c);
 *
 * Description
 * This function finds the last occurence of c (converted to a char)
 * in the string pointed to by string (including the terminating null character).
 *
 * This function is identical to strrchr.
 *
 * Returns
 * Returns a pointer to the located character, or a null pointer if c does not occur
 * in string.
 *
 * Portability
 * rindex requires no supporting OS subroutines.
 *
 */

#include <stdio.h>
#include <string.h>

int main ( void )
{
  char str[] = "This is a";
  char * pch;
  pch=rindex(str,'s');
  char cmp = pch-str+1;;
  return ( cmp - 7 ); // This i_[s]_ a <- position 7
}
