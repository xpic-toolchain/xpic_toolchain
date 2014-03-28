/*
 * atoi, atol---string to integer
 *
 * Synopsis
 * #include <stdlib.h>
 * int atoi(const char *s);
 * long atol(const char *s);
 *
 *
 * Description
 * atoi converts the initial portion of a string to an int.
 * atol converts the initial portion of a string to a long.
 *
 * atoi(s) is implemented as (int)strtol(s, NULL, 10). atol(s) is
 * implemented as strtol(s, NULL, 10).
 *
 * Returns
 * The functions return the converted value, if any. If no conversion
 * was made, 0 is returned.
 *
 * Portability
 * atoi is ANSI.
 *
 * No supporting OS subroutines are required.
 *
 */

#include <stdlib.h>

int main ( void )
{
  char my_string [12] = "\0\0\0\0\0\0\0\0\0\0\0\0";
  int number;

  my_string[0] = '0';
  number = atoi ( my_string );
  if ( number != 0 )
    return ( -1 );

  my_string[0] = '1';
  number = atoi ( my_string );
  if ( number != 1 )
    return ( -1 );

  my_string[0] = '-';
  my_string[1] = '1';
  number = atoi ( my_string );
  if ( number != -1 )
    return ( -1 );

  my_string[0] = '+';
  my_string[1] = '1';
  number = atoi ( my_string );
  if ( number != +1 )
    return ( -1 );

  my_string[0] = '2';
  my_string[1] = '1';
  my_string[2] = '4';
  my_string[3] = '7';
  my_string[4] = '4';
  my_string[5] = '8';
  my_string[6] = '3';
  my_string[7] = '6';
  my_string[8] = '4';
  my_string[9] = '7';
  number = atoi ( my_string );
  if ( number != 2147483647 )
    return ( -1 );

  my_string[0] = '-';
  my_string[1] = '2';
  my_string[2] = '1';
  my_string[3] = '4';
  my_string[4] = '7';
  my_string[5] = '4';
  my_string[6] = '8';
  my_string[7] = '3';
  my_string[8] = '6';
  my_string[9] = '4';
  my_string[10] = '8';
  number = atoi ( my_string );
  if ( number != -2147483648 )
    return ( -1 );

  my_string[0] = '+';
  my_string[1] = '2';
  my_string[2] = '1';
  my_string[3] = '4';
  my_string[4] = '7';
  my_string[5] = '4';
  my_string[6] = '8';
  my_string[7] = '3';
  my_string[8] = '6';
  my_string[9] = '4';
  my_string[10] = '7';
  number = atoi ( my_string );
  if ( number != +2147483647 )
    return ( -1 );

  return ( 0 );
}
