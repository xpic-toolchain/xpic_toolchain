//===-- XpicTargetInfo.cpp - Xpic Target Implementation -----------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "xpic.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/TargetRegistry.h"
//#include "xpicCompilerVersion.h"

using namespace llvm;

Target llvm::TheXpicTarget;

extern "C" void LLVMInitializexpicTargetInfo() { 
  RegisterTarget<Triple::xpic> X(TheXpicTarget,"xpic", "processor xPIC, netX-Chip (Hilscher GmbH: www.hilscher.com)");

}
