/* Copyright (C) 2002 Free Software Foundation.
  
   Test floating point negation produces the expected results.
  
   Written by Roger Sayle, 21st May 2002.  */

/* { dg-do run } */
/* { dg-options "-O2 -ffast-math" } */

void ab (void)
{
  while (1)
  {
    ;
  }
}


double
dneg (double x)
{
  return -x;
}

double
dmult (double x)
{
  return -1.0 * x;
}

double
ddiv (double x)
{
  return x / -1.0;
}


float
fneg (float x)
{
  return -x;
}

float
fmult (float x)
{
  return -1.0f * x;
}

float
fdiv (float x)
{
  return x / -1.0f;
}


void
ftest(float src, float dst)
{
  if (fneg (src) != dst)
    ab ();

  if (src != fneg (dst))
    ab ();

  if (fmult (src) != dst)
    ab ();

  if (src != fmult (dst))
    ab ();

  if (fdiv (src) != dst)
    ab ();

  if (src != fdiv(dst))
    ab ();
}

void
dtest(double src, double dst)
{
  if (dneg (src) != dst)
    ab ();

  if (src != dneg (dst))
    ab ();

  if (dmult (src) != dst)
    ab ();

  if (src != dmult (dst))
    ab ();

  if (ddiv (src) != dst)
    ab ();

  if (src != ddiv(dst))
    ab ();
}


int
main ()
{
  ftest (1.0f, -1.0f);
  ftest (2.0f, -2.0f);
  ftest (-3.0f, 3.0f);
  ftest (0.0f, -0.0f);
  ftest (-0.0f, 0.0f);

  dtest (1.0, -1.0);
  dtest (2.0, -2.0);
  dtest (-3.0, 3.0);
  dtest (0.0, -0.0);
  dtest (-0.0, 0.0);

  return 0;
}

