/*
 * divsf3_extract_mini.c
 *
 *  Created on: 06.09.2010
 *      Author: AH
 *
 *  Problem: Function __divsf3() returns 2.25f where 2.4f is expected (release-/
 *  debugrel version); debug version works fine.
 *
 */

long __divsf3(unsigned long u_f, unsigned long v_f)
{
  int msb = 0;
  long R_f = 1;
  int count = 26;

  do {
    msb = u_f >> 31;
    u_f <<= 1;
    R_f <<= 1;
    if (msb || (u_f >= v_f) )
    {
      u_f = u_f - v_f;
      R_f |= 1;
    }
  } while (--count > 0);

  return R_f;
}

int main (void)
{
  long ret = __divsf3(0x20000000,0xa0000000);
  if ( ret == 0x4cccccc )
    return ( 0 );

  return -1;
}
