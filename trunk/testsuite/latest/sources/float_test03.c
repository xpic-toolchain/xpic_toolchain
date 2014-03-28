typedef union
{
  float value;
  int word;
} ieee_float_shape_type;

int isinff(float x)
{
  int ix,t;
  ieee_float_shape_type gf_u;
  gf_u.value = x;
  ix = gf_u.word;
  t = ix & 0x7fffffff;
  t ^= 0x7f800000;
  t |= -t;
  return ~(t >> 31) & (1 - ((ix & 0x80000000) >> 30));
}

main ()
{
  float x = 1.0 / 0.0;
  int i = isinff (x);

  if (i == 0)
    return -1;


  return 0;
}
