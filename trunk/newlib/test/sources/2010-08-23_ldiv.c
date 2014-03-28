/*
 * Synopsis
 *
 * #include <stdlib.h>
 * ldiv_t ldiv(long n, long d);
 *
 * Description
 * Divide returning quotient and remainder as two long integers in
 * a structure ldiv_t.
 *
 * Returns
 * The result is represented with the structure
 *
 *  typedef struct
 *  {
 *   long quot;
 *   long rem;
 *  } ldiv_t;
 *
 * where the quot field represents the quotient, and rem the remainder.
 * For nonzero d, if `r = ldiv(n,d);' then n equals `r.rem + d*r.quot'.
 *
 * To divide int rather than long values, use the similar function div.
 *
 * Portability
 * ldiv is ANSI.
 *
 * No supporting OS subroutines are required.
 *
 */

#include <stdlib.h>

unsigned long long tests[][4] =
{
    {1000L, 500L, 2L, 0L},
    {1000L, 250L, 4L, 0L},
    {1000L, 10L, 100L, 0L},
    {1000L, 550L, 1L, 450L},
    {1L, 1L, 1L, 0L},
    {3L, 2L, 1L, 1L},
    {-2L, 2L, -1L, 0L},
    {-2L, -2L, 1L, 0L},
    {2L, -2L, -1L, 0L},
    {2L, 2L, 1L, 0L},
    {-3L, 2L, -1L, -1L},
    {-3L, -2L, 1L, -1L},
    {3L, -2L, -1L, 1L},
    {3L, 2L, 1L, 1L},
    {2345L, 1172L, 2L, 1L},
    {2345L, 1172L, 2L, 1L},
    {2147483647L, 2147483647L, 1L, 0L},
    {2147483647L, 2147483646L, 1L, 1L},
};

int main ( void )
{
  int n, d;
  ldiv_t ret;
  const unsigned N = sizeof(tests) / sizeof(tests[0]);
  unsigned i;
  for (i = 0; i < N; ++i)
  {
    n = tests[i][0];
    d = tests[i][1];
    ret = ldiv ( n, d );
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
