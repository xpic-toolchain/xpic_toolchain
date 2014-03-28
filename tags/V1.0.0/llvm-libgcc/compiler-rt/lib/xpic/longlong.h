#ifdef __XPIC__

#define __ll_B ((UWtype) 1 << (W_TYPE_SIZE / 2))
#define __ll_lowpart(t) ((UWtype) (t) & (__ll_B - 1))
#define __ll_highpart(t) ((UWtype) (t) >> (W_TYPE_SIZE / 2))

void __add_ssaaaa(long* sh, long* sl, long ah, long al, long bh, long bl);
#define add_ssaaaa(sh,sl,a1h,a1l,a2h,a2l) __add_ssaaaa(&sh,&sl,a1h,a1l,a2h,a2l)

void __sub_ddmmss(long* sh, long* sl, long ah, long al, long bh, long bl);
#define sub_ddmmss(dh,dl,mh,ml,sh,sl) __sub_ddmmss(&dh,&dl,mh,ml,sh,sl)

void __umul_ppmm(unsigned long* xh, unsigned long* xl, unsigned long a, unsigned long b);
#define umul_ppmm(ph,pl,m1,m2) __umul_ppmm(&ph,&pl,m1,m2)

#define __udiv_qrnnd_c(q, r, n1, n0, d) \
  do {                  \
    UWtype __d1, __d0, __q1, __q0;          \
    UWtype __r1, __r0, __m;           \
    __d1 = __ll_highpart (d);           \
    __d0 = __ll_lowpart (d);            \
                  \
    __r1 = (n1) % __d1;             \
    __q1 = (n1) / __d1;             \
    __m = (UWtype) __q1 * __d0;           \
    __r1 = __r1 * __ll_B | __ll_highpart (n0);        \
    if (__r1 < __m)             \
      {                 \
  __q1--, __r1 += (d);            \
  if (__r1 >= (d)) /* i.e. we didn't get carry when adding to __r1 */\
    if (__r1 < __m)           \
      __q1--, __r1 += (d);          \
      }                 \
    __r1 -= __m;              \
                  \
    __r0 = __r1 % __d1;             \
    __q0 = __r1 / __d1;             \
    __m = (UWtype) __q0 * __d0;           \
    __r0 = __r0 * __ll_B | __ll_lowpart (n0);       \
    if (__r0 < __m)             \
      {                 \
  __q0--, __r0 += (d);            \
  if (__r0 >= (d))            \
    if (__r0 < __m)           \
      __q0--, __r0 += (d);          \
      }                 \
    __r0 -= __m;              \
                  \
    (q) = (UWtype) __q1 * __ll_B | __q0;        \
    (r) = __r0;               \
  } while (0)

/* If udiv_qrnnd was not defined for this processor, use __udiv_qrnnd_c.  */
#if !defined (udiv_qrnnd)
#define UDIV_NEEDS_NORMALIZATION 1
#define udiv_qrnnd __udiv_qrnnd_c
#endif

#endif /* __XPIC__ */
