.text
.global __muldsi3
.align  2
.type __muldsi3,@function
__muldsi3:
    store [z0 + --r7], u0
    store [z0 + --r7], u1

    mulu u10, r2, r3
    nop
    or r2, u0, z0
    or r3, u1, z0

    load u1, [z0 + r7++]
    load u0, [z0 + r7++]

    inc pc, [r7++], #8
    nop
