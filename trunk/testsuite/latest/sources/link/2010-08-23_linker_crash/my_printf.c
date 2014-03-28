/*
 * my_printf.c
 *
 *  Overwriting libc functions
 */
#include <stdio.h>

int printf(const char * x, ...)
{
  return 0;
}

int sprintf(char *s, const char * x, ...)
{
  if(s)return 0;
  while(1);
}

int puts(const char* s)
{
  return 0;
}
