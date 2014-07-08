/*
 * qsort---sort an array
 *
 * Synopsis
 * #include <stdlib.h>
 * void qsort(void *base, size_t nmemb, size_t size,
 * int (*compar)(const void *, const void *) );
 *
 * Description
 * qsort sorts an array (beginning at base) of nmemb objects.
 * size describes the size of each element of the array.
 *
 * You must supply a pointer to a comparison function, using the
 * argument shown as compar. (This permits sorting objects of unknown
 * properties.) Define the comparison function to accept two arguments,
 * each a pointer to an element of the array starting at base. The result
 * of (*compar) must be negative if the first argument is less than the
 * second, zero if the two arguments match, and positive if the first
 * argument is greater than the second (where "less than" and "greater
 * than" refer to whatever arbitrary ordering is appropriate).
 *
 * The array is sorted in place; that is, when qsort returns,
 * the array elements beginning at base have been reordered.
 *
 * Returns
 * qsort does not return a result.
 *
 * Portability
 * qsort is required by ANSI (without specifying the sorting algorithm).
 *
 */

#include <stdio.h>
#include <stdlib.h>

int values[] = { 40, 10, 100, 90, 20, 25 };

int compare_ltoh (const void * a, const void * b)
{
  return ( *(int*)a - *(int*)b );
}

int compare_htol (const void * a, const void * b)
{
  return ( *(int*)b - *(int*)a );
}

int main ()
{
  int n;
  qsort ( &values, 6, sizeof(int), compare_htol );
  if ( values[5] != 10 || values[4] != 20 || values[3] != 25 || values[2] != 40 || values[1] != 90 || values[0] != 100)
    return ( 1 );

  qsort ( &values, 6, sizeof(int), compare_ltoh );
  if ( values[0] != 10 || values[1] != 20 || values[2] != 25 || values[3] != 40 || values[4] != 90 || values[5] != 100)
    return ( 2 );

  return ( 0 );
}
