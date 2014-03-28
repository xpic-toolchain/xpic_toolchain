/* Copyright (C) 2003 Free Software Foundation.

   Test that the -fwrapv command line option is accepted and disables
   "unsafe" optimizations that rely on undefined arithmetic overflow.

   Written by Roger Sayle, 24th March 2003.  */

#include <limits.h>

int test(int x)
{
  return (2*x)/2;
}

main()
{
  int x = INT_MAX;

  if (test(x) == x)
    return -1;
  return 0;
}

