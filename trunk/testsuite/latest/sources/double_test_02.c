/* Copyright (C) 2003 Free Software Foundation.

   Check that constant folding of mathematical expressions doesn't
   break anything.

   Written by Roger Sayle, 24th August 2003.
*/

#include <stdlib.h>

void ab(void) {
	while(1) {
	}
}

double foo(double x)
{
  return 12.0/(x*3.0);
}

double bar(double x)
{
  return (3.0/x)*4.0;
}

int main()
{
  if (foo(2.0) != 2.0)
    ab ();

  if (bar(2.0) != 6.0)
    ab ();

  return 0;
}

