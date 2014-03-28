.text
.global __add_ssaaaa
.align  2
.type __add_ssaaaa,@function
__add_ssaaaa:
    store [z0 + --r7], r0

    load r0, [r7 + #12]
    add r0, r5, r0
    store [r3], r0

    load r0, [r7 + #8]
    addc r0, r4, r0
    store [r2], r0

    load r0, [z0 + r7++]

    inc pc, [r7++], #8
    nop
