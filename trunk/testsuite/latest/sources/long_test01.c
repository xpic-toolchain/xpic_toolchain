void ab (void)
{
  while (1)
  {
    ;
  }
}

typedef unsigned long uint32_t;

uint32_t
f (uint32_t *S, int j)
{
  uint32_t A, B, k, L[2] = {1234, 5678};
  int i, m;

  A = B = 0;
  for (i = 0; i < j; i++)
    {
      k = (S[i] + A + B) & 0xffffffffL;
      A = S[i] =
      ((k << (3 & 0x1f)) | ((k & 0xffffffff) >> (32 - (3 & 0x1f)))); 

      m = (int) (A + B);
      k = (L[i] + A + B) & 0xffffffffL;
      B = L[i] =
	((k << (m & 0x1f)) | ((k & 0xffffffff) >> (32 - (m & 0x1f))));
    }
  return L[0] + L[1];
}

int
main ()
{
  uint32_t S[2] = {0xffff, 0xffffff};

  if (f (S,2)!= 1392607300)
    ab();

  return 0;
}
