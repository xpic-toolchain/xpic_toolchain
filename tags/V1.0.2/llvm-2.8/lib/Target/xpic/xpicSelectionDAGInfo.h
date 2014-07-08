//===-- SparcSelectionDAGInfo.h - Sparc SelectionDAG Info -------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the Sparc subclass for TargetSelectionDAGInfo.
//
//===----------------------------------------------------------------------===//

#ifndef XPICSELECTIONDAGINFO_H
#define XPICSELECTIONDAGINFO_H

#include "llvm/Target/TargetSelectionDAGInfo.h"

namespace llvm {

class xpicTargetMachine;

class xpicSelectionDAGInfo : public TargetSelectionDAGInfo {
public:
  explicit xpicSelectionDAGInfo(const xpicTargetMachine &TM);
  ~xpicSelectionDAGInfo();
};

}

#endif
