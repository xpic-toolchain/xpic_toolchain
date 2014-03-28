

double foo (double a, double b)
{
  double c;
  if (0.0 < a)
    c = a;
  else if (b > 0.0)
    c = 0.0;
  else
    return 0;
  return 2.0 * (b - c);
}

int main ()
{
  if (foo (1.0, 4.0) != 6.0)
    return -1;
  return 0;
}
