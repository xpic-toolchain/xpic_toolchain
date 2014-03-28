static volatile unsigned int space[0x80000] = {0};
unsigned int linkspace()
{
  return space[0];
}
