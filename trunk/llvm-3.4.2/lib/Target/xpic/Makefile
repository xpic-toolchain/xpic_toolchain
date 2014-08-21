##===- lib/Target/xpic/Makefile ---------------------------*- Makefile -*-===##
#
#                     The LLVM Compiler Infrastructure
#
# This file is distributed under the University of Illinois Open Source
# License. See LICENSE.TXT for details.
#
##===----------------------------------------------------------------------===##
LEVEL = ../../..
LIBRARYNAME = LLVMxpicCodeGen
TARGET = xpic

# Make sure that tblgen is run, first thing.
BUILT_SOURCES = xpicGenRegisterInfo.inc xpicGenInstrInfo.inc \
				xpicGenAsmWriter.inc \
				xpicGenDAGISel.inc xpicGenCallingConv.inc \
				xpicGenSubtargetInfo.inc

DIRS = TargetInfo MCTargetDesc Helper

include $(LEVEL)/Makefile.common

