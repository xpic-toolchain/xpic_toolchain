# xPIC startup code

  .text
  .global init_xpic
  .align  2
  .type init_xpic,@function
start:
/* nop-hack fixing debugger beginning at 0x0, until debugger is fixed and begins at 0xfffffffc */
  nop
init_xpic:
  nop

/* init user/irq stack */
  load r7, $__stack_top_std


/** clear vic
 * The following 16 instructions saves us 10ns in FIQ for initializing the stack pointer.
 * Without this init code the FIQ ISR must begin with something like "load r7, $__stack_top_fiq".
 *
 * A user register (u0) is used to address the vic,
 * so we can reuse the pointer in fiq_handler_init_stack
 */
  load u0, [pc + $XpicVicBase]
  add r1, #-1, z0
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

/* init arguments for main() */
  mov r2, z0, z0    /* int    argc */
  mov r3, z0, z0    /* char** argv */
  mov r4, z0, z0    /* char*  envp */
  store [--r7], pc
  jmp main
  nop
exit:
  jmp exit

fiq_handler_init_stack:
  store [u0 + #h14], u1
/* init fiq stack pointer */
  load r7, $__stack_top_fiq
  retf

.section  .constants,"aM",@progbits,4
XpicVicBase:
  .long 0x10140800
StackPattern:
  .long 0xdeadbeef
