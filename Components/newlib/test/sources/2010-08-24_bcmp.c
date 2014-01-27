/*
 * bcmp---compare two memory areas
 *
 * Synopsis
 * #include <string.h>
 * int bcmp(const char *s1, const char *s2, size_t n);
 *
 * Description
 * This function compares not more than n characters of the object pointed to
 * by s1 with the object pointed to by s2.
 *
 * This function is identical to memcmp.
 *
 * Returns
 * The function returns an integer greater than, equal to or less than zero
 * according to whether the object pointed to by s1 is greater than, equal
 * to or less than the object pointed to by s2.
 *
 * Portability
 * bcmp requires no supporting OS subroutines.
 *
 */

#include <stdio.h>
#include <string.h>

int main ( void )
{
  char str1[256] = "ABC";
  char str2[256] = "DEF";
  int ret = bcmp(&str1, &str2, 3);
  if ( ret >= 0 )
    return ( -1 );

  char str3[4] = "DEF";
  char str4[4] = "ABC";
  ret = bcmp(&str3, &str4, 3);
  if ( !(ret >= 0) )
    return ( -1 );

  char str5[4] = "GHH";
  char str6[4] = "GHH";
  ret = bcmp(&str5, &str6, 3);
  if ( ret != 0 )
    return ( -1 );

  char str7[4] = "GH1";
  char str8[4] = "GH2";
  ret = bcmp(&str7, &str8, 1);
  if ( ret != 0 )
    return ( -1 );

  ret = bcmp(&str7, &str8, 2);
  if ( ret != 0 )
    return ( -1 );

  ret = bcmp(&str7, &str8, 3);
  if ( ret >= 0 )
    return ( -1 );

  return ( 0 );
}
