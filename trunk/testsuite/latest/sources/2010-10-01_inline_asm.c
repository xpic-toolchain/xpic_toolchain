
int selfsub(int in)
{
  int out;

  asm volatile ("sub %0, %1, %1"
      :"=r"(out)
      :"r"(in)
  );

  return out;

//  return in-in;
}

int doCritical(int(*func)(int in))
{
  int crit, out;

  /* enter critical section: disable interrupts */
  asm volatile("load u0, #h20281"      "\n\t"
               "lsl u0, u0, #11"       "\n\t"
               "nop"                   "\n\t"
               "load %0, [u0 + #0]"    "\n\t"
               "store [u0 + #0], z0"   "\n\t"
      :"=r"(crit) /* output register into crit variable */
      :           /* no input */
      :"u0"       /* clobbers u0 */
  );

  /* critical section */
  out = func(1024);

  /* leave critical section: enable interrupts */
  asm volatile("load u0, #h20281"      "\n\t"
               "lsl u0, u0, #11"       "\n\t"
               "nop"                   "\n\t"
               "store [u0 + #0], %0"   "\n\t"
      :          /* no outputs */
      :"r"(crit) /* input is crit variable (must be a work register) */
      :"u0"      /* clobbers u0 */
  );

  return out;
}

int main ( void )
{
  if( selfsub(25) != 0 )
    return -1;

  if( doCritical(selfsub) != 0 )
    return -2;

  return 0;
} 
