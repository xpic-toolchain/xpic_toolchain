.text
.global __sub_ddmmss
.align  2
.type __sub_ddmmss,@function
__sub_ddmmss:
    store [z0 + --r7], r0

    load r0, [r7 + #12]
    sub r0, r5, r0
    store [r3], r0

    load r0, [r7 + #8]
    subc r0, r4, r0
    store [r2], r0

    load r0, [z0 + r7++]

    inc pc, [r7++], #8
    nop
