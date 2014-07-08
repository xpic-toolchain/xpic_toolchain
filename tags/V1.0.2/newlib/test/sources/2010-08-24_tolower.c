/*
 * tolower---translate characters to lower case
 *
 * Synopsis
 * #include <ctype.h>
 * int tolower(int c);
 * int _tolower(int c);
 *
 * Description
 * tolower is a macro which converts upper-case characters to lower case,
 * leaving all other characters unchanged. It is only defined when c is an
 * integer in the range EOF to 255.
 *
 * You can use a compiled subroutine instead of the macro definition by
 * undefining this macro using `#undef tolower'.
 *
 * _tolower performs the same conversion as tolower, but should only be used
 * when c is known to be an uppercase character (A--Z).
 *
 * Returns
 * tolower returns the lower-case equivalent of c when it is a character between
 * A and Z, and c otherwise.
 *
 * _tolower returns the lower-case equivalent of c when it is a character between
 * A and Z. If c is not one of these characters, the behaviour of _tolower is undefined.
 *
 * Portability
 * tolower is ANSI C. _tolower is not recommend
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

int main ( void )
{
  int i = 0x41;

  while( i != 0x5B )
  {
    if ( tolower(i) != ((i-0x41)+97) )
      return ( -1 );
    i++;
  }

  return ( 0 );
}
