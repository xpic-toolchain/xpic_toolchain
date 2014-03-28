/*
 * wctomb---minimal wide char to multibyte converter
 *
 * Synopsis
 *
 * #include <stdlib.h>
 * int wctomb(char *s, wchar_t wchar);
 *
 * Description
 * This is a minimal ANSI-conforming implementation of wctomb. The only "wide
 * characters" recognized are single bytes, and they are "converted" to themselves.
 *
 * Each call to wctomb copies the character wchar to *s, unless s is a null pointer.
 *
 * Returns
 * This implementation of wctomb returns 0 if s is NULL; it returns 1 otherwise
 * (reporting the length of the character "sequence" generated).
 *
 * Portability
 * wctomb is required in the ANSI C standard. However, the precise effects vary with
 * the locale.
 *
 * wctomb requires no supporting OS subroutines.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <stddef.h>

int wctomb(char *s, wchar_t wchar);

int main(void)
{
  char wc;
  wchar_t my_wchar = 'W';
  wctomb(&wc, my_wchar);


  if ( wc != 'W' )
    return ( -1 );

  return ( 0 );
}
