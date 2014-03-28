void abort (void)
{
  while (1)
  {
    ;
  }
}

int bar (int x, char **y)
{
  if (x != 56)
    abort ();
  if (**y != 'a')
    abort ();
  *y = "def";
  return 1;
}

int baz (int x, char **y)
{
  if (x != 56)
    abort ();
  if (**y != 'a')
    abort ();
  return 26;
}

int foo (int x, char *y)
{
  int a;
  char *b = y;
  a = bar (x, &y);
  if (a)
    {
      y = b;
      a = baz (x, &y);
    }
  if (a)
    return a;

  baz (x, &y);
  return 0;
}

int main ()
{
  if (foo (56, "abc") != 26)
    abort ();
  return 0;
}
