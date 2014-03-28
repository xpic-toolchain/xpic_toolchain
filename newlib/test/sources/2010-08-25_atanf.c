#include <math.h>
#define EPSILON 0.1f

float tests[][2] =
{
    {0.0f, 0.0f},
    {0.78f, 0.66f},
    {1.57f, 1.00f},
    {2.35f, 1.16f},
    {3.14f, 1.26f},
    {3.92f, 1.32f},
    {4.71f, 1.36f},
    {5.49f, 1.39f},
    {6.28f, 1.41f},
    {7.06, 1.43f },
    {7.85, 1.44f},
    {8.63f, 1.45f},
    {9.42f, 1.46f},
    {10.21f, 1.47f},
    {10.99f, 1.48f},
    {11.78f, 1.48f},
    {12.56f, 1.49f},
    {-1.74f, -1.05f},
    {-3.49f, -1.29f},
    {-5.24f, -1.38f},
    {-6.98f, -1.42f},
    {-73.31f, -1.55f},
    {-174.53f, -1.56f},
    {-214.67f, -1.56f},
    {-354.30f, -1.56f},
};

int main ( void )
{
  int n, d;
  float ret;
  const unsigned N = sizeof ( tests ) / sizeof ( tests[0] );
  unsigned i;
  for ( i = 0; i < N; ++i )
  {
    ret = atanf( tests[i][0] );
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
