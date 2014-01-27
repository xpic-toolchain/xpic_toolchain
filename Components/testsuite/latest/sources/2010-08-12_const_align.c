/*
 * llc -march=xpic
 * Wrong alignment bug for constant
 */

int main ()
{
  volatile char* str="1";
  volatile int x = 16;

  x = (str[0] << x) & 0xffff;

  return x;
}
