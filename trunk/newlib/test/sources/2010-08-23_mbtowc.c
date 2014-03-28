/*
 * mbtowc---minimal multibyte to wide char converter
 *
 * Synopsis
 * #include <stdlib.h>
 * int mbtowc(wchar_t *pwc, const char *s, size_t n);
 *
 * Description
 * This is a minimal ANSI-conforming implementation of mbtowc.
 * The only "multi-byte character sequences" recognized are single bytes,
 * and they are "converted" to themselves.
 *
 * Each call to mbtowc copies one character from *s to *pwc,
 * unless s is a null pointer.
 *
 * In this implementation, the argument n is ignored.
 *
 * Returns
 * This implementation of mbtowc returns 0 if s is NULL;
 * it returns 1 otherwise (reporting the length of the character "sequence" used).
 *
 *
 * Portability
 * mbtowc is required in the ANSI C standard. However,
 * the precise effects vary with the locale.
 *
 * mbtowc requires no supporting OS subroutines.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>

int mbtowc ( wchar_t * pwc, const char * pmb, size_t max );

int main(void)
{
  wchar_t wc;
  mbtowc(&wc, "W", 1);
  if ( wc != 'W' )
    return ( -1 );

  return ( 0 );
}

