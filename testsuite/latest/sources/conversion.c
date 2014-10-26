/* Test front-end conversions, optimizer conversions, and run-time
   conversions between different arithmetic types.

   Constants are specified in a non-obvious way to make them work for
   any word size.  Their value on a 32-bit machine is indicated in the
   comments.

   Note that this code is NOT intended for testing of accuracy of fp
   conversions.  */

float
u2f(u)
     unsigned int u;
{
  return u;
}

double
u2d(u)
     unsigned int u;
{
  return u;
}

 double
u2ld(u)
     unsigned int u;
{
  return u;
}

float
s2f(s)
     int s;
{
  return s;
}

double
s2d(s)
     int s;
{
  return s;
}

 double
s2ld(s)
     int s;
{
  return s;
}

int
fnear (float x, float y)
{
  float t = x - y;
  return t == 0 || x / t > 1000000.0;
}

int
dnear (double x, double y)
{
  double t = x - y;
  return t == 0 || x / t > 100000000000000.0;
}

int
ldnear ( double x,  double y)
{
   double t = x - y;
  return t == 0 || x / t > 100000000000000000000000000000000.0;
}

void ab(void) {
	while (1 ) {
	}
}

test_integer_to_float()
{
  if (u2f(0U) != (float) 0U)				/* 0 */
    ab();
  if (!fnear (u2f(~0U), (float) ~0U))			/* 0xffffffff */
    ab();
  if (!fnear (u2f((~0U) >> 1), (float) ((~0U) >> 1)))	/* 0x7fffffff */
    ab();
  if (u2f(~((~0U) >> 1)) != (float) ~((~0U) >> 1))	/* 0x80000000 */
    ab();

  if (u2d(0U) != (double) 0U)				/* 0 */
    ab();
  if (!dnear (u2d(~0U), (double) ~0U))			/* 0xffffffff */
    ab();
  if (!dnear (u2d((~0U) >> 1),(double) ((~0U) >> 1)))	/* 0x7fffffff */
    ab();
  if (u2d(~((~0U) >> 1)) != (double) ~((~0U) >> 1))	/* 0x80000000 */
    ab();

  if (u2ld(0U) != ( double) 0U)			/* 0 */
    ab();
  if (!ldnear (u2ld(~0U), ( double) ~0U))		/* 0xffffffff */
    ab();
  if (!ldnear (u2ld((~0U) >> 1),( double) ((~0U) >> 1)))	/* 0x7fffffff */
    ab();
  if (u2ld(~((~0U) >> 1)) != ( double) ~((~0U) >> 1))	/* 0x80000000 */
    ab();

  if (s2f(0) != (float) 0)				/* 0 */
    ab();
  if (!fnear (s2f(~0), (float) ~0))			/* 0xffffffff */
    ab();
  if (!fnear (s2f((int)((~0U) >> 1)), (float)(int)((~0U) >> 1))) /* 0x7fffffff */
    ab();
  if (s2f((int)(~((~0U) >> 1))) != (float)(int)~((~0U) >> 1)) /* 0x80000000 */
    ab();

  if (s2d(0) != (double) 0)				/* 0 */
    ab();
  if (!dnear (s2d(~0), (double) ~0))			/* 0xffffffff */
    ab();
  if (!dnear (s2d((int)((~0U) >> 1)), (double)(int)((~0U) >> 1))) /* 0x7fffffff */
    ab();
  if (s2d((int)~((~0U) >> 1)) != (double)(int)~((~0U) >> 1)) /* 0x80000000 */
    ab();

  if (s2ld(0) != ( double) 0)			/* 0 */
    ab();
  if (!ldnear (s2ld(~0), ( double) ~0))		/* 0xffffffff */
    ab();
  if (!ldnear (s2ld((int)((~0U) >> 1)), ( double)(int)((~0U) >> 1))) /* 0x7fffffff */
    ab();
  if (s2ld((int)~((~0U) >> 1)) != ( double)(int)~((~0U) >> 1)) /* 0x80000000 */
    ab();
}

unsigned int
f2u(float f)
{
  return (unsigned) f;
}

unsigned int
d2u(double d)
{
  return (unsigned) d;
}

unsigned int
ld2u( double d)
{
  return (unsigned) d;
}

int
f2s(float f)
{
  return (int) f;
}

int
d2s(double d)
{
  return (int) d;
}

