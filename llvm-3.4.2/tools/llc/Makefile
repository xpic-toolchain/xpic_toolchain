#===- tools/llc/Makefile -----------------------------------*- Makefile -*-===##
# 
#                     The LLVM Compiler Infrastructure
#
# This file is distributed under the University of Illinois Open Source
# License. See LICENSE.TXT for details.
# 
##===----------------------------------------------------------------------===##

LEVEL := ../..
TOOLNAME := llc
LINK_COMPONENTS := all-targets bitreader asmparser irreader

include $(LEVEL)/Makefile.common

GIT_RETURN := $(shell git rev-parse HEAD 1>&2 2> /dev/null; echo $$?)
ifeq ($(GIT_RETURN),0)
    GIT_HASH := $(shell git rev-parse HEAD)
    CFLAGS := $(CFLAGS) -DGIT_HASH=$(GIT_HASH)
    CPPFLAGS := $(CPPFLAGS) -DGIT_HASH=$(GIT_HASH)
endif
