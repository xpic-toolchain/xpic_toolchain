	.file	"temp/main.bc"
	.text
	.global	main
	.align	2
	.type	main,@function
main:                                   // @main
// BB#0:                                // %entry
	add r7, #-4, r7
	nop
	store [r7 + #0]d, z0
	store [r7 + #0]d, z0
	or r2, z0, z0
	add r7, #4, r7
	nop
	inc pc, [r7++], #8		// load register 'pc' from stack
	nop
	.size	main, .-main

	.type	longarray2,@object
	.data
	.global	longarray2
	.align	2
longarray2:                             // @longarray2
	.long	305419896               // 0x12345678
	.size	longarray2, 4


