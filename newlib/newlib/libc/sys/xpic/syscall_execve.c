/* _execve_r() system call */

#include <reent.h>

int _execve_r (struct _reent * ptr, const char * name, char *const *argv, char *const *env)
{
  return -1;
}
