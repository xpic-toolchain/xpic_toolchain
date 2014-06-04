/*
 */

volatile unsigned long long s_ullData = 0xffff7fffffffffffULL;


int main(void)
{
  unsigned long ulTest1, ulTest2;

  ulTest1 = s_ullData & 0xffffffff;
  ulTest1 -= 0xffffffff;

  ulTest2 = s_ullData >> 32;
  ulTest2 -= 0xffff7fff;

  return ulTest1 | ulTest2;
}
