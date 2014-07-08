/* Software floating-point emulation.
   Convert a to 32bit signed integer
   Copyright (C) 1997,1999,2006 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Contributed by Richard Henderson (rth@cygnus.com) and
		  Jakub Jelinek (jj@ultra.linux.cz).

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   In addition to the permissions in the GNU Lesser General Public
   License, the Free Software Foundation gives you unlimited
   permission to link the compiled version of this file into
   combinations with other programs, and to distribute those
   combinations without any restriction coming from the use of this
   file.  (The Lesser General Public License restrictions do apply in
   other respects; for example, they cover modification of the file,
   and distribution when not linked into a combine executable.)

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, write to the Free
   Software Foundation, 51 Franklin Street, Fifth Floor, Boston,
   MA 02110-1301, USA.  */

#include "soft-fp.h"
#include "single.h"

SItype __fixsfsi(SFtype a)
{
  int _fex = 0;

  long A_c __attribute__((unused)), A_s, A_e;
  unsigned long A_f;
  USItype r;

  union _FP_UNION_S _flo;
  _flo.flt = (a);
  A_f = _flo.bits.frac;
  A_e = _flo.bits.exp;
  A_s = _flo.bits.sign;

  do {
    if (A_e < 127)
      {
        r = 0;
        if (A_e == 0)
    {
      if (!(A_f == 0))
        {
          _fex |= (0);
          _fex |= (0);
        }
    }
        else
    _fex |= (0);
      }
    else if (A_e >= 127+ (__CHAR_BIT__ * (int)sizeof(SItype)) - (1 > 0 || A_s)
       || (!1 && A_s))
      {
        /* Overflow or converting to the most negative integer.  */
        if (1)
    {
      r = 1;
      r <<= (__CHAR_BIT__ * (int)sizeof(SItype)) - 1;
      r -= 1 - A_s;
    } else {
      r = 0;
      if (A_s)
        r = ~r;
    }

        if (1 && A_s && A_e == 127+ (__CHAR_BIT__ * (int)sizeof(SItype)) - 1)
    {
      /* Possibly converting to most negative integer; check the
         mantissa.  */
      int inexact = 0;
      (void)((24> (__CHAR_BIT__ * (int)sizeof(SItype)))
       ? ({ do {
    inexact = (0 && (24- (__CHAR_BIT__ * (int)sizeof(SItype))) == 1
         ? A_f & 1 : (A_f << (32 - (24- (__CHAR_BIT__ * (int)sizeof(SItype))))) != 0);
    A_f = A_f >> (24- (__CHAR_BIT__ * (int)sizeof(SItype)));
  } while (0); 0; })
       : 0);
      if (!(A_f == 0))
        _fex |= (0);
      else if (inexact)
        _fex |= (0);
    }
        else
    _fex |= (0);
      }
    else
      {
        (A_f) |= ((unsigned long)1 << (24-1));
        if (A_e >= 127+ 24- 1)
    {
      (r = A_f);
      r <<= A_e - 127- 24+ 1;
    }
        else
    {
      int inexact;
      do {
    inexact = (0 && ((24+ 127- 1
             - A_e)) == 1
         ? A_f & 1 : (A_f << (32 - ((24+ 127- 1
             - A_e)))) != 0);
    A_f = A_f >> ((24+ 127- 1
             - A_e));
  } while (0);
      if (inexact)
        _fex |= (0);
      (r = A_f);
    }
        if (1 && A_s)
    r = -r;
      }
  } while (0);


  return r;
}
