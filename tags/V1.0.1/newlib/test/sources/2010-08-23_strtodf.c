/*
 * strtod, strtodf---string to double or float
 *
 * Synopsis
 * #include <stdlib.h>
 * double strtod(const char *str, char **tail);
 * float strtodf(const char *str, char **tail);
 *
 * double _strtod_r(void *reent,
 *     const char *str, char **tail);
 *
 * Description
 * The function strtod parses the character string str,
 * producing a substring which can be converted to a double value.
 * The substring converted is the longest initial subsequence of str,
 * beginning with the first non-whitespace character, that has the format:
 *
 * [+|-]digits[.][digits][(e|E)[+|-]digits]
 *
 * The substring contains no characters if str is empty, consists entirely
 * of whitespace, or if the first non-whitespace character is something
 * other than +, -, ., or a digit. If the substring is empty, no conversion
 * is done, and the value of str is stored in *tail. Otherwise, the substring
 * is converted, and a pointer to the final string (which will contain at
 * least the terminating null character of str) is stored in *tail. If you
 * want no assignment to *tail, pass a null pointer as tail. strtodf is
 * identical to strtod except for its return type.
 *
 * This implementation returns the nearest machine number to the input decimal
 * string. Ties are broken by using the IEEE round-even rule.
 *
 * The alternate function _strtod_r is a reentrant version. The extra argument
 * reent is a pointer to a reentrancy structure.
 *
 * Returns
 * strtod returns the converted substring value, if any. If no conversion could
 * be performed, 0 is returned. If the correct value is out of the range of
 * representable values, plus or minus HUGE_VAL is returned, and ERANGE is
 * stored in errno. If the correct value would cause underflow, 0 is
 * returned and ERANGE is stored in errno.
 *
 * Supporting OS subroutines required: close, fstat, isatty, lseek, read, sbrk,
 * write.
 *
 */

#include <stdio.h>
#include <stdlib.h>

int main ( void )
{
  char szOrbits[] = "365.24 29.53";
  char * pEnd;
  float d1, d2;
  d1 = strtodf (szOrbits,&pEnd);
  d2 = strtodf (pEnd,NULL);

  if ( d1 != 365.24f )
    return ( -1 );

  if ( d2 != 29.53f )
    return ( -1 );

  return ( 0 );
}

