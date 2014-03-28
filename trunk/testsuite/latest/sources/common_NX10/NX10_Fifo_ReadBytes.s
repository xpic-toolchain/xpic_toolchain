        .text

/* void usb_read(unsigned long ulFillLevel, unsigned char* pbBuffer, volatile unsigned long* pulFifo);
                 r2                         r3                       r4
*/
        .global NX10_Fifo_ReadBytes
        .align  2
        .type   NX10_Fifo_ReadBytes,@function
NX10_Fifo_ReadBytes:
        store [z0 + --r7], r0

        store [z0 + --r7], r1
        store [z0 + --r7], u0
        store [z0 + --r7], u1

        and r0, #7, r2
        jmp z, read_8byte

        add r0, #-1, r0
read_1byte_loop:
        load r5, [z0 + r4]d
        store [z0 + r3++]b, r5
        jmpdec r0nz, read_1byte_loop

read_8byte:
        lsr r0, r2, #3
        jmp z, read_finish

        add r0, #-1, r0
read_8byte_loop:
//        load r5, [z0 + r4]d
//        store [z0 + r3++]b, r5
//        load r5, [z0 + r4]d
//        store [z0 + r3++]b, r5
//        load r5, [z0 + r4]d
//        store [z0 + r3++]b, r5
//        load r5, [z0 + r4]d
//        store [z0 + r3++]b, r5
//        load r5, [z0 + r4]d
//        store [z0 + r3++]b, r5
//        load r5, [z0 + r4]d
//        store [z0 + r3++]b, r5
//        load r5, [z0 + r4]d
//        store [z0 + r3++]b, r5
//        load r5, [z0 + r4]d
//        store [z0 + r3++]b, r5

        lsl r5, [r4]d, #8
        or  r5, [r4]d, r5
        lsl r1, [r4]d, #8
        or  r1, [r4]d, r1
        lsl u0, [r4]d, #8
        or  u0, [r4]d, u0
        lsl u1, [r4]d, #8
        or  u1, [r4]d, u1
        store [z0 + r3++]w_bs, r5
        store [z0 + r3++]w_bs, r1
        store [z0 + r3++]w_bs, u0
        store [z0 + r3++]w_bs, u1

        jmpdec r0nz, read_8byte_loop

read_finish:
        load u1, [z0 + r7++]
        load u0, [z0 + r7++]
        load r1, [z0 + r7++]

        load r0, [z0 + r7++]
        inc pc, [r7++], #8
        nop
