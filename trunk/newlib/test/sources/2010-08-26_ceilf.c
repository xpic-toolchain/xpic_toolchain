#include <math.h>
#define EPSILON 0.1f

float tests[][2] =
{
    {0.0f, 0.0f},
    {0.78f, 1.0f},
    {1.57f, 2.0f},
    {2.35f, 3.0f},
    {3.14f, 4.0f},
    {3.92f, 4.0f},
    {4.71f, 5.0f},
    {5.49f, 6.0f},
    {6.28f, 7.0f},
    {7.06, 8.0f },
    {7.85, 8.0f},
    {8.63f, 9.0f},
    {9.42f, 10.0f},
    {10.21f, 11.0f},
    {10.99f, 11.0f},
    {11.78f, 12.0f},
    {12.56f, 13.00f},
    {-1.74f, -1.0f},
    {-3.49f, -3.0f},
    {-5.24f, -5.0f},
    {-6.98f, -6.0f},
    {-73.31f, -73.0f},
    {-174.53f, -174.0f},
    {-214.67f, -214.0f},
    {-354.30f, -354.0f},
};

int main ( void )
{
  int n, d;
  float ret;
  const unsigned N = sizeof ( tests ) / sizeof ( tests[0] );
  unsigned i;
  for ( i = 0; i < N; ++i )
  {
    ret = ceilf( tests[i][0] );
    if ( !( ret <= tests[i][1]+EPSILON && ret >= tests[i][1]-EPSILON ) )
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
