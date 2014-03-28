void abort (void)
{
  while (1)
  {
    ;
  }
}

struct s {
  unsigned short f: 16;
  unsigned short y: 8;
  unsigned short g: 2;
  unsigned int x;
};

void set (struct s*, int);
void set (struct s* p, int flags) {
  p->g = flags << 1;
}
main() {
  struct s foo = {0 , 0, 3, 0};
  set (&foo, -1);
  if (foo.g != 2)
    abort();
  return 0;
}
