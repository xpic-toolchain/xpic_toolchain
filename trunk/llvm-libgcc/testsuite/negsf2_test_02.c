/*
 * negsf2_test_02.c
 *
 * — Runtime Function: float __negsf2 (float a)
 *
 * These functions return the negation of a. They simply flip the sign bit,
 * so they can produce negative zero and negative NaN.
 *
 */


float __negsf2 (float a);

int main ( void )
{
  union dl_a {
    float fvalue;
    long lvalue;
  } a;
  union dl_b {
    float fvalue;
    long lvalue;
  } b;

  a.fvalue = 2.0f;
  b.fvalue = __negsf2 ( a.fvalue );
  if ( b.fvalue != -2.0f )
    return 1;

  a.fvalue = 0.0f;
  b.fvalue = __negsf2 ( a.fvalue );
  if ( b.fvalue != 0.0f )
    return 2;

  a.fvalue = 3142134.232323f;
  b.fvalue = __negsf2 ( a.fvalue );
  if ( b.fvalue != -3142134.232323f )
    return 3;

  a.fvalue = 3.40282e+038f;
  b.fvalue = __negsf2 ( a.fvalue );
  if ( b.fvalue != -3.40282e+038f )
    return 4;

  a.fvalue = -3.40282e+038f;
  b.fvalue = __negsf2 ( a.fvalue );
  if ( b.fvalue != 3.40282e+038f )
    return 5;

  a.lvalue = 0x7FFFFFFF; // NaN
  b.fvalue = __negsf2 ( a.fvalue );
  if ( b.lvalue != 0xFFFFFFFF ) // -NaN
    return 6;


  return ( 0 );
}
