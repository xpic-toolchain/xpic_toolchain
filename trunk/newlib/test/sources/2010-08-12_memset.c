/*
 * Test function memset()
 * Test passed if main() returns a value of 0
 */

#include <string.h>
#include <stdlib.h>

#define BUFSIZE 1024

char buf[BUFSIZE+1];

void check(int pattern, int size)
{
  int j;

  memset(buf, pattern, size);
  for(j = 0; j < size; ++j)
  {
    if( (buf[j] & 0xff) != pattern )
      abort(); /* ERROR */
  }

  memset(buf, pattern, size + 1);
  for(j = 0; j < size; ++j)
  {
    if( (buf[j] & 0xff) != pattern )
      abort(); /* ERROR */
  }

  memset(&buf[1], pattern, size);
  for(j = 0; j < size; ++j)
  {
    if( (buf[j] & 0xff) != pattern )
      abort(); /* ERROR */
  }
}

int main(void)
{
  int i;

  for( i = 1; i < BUFSIZE; i <<= 1)
  {
    check(0x00, i);
    check(0xff, i);
    check(0xaa, i);
    check(0x55, i);
  }

  /* Test finished successfully */
  return 0;
}
