//=====-- xpicTargetAsmInfo.h - xpic asm properties ---------*- C++ -*--====//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the declaration of the xpicTargetAsmInfo class.
//
//===----------------------------------------------------------------------===//

#ifndef XPICTARGETASMINFO_H
#define XPICTARGETASMINFO_H

#include "llvm/MC/MCAsmInfoELF.h"

namespace llvm {
  class StringRef;

  class xpicELFMCAsmInfo : public MCAsmInfoELF {
  	 virtual void anchor();
  public:
    explicit xpicELFMCAsmInfo(StringRef TT);
 };
} // namespace llvm
#endif
