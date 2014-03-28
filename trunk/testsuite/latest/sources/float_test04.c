/*
 * float_test04.c
 *
 *  Created on: 12.08.2010
 *      Author: Alexander
 */

float floattest (float a, float b, float c)
{
  return ((a*b)+c);
}

float int_to_float(int i)
{
  return ( ((float)i) );
}

int main (void)
{
  float a, b, c, d, e, f, g, h, i;
  float *p;

  if ((0.1f+0.2f) != 0.3f)
    return -1;
  if ((0.1f+0.2f+0.3f) != 0.6f)
    return -1;
  if ((0.1f+0.2f+0.3f+0.4f) != 1.0f)
    return -1;
  if ((0.25f-0.25f+0.5f) != 0.5f)
    return -1;
  if ((0.25f-0.25f+0.5f*0.12345f) != 0.061725f)
    return -1;
  if ((0.25f-0.25f+0.5f*0.12345f)/0.061725f != 1.0f)
    return -1;
  if ((0.25f)/0.0f == 0.0f || (0.25f)/0.0f == 0.25f)
    return -1;
  if ((0.25f)*0.0f != 0.0f)
    return -1;

  a = 0xFFFFFFFF;
  b = 0x0000FFFF;
  c = 0xFFFF0000;
  if (b+c != a)
    return -1;

  d = int_to_float(23);
  if (d != 23.0f)
    return -1;
  e = int_to_float(-23);
  if (e != -23.0f)
    return -1;

  f = int_to_float(0);
  if (f != 0.0f)
    return -1;

  if (-1.25f/2.5f != -0.5f)
    return -1;
  if (1.25f/-2.5f != -0.5f)
    return -1;
  if (-1.25f/-2.5f != 0.5f)
    return -1;
  if (1.25f/2.5f != 0.5f)
    return -1;

  if (-1.25f*2.5f != -3.125f)
    return -1;
  if (1.25f*-2.5f != -3.125f)
    return -1;
  if (-1.25f*-2.5f != 3.125f)
    return -1;
  if (1.25f*2.5f != 3.125f)
    return -1;

  // FIXME: Nicht moeglich als FLOAT
  // Vergleich der Consolenausgabe (win32) mit UART wenn printf auf dem xpic geht
  h = floattest(3.20f, 21.20f, -23.20f);
  if ( !(h <= 44.64f+0.1f || h >= 44.64f-0.1f))
    return -2;

  return 0;
}
