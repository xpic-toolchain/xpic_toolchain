#include <math.h>
#define EPSILON 0.1f

float tests[][2] =
{
    {0.0f, 0.0f},
    {0.78f, 0.70f},
    {1.57f, 1.0f},
    {2.35f, 0.70f},
    {3.14f, 0.0f},
    {3.92f, -0.70f},
    {4.71f, -1.0f},
    {5.49f, -0.70f},
    {6.28f, 0.0f},
    {7.06, 0.70f },
    {7.85, 1.0f},
    {8.63f, 0.70f},
    {9.42f, 0.0f},
    {10.21f, -0.70f},
    {10.99f, -1.0f},
    {11.78f, -0.70f},
    {12.56f, 0.00f},
    {-1.74f, -0.98f},
    {-3.49f, 0.35f},
    {-5.24f, 0.86f},
    {-6.98f, -0.64f},
    {-73.31f, 0.87f},
    {-174.53f, 0.98f},
    {-214.67f, -0.86f},
    {-354.30f, -0.63f},
};

int main ( void )
{
  int n, d;
  float ret;
  const unsigned N = sizeof ( tests ) / sizeof ( tests[0] );
  unsigned i;
  for ( i = 0; i < N; ++i )
  {
    ret = sinf( tests[i][0] );
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
