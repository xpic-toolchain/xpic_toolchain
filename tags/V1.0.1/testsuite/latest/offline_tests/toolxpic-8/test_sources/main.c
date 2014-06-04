int main( void )
{
  asm( "gie r0" );
  asm( "gsie r0, #3, #0" );
  asm( "bcd2hex r4, r0, z0" );
  asm( "hex2bcd r4, r0, z0" );
  asm( "sie r0" );
}
