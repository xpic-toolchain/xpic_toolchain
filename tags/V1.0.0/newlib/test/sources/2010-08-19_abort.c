// RUNTEST! exit_code=1
/*
 * abort---abnormal termination of a program
 *
 * Synopsis
 * #include <stdlib.h>
 * void abort(void);
 *
 * Description
 * Use abort to signal that your program has detected a condition it cannot
 * deal with. Normally, abort ends your program's execution.
 *
 * Before terminating your program, abort raises the exception SIGABRT
 * (using `raise(SIGABRT)'). If you have used signal to register an
 * exception handler for this condition, that handler has the opportunity
 * to retain control, thereby avoiding program termination.
 *
 * In this implementation, abort does not perform any stream- or file-related
 * cleanup (the host environment may do so; if not, you can arrange for your
 *  program to do its own cleanup with a SIGABRT exception handler).
 *
 * Returns
 * abort does not return to its caller.
 *
 * Portability
 * ANSI C requires abort.
 *
 * Supporting OS subroutines required: getpid, kill.
 *
 */

#include <stdlib.h>

int main ( void )
{
  int i = 0;
  if (i != 1)
  {
    abort();
  }
  i--;
  return ( i );
}
