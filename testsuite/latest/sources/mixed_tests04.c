float f1(float x, unsigned short y)
{
  return x * y;
}

float f2(float x, short y)
{
  return x * y;
}

float f3(float x, char y)
{
  return x * y;
}

float f4(float x, unsigned char y)
{
  return x * y;
}

float f5(float x, int y)
{
  return x * y;
}

double c1(float x, unsigned short y, int z)
{
  return z ? x + x : y;
}

double c2(float x, short y, int z)
{
  return z ? x + x : y;
}

double c3(float x, char y, int z)
{
  return z ? x + x : y;
}

double c4(float x, unsigned char y, int z)
{
  return z ? x + x : y;
}

double c5(float x, int y, int z)
{
  return z ? x + x : y;
}

int main(){
  f1(4.3, 34);
  f2(4.342, -23);
  f3(422.3, 'A');
  f4(342.34, 'Z');
  f5(34.43, 342);
  c1(213.23, 23,34);
  c2(323.23, -34,33);
  c3(342.32, '-',34);
  c4(562.34, 'b',34);
  c5(102.23,3,3);
  return 0;
}
