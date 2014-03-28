/*
 * const_long_long_test_01.c
 *
 *  Created on: 17.08.2010
 *      Author: Alexander
 *
 *  BI: Simplified to a single check
 */

unsigned long long test = 0xFFFFFFFEFFFFFFFDuLL;

unsigned long long a = 0;

int main ( void )
{
  a = test;

  if ( a != 0xFFFFFFFEFFFFFFFDuLL )
    return ( -1 );

  return ( 0 );
}
