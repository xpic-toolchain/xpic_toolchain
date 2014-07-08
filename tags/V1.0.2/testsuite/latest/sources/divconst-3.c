/* This test is expected to pass when the udivmoddi4 function of libgcc is working correctly! */

long long
f (long long x)
{
  return x / 10000000000LL;
}

main ()
{
  if (f (10000000000LL) != 1 || f (100000000000LL) != 10)
    return -1;
  return 0;
}
