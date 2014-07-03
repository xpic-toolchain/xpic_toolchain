#include <stdint.h>

#define PEEK(addr)      (*(unsigned int *)(addr))

typedef uint32_t      DWORD;
typedef uint16_t      WORD;
typedef uint8_t       BYTE;

struct __attribute__((packed)) test_struct2 
{ 
        WORD        byte1; 
        DWORD        dword1; 
} test2; 
 


int main( void )
{
  test2.byte1 = 0xAA; 
  test2.dword1 = 0x12345678;  
  // xpic_break_irq_raw -> 0x1018c4bc (netX51) - 0x10 -> misalignment_irq
  return PEEK(0x1018c4bc);
}