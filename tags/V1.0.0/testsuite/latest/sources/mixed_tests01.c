extern void abort7 (void)
{
  while (1) { ; }
}

void foo7 (unsigned long x)
{
}

typedef struct a {
  volatile unsigned int a1, a2, a3, a4, a5;
} *A;
typedef struct {
  volatile unsigned int b1, b2, b3, b4, b5;
} *B;
struct C {
  void *c1, *c2;
  A c3;
  unsigned char c4;
};

void
bar7 (struct C *c, unsigned int *d)
{
  *d = *d | 1;
  ((c->c4 >= 2)
   ? (*(volatile unsigned int *) ((void *)(&((A)c->c3)->a5)) = *d)
   : (*(volatile unsigned int *) ((void *)(&((B)c->c3)->b5)) = *d));

  foo7 (50);
}

int main (void)
{
  struct a a;
  struct C c;
  unsigned int d = 8;
  c.c3 = &a;
  c.c4 = 0;
  bar7 (&c, &d);
  if (a.a5 != 9)
    abort7 ();
  return 0;
}
