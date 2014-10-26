void ab (void)
{
  while (1)
  {
    ;
  }
}

int bar (int x, char **y)
{
  if (x != 56)
    ab ();
  if (**y != 'a')
    ab ();
  *y = "def";
  return 1;
}

int baz (int x, char **y)
{
  if (x != 56)
    ab ();
  if (**y != 'a')
    ab ();
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
    ab ();
  return 0;
}
