/*
 * Test functions memmove() memcmp()
 * Test passed if main() returns a value of 0
 */

#include <string.h>
#include <stdlib.h>

#define BUFSIZE 1024

char buf1[BUFSIZE+1];
char buf2[BUFSIZE+1];

void check(int size)
{
  int j;

  for( j=0; j < size; ++j )
  {
    buf1[j] = (j & 0xff);
  }

  memmove(buf2, buf1, size);
  if( memcmp(buf2, buf1, size) )
    abort(); /* ERROR */
  buf2[size >> 1] = ~buf2[size >> 1];
  if( memcmp(buf2, buf1, size) == 0 )
    abort(); /* ERROR */

  memmove(buf2, buf1, size);
  if( memcmp(buf2, buf1, size) )
    abort(); /* ERROR */
  memmove(&buf1[1], buf1, size);
  if( memcmp(&buf1[1], buf2, size) )
    abort(); /* ERROR */

  memmove(buf2, buf1, size);
  if( memcmp(buf2, buf1, size) )
    abort(); /* ERROR */
  memmove(buf1, &buf1[1], size-1);
  if( memcmp(buf1, &buf2[1], size-1) )
    abort(); /* ERROR */
}

int main(void)
{
  int i;

  for( i = 1; i < BUFSIZE; i <<= 1)
  {
    check(i);
  }

  /* Test finished successfully */
  return 0;
}
