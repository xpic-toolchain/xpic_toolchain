
int callFunction(int(*pfn)(int arg))
{
  return pfn(3);
}

int function(int x)
{
  return x + 1;
}

int main(void)
{
  int result = callFunction(function);
  if( result != 4 )
    return 1;

  return 0;
}
