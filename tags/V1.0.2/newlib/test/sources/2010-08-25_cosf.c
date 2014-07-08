#include <math.h>
#define EPSILON 0.1f

float tests[][2] =
{
    {0.0f, 1.0f},
    {0.78f, 0.70f},
    {1.57f, 0.0f},
    {2.35f, -0.70f},
    {3.14f, -1.0f},
    {3.92f, -0.7f},
    {4.71f, 0.0f},
    {5.49f, 0.70f},
    {6.28f, 1.00f},
    {7.06, 0.70f },
    {7.85, 0.00f},
    {8.63f, -0.70f},
    {9.42f, -1.00f},
    {10.21f, -0.70f},
    {10.99f, 0.0f},
    {11.78f, 0.70f},
    {12.56f, 1.00f},
    {-1.74f, -0.17f},
    {-3.49f, -0.93f},
    {-5.24f, 0.55f},
    {-6.98f, 0.76f},
    {-73.31f, -0.49f},
    {-174.53f, 0.17f},
    {-214.67f, 0.49f},
    {-354.30f, -0.76f},
};

int main ( void )
{
  int n, d;
  float ret;
  const unsigned N = sizeof ( tests ) / sizeof ( tests[0] );
  unsigned i;
  for ( i = 0; i < N; ++i )
  {
    ret = cosf( tests[i][0] );
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
