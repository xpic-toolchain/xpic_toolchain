/*
 * fixunssfsi_test.c
 *
 *  Tests runtime function: unsigned int __fixunssfsi (float [x])
 *  These function converts [x] to an unsigned integer, rounding toward zero.
 *  The negative values will become zero.
 *
 */

unsigned int __fixunssfsi(float x);

int main (void)
{
  float a, b, c, d, e, f, g;
  unsigned int ret;

  a = 0.0f;
  ret = __fixunssfsi(a);
  if (ret != 0)
  {
    return -1;
  }

  b = 4.25f;
  ret = __fixunssfsi(b);
  if (ret != 4)
  {
    return -1;
  }











  return 0;
}
