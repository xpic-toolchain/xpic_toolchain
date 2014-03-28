//===-- divsi3_test.c - Test __divsi3 -------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file tests __divsi3 for the compiler_rt library.
//
//===----------------------------------------------------------------------===//

#include "int_lib.h"
#include <stdio.h>

// Returns: a / b

si_int __divsi3(si_int a, si_int b);

int test__divsi3(si_int a, si_int b, si_int expected)
{
    si_int x = __divsi3(a, b);
    if (x != expected)
      return 1;
    return 0;
}

int main()
{
    if (test__divsi3(0, 1, 0))
        return 1;
    if (test__divsi3(0, -1, 0))
        return 2;

    if (test__divsi3(2, 1, 2))
        return 3;
    if (test__divsi3(2, -1, -2))
        return 4;
    if (test__divsi3(-2, 1, -2))
        return 5;
    if (test__divsi3(-2, -1, 2))
        return 6;

    if (test__divsi3(0x80000000, 1, 0x80000000))
        return 7;
    if (test__divsi3(0x80000000, -1, 0x80000000))
        return 8;
    if (test__divsi3(0x80000000, -2, 0x40000000))
        return 9;
    if (test__divsi3(0x80000000, 2, 0xC0000000))
        return 10;

    return 0;
}
