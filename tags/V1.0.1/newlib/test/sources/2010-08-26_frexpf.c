#include <math.h>
#define EPSILON 0.5f

float tests[][2] =
{
    {0.0f, 0.0f},
    {0.78f, 0.78f},
    {1.57f, 0.78f},
    {2.35f, 0.58f},
    {3.14f, 0.78f},
    {3.92f, 0.98f},
    {4.71f, 0.58f},
    {5.49f, 0.68f},
    {6.28f, 0.78f},
    {7.06, 0.88f},
    {7.85, 0.98f},
    {8.63f, 0.53f},
    {9.42f, 0.58f},
    {10.21f, 0.63f},
    {10.99f, 0.68f},
    {11.78f, 0.73f},
    {12.56f, 0.78f},
    {-1.74f, -0.87f},
    {-3.49f, -0.87f},
    {-5.24f, -0.65f},
    {-6.98f, -0.87f},
    {-73.31f, -0.57f},
    {-174.53f, -0.68f},
    {-214.67f, -0.83f},
    {-354.30f, -0.69f},
};

int int_n_tests[][1] =
{
    {0},
    {1},
    {2},
    {2},
    {2},
    {3},
    {3},
    {3},
    {3},
    {3},
    {3},
    {4},
    {4},
    {4},
    {4},
    {4},
    {4},
    {1},
    {2},
    {3},
    {3},
    {7},
    {8},
    {8},
    {9},
};

long *ptr_n;
long *ptr_res;

int main ( void )
{
  float param = 0, result = 0;
  int n = 0;
  const unsigned N = sizeof ( tests ) / sizeof ( tests[0] );
  unsigned i;
  for ( i = 0; i < N; ++i )
  {
    param = tests[i][0];
    result = frexpf (param , &n);
    if ( !( result <= tests[i][1]+EPSILON && result >= tests[i][1]-EPSILON ) || n != int_n_tests[i])
    {
      return ( i );
    }
    else
    {
      ; // Default
    }
  }
  return ( 0 );
}
