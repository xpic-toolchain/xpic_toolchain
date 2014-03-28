/*
 * fixsfsi_test.c
 *
 *  Created on: 13.08.2010
 *      Author: Burkhard
 *
 * Test for runtime functions:
 *     int __fixsfsi(float a);
 *     float __floatsisf(int a);
 *
 * Explicit vs. implicit calls
 *
 */
int __fixsfsi(float a);
float __floatsisf(int a);

int main (void)
{
  float f;
  int i;


  f = 3.98f;
  i = __fixsfsi(f);
  if (i != 3)
    return 1;

  f = i;
  if( f != 3.0f )
    return 2;

  i = -150;
  f = __floatsisf(i);
  if( f != -150.0f )
    return 3;

  i = f;
  if( i != -150 )
    return 4;

  return 0;
}
