/* ===-- muldi3.c - Implement __muldi3 -------------------------------------===
 *
 *                     The LLVM Compiler Infrastructure
 *
 * This file is distributed under the University of Illinois Open Source
 * License. See LICENSE.TXT for details.
 *
 * ===----------------------------------------------------------------------===
 *
 * This file implements __muldi3 for the compiler_rt library.
 *
 * Hilscher xPIC Processor
 *
 * ===----------------------------------------------------------------------===
 */ 

#include "int_lib.h"

/* Returns: a * b */
di_int __muldsi3(su_int a, su_int b);

/* Returns: a * b */

di_int
__muldi3(di_int a, di_int b)
{
    dwords x;
    x.all = a;
    dwords y;
    y.all = b;
    dwords r;
    r.all = __muldsi3(x.s.low, y.s.low);
    r.s.high += x.s.high * y.s.low + x.s.low * y.s.high;
    return r.all;
}
