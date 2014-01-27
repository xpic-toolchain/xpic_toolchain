/*
 * double_test04.c
 *
 *  Created on: 12.08.2010
 *      Author: Alexander
 */

double doubletest (double a, double b, double c)
{
  return ((a*b)+c);
}

double int_to_double(int i)
{
  return ( ((double)i) );
}

int main (void)
{
  // FIXME: Nicht moeglich als DOUBLE
  // Vergleich der Consolenausgabe (win32) mit UART wenn printf auf dem xpic geht
  double a, b, c, d, e, f, g, h, i;
  double *p;

  a = 0.1;
  b = 0.2;
  c = 0.3;
  if ( !((a+b) <= (c+0.000001) || (a+b) >= (c-0.000001)))
    return 10;

  if ( !(((0.1+0.2+0.3)) <= (0.6+0.000001) || ((0.1+0.2+0.3)) >= (0.6-0.000001)))
    return 11;
  if ((0.1+0.2+0.3+0.4) != 1.0)
    return 12;
  if ((0.25-0.25+0.5) != 0.5)
    return 13;
  if ((0.25-0.25+0.5*0.12345) != 0.061725)
    return 14;
  if ((0.25-0.25+0.5*0.12345)/0.061725 != 1.0)
    return 15;
  if ((0.25)/0.0f == 0.0 || (0.25)/0.0 == 0.25)
    return 16;
  if ((0.25)*0.0f != 0.0)
    return 17;

  a = 0xFFFFFFFF;
  b = 0x0000FFFF;
  c = 0xFFFF0000;
  if (b+c != a)
    return 18;

  d = int_to_double(23);
  if (d != 23.0)
    return 19;
  e = int_to_double(-23);
  if (e != -23.0)
    return 20;

  f = int_to_double(0);
  if (f != 0.0)
    return 21;

  if (-1.25f/2.5f != -0.5f)
    return 22;
  if (1.25f/-2.5f != -0.5f)
    return 23;
  if (-1.25f/-2.5f != 0.5f)
    return 24;
  if (1.25f/2.5f != 0.5f)
    return 25;

  if (-1.25f*2.5f != -3.125f)
    return 26;
  if (1.25f*-2.5f != -3.125f)
    return 27;
  if (-1.25f*-2.5f != 3.125f)
    return 28;
  if (1.25f*2.5f != 3.125f)
    return 29;

  h = doubletest(3.20, 21.20, -23.20);
  if ( !(h <= 44.64+0.1 || h >= 44.64-0.1))
    return -2;

  return 0;
}
