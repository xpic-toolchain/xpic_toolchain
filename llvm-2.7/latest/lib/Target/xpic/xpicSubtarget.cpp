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
#include "xpicGenSubtarget.inc"
using namespace llvm;

// command line support:
#include "llvm/Support/CommandLine.h"
/*
namespace {
  cl::opt<bool> EnableV9("enable-xpic-insts", cl::Hidden, cl::desc("Enable instructions in the xpic target"));
}
*/

xpicSubtarget::xpicSubtarget(const std::string &TT, const std::string &FS){
  // Set the default features.
  IsXPIC = true;

  // Determine default and user specified characteristics
  std::string CPU = "generic";

  // Parse features string.
  ParseSubtargetFeatures(FS, CPU);

}