int
ld2s( double d)
{
  return (int) d;
}

test_float_to_integer()
{
  if (f2u(0.0) != 0)
    ab();
  if (f2u(0.999) != 0)
    ab();
  if (f2u(1.0) != 1)
    ab();
  if (f2u(1.99) != 1)
    ab();

  if (f2u((float) ((~0U) >> 1)) != (~0U) >> 1 &&	/* 0x7fffffff */
      f2u((float) ((~0U) >> 1)) != ((~0U) >> 1) + 1)
    ab();

  if (f2u((float) ~((~0U) >> 1)) != ~((~0U) >> 1))	/* 0x80000000 */
    ab();

 /* These tests require double precision, so for hosts that don't offer
    that much precision, just ignore these test.  */
 if (sizeof (double) >= 8) {
  if (d2u(0.0) != 0)
    ab();
  if (d2u(0.999) != 0)
    ab();
  if (d2u(1.0) != 1)
    ab();
  if (d2u(1.99) != 1)
    ab();
  if (d2u((double) (~0U)) != ~0U)			/* 0xffffffff */
    ab();
  if (d2u((double) ((~0U) >> 1)) != (~0U) >> 1)		/* 0x7fffffff */
    ab();
  if (d2u((double) ~((~0U) >> 1)) != ~((~0U) >> 1))	/* 0x80000000 */
    ab();
 }

 /* These tests require  double precision, so for hosts that don't offer
    that much precision, just ignore these test.  */
 if (sizeof ( double) >= 8) {
  if (ld2u(0.0) != 0)
    ab();
  if (ld2u(0.999) != 0)
    ab();
  if (ld2u(1.0) != 1)
    ab();
  if (ld2u(1.99) != 1)
    ab();
  if (ld2u(( double) (~0U)) != ~0U)			/* 0xffffffff */
    ab();
  if (ld2u(( double) ((~0U) >> 1)) != (~0U) >> 1)	/* 0x7fffffff */
    ab();
  if (ld2u(( double) ~((~0U) >> 1)) != ~((~0U) >> 1))	/* 0x80000000 */
    ab();
 }

  if (f2s(0.0) != 0)
    ab();
  if (f2s(0.999) != 0)
    ab();
  if (f2s(1.0) != 1)
    ab();
  if (f2s(1.99) != 1)
    ab();
  if (f2s(-0.999) != 0)
    ab();
  if (f2s(-1.0) != -1)
    ab();
  if (f2s(-1.99) != -1)
    ab();
  if (f2s((float)(int)~((~0U) >> 1)) != (int)~((~0U) >> 1)) /* 0x80000000 */
    ab();

 /* These tests require double precision, so for hosts that don't offer
    that much precision, just ignore these test.  */
 if (sizeof (double) >= 8) {
  if (d2s(0.0) != 0)
    ab();
  if (d2s(0.999) != 0)
    ab();
  if (d2s(1.0) != 1)
    ab();
  if (d2s(1.99) != 1)
    ab();
  if (d2s(-0.999) != 0)
    ab();
  if (d2s(-1.0) != -1)
    ab();
  if (d2s(-1.99) != -1)
    ab();
  if (d2s((double) ((~0U) >> 1)) != (~0U) >> 1)		/* 0x7fffffff */
    ab();
  if (d2s((double)(int)~((~0U) >> 1)) != (int)~((~0U) >> 1)) /* 0x80000000 */
    ab();
 }

 /* These tests require  double precision, so for hosts that don't offer
    that much precision, just ignore these test.  */
 if (sizeof ( double) >= 8) {
  if (ld2s(0.0) != 0)
    ab();
  if (ld2s(0.999) != 0)
    ab();
  if (ld2s(1.0) != 1)
    ab();
  if (ld2s(1.99) != 1)
    ab();
  if (ld2s(-0.999) != 0)
    ab();
  if (ld2s(-1.0) != -1)
    ab();
  if (ld2s(-1.99) != -1)
    ab();
  if (ld2s(( double) ((~0U) >> 1)) != (~0U) >> 1)		/* 0x7fffffff */
    ab();
  if (ld2s(( double)(int)~((~0U) >> 1)) != (int)~((~0U) >> 1)) /* 0x80000000 */
    ab();
 }
}

main()
{
  test_integer_to_float();
  test_float_to_integer();
  return 0;
}
