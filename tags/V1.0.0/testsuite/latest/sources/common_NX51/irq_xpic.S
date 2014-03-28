	.text

/*  enables interrupts             */
/*  r2 is interrupt enable flag:   */
/*  r2 = 1  - irq                  */
/*  r2 = 2  - fiq                  */
/*  r2 = 3  - irq & fiq            */
	.global	xpic2_enable_int
	.align	2
	.type	xpic2_enable_int,@function
xpic2_enable_int:
	sie r2
	inc pc, [r7++], #8
	nop

/*  disables all interrupts        */
/*  retutrns interrupt enable flag */
	.global	xpic2_disable_int
	.align	2
	.type	xpic2_disable_int,@function
xpic2_disable_int:
	gsie r2, #3, #0
	inc pc, [r7++], #8
	nop

/*  disables just irq              */
/*  retutrns interrupt enable flag */
	.global	xpic2_disable_irq
	.align	2
	.type	xpic2_disable_irq,@function
xpic2_disable_irq:
	gsie r2, #3, #2
	inc pc, [r7++], #8
	nop

/*  disables just fiq              */
/*  retutrns interrupt enable flag */
	.global	xpic2_disable_fiq
	.align	2
	.type	xpic2_disable_fiq,@function
xpic2_disable_fiq:
	gsie r2, #3, #1
	inc pc, [r7++], #8
	nop

/*  retutrns interrupt enable flag */
	.global	xpic2_int_status
	.align	2
	.type	xpic2_int_status,@function
xpic2_int_status:
	gie r2
	inc pc, [r7++], #8
	nop
