/*
 * strcspn---count chars not in string
 *
 * Synopsis
 * size_t strcspn(const char *s1, const char *s2);
 *
 * Description
 * This function computes the length of the initial part of the string
 * pointed to by s1 which consists entirely of characters NOT from the
 * string pointed to by s2 (excluding the terminating null character).
 *
 * Returns
 * strcspn returns the length of the substring found.
 *
 * Portability
 * strcspn is ANSI C.
 *
 * strcspn requires no supporting OS subroutines.
 *
 */

#include <stdio.h>
#include <string.h>

int main ( void )
{
  char str[] = "fcba73";
  char keys[] = "1234567890";
  int i;
  i = strcspn (str,keys);
  return ( (i+1) - 5 ); // first match "7" at position 5
}
