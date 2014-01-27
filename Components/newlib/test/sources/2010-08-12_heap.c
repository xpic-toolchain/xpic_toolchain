/*
 * Test functions malloc(), calloc(), realloc() and free()
 * Test passed if main() returns a value of 0
 */

#include <stdlib.h>

void libc_mem_init();

char *buffer1, *buffer2, *buffer3;

int main(void)
{
  int i;

  libc_mem_init();


  buffer1 = (char*) malloc (32);
  if( buffer1 == NULL ) return 1; /* ERROR */

  free(buffer1);

  buffer1 = (char*) malloc (60);
  if( buffer1 == NULL ) return 2; /* ERROR */

  buffer2 = (char*) calloc (0x21a1, sizeof(char));
  if( buffer2 == NULL ) return 3; /* ERROR */

  buffer3 = (char*) realloc (buffer1, 200);
  if( buffer3 == NULL ) return 4; /* ERROR */

  buffer1 = (char*) realloc (buffer2, 1);
  if( buffer1 == NULL ) return 5; /* ERROR */

  free(buffer1);
  free(buffer3);

  /* Test finished successfully */
  return 0;
}
