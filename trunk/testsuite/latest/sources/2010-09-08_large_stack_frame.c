/* BI: The stack frame for this function is very large
 * The frame can not be allocated with "add r7, #-16384, r7
 */

int bigStack(unsigned x)
{
  char buf[16385];
  do {
    buf[x] = x;
  }
  while(x--);
  return buf[0];
}

int main(void)
{
  return bigStack(3);
}
