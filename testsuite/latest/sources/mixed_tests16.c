/* This used to ab due to a loop bug on s390*.  */

void ab (void)
{
  while (1)
  {
    ;
  }
}

int count = 0;
char *str;

void test (int flag)
{
  char *p;

  for (;;)
    {
      if (count > 5)
	return;

      p = "test";

      if (flag)
	count++;

      str = p;
    }
}

int main (void)
{
  test (1);

  if (str[0] != 't')
    ab ();

  return 0;
}

