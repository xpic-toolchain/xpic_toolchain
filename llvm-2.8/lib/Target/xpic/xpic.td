//===- xpic.td - Describe the xpic Target Machine -------------*- C++ -*-===//
// 
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
// 
//===----------------------------------------------------------------------===//
//
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// Target-independent interfaces which we are implementing
//===----------------------------------------------------------------------===//

//include "../Target.td"
include "llvm/Target/Target.td"
//===----------------------------------------------------------------------===//
// xpic Subtarget features.
//

def FeatureIsXPIC
  : SubtargetFeature<"xpic", "IsXPIC", "true",
                     "Enable xpic instructions">;

//===----------------------------------------------------------------------===//
// Register File, Calling Conv, Instruction Descriptions
//===----------------------------------------------------------------------===//

include "xpicRegisterInfo.td"
include "xpicCallingConv.td"
include "xpicInstrInfo.td"

def xpicInstrInfo : InstrInfo {
  // Define how we want to layout our target-specific information field.
}

//===----------------------------------------------------------------------===//
// xpic processors supported.
//===----------------------------------------------------------------------===//

class Proc<string Name, list<SubtargetFeature> Features>
 : Processor<Name, NoItineraries, Features>;

def : Proc<"generic",         []>;

//===----------------------------------------------------------------------===//
// Declare the target which we are implementing
//===----------------------------------------------------------------------===//

def xpic : Target {
  // Pull in Instruction Info:
  let InstructionSet = xpicInstrInfo;
}
