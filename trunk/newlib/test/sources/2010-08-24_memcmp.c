/*
 * memcpy---copy memory regions
 *
 * Synopsis
 * #include <string.h>
 * void* memcpy(void *out, const void *in, size_t n);
 *
 * Description
 * This function copies n bytes from the memory region pointed to by in to the
 * memory region pointed to by out.
 *
 * If the regions overlap, the behavior is undefined.
 *
 * Returns
 * memcpy returns a pointer to the first byte of the out region.
 *
 * Portability
 * memcpy is ANSI C.
 *
 * memcpy requires no supporting OS subroutines.
 *
 */

#include <stdio.h>
#include <string.h>


int main ( void )
{
  char str1[256] = "ABC";
  char str2[256] = "DEF";
  int ret = memcmp(&str1, &str2, 3);
  if ( ret >= 0 )
    return ( -1 );

  char str3[4] = "DEF";
  char str4[4] = "ABC";
  ret = memcmp(&str3, &str4, 3);
  if ( !(ret >= 0) )
    return ( -1 );

  char str5[4] = "GHH";
  char str6[4] = "GHH";
  ret = memcmp(&str5, &str6, 3);
  if ( ret != 0 )
    return ( -1 );

  char str7[4] = "GH1";
  char str8[4] = "GH2";
  ret = memcmp(&str7, &str8, 1);
  if ( ret != 0 )
    return ( -1 );

  ret = memcmp(&str7, &str8, 2);
  if ( ret != 0 )
    return ( -1 );

  ret = memcmp(&str7, &str8, 3);
  if ( ret >= 0 )
    return ( -1 );

  return ( 0 );
}
