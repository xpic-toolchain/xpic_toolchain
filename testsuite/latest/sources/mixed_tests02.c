void abort8 (void)
{
  while(1) { ; }
}

void exit8 (void)
{
  while(1) { ; }
}

int bar8 (int x, char **y)
{
  if (x != 56)
    abort8 ();
  if (**y != 'a')
    abort8 ();
  *y = "def";
  return 1;
}

int baz8 (int x, char **y)
{
  if (x != 56)
    abort8 ();
  if (**y != 'a')
    abort8 ();
  return 26;
}

int foo8 (int x, char *y)
{
  int a;
  char *b = y;
  a = bar8 (x, &y);
  if (a)
    {
      y = b;
      a = baz8 (x, &y);
    }
  if (a)
    return a;

  baz8 (x, &y);
  return 0;
}

int main ()
{
  if (foo8 (56, "abc") != 26)
    abort8 ();
  return 0;
}
