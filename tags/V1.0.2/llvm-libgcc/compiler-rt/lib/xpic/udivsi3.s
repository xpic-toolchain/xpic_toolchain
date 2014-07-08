.text
.global __udivsi3
.align  2
.type __udivsi3,@function

// su_int __udivsi3(su_int n, su_int d)

__udivsi3:
    store [z0 + --r7]d, r0

    // if (n == 0) return 0;
    jmp r2z, ret_zero

    // if (d == 0) return 0; result of division by zero is zero?!
    jmp r3z, ret_zero

    // if (d == 1) return n;
    xor z0, #1, r3
    jmp z, ret

    // sr = __builtin_clz(d) - __builtin_clz(n);
    clz r4, r2, z0
    clz r5, r3, z0
    sub r0, r5, r4

    // if (sr > n_uword_bits - 1) return 0; d > n
    jmp s, ret_zero

    // use negative d for faster computation in loop
    sub r3, #0, r3

    // r(est) = n >> (sr + 1);
    add r4, #1, r0
    sub r5, #32, r4
    lsr r4, r2, r4

    // q(uotient) = n << (32 - sr);
    lsl r2, r2, r5
    lsl r2, r2, #1  // carry bit is q[32]

loop:
    // r = 2r + q[32]
    addc r4, r4, r4

    // if (r >= d) { r = r - d; q = 2q + 1; }
    // else        { q = 2q; }
    add r5, r3, r4     // help = -d + r; carry is set if (r >= d)
    addc r2, r2, r2    // q = 2q + carry;
    [c] mov r4, r5, z0 // if (r >= d) r = help;

    jmpdec r0nz, loop

    jmp ret

ret_zero:
    mov r2, z0, z0

ret:
    load r0, [z0 + r7++]d
    inc pc, [r7++], #8
    nop
