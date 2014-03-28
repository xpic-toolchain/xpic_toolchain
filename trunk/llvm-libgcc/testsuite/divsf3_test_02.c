/*
 * divsf3_test_02.c
 *
 * — Runtime Function: float __divsf3 (float a, float b)
 * These functions return the quotient of a and b; that is, a / b.
 *
 */

float __divsf3(float a, float b);

int main (void)
{
  float a, b;
  float ret;

  a = 10.0f;
  b = 5.0f;
  ret = __divsf3(a,b);
  if (ret != 2.0f)
    return 1;

  a = -10.0f;
  b = 5.0f;
  ret = __divsf3(a,b);
  if (ret != -2.0f)
    return 2;

  a = 10.0f;
  b = -5.0f;
  ret = __divsf3(a,b);
  if (ret != -2.0f)
    return 3;

  a = -10.0f;
  b = -5.0f;
  ret = __divsf3(a,b);
  if (ret != 2.0f)
    return 4;

  a = 2.50f;
  b = 1.25f;
  ret = __divsf3(a,b);
  if (ret != 2.0f)
    return 5;

  a = 3.0f;
  b = 1.25f;
  ret = __divsf3(a,b);
  if (ret != 2.4f)
    return 6;

  a = -3.0f;
  b = 1.25f;
  ret = __divsf3(a,b);
  if (ret != -2.4f)
    return 7;

  a = 3.0f;
  b = -1.25f;
  ret = __divsf3(a,b);
  if (ret != -2.4f)
    return 8;

  a = -3.0f;
  b = -1.25f;
  ret = __divsf3(a,b);
  if (ret != 2.4f)
    return 9;

  a = 1.0f;
  b = 1.0f;
  ret = __divsf3(a,b);
  if (ret != 1.0f)
    return 10;

  return 0;
}
