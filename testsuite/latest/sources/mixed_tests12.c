#ifndef UCHAR_MAX
#define UCHAR_MAX 255
#endif

void abort (void)
{
  while (1)
  {
    ;
  }
}

void foo (void)
{
  int i;
  char *p;

  p = (char *) &i;
  *p = -10;
  if (* (unsigned char *) p != 0x100 - 10)
    abort();
}

int main (void)
{
  if (UCHAR_MAX == 255)
    foo ();
  return 0;
}
