/* In standard C, enumeration constants always have type int.  If they
   are not representables are int, they're ill-formed.  In GNU C, we
   give such ill-formed constructs well-defined meaning.  Make sure it
   works.  */

enum foo
{
  foo1   = 0,
  foo2   = 0xffffffffffffffffULL,
  foo3   = 0xf0fffffffffffffeULL
};

int main ()
{
  if (sizeof (enum foo) != sizeof (unsigned long long))
    return -1;
  return 0;
}
