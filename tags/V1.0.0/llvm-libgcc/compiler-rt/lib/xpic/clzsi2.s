.text
.global __clzsi2
.align  2
.type __clzsi2,@function
__clzsi2:
    clz r2, r2, z0
    inc pc, [r7++], #8
    nop
