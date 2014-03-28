/*
 * memchr---find character in memory
 *
 * Synopsis
 * #include <string.h>
 * void *memchr(const void *src, int c, size_t length);
 *
 * Description
 * This function searches memory starting at *src for the character c.
 * The search only ends with the first occurrence of c, or after length characters;
 * in particular, NULL does not terminate the search.
 *
 * Returns
 * If the character c is found within length characters of *src, a pointer to the
 * character is returned. If c is not found, then NULL is returned.
 *
 * Portability
 * memchr> is ANSI C.
 *
 * memchr requires no supporting OS subroutines.
 *
 */

#include <stdio.h>
#include <string.h>

int main ()
{
  char * pch;
  char str[] = "Example string";
  pch = (char*) memchr (str, 'p', strlen(str));
  if (pch!=NULL)
  {
    if ( pch-str+1-5 == 0 ) // p is at position 5
    {
      return ( 0 );
    }
    else
    {
      return ( -1 );
    }
  }
  else
  {
    return ( -1 );
  }
}
