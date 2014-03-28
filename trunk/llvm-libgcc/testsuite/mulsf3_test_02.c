/*
 * mulsf3_test_02.c
 *
 *  Test for runtime function: float __mulsf3 (float [a], float [b])
 *  The function (__mulsf3) returns the product of [a] and [b].
 *
 */

float __mulsf3(float a, float b);

int main (void)
{
  float a, b;
  float ret;

  a = 4.0; // ... = 0x40800000 = 1082130432
  b = 10.0; // ... = 0x41200000 = 1092616192
  ret = __mulsf3(a,b);
  if (ret != 40.f ) // ... = 0x42200000 = 1109393408
    return -1;

  a = 0.0;
  b = 0.0;
  ret = __mulsf3(a,b);
  if (ret != 0.0f )
    return -1;

  a = 1.0; // ... = 0x3F800000 = 1065353216
  b = 0.0;
  ret = __mulsf3(a,b);
  if (ret != 0.f )
    return -1;

  a = 0.0;
  b = 1.0;
  ret = __mulsf3(a,b);
  if (ret != 0.f )
    return -1;

  a = 1.0;
  b = 1.0;
  ret = __mulsf3(a,b);
  if (ret != 1.f )
    return -1;

  a = -1.0;
  b = -1.0;
  ret = __mulsf3(a,b);
  if (ret != 1.f )
    return -1;

  a = -1.0;
  b = 1.0;
  ret = __mulsf3(a,b);
  if (ret != -1.f )
    return -1;

  a = 1.0;
  b = -1.0;
  ret = __mulsf3(a,b);
  if (ret != -1.f )
    return -1;

  a = 5.5;
  b = -1.0;
  ret = __mulsf3(a,b);
  if (ret != -5.5f )
    return -1;

  a = 5.5;
  b = 1.0;
  ret = __mulsf3(a,b);
  if (ret != 5.5f )
    return -1;

  a = 23.50;
  b = 14.25;
  ret = __mulsf3(a,b);
  if (ret != 334.875f )
    return -1;

  a = -23.50;
  b = 14.25;
  ret = __mulsf3(a,b);
  if (ret != -334.875f )
    return -1;

  a = 23.50;
  b = -14.25;
  ret = __mulsf3(a,b);
  if (ret != -334.875f )
    return -1;

  return 0;
}
