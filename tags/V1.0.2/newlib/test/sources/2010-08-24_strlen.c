/*
 * strlen---character string length
 *
 * Synopsis
 * #include <string.h>
 * size_t strlen(const char *str);
 *
 * Description
 * The strlen function works out the length of the string starting at *str by
 * counting chararacters until it reaches a NULL character.
 *
 * Returns
 * strlen returns the character count.
 *
 * Portability
 * strlen is ANSI C.
 *
 * strlen requires no supporting OS subroutines.
 *
 */

#include <stdio.h>
#include <string.h>

int main ( void )
{
  char szInput[256] = "just testing";
  return ( strlen(szInput)-12 ); // szInput is 12 chars long
}
