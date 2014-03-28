##===- lib/Target/xpic/TargetInfo/Makefile ----------------*- Makefile -*-===##
#
#                     The LLVM Compiler Infrastructure
#
# This file is distributed under the University of Illinois Open Source
# License. See LICENSE.TXT for details.
#
##===----------------------------------------------------------------------===##
LEVEL = ../../../..
LIBRARYNAME = LLVMXpicInfo

# Hack: we need to include 'main' target directory to grab private headers
CPPFLAGS = -I$(PROJ_OBJ_DIR)/.. -I$(PROJ_SRC_DIR)/..

# BI: Build number must be set by automated build system
ifdef XPIC_BUILD
  CPPFLAGS += -DXPIC_BUILD=\"$(XPIC_BUILD)\"
endif

include $(LEVEL)/Makefile.common
