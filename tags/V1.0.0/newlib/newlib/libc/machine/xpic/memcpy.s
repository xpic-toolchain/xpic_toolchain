  .text
  .align 2
  
  .global memcpy
memcpy:

    /*
     * r2 = void* dst (destination address)
     * r3 = void* src (source address)
     * r4 = unsigned long num (number of bytes)
     */
   
    store [z0 + --r7]d, r0    // push r0

    /* if num is less than 8 we copy num bytes from src to dst byte by byte,
       otherwise we copy as much dwords as we can to minimize load/store operations */
    sub z0, #8, r4
    store [z0 + --r7]d, r2    // push r2
    jmp gu, memcpy_simple

memcpy_much:
    // skip if dst is 2-byte-aligned
    and z0, #1, r2
    jmp z, memcpy_2bytes
    // copy one byte
    load r5, [z0 + r3++]b
    store [z0 + r2++]b, r5
    dec r4, r4, #1           // num = num - 1
    // dst is now 2-byte-aligned

memcpy_2bytes:
    // skip if dst is 4-byte-aligned
    and z0, #2, r2
    jmp z, memcpy_dwords
    // copy two bytes (r3 might not be 2 byte aligned!)
    lsl r5, [r3++]b, #8
    or r5, [r3++]b, r5
    store [z0 + r2++]w_bs, r5
    dec r4, r4, #2           // num = num - 2
    // dst is now 4-byte-aligned

memcpy_dwords:
    // r0 = number of dwords to copy - 1
    lsr r0, r4, #2
    dec r0, r0, #1
    // r4 = number of rest bytes to copy
    // these bytes will be copied by memcpy_simple
    and r4, #3, r4

    // if src is not 4-byte-aligned copying becomes more complex ...
    and r5, #3, r3
    jmp z, memcpy_aligned_dword

    // inner function memcpy_unaligned
memcpy_unaligned:
    // u0 = shift right term for [align(src(n-1)]
    // u1 = shift left term for [align(src(n))]
    // u2 = left_bits of dword to copy
    // u3 = right_bits of dword to copy
    store [z0 + --r7]d, u0    // push u0
    bs r3, #3, r3
    store [z0 + --r7]d, u1    // push u1
    lsl u0, r5, #3
    store [z0 + --r7]d, u2    // push u2
    sub u1, #32, u0
    store [z0 + --r7]d, u3    // push u3
    load r5, [z0 + r3++]d

memcpy_unaligned_dword:
    lsr u2, r5, u0            // lo_bits
    load r5, [z0 + r3++]d
    lsl u3, r5, u1            // hi_bits
    or u2, u2, u3             // dword = lo_bits OR hi_bits
    store [z0 + r2++]d, u2
    jmpdec r0nz, memcpy_unaligned_dword

    load u3, [z0 + r7++]d     // pop u3
    lsr u1, u1, #3
    load u2, [z0 + r7++]d     // pop u2
    sub r3, r3, u1
    load u1, [z0 + r7++]d     // pop u1
    load u0, [z0 + r7++]d     // pop u0
    jmp memcpy_simple

    // in most cases src and dst have the same alignment and we can copy dwords directly
memcpy_aligned_dword:
    load r5, [z0 + r3++]d
    store [z0 + r2++]d, r5
    jmpdec r0nz, memcpy_aligned_dword
    // fallthrough

memcpy_simple:
    // return if nothing to copy
    dec r0, r4, #1
    jmp lu, memcpy_return

    // copy everything
memcpy_bytes:
    load r5, [z0 + r3++]b
    store [z0 + r2++]b, r5
    jmpdec r0nz, memcpy_bytes

memcpy_return:
    load r2, [z0 + r7++]d     // pop r2 (return value = destination address)
    load r0, [z0 + r7++]d     // pop r0
    inc pc,  [r7++]d, #8      // return
    nop
