/* _fcntl_r() system call */

#include <reent.h>

int _fcntl_r(struct _reent *ptr, int fd, int cmd, int arg)
{
  return -1;
}
