//===-- xpicTargetAsmInfo.cpp - xpic asm properties -----------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the declarations of the xpicTargetAsmInfo properties.
//
//===----------------------------------------------------------------------===//

#include "xpicMCAsmInfo.h"
#include "llvm/ADT/SmallVector.h"

using namespace llvm;

xpicELFMCAsmInfo::xpicELFMCAsmInfo(const Target &T,const StringRef &TT)
{
  CommentString = "//";

// xpic:
  InlineAsmStart ="begin inline asm";
  InlineAsmEnd = "end inline asm";
  GlobalDirective = "\t.global\t";

  PrivateGlobalPrefix = "";

/// Sections
  HasLEB128 = true;
  AbsoluteDebugSectionOffsets = true;
  SupportsDebugInformation = true;

  UsesELFSectionDirectiveForBSS= true;

/* BI: Disabling this directive expands a ".quad" into two ".long" directives
   We do this because GNU binutils expression parser is erroneous for some
   negative 64bit values (e.g. 0xffff7fffffffffff)
   TODO: This should rather be fixed in binutils! */
  Data64bitsDirective = 0;
}
