  .text
  .align 2

  .global memset
memset:

    /*
     * r2 = void* dst (destination address)
     * r3 = int val (pattern to set)
     * r4 = unsigned long num (number of bytes)
     */
   
    store [z0 + --r7]d, r0    // push r0
    and r3, #hff, r3          // val is really an unsigned char
    store [z0 + --r7]d, r2    // push r2
       
    /* if num is less than 8 we set num bytes byte by byte,
       otherwise we set as much dwords as we can to minimize load/store operations */
    sub z0, #8, r4
    jmp gu, memset_simple
    
memset_much:
    // skip if dst is 2-byte-aligned
    and z0, #1, r2
    jmp z, memset_2bytes
    // set one byte
    store [z0 + r2++]b, r3
    dec r4, r4, #1           // num = num - 1
    // dst is now 2-byte-aligned
    
memset_2bytes:
    // double the 8 bits of value to word size
    lsl r5, r3, #8
    or r3, r3, r5
    // skip if dst is 4-byte-aligned
    and z0, #2, r2
    jmp z, memset_dwords
    // set two bytes
    store [z0 + r2++]w, r3
    dec r4, r4, #2           // num = num - 2
    // dst is now 4-byte-aligned
    
memset_dwords:
    // double the 16 bits of value to dword size
    lsl r5, r3, #16
    or r3, r3, r5
    // r0 = number of dwords to set - 1
    lsr r0, r4, #2
    dec r0, r0, #1
    // r4 = number of rest bytes to set
    and r4, #3, r4
        
    // here is the main worker
memset_aligned_dword:
    store [z0 + r2++]d, r3
    jmpdec r0nz, memset_aligned_dword

memset_simple:
    // return if nothing to copy
    dec r0, r4, #1
    jmp lu, memset_return
    
    // set single bytes
memset_bytes:
    store [z0 + r2++]b, r3
    jmpdec r0nz, memset_bytes
    
memset_return:
    load r2, [z0 + r7++]d     // pop r2 (return value = destination address)
    load r0, [z0 + r7++]d     // pop r0
    inc pc,  [r7++]d, #8      // return
    nop
