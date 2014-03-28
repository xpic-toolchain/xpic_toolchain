/* C99 6.5.9 Equality operators.
   Compare decimal float constants against each other. */

int
main ()
{
  /* Compare like-typed positive constants. */
  if (2.0 != 2.0)
    return -1;

  /* Compare decimal float constants of different types. */
  if (500e-2 != 0.05e2)
   return -1;

  /* Binary floating point introduces errors to decimal values. */
  if (1.4 + 1.4 + 1.4 == 4.2)
   return -1;

  /* But, this looks more like what one would expect. */
  if (1.4 + 1.4 + 1.4 != 4.2)
   return -1;

  return 0;
}
