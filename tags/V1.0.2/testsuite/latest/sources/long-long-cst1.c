/* PR middle-end/27724 */

struct st{
  int _mark;
};
unsigned long long t = ((int)(__SIZE_TYPE__)&(((struct st*)16)->_mark) - 32);

int main()
{
  if (t != (unsigned long long)(int)-16)
    return -1;
  return 0;
}

