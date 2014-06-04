#include "main.h"
#include "some.h"

unsigned long longarray[200] = {
0x12345678
};

unsigned long longarray2[70000] = { 0x12345678 };

int main( void )
{
  longarray[1] = somelongarray2[69000] + longarray[100];

  return 0;
}
