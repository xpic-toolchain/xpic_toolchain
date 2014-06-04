int __eqsf2 (float a, float b);
/* — Runtime Function: int __eqsf2 (float a, float b)
 * These functions return zero if neither argument is NaN,
 * and a and b are equal.
 */

int __nesf2 (float a, float b);
/* — Runtime Function: int __nesf2 (float a, float b)
 * These functions return a nonzero value if either argument is NaN,
 * or if a and b are unequal.
 */

int __gtsf2 (float a, float b);
/* — Runtime Function: int __gtsf2 (float a, float b)
 * These functions return a value greater than zero if neither argument is NaN,
 * and a is strictly greater than b.
 */

int __gesf2 (float a, float b);
/* — Runtime Function: int __gesf2 (float a, float b)
 * These functions return a value greater than or equal to zero if neither argument
 * is NaN, and a is greater than or equal to b.
 */

int __ltsf2 (float a, float b);
/* — Runtime Function: int __ltsf2 (float a, float b)
 * These functions return a value less than zero if neither argument is NaN,
 * and a is strictly less than b.
 */

int __lesf2 (float a, float b);
/* — Runtime Function: int __lesf2 (float a, float b)
 * These functions return a value less than or equal to zero if neither argument is NaN,
 * and a is less than or equal to b.
 */

float __floatsisf (int i);
/* — Runtime Function: float __floatsisf (int i)
 * These functions convert i, a signed integer, to floating point.
 */

double __floatsidf (int i);
/* — Runtime Function: double __floatsidf (int i)
 * These functions convert i, a signed integer, to floating point.
 */

int __fixsfsi (float f);
/* — Runtime Function: int __fixsfsi (float a)
 * These functions convert a to a signed integer, rounding toward zero.
 */

int __fixdfsi (double d);
/* — Runtime Function: int __fixdfsi (double a)
 * These functions convert a to a signed integer, rounding toward zero.
 */

unsigned int __fixunssfsi (float f);
/* — Runtime Function: unsigned int __fixunssfsi (float a)
 * These functions convert a to an unsigned integer, rounding toward zero.
 * Negative values all become zero.
 */

unsigned int __fixunsdfsi (double d);
/* — Runtime Function: unsigned int __fixunsdfsi (double a)
 * These functions convert a to an unsigned integer, rounding toward zero.
 * Negative values all become zero.
 */

int fail_count = 0;

int fail (char *msg)
{
  fail_count++;
}

int main(void)
{
  if (__floatsisf (1) != 1)
    fail ("floatsisf 1"); //
  if (__floatsidf (1) != 1)
    fail ("floatsidf 1"); //
  if (__fixsfsi (1.42) != 1)
    fail ("fixsfsi 1.42"); //
  if (__fixunssfsi (1.42) != 1)
    fail ("fixunssfsi 1.42"); //
  if (__fixdfsi (1.42) != 1)
    fail ("fixdfsi 1.42"); //
  if (__fixunsdfsi (1.42) != 1)
    fail ("fixunsdfsi 1.42"); //

  if (__eqsf2 (1.f, 1.f) != 0)
    fail ("eqsf2 1==1");
  if (__eqsf2 (1, 2) == 0)
    fail ("eqsf2 1==2");

  if (__nesf2 (1, 2) == 0)
    fail ("nesf2 1!=1");
  if (__nesf2 (1, 1) != 0)
    fail ("nesf2 1!=1");

  if (__gtsf2 (2, 1) == 0)
    fail ("gtsf2 2>1");
  if (__gtsf2 (1, 1) != 0)
    fail ("gtsf2 1>1");

  if (__gesf2 (2, 1) == 0)
    fail ("gesf2 2>=1");
  if (__gesf2 (1, 1) != 0)
    fail ("gesf2 1>=1");

  if (__ltsf2 (1, 2) == 0)
    fail ("ltsf2 1<2");
  if (__ltsf2 (1, 1) != 0)
    fail ("ltsf2 1<1");

  if (__lesf2 (1, 2) == 0)
    fail ("lesf2 1<=2");
  if (__lesf2 (1, 1) != 0)
    fail ("lesf2 1<=1");

  if (fail_count != 0)
    return fail_count;
  return 0;
}
