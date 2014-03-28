#define inline_asm1()\
  asm volatile (\
                "load r2, [z0 + r7]         \n\t"\
                "load r3, [z0 + r7]         \n\t"\
                "load r4, [z0 + r7]         \n\t"\
                "load r5, [z0 + r7]         \n\t"\
                :::"r2","r3","r4","r5"\
                );

#define inline_asm2()\
  asm volatile (\
                "load r2, #0         \n\t"\
                "load r3, #0         \n\t"\
                "load r4, #0         \n\t"\
                "load r5, #0         \n\t"\
                :::"r2","r3","r4","r5"\
                );


int main ( void )
{
  int a = 3;
  //inline_asm1();
  int b = 4;
  //inline_asm2();
  volatile int c = 5;
  //inline_asm2();
  int d = 6;
  a = c*(d/a); // 5 * (6/3) = 5 * 2 = 10

  inline_asm1();

  a = a +(c*(a/c)); // 10 + 5 * (10/5) = 10 + 5*2 = 20

  inline_asm1();
  inline_asm2();
  inline_asm1();
  inline_asm2();

  a = 10 * a;

  inline_asm2();

  a = a/c;

  inline_asm1();
  inline_asm2();
  inline_asm1();
  inline_asm2();

  return ( a-40 );
} 
