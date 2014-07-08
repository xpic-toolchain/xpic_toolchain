/*
 * calloc---allocate space for arrays
 *
 * Synopsis
 *
 * #include <stdlib.h>
 * void *calloc(size_t n, size_t s);
 * void *calloc_r(void *reent, size_t <n>, <size_t> s);
 *
 * Description
 * Use calloc to request a block of memory sufficient to hold an array of n
 * elements, each of which has size s.
 *
 * The memory allocated by calloc comes out of the same memory pool used by
 * malloc, but the memory block is initialized to all zero bytes.
 * (To avoid the overhead of initializing the space, use malloc instead.)
 *
 * The alternate functios _calloc_r is reentrant. The extra argument reent is
 * a pointer to a reentrancy structure.
 *
 * Returns
 * If successful, a pointer to the newly allocated space.
 *
 * If unsuccessful, NULL.

 * Portability
 * calloc is ANSI.
 *
 * Supporting OS subroutines required: close, fstat, isatty, lseek, read, sbrk, write.
 *
 */

#include <stdlib.h>

void libc_mem_init();

int main ( void )
{
  libc_mem_init();
  int * pData;
  int i = 20;
  int n;
  pData = (int*) calloc (i,sizeof(int));
  if ( pData==NULL )
  {
    return ( 1 );
  }

  for (n=0;n<i;n++)
  {
    pData[n] = n;
  }

  for (n=0;n<i;n++)
  {
    if ( pData[n] != n )
    {
      return ( 2 );
    }
  }

  return ( 0 );
}
