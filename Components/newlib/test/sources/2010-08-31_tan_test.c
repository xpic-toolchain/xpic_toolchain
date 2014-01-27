#include <math.h>

long tan_vec[][8] = {
    {64, 0,123,__LINE__, 0xc00493c4, 0x3acb164c, 0xbff33333, 0x33333333},     /* -2.57215=f(-1.20000)*/
    {63, 0,123,__LINE__, 0xc003fbb2, 0xac8d6e09, 0xbff30a3d, 0x70a3d70a},     /* -2.49789=f(-1.19000)*/
    {63, 0,123,__LINE__, 0xc0036b0a, 0xa04728b5, 0xbff2e147, 0xae147ae1},     /* -2.42726=f(-1.18000)*/
    {64, 0,123,__LINE__, 0xc002e13d, 0xc4352012, 0xbff2b851, 0xeb851eb8},     /* -2.35998=f(-1.17000)*/
    {64, 0,123,__LINE__, 0xc0025dcb, 0xa00cf2d1, 0xbff28f5c, 0x28f5c28f},     /* -2.29579=f(-1.16000)*/
    {64, 0,123,__LINE__, 0xc001e03f, 0xefaf53c9, 0xbff26666, 0x66666666},     /* -2.23449=f(-1.15000)*/
    {64, 0,123,__LINE__, 0xc0016831, 0x3887b2e3, 0xbff23d70, 0xa3d70a3d},     /* -2.17587=f(-1.14000)*/
    {64, 0,123,__LINE__, 0xc000f53f, 0x9047cd43, 0xbff2147a, 0xe147ae14},     /* -2.11975=f(-1.13000)*/
    {64, 0,123,__LINE__, 0xc0008713, 0x8d550701, 0xbff1eb85, 0x1eb851eb},     /* -2.06595=f(-1.12000)*/
    {64, 0,123,__LINE__, 0xc0001d5d, 0x5a921689, 0xbff1c28f, 0x5c28f5c2},     /* -2.01433=f(-1.11000)*/
    {64, 0,123,__LINE__, 0xbfff6fa7, 0xd286214a, 0xbff19999, 0x99999999},     /* -1.96476=f(-1.10000)*/
    {64, 0,123,__LINE__, 0xbffeac68, 0x7953b03b, 0xbff170a3, 0xd70a3d70},     /* -1.91709=f(-1.09000)*/
    {64, 0,123,__LINE__, 0xbffdf081, 0x975fac4d, 0xbff147ae, 0x147ae147},     /* -1.87121=f(-1.08000)*/
    {64, 0,123,__LINE__, 0xbffd3b81, 0xeb0fcaad, 0xbff11eb8, 0x51eb851e},     /* -1.82702=f(-1.07000)*/
    {64, 0,123,__LINE__, 0xbffc8d01, 0x0d5ddd8b, 0xbff0f5c2, 0x8f5c28f5},     /* -1.78442=f(-1.06000)*/
    {64, 0,123,__LINE__, 0xbffbe49e, 0x982fe6aa, 0xbff0cccc, 0xcccccccc},     /* -1.74331=f(-1.05000)*/
    {64, 0,123,__LINE__, 0xbffb4201, 0x654a4047, 0xbff0a3d7, 0x0a3d70a3},     /* -1.70361=f(-1.04000)*/
    {64, 0,123,__LINE__, 0xbffaa4d6, 0xe2aca2f8, 0xbff07ae1, 0x47ae147a},     /* -1.66524=f(-1.03000)*/
    {64, 0,123,__LINE__, 0xbffa0cd2, 0x79a136ed, 0xbff051eb, 0x851eb851},     /* -1.62813=f(-1.02000)*/
    {64, 0,123,__LINE__, 0xbff979ad, 0x06284697, 0xbff028f5, 0xc28f5c28},     /* -1.59220=f(-1.01000)*/
    {63, 0,123,__LINE__, 0xbff8eb24, 0x5cbee3a1, 0xbfefffff, 0xfffffffe},     /* -1.55740=f(-0.01000)*/
    {58, 0,123,__LINE__, 0xbff860fa, 0xdcc59060, 0xbfefae14, 0x7ae147ac},     /* -1.52367=f(-0.99000)*/
    {64, 0,123,__LINE__, 0xbff7daf7, 0x0e07fa59, 0xbfef5c28, 0xf5c28f5a},     /* -1.49095=f(-0.98000)*/
    {62, 0,123,__LINE__, 0xbff758e3, 0x4819d01a, 0xbfef0a3d, 0x70a3d708},     /* -1.45920=f(-0.97000)*/
    {64, 0,123,__LINE__, 0xbff6da8d, 0x62683760, 0xbfeeb851, 0xeb851eb6},     /* -1.42835=f(-0.96000)*/
    {64, 0,123,__LINE__, 0xbff65fc6, 0x6c04955b, 0xbfee6666, 0x66666664},     /* -1.39838=f(-0.95000)*/
    {62, 0,123,__LINE__, 0xbff5e862, 0x6a4d39ca, 0xbfee147a, 0xe147ae12},     /* -1.36923=f(-0.94000)*/
    {63, 0,123,__LINE__, 0xbff57438, 0x1db3e2ff, 0xbfedc28f, 0x5c28f5c0},     /* -1.34087=f(-0.93000)*/
    {64, 0,123,__LINE__, 0xbff50320, 0xcbf9abc7, 0xbfed70a3, 0xd70a3d6e},     /* -1.31326=f(-0.92000)*/
    {63, 0,123,__LINE__, 0xbff494f8, 0x0f4c58a1, 0xbfed1eb8, 0x51eb851c},     /* -1.28636=f(-0.91000)*/
    {64, 0,123,__LINE__, 0xbff4299b, 0xa9c2a134, 0xbfeccccc, 0xccccccca},     /* -1.26015=f(-0.90000)*/
//    {63, 0,123,__LINE__, 0xbff3c0eb, 0x5cc4656d, 0xbfec7ae1, 0x47ae1478},     /* -1.23459=f(-0.89000)*/

    {64, 0,123,__LINE__, 0xbfd05785, 0xa43c4c45, 0xbfcfffff, 0xffffffe0},     /* -0.25534=f(-0.25000)*/
    // FIXME: Parameter unter 0.9 fuehren zum Ergebnis 0

//    {64, 0,123,__LINE__, 0x3ff3c0eb, 0x5cc46582, 0x3fec7ae1, 0x47ae1489},     /* 1.23459=f(0.89000)*/
    {64, 0,123,__LINE__, 0x3ff4299b, 0xa9c2a14a, 0x3feccccc, 0xccccccdb},     /* 1.26015=f(0.90000)*/
    {64, 0,123,__LINE__, 0x3ff494f8, 0x0f4c58b7, 0x3fed1eb8, 0x51eb852d},     /* 1.28636=f(0.91000)*/
    {64, 0,123,__LINE__, 0x3ff50320, 0xcbf9abde, 0x3fed70a3, 0xd70a3d7f},     /* 1.31326=f(0.92000)*/
    {63, 0,123,__LINE__, 0x3ff57438, 0x1db3e317, 0x3fedc28f, 0x5c28f5d1},     /* 1.34087=f(0.93000)*/
    {63, 0,123,__LINE__, 0x3ff5e862, 0x6a4d39e3, 0x3fee147a, 0xe147ae23},     /* 1.36923=f(0.94000)*/
    {64, 0,123,__LINE__, 0x3ff65fc6, 0x6c049574, 0x3fee6666, 0x66666675},     /* 1.39838=f(0.95000)*/
    {64, 0,123,__LINE__, 0x3ff6da8d, 0x6268377b, 0x3feeb851, 0xeb851ec7},     /* 1.42835=f(0.96000)*/
    {64, 0,123,__LINE__, 0x3ff758e3, 0x4819d034, 0x3fef0a3d, 0x70a3d719},     /* 1.45920=f(0.97000)*/
    {64, 0,123,__LINE__, 0x3ff7daf7, 0x0e07fa74, 0x3fef5c28, 0xf5c28f6b},     /* 1.49095=f(0.98000)*/
    {64, 0,123,__LINE__, 0x3ff860fa, 0xdcc5907c, 0x3fefae14, 0x7ae147bd},     /* 1.52367=f(0.99000)*/
    {63, 0,123,__LINE__, 0x3ff8eb24, 0x5cbee3bd, 0x3ff00000, 0x00000007},     /* 1.55740=f(1.00000)*/
    {61, 0,123,__LINE__, 0x3ff979ad, 0x062846b5, 0x3ff028f5, 0xc28f5c30},     /* 1.59220=f(1.01000)*/
    {62, 0,123,__LINE__, 0x3ffa0cd2, 0x79a1370a, 0x3ff051eb, 0x851eb859},     /* 1.62813=f(1.02000)*/
    {64, 0,123,__LINE__, 0x3ffaa4d6, 0xe2aca316, 0x3ff07ae1, 0x47ae1482},     /* 1.66524=f(1.03000)*/
    {64, 0,123,__LINE__, 0x3ffb4201, 0x654a4066, 0x3ff0a3d7, 0x0a3d70ab},     /* 1.70361=f(1.04000)*/
    {64, 0,123,__LINE__, 0x3ffbe49e, 0x982fe6cb, 0x3ff0cccc, 0xccccccd4},     /* 1.74331=f(1.05000)*/
    {64, 0,123,__LINE__, 0x3ffc8d01, 0x0d5dddad, 0x3ff0f5c2, 0x8f5c28fd},     /* 1.78442=f(1.06000)*/
    {63, 0,123,__LINE__, 0x3ffd3b81, 0xeb0fcacf, 0x3ff11eb8, 0x51eb8526},     /* 1.82702=f(1.07000)*/
    {64, 0,123,__LINE__, 0x3ffdf081, 0x975fac71, 0x3ff147ae, 0x147ae14f},     /* 1.87121=f(1.08000)*/
    {64, 0,123,__LINE__, 0x3ffeac68, 0x7953b060, 0x3ff170a3, 0xd70a3d78},     /* 1.91709=f(1.09000)*/
    {63, 0,123,__LINE__, 0x3fff6fa7, 0xd2862171, 0x3ff19999, 0x999999a1},     /* 1.96476=f(1.10000)*/
    {64, 0,123,__LINE__, 0x40001d5d, 0x5a92169e, 0x3ff1c28f, 0x5c28f5ca},     /* 2.01433=f(1.11000)*/
    {64, 0,123,__LINE__, 0x40008713, 0x8d550717, 0x3ff1eb85, 0x1eb851f3},     /* 2.06595=f(1.12000)*/
    {64, 0,123,__LINE__, 0x4000f53f, 0x9047cd58, 0x3ff2147a, 0xe147ae1c},     /* 2.11975=f(1.13000)*/
    {64, 0,123,__LINE__, 0x40016831, 0x3887b2fa, 0x3ff23d70, 0xa3d70a45},     /* 2.17587=f(1.14000)*/
    {64, 0,123,__LINE__, 0x4001e03f, 0xefaf53e1, 0x3ff26666, 0x6666666e},     /* 2.23449=f(1.15000)*/
    {64, 0,123,__LINE__, 0x40025dcb, 0xa00cf2ea, 0x3ff28f5c, 0x28f5c297},     /* 2.29579=f(1.16000)*/
    {64, 0,123,__LINE__, 0x4002e13d, 0xc435202c, 0x3ff2b851, 0xeb851ec0},     /* 2.35998=f(1.17000)*/
    {64, 0,123,__LINE__, 0x40036b0a, 0xa04728d0, 0x3ff2e147, 0xae147ae9},     /* 2.42726=f(1.18000)*/
    {64, 0,123,__LINE__, 0x4003fbb2, 0xac8d6e26, 0x3ff30a3d, 0x70a3d712},     /* 2.49789=f(1.19000)*/
};

#define EPSILON 0.0001

union testcase
{
  double d_value;
  struct { long low, high; } l_value;
} ret, param, result;

int main ( void )
{
  int *ptri;
  int n, d;
  const unsigned N = sizeof ( tan_vec ) / sizeof ( tan_vec[0] );
  unsigned i;
  ptri = &i;
  for ( i = 0; i < N; ++i )
  {
    param.l_value.high = tan_vec[i][6];
    param.l_value.low = tan_vec[i][7];
    ret.d_value = tan(param.d_value);
    result.l_value.high = tan_vec[i][4];
    result.l_value.low = tan_vec[i][5];
    if (tan_vec[i][1] == 13)
    {
      if ( ( ret.d_value <= result.d_value+(tan_vec[i][2]*EPSILON) && ret.d_value >= result.d_value-(tan_vec[i][2]*EPSILON) ) )
      {
        return ( i );
      }
    }
    else if (tan_vec[i][1] == 0)
    {
      if ( !( ret.d_value <= result.d_value+(tan_vec[i][2]*EPSILON) && ret.d_value >= result.d_value-(tan_vec[i][2]*EPSILON) ) )
      {
        return ( i );
      }
    }
    else
    {
      ; // Default
    }
  }

  return ( 0 );
}
