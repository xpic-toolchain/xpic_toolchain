/*
 * 2010-09-03_function_pointer.c
 *
 *  Created on: 03.09.2010
 *      Author: Alexander
 */

 int caller ( void * base, int num, int size, int ( * comparator ) ( const void *, const void * ) );

int values[] = { 40, 10, 100, 90, 20, 25 };

int compare_htol (const void * a, const void * b)
{
  return ( *(int*)a - *(int*)b );
}

int caller ( void * base, int num, int size, int ( * comparator ) ( const void *, const void * ) )
//int caller ( void * base, int num, int size, int ( * comparator ) () )
{
  char *pa, *pb;
  int n = 0;
  pa = &values[0];
  pb = &values[1];

  n = comparator( pa, pb );
  if ( n != 30 )
    return ( -1 );

  pa = &values[1];
  pb = &values[2];

  n = comparator( pa, pb );
  if ( n != -90 )
    return ( -1 );

  return ( 0 );
}


int main ( void )
{
  int ret = 0;

  ret = caller ( &values, 6, sizeof(int), compare_htol );

  return ( ret );
}
