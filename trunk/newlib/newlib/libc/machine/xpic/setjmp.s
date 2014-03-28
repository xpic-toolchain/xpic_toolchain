
	.text
	.global	setjmp
setjmp:
	store [z0 + r2++], r0
	store [z0 + r2++], r1
	store [z0 + r2++], u0
	store [z0 + r2++], u1
	store [z0 + r2++], u2
	store [z0 + r2++], u3
	store [z0 + r2++], u4
	store [z0 + r2++], r7
	load r6, [r7]
	store	[r2], r6

/* Return 0 to caller. */
	add r2, z0, z0
	inc pc, [r7++], #8
  nop

	.global	longjmp
longjmp:
	load r0, [z0 + r2++]
	load r1, [z0 + r2++]
	load u0, [z0 + r2++]
	load u1, [z0 + r2++]
	load u2, [z0 + r2++]
	load u3, [z0 + r2++]
	load r7, [z0 + r2++]
	load r6, [z0 + r2++]

/* If caller attempted to return 0, return 1 instead. */

	mov	r2, r3, z0
	[r3z]inc r2, r2, #1
	inc pc, r6, #8
  nop
	
