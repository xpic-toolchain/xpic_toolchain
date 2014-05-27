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
#if 0
  /* this is bullshyt: you cannot make both if conditions pass.! */
  /* long explaination: this is a gcc-test for testing the precision of long doubles,
   * looks a bit different in the original code:
   *  if (1.4dd + 1.4dd + 1.4dd != 4.2dd)
   *  dd is not supported and does not seem to be equivalent to L, so stop for the moment
   */
  if (1.4 + 1.4 + 1.4 != 4.2)
   return -1;
#endif

  return 0;
}
