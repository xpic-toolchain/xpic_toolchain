/* PR tree-optimization/9814  */

void ab(void) {
	while(1) {
	}
}

int test1(int x)
{
  if (x & 2)
    x |= 2;
  return x;
}

int test2(int x)
{
  if (!(x & 2))
    x |= 2;
  return x;
}

int test3(int x)
{
  if (x & 2)
    x ^= 2;
  return x;
}

int test4(int x)
{
  if (!(x & 2))
    x ^= 2;
  return x;
}

int test5(int x)
{
  if (x & 2)
    x &= ~2;
  return x;
}

int test6(int x)
{
  if (!(x & 2))
    x &= ~2;
  return x;
}

int main()
{
  if (test1(0) != 0)
    ab();
  if (test1(2) != 2)
    ab();
  if (test1(5) != 5)
    ab();
  if (test1(7) != 7)
    ab();

  if (test2(0) != 2)
    ab();
  if (test2(2) != 2)
    ab();
  if (test2(5) != 7)
    ab();
  if (test2(7) != 7)
    ab();

  if (test3(0) != 0)
    ab();
  if (test3(2) != 0)
    ab();
  if (test3(5) != 5)
    ab();
  if (test3(7) != 5)
    return -1;

  if (test4(0) != 2)
    return -1;
  if (test4(2) != 2)
    return -1;
  if (test4(5) != 7)
    return -1;
  if (test4(7) != 7)
    return -1;

  if (test5(0) != 0)
    return -1;
  if (test5(2) != 0)
    return -1;
  if (test5(5) != 5)
    return -1;
  if (test5(7) != 5)
    return -1;

  if (test6(0) != 0)
    return -1;
  if (test6(2) != 2)
    return -1;
  if (test6(5) != 5)
    return -1;
  if (test6(7) != 7)
    return -1;

  return 0;
}

