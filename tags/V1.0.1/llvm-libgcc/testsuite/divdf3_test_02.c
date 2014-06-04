/*
 * divdf3_test_02.c
 *
 * — Runtime Function: double __divdf3 (double a, double b)
 *
 *  These functions return the quotient of a and b; that is, a / b.
 *
 */

#define epsilon 0.1

double __divdf3 (double a, double b);

int main ( void )
{
  double a, b, c;

  a = 1.0;
  b = 1.0;
  c = __divdf3 ( a, b );
  if ( c != 1.0 )
    return ( -1 );

  a = 1.0;
  b = -1.0;
  c = __divdf3 ( a, b );
  if ( c != -1.0 )
    return ( -1 );

  a = -1.0;
  b = 1.0;
  c = __divdf3 ( a, b );
  if ( c != -1.0 )
    return ( -1 );

  a = -1.0;
  b = -1.0;
  c = __divdf3 ( a, b );
  if ( c != 1.0 )
    return ( -1 );

  a = 0.0;
  b = 1.0;
  c = __divdf3 ( a, b );
  if ( c != 0.0 )
    return ( -1 );

  a = 2.0;
  b = 2.0;
  c = __divdf3 ( a, b );
  if ( c != 1.0 )
    return ( -1 );

  a = 32423423435.0;
  b = 2.0;
  c = __divdf3 ( a, b );
  if ( c != 16211711717.5 )
    return ( -1 );

  a = 32423423435.232132;
  b = 2.0;
  c = __divdf3 ( a, b );
  if ( c != 16211711717.616066 )
    return ( -1 );

  a = 32423423435.232132;
  b = 4.56789;
  c = __divdf3 ( a, b );
  if ( c != 7098118263.6254664626337324235041 )
    return ( -1 );

  a = 32423423435.232132;
  b = -4.56789;
  c = __divdf3 ( a, b );
  if ( c != -7098118263.6254664626337324235041 )
    return ( -1 );

  a = -32423423435.232132;
  b = 4.56789;
  c = __divdf3 ( a, b );
  if ( c != -7098118263.6254664626337324235041 )
    return ( -1 );

  a = -32423423435.232132;
  b = -4.56789;
  c = __divdf3 ( a, b );
  if ( c != 7098118263.6254664626337324235041 )
    return ( -1 );

  return ( 0 );
}
