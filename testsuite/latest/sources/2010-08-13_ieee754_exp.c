/*
 * e**(double)x
 * libm
 * Author: bilsen@hilscher.com
 */

double __ieee754_exp(double x);

int main(void)
{
  double x = __ieee754_exp(0);
  if( x != 1.0 )
    return 1;

  return 0;
}
