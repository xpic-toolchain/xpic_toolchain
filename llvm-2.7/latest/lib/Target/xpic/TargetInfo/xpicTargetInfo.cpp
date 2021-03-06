//===-- XpicTargetInfo.cpp - Xpic Target Implementation -----------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "xpic.h"
#include "llvm/Module.h"
#include "llvm/Target/TargetRegistry.h"
#include "xpicCompilerVersion.h"

using namespace llvm;

Target llvm::TheXpicTarget;

extern "C" void LLVMInitializexpicTargetInfo() { 
#ifdef SHOW_XPIC_BACKEND_VERSION
  RegisterTarget<Triple::xpic> X(TheXpicTarget,"xpic", "processor xPIC, NetX10-chip/ (Hilscher GmbH: www.hilscher.com)"\
"\n           backend version "\
XPIC_MAJOR"."\
XPIC_MINOR"."\
XPIC_RELEASE"."\
XPIC_BUILD
);
#else
  RegisterTarget<Triple::xpic> X(TheXpicTarget,"xpic", "processor xPIC, NetX10-chip/ (Hilscher GmbH: www.hilscher.com)");
#endif

}
