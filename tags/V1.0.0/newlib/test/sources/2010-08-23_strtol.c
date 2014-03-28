/*
 * strtol---string to long
 *
 * Synopsis
 * #include <stdlib.h>
 * long strtol(const char *s, char **ptr,int base);
 *
 * long _strtol_r(void *reent,
 *     const char *s, char **ptr,int base);
 *
 * Description
 * The function strtol converts the string *s to a long. First, it breaks down
 * the string into three parts: leading whitespace, which is ignored; a subject
 * string consisting of characters resembling an integer in the radix specified
 * by base; and a trailing portion consisting of zero or more unparseable characters,
 * and always including the terminating null character. Then, it attempts to convert
 * the subject string into a long and returns the result.
 *
 * If the value of base is 0, the subject string is expected to look like a normal C
 * integer constant: an optional sign, a possible `0x' indicating a hexadecimal base,
 * and a number. If base is between 2 and 36, the expected form of the subject is a
 * sequence of letters and digits representing an integer in the radix specified
 * by base, with an optional plus or minus sign. The letters a--z (or, equivalently,
 * A--Z) are used to signify values from 10 to 35; only letters whose ascribed values
 * are less than base are permitted. If base is 16, a leading 0x is permitted.
 *
 * The subject sequence is the longest initial sequence of the input string that has
 * the expected form, starting with the first non-whitespace character. If the string
 * is empty or consists entirely of whitespace, or if the first non-whitespace
 * character is not a permissible letter or digit, the subject string is empty.
 *
 * If the subject string is acceptable, and the value of base is zero, strtol attempts
 * to determine the radix from the input string. A string with a leading 0x is
 * treated as a hexadecimal value; a string with a leading 0 and no x is treated as
 * octal; all other strings are treated as decimal. If base is between 2 and 36, it
 * is used as the conversion radix, as described above. If the subject string begins
 * with a minus sign, the value is negated. Finally, a pointer to the first character
 * past the converted subject string is stored in ptr, if ptr is not NULL.
 *
 * If the subject string is empty (or not in acceptable form), no conversion is
 * performed and the value of s is stored in ptr (if ptr is not NULL).
 *
 * The alternate function _strtol_r is a reentrant version. The extra argument reent
 * is a pointer to a reentrancy structure.
 *
 * Returns
 * strtol returns the converted value, if any. If no conversion was made, 0 is
 * returned.
 *
 * strtol returns LONG_MAX or LONG_MIN if the magnitude of the converted value is
 * too large, and sets errno to ERANGE.
 *
 * Portability
 * strtol is ANSI.
 *
 * No supporting OS subroutines are required.
 *
 */

#include <stdio.h>
#include <stdlib.h>

int main ( void )
{
  char szNumbers[] = "2001 60c0c0 -1101110100110100100000 0x6fffff";
  char * pEnd;
  long int li1, li2, li3, li4;
  li1 = strtol (szNumbers,&pEnd,10);
  li2 = strtol (pEnd,&pEnd,16);
  li3 = strtol (pEnd,&pEnd,2);
  li4 = strtol (pEnd,NULL,0);

  if ( li1 != 2001L )
    return ( -1 );

  if ( li2 != 6340800L )
    return ( -1 );

  if ( li3 != -3624224L )
    return ( -1 );

  if ( li4 != 7340031L )
    return ( -1 );

  return 0;
}
