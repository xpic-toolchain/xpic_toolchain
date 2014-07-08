/*
 * extendsfdf2_test_02.c
 *
 * — Runtime Function: double __extendsfdf2 (float a)
 * These functions extend a to the wider mode of their return type.
 *
 */

double __extendsfdf2 (float a);

int main (void)
{
  float a;
  double b, ret;

  a = 1.0f; // ... = 0x3F800000 = 1065353216
  b = 1.0; // ... = 0x3FF0000000000000 = [1072693248][0]
  ret = __extendsfdf2(a);
  if (ret != b)
    return -1;

  a = -1.0f;
  b = -1.0;
  ret = __extendsfdf2(a);
  if (ret != b)
    return -1;

  a = 0.0f;
  b = 0.0;
  ret = __extendsfdf2(a);
  if (ret != b)
    return -1;

  a = 5.25f;
  b = 5.25;
  ret = __extendsfdf2(a);
  if (ret != b)
    return -1;

  a = -5.25f;
  b = -5.25;
  ret = __extendsfdf2(a);
  if (ret != b)
    return -1;

  a = 53423.25f;
  b = 53423.25;
  ret = __extendsfdf2(a);
  if (ret != b)
    return -1;

  a = -534.25f;
  b = -534.25;
  ret = __extendsfdf2(a);
  if (ret != b)
    return -1;

  a = 53423.25f;
  b = -53423.25;
  ret = __extendsfdf2(a);
  if (ret == b)
    return -1;

  a = -534.25f;
  b = 534.25;
  ret = __extendsfdf2(a);
  if (ret == b)
    return -1;

  return 0;
}
