div1 (signed char x)
{
  return x / -1;
}

div2 (signed short x)
{
  return x / -1;
}

div3 (signed char x, signed char y)
{
  return x / y;
}

div4 (signed short x, signed short y)
{
  return x / y;
}

mod1 (signed char x)
{
  return x % -1;
}

mod2 (signed short x)
{
  return x % -1;
}

mod3 (signed char x, signed char y)
{
  return x % y;
}

mod4 (signed short x, signed short y)
{
  return x % y;
}

signed long
mod5 (signed long x, signed long y)
{
  return x % y;
}
     
unsigned long
mod6 (unsigned long x, unsigned long y)
{
  return x % y;
}
     
main ()
{
  if (div1 (-(1 << 7)) != 1 << 7)
    return -1;
  if (div2 (-(1 << 15)) != 1 << 15)
    return -1;
  if (div3 (-(1 << 7), -1) != 1 << 7)
    return -1;
  if (div4 (-(1 << 15), -1) != 1 << 15)
    return -1;
  if (mod1 (-(1 << 7)) != 0)
    return -1;
  if (mod2 (-(1 << 15)) != 0)
    return -1;
  if (mod3 (-(1 << 7), -1) != 0)
    return -1;
  if (mod4 (-(1 << 15), -1) != 0)
    return -1;
  if (mod5 (0x50000000, 2) != 0)
    return -1;
  if (mod6 (0x50000000, 2) != 0)
    return -1;
  
  return 0;
}
