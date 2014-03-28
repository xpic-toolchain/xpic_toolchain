/* Contributed by Mikulas Patocka <mikulas@artax.karlin.mff.cuni.cz> */

void abort (void)
{
  while (1)
  {
    ;
  }
}

unsigned f (int x)
{
  return (unsigned) (x / 2) / 2;
}

unsigned f1 (int x)
{
  unsigned xx = x / 2;
  return xx / 2;
}

int main ()
{
  if (f1 (-5) != f (-5))
    abort ();
  return 0;
}
