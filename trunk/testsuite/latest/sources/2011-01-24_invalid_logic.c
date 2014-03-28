int main ( void )
{
  long A_e;

  int r;

  A_e = 1048; // 0x00000418 => inv = 0xFFFFFBE7;

  r = !( !A_e && (0) );

  if( r != 0 )return 0;

  return -1;
}

/*
 * Release:
 * 0x08030000 <main>:    load  r2, #1
 * 0x08030004 <main+4>:  inc  pc, [ r7++ ]d, #8
 * 0x08030008 <main+8>:  nop
 */

/*
 * Debug:
 * 0x08030014 <label3>:     rol  z0, r2, #0 // r2=1048
 * 0x08030018 <label3+4>:   jmp  ne, #8
 * 0x0803001c <label3+8>:   or  r0, z0, z0
 * 0x08030020 <BB1_2>:      bs  r2, #1, r0 // r2=0
 * 0x08030024 <BB1_2+4>:    store  [ r7 + #0 ]d, r2
 * 0x08030028 <label4>:     store  [ r7 + #16 ]d, r2
 * 0x0803002c <label6>:     add  r7, #20, r7
 * 0x08030030 <label6+4>:   nop
 * 0x08030034 <label6+8>:   inc  pc, [ r7++ ]d, #8
 * 0x08030038 <label6+12>:  nop
 */
