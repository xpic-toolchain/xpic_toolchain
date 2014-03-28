/* { dg-do compile } */
/* { dg-options "-O2" } */

double
crashme (double v, double *p)
{
  if (v < 0.0 && *p == 1.)
    v = 0.0;
  return v;
}

int main (void)
{
  double x = 1.0;
  double *p = &x;
  int ret = crashme(-1.5,p);
  return (ret);
}
