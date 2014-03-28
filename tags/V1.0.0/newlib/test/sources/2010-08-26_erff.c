#include <math.h>
#define EPSILON 0.05f

float tests[][2] =
{
    {0.0f, 0.0f},
    {0.78f, 0.73f},
    {1.57f, 0.97f},
    {2.35f, 0.99f},
    {3.14f, 1.0f},
    {3.92f, 1.0f},
    {4.71f, 1.0f},
    {5.49f, 1.0f},
    {6.28f, 1.0f},
    {7.06, 1.0f },
    {7.85, 1.0f},
    {8.63f, 1.0f},
    {9.42f, 1.0f},
    {10.21f, 1.0f},
    {10.99f, 1.0f},
    {11.78f, 1.0f},
    {12.56f, 1.00f},
    {-1.74f, -0.98f},
    {-3.49f, -0.99f},
    {-5.24f, -1.0f},
    {-6.98f, -1.0f},
    {-73.31f, -1.0f},
    {-174.53f, -1.0f},
    {-214.67f, -1.0f},
    {-354.30f, -1.0f},
};

int main ( void )
{
  int n, d;
  float ret;
  const unsigned N = sizeof ( tests ) / sizeof ( tests[0] );
  unsigned i;
  for ( i = 0; i < N; ++i )
  {
    ret = erff( tests[i][0] );
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
