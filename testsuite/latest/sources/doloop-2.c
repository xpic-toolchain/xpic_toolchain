/* { dg-do run } */
/* { dg-options "-O2" } */
void abort (void)
{
  while (1)
  {
    ;
  }
}
unsigned global_iters;

void bi_reverse(int len)
{
    do {
	global_iters++;
    } while (--len > 0);
}

int main()
{
  bi_reverse(5);
  if (global_iters != 5)
    abort ();
  return 0;
}
