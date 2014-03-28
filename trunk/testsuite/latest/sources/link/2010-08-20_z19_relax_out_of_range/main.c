
typedef struct
{
  float in1;
  float in2;
  float out;
}real_2op_struct;

typedef struct
{
  char *pszComponentName;
  unsigned long ulSize;
  unsigned long *pResult;
  unsigned char *pMemResult;
} sysmemallocdata_struct;

void real32__add(real_2op_struct* p);
void sysmemallocdata(sysmemallocdata_struct *p);

static real_2op_struct tOp;
static sysmemallocdata_struct tAlloc;


int main()
{

#if 1
  /* SysInternalLibDefault
   * SysTimeGetMs
   */
  tOp.in1 = 1.0f;
  tOp.in2 = 2.0f;
  real32__add(&tOp);

  if( tOp.out != 3.0f )
    return 1;
#endif

#if 0
  /* SysMem
   * SysMemOS
   */
  tAlloc.ulSize = 0x40;
  tAlloc.pszComponentName = "test";
  sysmemallocdata(&tAlloc);
  if( !tAlloc.pMemResult )
    return 2;

#endif


  return 0;
}
