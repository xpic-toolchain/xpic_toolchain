/*
 * truncdfsf2_test_02.c
 *
 * — Runtime Function: float __truncdfsf2 (double a)
 *
 * These functions truncate a to the narrower mode of their return type,
 * rounding toward zero.
 *
 */

float __truncdfsf2 (double a);

int main (void)
{
  double a;
  float b, ret;

  a = 0.0;
  b = 0.0f;
  ret = __truncdfsf2(a);
  if (ret != b)
    return -1;

  a = 1.0;
  b = 1.0f;
  ret = __truncdfsf2(a);
  if (ret != b)
    return -1;

  a = 1.25;
  b = 1.25f;
  ret = __truncdfsf2(a);
  if (ret != b)
    return -1;

  a = 1.253;
  b = 1.253f;
  ret = __truncdfsf2(a);
  if (ret != b)
    return -1;

  a = 1.2535;
  b = 1.2535f;
  ret = __truncdfsf2(a);
  if (ret != b)
    return -1;

  a = 1.25356;
  b = 1.25356f;
  ret = __truncdfsf2(a);
  if (ret != b)
    return -1;

  a = 0.1;
  b = 0.1f;
  ret = __truncdfsf2(a);
  if (ret != b)
    return -1;

  a = 0.125;
  b = 0.125f;
  ret = __truncdfsf2(a);
  if (ret != b)
    return -1;

  a = 0.1255;
  b = 0.1255f;
  ret = __truncdfsf2(a);
  if (ret != b)
    return -1;

  a = 0.12557;
  b = 0.12557f;
  ret = __truncdfsf2(a);
  if (ret != b)
    return -1;

  a = 2541.12557;
  b = 2541.12557f;
  ret = __truncdfsf2(a);
  if (ret != b)
    return -1;

  /* Negativ */

  a = -1.0;
  b = -1.0f;
  ret = __truncdfsf2(a);
  if (ret != b)
    return -1;

  a = -1.25;
  b = -1.25f;
  ret = __truncdfsf2(a);
  if (ret != b)
    return -1;

  a = -1.253;
  b = -1.253f;
  ret = __truncdfsf2(a);
  if (ret != b)
    return -1;

  a = -1.2535;
  b = -1.2535f;
  ret = __truncdfsf2(a);
  if (ret != b)
    return -1;

  a = -1.25356;
  b = -1.25356f;
  ret = __truncdfsf2(a);
  if (ret != b)
    return -1;

  a = -0.1;
  b = -0.1f;
  ret = __truncdfsf2(a);
  if (ret != b)
    return -1;

  a = -0.125;
  b = -0.125f;
  ret = __truncdfsf2(a);
  if (ret != b)
    return -1;

  a = -0.1255;
  b = -0.1255f;
  ret = __truncdfsf2(a);
  if (ret != b)
    return -1;

  a = -0.12557;
  b = -0.12557f;
  ret = __truncdfsf2(a);
  if (ret != b)
    return -1;

  a = -2541.12557;
  b = -2541.12557f;
  ret = __truncdfsf2(a);
  if (ret != b)
    return -1;

  return 0;
}
