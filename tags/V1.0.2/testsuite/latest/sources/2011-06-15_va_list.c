#include "stdarg.h"

int f1(int factor, int num,...)
{
  int a = 0;
  va_list vl;
  va_start(vl,num);

  while(num--)
  {
    a += va_arg(vl,int)*factor;
  }
  va_end(vl);

  return a;
}

int main ( void )
{
  return f1(-1,3,1,-3,2);
}

