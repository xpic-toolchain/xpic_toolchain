/*
 * div---divide two integers
 *
 * Synopsis
 * #include <stdlib.h>
 * div_t div(int n, int d);
 *
 * Description
 * Divide returning quotient and remainder as two integers in a structure div_t.
 *
 * Returns
 * The result is represented with the structure
 *
 * typedef struct
 * {
 *   int quot;
 *   int rem;
 * } div_t;
 *
 * where the quot field represents the quotient, and rem the remainder. For nonzero d,
 * if `r = div(n,d);' then n equals `r.rem + d*r.quot'.
 *
 * To divide long rather than int values, use the similar function ldiv.
 *
 * Portability
 * div is ANSI.
 *
 * No supporting OS subroutines are required.
 *
 */

#include <stdlib.h>

unsigned long long tests[][4] =
{
    {1000, 500, 2, 0},
    {1000, 250, 4, 0},
    {1000, 10, 100, 0},
    {1000, 550, 1, 450},
    {1, 1, 1, 0},
    {3, 2, 1, 1},
    {-2, 2, -1, 0},
    {-2, -2, 1, 0},
    {2, -2, -1, 0},
    {2, 2, 1, 0},
    {-3, 2, -1, -1},
    {-3, -2, 1, -1},
    {3, -2, -1, 1},
    {3, 2, 1, 1},
    {2345, 1172, 2, 1},
    {2345, 1172, 2, 1},
    {2147483647, 2147483647, 1, 0},
    {2147483647, 2147483646, 1, 1},
    {-2147483648, -2147483648, 1, 0},
    {-2147483648, -2147483647, 1, -1},
};

int main ( void )
{
  int n, d;
  div_t ret;
  const unsigned N = sizeof(tests) / sizeof(tests[0]);
  unsigned i;
  for (i = 0; i < N; ++i)
  {
    n = tests[i][0];
    d = tests[i][1];
    ret = div ( n, d );
    if ( ret.quot != tests[i][2] || ret.rem != tests[i][3] )
    {
      return ( -1 );
    }
    else
    {
      ; // Default
    }
  }

  return ( 0 );
}
