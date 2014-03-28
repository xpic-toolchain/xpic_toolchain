//===-- divdi3_test.c - Test __divdi3 -------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file tests __divdi3 for the compiler_rt library.
//
//===----------------------------------------------------------------------===//

#include "int_lib.h"
#include <stdio.h>

// Returns: a / b

di_int __divdi3(di_int a, di_int b);

long long *p;

int test__divdi3(di_int a, di_int b, di_int expected)
{
    di_int x = __divdi3(a, b);
    p = &x;
    if (x != expected)
      return 1;
    return 0;
}

char assumption_1[sizeof(di_int) == 2*sizeof(si_int)] = {0};

int main()
{
    if (test__divdi3(0, 1, 0))
        return 1;
    if (test__divdi3(0, -1, 0))
        return 2;

    if (test__divdi3(2, 1, 2))
        return 3;
    if (test__divdi3(2, -1, -2))
        return 4;
    if (test__divdi3(-2, 1, -2))
        return 5;
    if (test__divdi3(-2, -1, 2))
        return 6;

    if (test__divdi3(0x8000000000000000LL, 1, 0x8000000000000000LL))
        return 7;
    if (test__divdi3(0x8000000000000000LL, -1, 0x8000000000000000LL))
        return 8;
    if (test__divdi3(0x8000000000000000LL, -2, 0x4000000000000000LL))
        return 9;
    if (test__divdi3(0x8000000000000000LL, 2, 0xC000000000000000LL))
        return 10;

    if (test__divdi3(10000000000LL, 10000000000LL, 1))
        return 11;

    if (test__divdi3(100000000000LL, 10000000000LL, 10))
        return 12;

    return 0;
}
