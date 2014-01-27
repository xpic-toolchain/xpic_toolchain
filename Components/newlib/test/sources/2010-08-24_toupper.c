/*
 * toupper---translate characters to upper case
 *
 * Synopsis
 * #include <ctype.h>
 * int toupper(int c);
 * int _toupper(int c);
 *
 * Description
 * toupper is a macro which converts lower-case characters to upper case,
 * leaving all other characters unchanged. It is only defined when c is an
 * integer in the range EOF to 255.
 *
 * You can use a compiled subroutine instead of the macro definition by undefining
 * this macro using `#undef toupper'.
 *
 * _toupper performs the same conversion as toupper, but should only be used when c
 * is known to be a lowercase character (a--z).
 *
 * Returns
 * toupper returns the upper-case equivalent of c when it is a character between a and z,
 * and c otherwise.
 *
 * _toupper returns the upper-case equivalent of c when it is a character between a and z.
 * If c is not one of these characters, the behaviour of _toupper is undefined.
 *
 * Portability
 * toupper is ANSI C. _toupper is not recommended for portable programs.
 *
 * No supporting OS subroutines are required.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

int main ( void )
{
  int i = 0x61;

  while( i != 0x7B )
  {
    if ( toupper(i) != i-0x20 )
      return ( -1 );
    i++;
  }

  return ( 0 );
}
