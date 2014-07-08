struct A11 {
  unsigned long long b11 : 8;
  unsigned long long c11 : 18;
};

struct A11 a11;
long long l11;

int main()
{
  l11 = a11.c11 = 0x123456789aULL;
  return 0;
}
