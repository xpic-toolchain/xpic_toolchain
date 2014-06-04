/*
 * atof, atoff---string to double or float
 *
 * Synopsis
 * #include <stdlib.h>
 * double atof(const char *s);
 * float atoff(const char *s);
 *
 * Description
 * atof converts the initial portion of a string to a double.
 * atoff converts the initial portion of a string to a float.
 *
 * The functions parse the character string s, locating a substring which can
 * be converted to a floating point value. The substring must match the format:
 *
 * [+|-]digits[.][digits][(e|E)[+|-]digits]
 *
 * The substring converted is the longest initial fragment of s that has the
 * expected format, beginning with the first non-whitespace character.
 * The substring is empty if str is empty, consists entirely of whitespace,
 * or if the first non-whitespace character is something other than +, -, .,
 * or a digit.
 *
 * atof(s) is implemented as strtod(s, NULL). atoff(s) is implemented as
 * strtodf(s, NULL).
 *
 * Returns
 * atof returns the converted substring value, if any, as a double; or 0.0,
 * if no conversion could be performed. If the correct value is out of the
 * range of representable values, plus or minus HUGE_VAL is returned,
 * and ERANGE is stored in errno. If the correct value would cause
 * underflow, 0.0 is returned and ERANGE is stored in errno.
 *
 * atoff obeys the same rules as atof, except that it returns a float.
 *
 * Portability
 * atof is ANSI C. atof, atoi, and atol are subsumed by strod and strol,
 * but are used extensively in existing code. These functions are less reliable,
 * but may be faster if the argument is verified to be in a valid range.
 *
 * Supporting OS subroutines required: close, fstat, isatty, lseek, read,
 * sbrk, write.
 *
 */

#include <stdlib.h>

int main ( void )
{
  char my_string [10] = "\0\0\0\0\0\0\0\0\0\0";
  float number;

  my_string[0] = '0';
  my_string[1] = '.';
  my_string[2] = '0';
  number = atof ( my_string );
  if ( number != 0.0f )
    return ( -1 );

  my_string[0] = '2';
  my_string[1] = '.';
  my_string[2] = '5';
  number = atof ( my_string );
  if ( number != 2.5f )
    return ( -1 );

  my_string[0] = '-';
  my_string[1] = '2';
  my_string[2] = '.';
  my_string[3] = '5';
  number = atof ( my_string );
  if ( number != -2.5f )
    return ( -1 );

  my_string[0] = '1';
  my_string[1] = '.';
  my_string[2] = '0';
  my_string[3] = '\0';
  number = atof ( my_string );
  if ( number != 1.0f )
    return ( -1 );

  my_string[0] = '-';
  my_string[1] = '1';
  my_string[2] = '.';
  my_string[3] = '0';
  number = atof ( my_string );
  if ( number != -1.0f )
    return ( -1 );

  my_string[0] = '-';
  my_string[1] = '2';
  my_string[2] = '.';
  my_string[3] = '0';
  my_string[4] = '4';
  my_string[5] = '5';
  number = atof ( my_string );
  if ( number != -2.045f )
    return ( -1 );

  my_string[0] = '+';
  my_string[1] = '2';
  my_string[2] = '.';
  my_string[3] = '0';
  my_string[4] = '4';
  my_string[5] = '5';
  number = atof ( my_string );
  if ( number != +2.045f )
    return ( -1 );

  my_string[0] = '5';
  my_string[1] = '2';
  my_string[2] = '.';
  my_string[3] = '0';
  my_string[4] = '4';
  my_string[5] = '5';
  number = atof ( my_string );
  if ( number != 52.045f )
    return ( -1 );

  my_string[0] = '2';
  my_string[1] = '.';
  my_string[2] = '3';
  my_string[3] = '5';
  my_string[4] = '5';
  my_string[5] = 'E';
  my_string[6] = '+';
  my_string[7] = '5';
  number = atof ( my_string );
  if ( number != 2.355E+5f )
    return ( -1 );

  my_string[0] = '2';
  my_string[1] = '.';
  my_string[2] = '3';
  my_string[3] = '5';
  my_string[4] = '5';
  my_string[5] = 'E';
  my_string[6] = '-';
  my_string[7] = '5';
  number = atof ( my_string );
  if ( number != 2.355E-5f )
    return ( -1 );

  return ( 0 );
}
