
struct S { int x; };
 
int main()
{
  if (sizeof(struct S) != sizeof(int))
    return -1;
  return 0;
}
