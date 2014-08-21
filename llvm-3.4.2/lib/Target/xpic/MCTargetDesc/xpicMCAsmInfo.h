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

#include "llvm/MC/MCAsmInfo.h"

namespace llvm {
  // Forward declaration.
  //class Target;
  class StringRef;

  struct xpicELFMCAsmInfo : public MCAsmInfo {
  	 virtual void anchor();
  public:
    explicit xpicELFMCAsmInfo(StringRef TT);
 };
} // namespace llvm
#endif
