/*
 * strtoul---string to unsigned long
 *
 * Synopsis
 * #include <stdlib.h>
 * unsigned long strtoul(const char *s, char **ptr,
 *     int base);
 *
 * unsigned long _strtoul_r(void *reent, const char *s,
 *     char **ptr, int base);
 *
 * Description
 * The function strtoul converts the string *s to an unsigned long. First, it breaks
 * down the string into three parts: leading whitespace, which is ignored; a subject
 * string consisting of the digits meaningful in the radix specified by base (for
 * example, 0 through 7 if the value of base is 8); and a trailing portion consisting
 * of one or more unparseable characters, which always includes the terminating null
 * character. Then, it attempts to convert the subject string into an unsigned long
 * integer, and returns the result.
 *
 * If the value of base is zero, the subject string is expected to look like a normal
 * C integer constant (save that no optional sign is permitted): a possible 0x indicating
 * hexadecimal radix, and a number. If base is between 2 and 36, the expected form of the
 * subject is a sequence of digits (which may include letters, depending on the base)
 * representing an integer in the radix specified by base. The letters a--z (or A--Z)
 * are used as digits valued from 10 to 35. If base is 16, a leading 0x is permitted.
 *
 * The subject sequence is the longest initial sequence of the input string that has the
 * expected form, starting with the first non-whitespace character. If the string is
 * empty or consists entirely of whitespace, or if the first non-whitespace character
 * is not a permissible digit, the subject string is empty.
 *
 * If the subject string is acceptable, and the value of base is zero, strtoul attempts
 * to determine the radix from the input string. A string with a leading 0x is treated
 * as a hexadecimal value; a string with a leading 0 and no x is treated as octal;
 * all other strings are treated as decimal. If base is between 2 and 36, it is used
 * as the conversion radix, as described above. Finally, a pointer to the first character
 * past the converted subject string is stored in ptr, if ptr is not NULL.
 *
 * If the subject string is empty (that is, if *s does not start with a substring in
 * acceptable form), no conversion is performed and the value of s is stored in ptr
 * (if ptr is not NULL).
 *
 * The alternate function _strtoul_r is a reentrant version. The extra argument reent
 * is a pointer to a reentrancy structure.
 *
 * Returns
 * strtoul returns the converted value, if any. If no conversion was made, 0 is returned.
 *
 * strtoul returns ULONG_MAX if the magnitude of the converted value is too large, and
 * sets errno to ERANGE.
 *
 * Portability
 * strtoul is ANSI.
 *
 * strtoul requires no supporting OS subroutines.
 *
 */

#include <stdio.h>
#include <stdlib.h>

int main ( void )
{
  char szNumbers[] = "2001 60c0c0 -2010 0x6fffff";
  char * pEnd;
  long int li1, li2, li3, li4;
  li1 = strtoul (szNumbers,&pEnd,10);
  li2 = strtoul (pEnd,&pEnd,16);
  li3 = strtoul (pEnd,&pEnd,10);
  li4 = strtoul (pEnd,NULL,0);

  if ( li1 != 2001L )
    return ( -1 );

  if ( li2 != 6340800L )
    return ( -1 );

  if ( li3 != -2010L )
    return ( -1 );

  if ( li4 != 7340031L )
    return ( -1 );

  return 0;
}

