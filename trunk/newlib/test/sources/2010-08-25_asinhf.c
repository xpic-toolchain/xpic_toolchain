#include <math.h>
#define EPSILON 0.25f

float tests[][2] =
{
    {0.0f, 0.0f},
    {0.78f, 0.72f},
    {1.57f, 1.23f},
    {2.35f, 1.59f},
    {3.14f, 1.86f},
    {3.92f, 2.07f},
    {4.71f, 2.25f},
    {5.49f, 2.40f},
    {6.28f, 2.53f},
    {7.06, 2.65f },
    {7.85, 2.75f},
    {8.63f, 2.85f},
    {9.42f, 2.93f},
    {10.21f, 3.01f},
    {10.99f, 3.09f},
    {11.78f, 3.16f},
    {12.56f, 3.22f},
    {-1.74f, -1.32f},
    {-3.49f, -1.96f},
    {-5.24f, -2.35f},
    {-6.98f, -2.64f},
    {-73.31f, -4.98f},
    {-174.53f, -5.85f},
    {-214.67f, -6.06f},
    {-354.30f, -6.56f},
};

int main ( void )
{
  int n, d;
  float ret;
  const unsigned N = sizeof ( tests ) / sizeof ( tests[0] );
  unsigned i;
  for ( i = 0; i < N; ++i )
  {
    ret = asinhf( tests[i][0] );
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
