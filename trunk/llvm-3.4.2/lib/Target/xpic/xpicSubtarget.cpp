//===- xpicSubtarget.cpp - xpic Subtarget Information -------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the xpic specific subclass of TargetSubtarget.
//
//===----------------------------------------------------------------------===//

#include "xpicSubtarget.h"
#include "xpic.h"
#include "llvm/Support/MathExtras.h"
#include "llvm/Support/TargetRegistry.h"

#define GET_SUBTARGETINFO_TARGET_DESC
#define GET_SUBTARGETINFO_CTOR
#include "xpicGenSubtargetInfo.inc"

using namespace llvm;

void xpicSubtarget::anchor() { }

xpicSubtarget::xpicSubtarget(const std::string &TT, const std::string &CPU, const std::string &FS) :
  xpicGenSubtargetInfo(TT, CPU, FS){
  // Set the default features.
  IsXPIC = true;

  // Determine default and user specified characteristics
  std::string CPUName = CPU;

  // Parse features string.
  ParseSubtargetFeatures(CPUName, FS);

}
