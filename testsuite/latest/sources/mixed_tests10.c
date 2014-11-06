void abort (void);

#define small   __attribute__((mode(QI))) int
int main()
{
#ifdef __gcc__
  int x, y = 0x400;

  x = (small) y;
  if (sizeof (small) != sizeof (char))
    abort ();
  if (sizeof (x) != sizeof (char) && x == y)
    abort ();
#endif
  return 0;
}
