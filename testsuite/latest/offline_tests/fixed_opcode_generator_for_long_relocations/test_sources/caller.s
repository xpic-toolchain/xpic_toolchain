	.file	"temp/caller.bc"
	.text
	.global	caller
	.align	2
	.type	caller,@function
caller:                                 // @caller
// BB#0:                                // %entry
	add r7, #-8, r7
	load r2, $longarray2
	and r2, [pc + $Const.h00042680], r2
	store [r7 + #0]d, z0
	load r2, [r2 + #0]d
	or r2, #4711, r2
	store [r7 + #0]d, r2
	store [r7 + #4]d, r2
	add r7, #8, r7
	nop
	inc pc, [r7++], #8		// load register 'pc' from stack
	nop
	.size	caller, .-caller

	.type	longarray2,@object
	.comm	longarray2,4,4          // @longarray2
	.type	Const.h00042680,@object
	.section	.constants,"aM",@progbits,4
	.align	2
Const.h00042680:                        // @Const.h00042680
	.long	272000                  // 0x42680
	.size	Const.h00042680, 4


