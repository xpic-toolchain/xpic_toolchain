typedef struct
{
  volatile unsigned int in;
  volatile unsigned int uiType;
  float out;
}any32_to_real32_struct;

typedef enum
{
  TYPE3_BOOL,
  TYPE3_BIT,
  TYPE3_BYTE,
  TYPE3_WORD,
  TYPE3_DWORD,
  TYPE3_LWORD,
  TYPE3_SINT,
  TYPE3_INT,
  TYPE3_DINT,
  TYPE3_LINT,
  TYPE3_USINT,
  TYPE3_UINT,
  TYPE3_UDINT,
  TYPE3_ULINT,
  TYPE3_REAL,
  TYPE3_LREAL,
  TYPE3_STRING,
  TYPE3_WSTRING,
  TYPE3_TIME,
  TYPE3_DATE,
  TYPE3_DATEANDTIME,
  TYPE3_TIMEOFDAY,
  TYPE3_POINTER,
  TYPE3_REFERENCE,
  TYPE3_SUBRANGE,
  TYPE3_ENUM,
  TYPE3_ARRAY,
  TYPE3_PARAMS,
  TYPE3_USERDEF,
  TYPE3_NONE,
  TYPE3_ANY,    /* all types */
  TYPE3_ANYBIT, /* all "bit"-types: bit, byte, word, dword, lword */
  TYPE3_ANYDATE,  /* time, dat, tod, date*/
  TYPE3_ANYINT,
  TYPE3_ANYNUM,
  TYPE3_ANYREAL,
  TYPE3_LAZY,
  TYPE3_LTIME,
  TYPE3_BITCONST,
  TYPE3_MAX_TYPE
} TypeClass3;

static void any32__to__real32(any32_to_real32_struct* p)
{
  TypeClass3 tc = (TypeClass3)p->uiType;
  switch (tc)
  {
    case TYPE3_SINT:
    case TYPE3_INT:
    case TYPE3_DINT:
    {
      long lout = *(long*)&(p->in);
      p->out = (float)lout;
      break;
    }
    case TYPE3_BOOL:
    case TYPE3_BYTE:
    case TYPE3_WORD:
    case TYPE3_DWORD:
    case TYPE3_USINT:
    case TYPE3_UINT:
    case TYPE3_TIME:
    case TYPE3_UDINT:
    case TYPE3_DATE:
    case TYPE3_DATEANDTIME:
    case TYPE3_TIMEOFDAY:
    {
      p->out = (float)p->in;
      break;
    }
    default:
    {
      p->out = 0;
      break;
    }
  }
}

static any32_to_real32_struct tS;

int main(void)
{
  tS.in = 16;
  tS.out = 0.0f;
  tS.uiType = TYPE3_INT;

  any32__to__real32(&tS);
  if( tS.out != 16.0f )
    return 1;

  return 0;
}
