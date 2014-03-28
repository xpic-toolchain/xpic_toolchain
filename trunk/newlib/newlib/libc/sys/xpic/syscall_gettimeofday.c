/* _gettimeofday_r() system call */

#include <sys/time.h>

int
_gettimeofday (struct _reent *ptr, struct timeval * tp, void * tzvp)
{
  if (tp)
  {
    tp->tv_sec = 0;
    tp->tv_usec = 0;
  }

  return 0;
}
