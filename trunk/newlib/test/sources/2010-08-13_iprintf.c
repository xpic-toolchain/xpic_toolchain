#include <stdio.h>

int main(void)
{
  int test = 2010;

  test = iprintf("Source: %d-%02d-%d_printf.c", test, 8, 13);

  return (test - 27);
}
