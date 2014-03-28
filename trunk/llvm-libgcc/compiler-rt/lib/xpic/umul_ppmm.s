.text
.global __umul_ppmm
.align  2
.type __umul_ppmm,@function
__umul_ppmm:
    store [z0 + --r7], u0
    store [z0 + --r7], u1

    mulu u10, r4, r5
    nop

    store [r2], u1
    store [r3], u0

    load u1, [z0 + r7++]
    load u0, [z0 + r7++]

    inc pc, [r7++], #8
    nop
