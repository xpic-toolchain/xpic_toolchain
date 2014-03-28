/*
 * Synopsis
 * #include <stdlib.h>
 * void *bsearch(const void *key, const void *base,
 *     size_t nmemb, size_t size,
 *     int (*compar)(const void *, const void *));
 *
 * Description
 * bsearch searches an array beginning at base for any element that matches key,
 * using binary search. nmemb is the element count of the array; size is the size
 * of each element.
 *
 * The array must be sorted in ascending order with respect to the comparison
 * function compar (which you supply as the last argument of bsearch).
 *
 * You must define the comparison function (*compar) to have two arguments;
 * its result must be negative if the first argument is less than the second,
 * zero if the two arguments match, and positive if the first argument is greater
 * than the second (where "less than" and "greater than" refer to whatever
 * arbitrary ordering is appropriate).
 *
 * Returns
 * Returns a pointer to an element of array that matches key.
 * If more than one matching element is available, the result may point to
 * any of them.
 *
 * Portability
 * bsearch is ANSI.
 *
 * No supporting OS subroutines are required.
 *
 */

#include <stdlib.h>

int compareints ( const void * a, const void * b )
{
  return ( *(int*)a - *(int*)b );
}

int values[] = { 10, 20, 25, 40, 90, 100 };

int main ( void )
{
  int * pItem;
  int key = 40; // Is in values
  pItem = (int*) bsearch (&key, values, 6, sizeof (int), compareints);
  if (pItem!=NULL)
  {
    ; // Default
  }
  else
  {
    return ( -1 );
  }

  // =============================================================================

  key = 50; // Is NOT in values
  pItem = (int*) bsearch (&key, values, 6, sizeof (int), compareints);
  if (pItem!=NULL)
  {
    return ( -1 );
  }
  else
  {
    ; // Default
  }

  return ( 0 );
}
