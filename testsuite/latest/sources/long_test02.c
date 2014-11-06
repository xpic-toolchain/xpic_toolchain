void abort (void)
{
  while (1)
  {
    ;
  }
}

typedef unsigned long uint32_t;

#define rotl(x,n)   (((x) << ((int)(n))) | ((x) >> (32 - (int)(n))))

uint32_t
f (uint32_t a, uint32_t b)
{

  b = rotl (a, b & 31);
  return b;
}

int
main ()
{
  if (f(2,31) != 1)
    abort ();
  return 0;
}
