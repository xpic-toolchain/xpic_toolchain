/*
 * fixsfsi_test.c
 *
 * Test for runtime function: int __fixsfsi (float [x])
 * These function (__fixsfsi) converts [x] to a signed integer, rounding toward zero.
 * Example: 1.42 => 1
 *
 */

int __fixsfsi (float f);

int main (void)
{
  float a, b, c, d, e, f, g;
  int ret;

  a = 0.0f;
  ret = __fixsfsi(a);
  if (ret != 0)
  {
    int reta = ret;
    return -1;
  }

  b = 3.0f;
  ret = __fixsfsi(b);
  if (ret != 3)
  {
    int retb = ret;
    return -1;
  }


  c = 50.0f;
  ret = __fixsfsi(c);
  if (ret != 50)
  {
    int retc = ret;
    return -1;
  }
  return 0;
}
