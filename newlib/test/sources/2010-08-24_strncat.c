/*
 * strncat---concatenate strings
 *
 * Synopsis
 *
 * #include <string.h>
 * char *strncat(char *dst, const char *src, size_t length);
 *
 * Description
 * strncat appends not more than length characters from the string pointed
 * to by src (including the terminating null character) to the end of the
 * string pointed to by dst. The initial character of src overwrites the
 * null character at the end of dst. A terminating null character is always
 * appended to the result
 *
 * Warnings
 * Note that a null is always appended, so that if the copy is limited by the
 * length argument, the number of characters appended to dst is n + 1.
 *
 * Returns
 * This function returns the initial value of dst
 *
 * Portability
 * strncat is ANSI C.
 *
 * strncat requires no supporting OS subroutines.
 *
 */

#include <stdio.h>
#include <string.h>

int main ( void )
{
  char str1[20];
  char str2[20];
  char cmpstr[20] = "To be or not";
  strcpy (str1,"To be ");
  strcpy (str2,"or not to be");
  strncat (str1, str2, 6);
  return ( strcmp ( str1, cmpstr ) );
}
