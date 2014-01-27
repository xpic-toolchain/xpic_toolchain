/*
 * Test functions memcpy() memcmp()
 * Test passed if main() returns a value of 0
 */

#include <string.h>
#include <stdlib.h>

#define BUFSIZE 1024

#define PERF_MEASURE

#include "common.h"
#ifdef PERF_MEASURE
#include XPIC_TIMER_H
#endif

char buf1[BUFSIZE+1];
char buf2[BUFSIZE+1];


void check(int size)
{
  int j;

  for( j=0; j < size; ++j )
  {
    buf1[j] = (j & 0xff);
  }

  memcpy(buf2, buf1, size);
  if( memcmp(buf2, buf1, size) )
    abort(); /* ERROR */
  buf2[size >> 1] = ~buf2[size >> 1];
  if( memcmp(buf2, buf1, size) == 0 )
    abort(); /* ERROR */

  memcpy(buf2, buf1, size);
  if( memcmp(buf2, buf1, size >> 1) )
    abort(); /* ERROR */

  memcpy(&buf2[1], &buf1[1], size);
  if( memcmp(&buf2[1], &buf1[1], size >> 1) )
    abort(); /* ERROR */

  memcpy(&buf2[2], &buf1[2], size);
  if( memcmp(&buf2[2], &buf1[2], size >> 1) )
    abort(); /* ERROR */

  memcpy(&buf2[3], &buf1[3], size);
  if( memcmp(&buf2[3], &buf1[3], size >> 1) )
    abort(); /* ERROR */


  memcpy(&buf2[1], buf1, size);
  if( memcmp(&buf2[1], buf1, size) )
    abort(); /* ERROR */

  memcpy(&buf2[2], buf1, size);
  if( memcmp(&buf2[2], buf1, size) )
    abort(); /* ERROR */

  memcpy(&buf2[2], &buf1[1], size);
  if( memcmp(&buf2[2], &buf1[1], size) )
    abort(); /* ERROR */

  memcpy(&buf2[3], buf1, size);
  if( memcmp(&buf2[3], buf1, size) )
    abort(); /* ERROR */

  memcpy(&buf2[3], &buf1[1], size);
  if( memcmp(&buf2[3], &buf1[1], size) )
    abort(); /* ERROR */

  memcpy(&buf2[3], &buf1[2], size);
  if( memcmp(&buf2[3], &buf1[2], size) )
    abort(); /* ERROR */


  memcpy(buf2, &buf1[1], size);
  if( memcmp(buf2, &buf1[1], size) )
    abort(); /* ERROR */

  memcpy(buf2, &buf1[2], size);
  if( memcmp(buf2, &buf1[2], size) )
    abort(); /* ERROR */

  memcpy(&buf2[1], &buf1[2], size);
  if( memcmp(&buf2[1], &buf1[2], size) )
    abort(); /* ERROR */

  memcpy(buf2, &buf1[3], size);
  if( memcmp(buf2, &buf1[3], size) )
    abort(); /* ERROR */

  memcpy(&buf2[1], &buf1[3], size);
  if( memcmp(&buf2[1], &buf1[3], size) )
    abort(); /* ERROR */

  memcpy(&buf2[2], &buf1[3], size);
  if( memcmp(&buf2[2], &buf1[3], size) )
    abort(); /* ERROR */
}

int main(void)
{
  int i;
#ifdef PERF_MEASURE
  unsigned long ulS, ulNs;

  XpicTimer_GetSystime(&ulS, &ulNs);
#endif

  for( i = 2; i < BUFSIZE; i <<= 1)
  {
    check(i-1);
    check(i);
    check(i+1);
  }

#ifdef PERF_MEASURE
  XpicTimer_GetSystimeDiff(&ulS, &ulNs);
#endif

  /* Test finished successfully */
  return 0;
}
