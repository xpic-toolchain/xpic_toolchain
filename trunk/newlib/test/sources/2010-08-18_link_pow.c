#include <math.h>

int main()
{
  volatile double x = 3.0;

  x = pow(2.0, x);

  x -= 8.0;

  return (int)x;
}
