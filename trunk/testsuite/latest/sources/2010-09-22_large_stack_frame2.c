/* BI: The stack frame for this main function is very large
 * The addp function call tries to construct a parameter by adding a large constant
 * to r7 but the constant is too big for immediate adding.
 */

int addp(int* p1, char*p2)
{
  return *p1 + *p2;
}

int main(void)
{
  int a = 5;
  char buf[16385];

  buf[0] = -5;

  return addp(&a, buf);
}
