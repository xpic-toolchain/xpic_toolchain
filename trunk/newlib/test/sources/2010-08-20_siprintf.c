/*
 * siprintf---write formatted output (integer only)
 *
 * Synopsis
 * #include <stdio.h>
 *
 * int siprintf(char *str, const char *format [, arg, ...]);
 *
 * Description
 * siprintf is a restricted version of sprintf: it has the same arguments and
 * behavior, save that it cannot perform any floating-point formatting:
 * the f, g, G, e, and F type specifiers are not recognized.
 *
 * Returns
 * siprintf returns the number of bytes in the output string, save that the
 * concluding NULL is not counted. siprintf returns when the end of the format
 * string is encountered.
 *
 * Portability
 * siprintf is not required by ANSI C.
 *
 * Supporting OS subroutines required: close, fstat, isatty, lseek, read, sbrk, write.
 *
 */

#include <stdio.h>
#include <stdlib.h>

int main ( void )
{
  char test[16] = "ADCDEF00";
  int retval_test = siprintf(test, "%d %d %d %d", 1,2,3,4);
  return ( (retval_test-7) );
}
