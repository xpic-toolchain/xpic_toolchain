	.text
	.global	exp2
	.align	2
	.type	exp2,@function
exp2:
        // call pow(2.0, arg)

        // arg
        mov r4, r2, z0
        mov r5, r3, z0
        // 2.0
        lsl r3, #1, #30
        mov r2, z0, z0
	store [z0 + --r7], pc		// xSTORE_TO_STACK save register 'pc' to stack
	jmp pow
	inc pc, [r7++], #8		// load register 'pc' from stack
	nop
	.size	exp2, .-exp2


