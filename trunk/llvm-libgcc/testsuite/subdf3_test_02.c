/*
 * subdf3_test_02.c
 *
 * — Runtime Function: double __subdf3 (double a, double b)
 *
 *  These functions return the difference between b and a; that is, a - b.
 *
 */

double __subdf3 (double a, double b);

int main ( void )
{
  double a, b ,c;

   a = 0.0;
   b = 0.0;
   c = __subdf3( a, b );
   if ( c != 0.0 )
     return ( -1 );

   a = -0.0;
   b = 0.0;
   c = __subdf3( a, b );
   if ( c != 0.0 )
     return ( -1 );

   a = 0.0;
   b = -0.0;
   c = __subdf3( a, b );
   if ( c != 0.0 )
     return ( -1 );

   a = -0.0;
   b = -0.0;
   c = __subdf3( a, b );
   if ( c != 0.0 )
     return ( -1 );

   a = 2.0;
   b = 2.0;
   c = __subdf3 ( a, b );
   if ( c != 0.0 )
     return ( -1 );

   a = -2.0;
   b = 2.0;
   c = __subdf3 ( a, b );
   if ( c != -4.0 )
     return ( -1 );

   a = 2.0;
   b = -2.0;
   c = __subdf3 ( a, b );
   if ( c != 4.0 )
     return ( -1 );

   a = -2.0;
   b = -2.0;
   c = __subdf3 ( a, b );
   if ( c != 0.0 )
     return ( -1 );

   a = 322.35;
   b = 2334.25;
   c = __subdf3 ( a, b );
   if ( c != -2011.9 )
     return ( -1 );

   a = -322.35;
   b = 2334.25;
   c = __subdf3 ( a, b );
   if ( c != -2656.6 )
     return ( -1 );

   a = 1.0;
   b = 1.0;
   c = __subdf3 ( a, b );
   if ( c != 0.0 )
     return ( -1 );

   a = -1.0;
   b = 1.0;
   c = __subdf3 ( a, b );
   if ( c != -2.0 )
     return ( -1 );

   a = 1.0;
   b = -1.0;
   c = __subdf3 ( a, b );
   if ( c != 2.0 )
     return ( -1 );

   a = -1.0;
   b = -1.0;
   c = __subdf3 ( a, b );
   if ( c != 0.0 )
     return ( -1 );

   a = -1.2587;
   b = -1.2587;
   c = __subdf3 ( a, b );
   if ( c != 0.0 )
     return ( -1 );

   a = 1.2587;
   b = 1.2587;
   c = __subdf3 ( a, b );
   if ( c != 0.0 )
     return ( -1 );

   a = 3.40282e+038;
   b = 0.0f;
   c = __subdf3 ( a, b );
   if ( c != 3.40282e+038 )
     return ( -1 );

   a = 3.40282e+038;
   b = 3.40282e+038;
   c = __subdf3 ( a, b );
   if ( c != 0.0 )
     return ( -1 );

   a = 1.40129e-45;
   b = 0.0;
   c = __subdf3 ( a, b );
   if ( c != 1.40129e-45 )
     return ( -1 );

   a = 1.40129e-45;
   b = 1.40129e-45;
   c = __subdf3 ( a, b );
   if ( c != 0.0 )
     return ( -1 );

  return ( 0 );
}
