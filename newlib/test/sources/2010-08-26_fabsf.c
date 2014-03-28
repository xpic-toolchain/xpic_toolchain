#include <math.h>
#define EPSILON 0.1f

float tests[][2] =
{
    {0.0f, 0.0f},
    {0.78f, 0.78f},
    {1.57f, 1.57f},
    {2.35f, 2.35f},
    {3.14f, 3.14f},
    {3.92f, 3.92f},
    {4.71f, 4.71f},
    {5.49f, 5.49f},
    {6.28f, 6.28f},
    {7.06, 7.06f },
    {7.85, 7.85f},
    {8.63f, 8.63f},
    {9.42f, 9.42f},
    {10.21f, 10.21f},
    {10.99f, 10.99f},
    {11.78f, 11.78f},
    {12.56f, 12.56f},
    {-1.74f, 1.74f},
    {-3.49f, 3.49f},
    {-5.24f, 5.24f},
    {-6.98f, 6.98f},
    {-73.31f, 73.31f},
    {-174.53f, 174.53f},
    {-214.67f, 214.67f},
    {-354.30f, 354.30f},
};

int main ( void )
{
  int n, d;
  float ret;
  const unsigned N = sizeof ( tests ) / sizeof ( tests[0] );
  unsigned i;
  for ( i = 0; i < N; ++i )
  {
    ret = fabsf( tests[i][0] );
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
