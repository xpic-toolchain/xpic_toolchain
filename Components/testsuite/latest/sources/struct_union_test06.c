void abort (void)
{
  while (1)
  {
    ;
  }
}

int main (void)
{
  struct { int count: 2; } s = { -2 };
  while (s.count-- != -2)
    abort ();
  return 0;
}
