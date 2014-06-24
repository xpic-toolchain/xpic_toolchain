//===- xpicCallingConv.td - Calling Conventions xpic -----*- tablegen -*-===//
// 
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
// 
//===----------------------------------------------------------------------===//
//
// This describes the calling conventions for the xpic architectures.
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// Return Value Calling Conventions
//===----------------------------------------------------------------------===//

// xpic 32-bit C return-value convention.
def RetCC_xpic32 : CallingConv<[
  CCIfType<[i32], CCAssignToReg<[r2, r3]>>
]>;


// xpic 32-bit C Calling convention.
def CC_xpic32 : CallingConv<[

  CCIfType<[i8, i16], CCPromoteToType<i32>>,

  // All arguments get passed in integer registers if there is space.
  CCIfType<[i32, f32, f64], CCAssignToReg<[r2, r3, r4, r5]>>,
  
  // Alternatively, they are assigned to the stack in 4-byte aligned units.
  CCAssignToStack<4, 4>
]>;
