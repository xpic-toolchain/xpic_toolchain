#include <stdlib.h>
#include <stdarg.h>

int AddIntegers ( int amount, ... )
{
  int i;
  int val = 0;
  va_list vl;
  va_start( vl, amount);
  for (i=0;i<amount;i++)
  {
    val = val + va_arg(vl,int);
  }
  va_end(vl);
  return ( val );
}

double AddDoubles ( int amount, ... )
{
  int i;
  double val = 0;
  va_list vl;
  va_start( vl, amount);
  for (i=0;i<amount;i++)
  {
    val = val + va_arg(vl,double);
  }
  va_end(vl);
  return ( val );
}

int varg_test( char *format, ... ) {
  va_list argptr;
  va_start( argptr, format );

  while( *format != '\0' ) {
    // string
    if( *format == 's' ) {
      char* s = va_arg( argptr, char * );
      if ( s[0] != 'T' || s[1] != 'h' || s[2] != 'i' || s[3] != 's' || s[4] != ' ' || s[5] != 'i' || s[6] != 's' || s[7] != ' ' || s[8] != 'a')
      {
        return ( -1 );
      }
    }
    // character
    else if( *format == 'c' ) {
      char c = (char) va_arg( argptr, int );
      if ( c != 'X' )
      {
        return ( -1 );
      }
    }
    // integer
    else if( *format == 'd' ) {
      int d = va_arg( argptr, int );
      if ( d != 29 )
      {
        return ( -1 );
      }
    }

    format++;
  }

  va_end( argptr );
  return ( 0 );
}

int varg_test_reloaded( char *format, char *string, ... )
{
  va_list argptr;
  va_start( argptr, string);

  if( format[0] != 's' || format[1] != 'd' || format[2] != 'c')
  {
    return ( -1 );
  }
  if ( string[0] != 'T' || string[1] != 'h' || string[2] != 'i' || string[3] != 's' )
  {
    return ( -1 );
  }
  int d = va_arg( argptr, int );
  if ( d != 1 )
    return ( -1 );

  va_end( argptr );
  return ( 0 );
}

int ex_siprintf (char *str, char* fmt, ...)
{
  va_list argptr;
  va_start( argptr, fmt );

  if( str[0] != 'A' || str[1] != 'B' || str[2] != 'C' )
  {
    return ( -1 );
  }
  if( fmt[0] != '%' || fmt[1] != 'd' || fmt[2] != ' ' )
  {
    return ( -1 );
  }
  int d = va_arg( argptr, int );
  if ( d != 1 )
    return ( -1 );
  d = va_arg( argptr, int );
  if ( d != 2 )
    return ( -1 );
  d = va_arg( argptr, int );
  if ( d != 3 )
    return ( -1 );
  d = va_arg( argptr, int );
  if ( d != 4 )
    return ( -1 );

  va_end( argptr );
  return ( 0 );
}

int
_DEFUN(defun_ex_siprintf, (str, fmt),
       char *str _AND
       _CONST char *fmt _DOTS)
{
  va_list argptr;
  va_start( argptr, fmt );

  if(strlen(str)!=8)
    return 1;
  if(strlen(fmt)!=11)
    return 1;

  if( str[0] != 'A' || str[1] != 'B' || str[2] != 'C' )
  {
    return ( -1 );
  }
  if( fmt[0] != '%' || fmt[1] != 'd' || fmt[2] != ' ' )
  {
    return ( -1 );
  }
  int d = va_arg( argptr, int );
  if ( d != 1 )
    return ( -1 );
  d = va_arg( argptr, int );
  if ( d != 2 )
    return ( -1 );
  d = va_arg( argptr, int );
  if ( d != 3 )
    return ( -1 );
  d = va_arg( argptr, int );
  if ( d != 4 )
    return ( -1 );

  va_end( argptr );
  return ( 0 );
}


int main ( void )
{
  int i_ret = AddIntegers ( 3, 4, 5, 6 );
  i_ret = i_ret - 15;
  if ( i_ret != 0 )
    return ( -1 );

  float f_ret = AddDoubles ( 3, 12.0, 0.0, 3.0 );
  f_ret = f_ret - 15;
  if ( f_ret != 0.0 )
    return ( -1 );

  int v_ret = varg_test( "sdc", "This is a string", 29, 'X' );
  if ( v_ret != 0 )
    return ( -1 );

  int r_ret = varg_test_reloaded( "sdc", "This is a string", 1,2,3,4);
  if ( r_ret != 0 )
    return ( -1 );

  char test[16] = "ABCDEF00";

  int exsiprintf_test = ex_siprintf(test, "%d %d %d %d", 1,2,3,4);
  if ( exsiprintf_test != 0 )
    return ( -1 );

  int defun_exsiprintf_test = defun_ex_siprintf(test, "%d %d %d %d", 1,2,3,4);
  if ( defun_exsiprintf_test != 0 )
    return ( -1 );

  return ( 0 );
}
