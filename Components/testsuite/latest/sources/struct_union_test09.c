/* This failed because if conversion didn't handle insv patterns properly.  */

void abort (void)
{
  while (1)
  {
    ;
  }
}

union y
{
  int a;
  unsigned short b;
};

void bar (unsigned short u, union y v)
{
  if (u != 1)
    abort ();
}

void foo (int check)
{
  union y x;

  if (check != 0)
    x.b = 1;
  else
    x.b = 2;
  bar (x.b, x);
}

int main ()
{
  foo (1);
  return 0;
}
