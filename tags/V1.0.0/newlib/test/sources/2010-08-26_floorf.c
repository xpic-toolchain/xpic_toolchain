#include <math.h>
#define EPSILON 0.1f

float tests[][2] =
{
    {0.0f, 0.0f},
    {0.78f, 0.0f},
    {1.57f, 1.0f},
    {2.35f, 2.0f},
    {3.14f, 3.0f},
    {3.92f, 3.0f},
    {4.71f, 4.0f},
    {5.49f, 5.0f},
    {6.28f, 6.0f},
    {7.06, 7.0f },
    {7.85, 7.0f},
    {8.63f, 8.0f},
    {9.42f, 9.0f},
    {10.21f, 10.0f},
    {10.99f, 10.0f},
    {11.78f, 11.0f},
    {12.56f, 12.0f},
    {-1.74f, -2.0f},
    {-3.49f, -4.0f},
    {-5.24f, -6.0f},
    {-6.98f, -7.0f},
    {-73.31f, -74.0f},
    {-174.53f, -175.0f},
    {-214.67f, -215.0f},
    {-354.30f, -355.0f},
};

int main ( void )
{
  int n, d;
  float ret;
  const unsigned N = sizeof ( tests ) / sizeof ( tests[0] );
  unsigned i;
  for ( i = 0; i < N; ++i )
  {
    ret = floorf( tests[i][0] );
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
