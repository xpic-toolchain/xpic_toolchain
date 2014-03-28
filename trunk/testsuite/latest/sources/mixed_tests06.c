void abort14(void)
{
  while(1) { ; }
}

void b14(int*,int*);
    
typedef struct {
    double T1;
    char c;
} S;

int main(void)
{
  int i,j;
  double s;

  S x1[2][2];
  S *x[2] = { x1[0], x1[1] };
  S **E = x;

  int test = (int) E[j][i].T1;
  //E[j][i].T1 = (double) test;
  E[0][0].T1 = (double) test;
  E[0][1].T1 = (double) test;
  E[1][0].T1 = (double) test;
  E[1][1].T1 = (double) test;
  E[0][0].T1 = 1;

  for( i=0; i < 2; i++ )
  {
      E[0][0].T1 = 1;
  }

  for( i=0; i < 2; i++ )
    for( j=0; j < 2; j++ )
      E[j][i].T1 = 1;

  for( i=0; i < 2; i++ )
    for( j=0; j < 2; j++ )
      s = E[j][i].T1;

//  b14(&j,&i);
  if (s != 1)
    abort14 ();
  return 0;
}

void b14(int *i, int *j) { ; }
