/* Problem only noticed on SH for -mcbranchdi DImode comparison with constants.
 * Target dependant failure but test valid for alls.  */

int test2(long long n)
{
  if (n < 2)
    return 1;
  return 0;
}

int test1(long long n)
{
  if (n < 1)
    return 1;
  return 0;
}

int test0(long long n)
{
  if (n < 0)
    return 1;
  return 0;
}

int test1n(long long n)
{
  if (n < -1LL)
    return 1;
  return 0;
}

int test2n(long long n)
{
  if (n < -2LL)
    return 1;
  return 0;
}

int main()
{
  if (test2n (-1LL))
    return -1;

  if (test2n (-2LL))
    return -1;

  if (test2n (0LL))
    return -1;

  if (test2n (1LL))
    return -1;

  if (test2n (2LL))
    return -1;
 
  if (test1n (-1LL))
    return -1;

  if (!test1n (-2LL))
    return -1;

  if (test1n (0LL))
    return -1;

  if (test1n (1LL))
    return -1;

  if (test1n (2LL))
    return -1;

  if (!test0 (-1LL))
    return -1;

  if (!test0 (-2LL))
    return -1;

  if (test0 (0LL))
    return -1;

  if (test0 (1LL))
    return -1;

  if (test0 (2LL))
    return -1;

  if (!test2 (-1LL))
    return -1;

  if (!test2 (-2LL))
    return -1;

  if (!test2 (0LL))
    return -1;

  if (!test2 (1LL))
    return -1;

  if (test2 (2LL))
    return -1;

  if (!test1 (-1LL))
    return -1;

  if (!test1 (-2LL))
    return -1;

  if (!test1 (0LL))
    return -1;

  if (test1 (1LL))
    return -1;

  if (test1 (2LL))
    return -1;

  return 0;
}



