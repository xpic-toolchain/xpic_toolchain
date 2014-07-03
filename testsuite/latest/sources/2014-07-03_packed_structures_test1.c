#include <stdint.h>

#define PEEK(addr)      (*(unsigned int *)(addr))

typedef uint32_t      DWORD;
typedef uint8_t       BYTE;

struct __attribute__((packed)) test_struct1 
{ 
  BYTE         byte1; 
  DWORD        dword1; 
}/* __attribute__((packed))*/; 

typedef struct test_struct1 test_struct1_t; 


int main( void )
{
  volatile test_struct1_t test1;
  test1.dword1 = 0x12345678;
  // xpic_break_irq_raw -> 0x1018c4bc (netX51) - 0x10 -> misalignment_irq
  return PEEK(0x1018c4bc);
}