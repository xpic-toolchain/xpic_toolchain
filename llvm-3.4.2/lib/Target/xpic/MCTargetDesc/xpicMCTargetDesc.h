//===-- xpicMCTargetDesc.h - xpic Target Descriptions ---------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file provides xpic specific target descriptions.
//
//===----------------------------------------------------------------------===//

#ifndef XPICMCTARGETDESC_H
#define XPICMCTARGETDESC_H

namespace llvm {
class Target;

extern Target TheXpicTarget;

} // End llvm namespace

// Defines symbolic names for xpic registers.  This defines a mapping from
// register name to register number.
//
#define GET_REGINFO_ENUM
#include "xpicGenRegisterInfo.inc"

// Defines symbolic names for the xpic instructions.
//
#define GET_INSTRINFO_ENUM
#include "xpicGenInstrInfo.inc"

#define GET_SUBTARGETINFO_ENUM
#include "xpicGenSubtargetInfo.inc"

#endif
