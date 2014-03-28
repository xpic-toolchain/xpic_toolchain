//
//  -> initialize stack
//  -> other initialisations
//  ->
//  -> go to the main function
//

  .text
  .global start
  .align  2
  .type start,@function
start:
    /* nop-hack fixing debugger beginning at 0x0, until debugger is fixed and begins at 0xfffffffc */
    nop

init_xpic:
    /* init user/irq stack */
    load r7, $__stack_top_std

    /* clear .bss section */
    load r0, $__bss_start
    load r1, $__bss_end
    sub r1, r1, r0
    lsr r2, r1, #2
    jmp r1z, end_clear_bss
    add r2, #-1, r2
loop_clear_bss:
    store [r0 + --r1], z0
    jmpdec r2nz, loop_clear_bss
end_clear_bss:

/** clear vic
 * The following instructions saves us 10ns in FIQ for initializing the stack pointer.
 * Without this init code the FIQ ISR must begin with something like "load r7, $__stack_top_fiq".
 *
 * A user register (u0) is used to address the vic,
 * so we can reuse the pointer in fiq_handler_init_stack
 */
    /* XPIC_VIC_BASE 0x10140800 */
    load u0, #h20281
    lsl u0, u0, #11
    add r1, #-1, z0

    /* clear software irqs */
    store [u0 + #h14], r1
    store [u0 + #h18], r1

    /* clear irq vectors */
    load r1, #h30
    load r2, #hf
loop_clear_vic_vect:
    store [u0 + r1++], z0
    jmpdec r2nz, loop_clear_vic_vect

    /* setup fiq vector */
    load r2, #h40
    store [u0 + #h2c], r2
    load r1, $fiq_handler_init_stack
    store [u0 + #h1c], r1
    /* trigger fiq by software interrupt */
    load u1, #1
    store [u0 + #0], u1
    store [u0 + #hc], u1
    nop

    /* call main(0, NULL, NULL) */
    mov r2, z0, z0    /* int    argc */
    mov r3, z0, z0    /* char** argv */
    mov r4, z0, z0    /* char*  envp */
    store [z0 + --r7], pc
    jmp main
    nop
    jmp _exit

/* FIQ handler called by software interrupt to initialize the pointer for FIQ mode */
fiq_handler_init_stack:
    store [u0 + #h14], u1
    /* init fiq stack pointer */
    load r7, $__stack_top_fiq
    retf


/* Function _exit() */
  .global _exit
  .type _exit,@function
_exit:
    jmp _exit

