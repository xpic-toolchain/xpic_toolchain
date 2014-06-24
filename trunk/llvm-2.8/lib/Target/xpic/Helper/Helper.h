// Helper/Helper.h
#include "llvm/Target/TargetLowering.h"
#include "llvm/CodeGen/SelectionDAG.h"

#ifndef XPIC_HELPER_H
#define XPIC_HELPER_H

using namespace llvm;

// helper for Debug: shows opcode and operands
void  SDValueInfo(const char *Name,SDValue Op,SelectionDAG *CurDAG);

// Create a string form an unsigned integer with pattern "Const.hXXXXXXXX"
GlobalVariable *xpicHlprGetGlobalVariable(const Module *M, unsigned int val);

#endif
