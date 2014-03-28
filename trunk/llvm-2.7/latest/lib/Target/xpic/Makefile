##===- lib/Target/xpic/Makefile ---------------------------*- Makefile -*-===##
# 
#                     The LLVM Compiler Infrastructure
#
# This file is distributed under the University of Illinois Open Source
# License. See LICENSE.TXT for details.
# 
##===----------------------------------------------------------------------===##
LEVEL = ../../..
LIBRARYNAME = LLVMXpicCodeGen
TARGET = xpic

# Make sure that tblgen is run, first thing.
BUILT_SOURCES = xpicGenRegisterInfo.h.inc xpicGenRegisterNames.inc \
                xpicGenRegisterInfo.inc xpicGenInstrNames.inc \
                xpicGenInstrInfo.inc xpicGenAsmWriter.inc \
                xpicGenDAGISel.inc xpicGenSubtarget.inc xpicGenCallingConv.inc

DIRS = AsmPrinter TargetInfo Helper

include $(LEVEL)/Makefile.common

