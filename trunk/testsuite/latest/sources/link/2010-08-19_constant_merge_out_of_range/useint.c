unsigned int getint();
unsigned int linkspace();
int main() {
  unsigned int i = linkspace();
  i |= getint();
  i ^= 0xfff0000;
  return i;
}
