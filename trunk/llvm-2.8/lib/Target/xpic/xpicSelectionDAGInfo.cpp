//===-- xpicSelectionDAGInfo.cpp - xpic SelectionDAG Info ---------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the xpicSelectionDAGInfo class.
//
//===----------------------------------------------------------------------===//

#define DEBUG_TYPE "xpic-selectiondag-info"
#include "xpicTargetMachine.h"
using namespace llvm;

xpicSelectionDAGInfo::xpicSelectionDAGInfo(const xpicTargetMachine &TM)
  : TargetSelectionDAGInfo(TM) {
}

xpicSelectionDAGInfo::~xpicSelectionDAGInfo() {
}
