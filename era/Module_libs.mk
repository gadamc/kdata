# -*- mode: makefile -*-
#
# Makefile containing platform dependencies for ROOT based projects.
#
# Copyright (c) 2000 Rene Brun and Fons Rademakers
#
# Author: Fons Rademakers, 29/2/2000
# modified Adam Cox 24/8/2010 for the ERA module in KData

ROOTCONFIG   := root-config

#CXX           =
ObjSuf        = o
SrcSuf        = cxx
ExeSuf        = 
DllSuf        = so
OutPutOpt     = -o # keep whitespace after "-o"

ROOTCFLAGS   := $(shell $(ROOTCONFIG) --cflags)
ROOTLDFLAGS  := $(shell $(ROOTCONFIG) --ldflags)

ifeq ($(ARCH),aix)
# IBM AIX xlC 4.x
DllSuf        = a
endif

ifeq ($(ARCH),aix5)
# IBM AIX xlC 5.x
DllSuf        = a
endif

ifeq ($(ARCH),aixgcc)
# IBM AIX with GCC
DllSuf        = a
endif

ifeq ($(ARCH),macosx)
# MacOS X with cc (GNU cc 2.95.2 and gcc 3.3)
MACOSX_MINOR := $(shell sw_vers | sed -n 's/ProductVersion://p' | cut -d . -f 2)
ifeq ($(MACOSX_DEPLOYMENT_TARGET),)
MACOSXTARGET := MACOSX_DEPLOYMENT_TARGET=10.$(MACOSX_MINOR)
else
MACOSXTARGET := MACOSX_DEPLOYMENT_TARGET=$(MACOSX_DEPLOYMENT_TARGET)
endif

# The SOFLAGS will be used to create the .dylib,
# the .so will be created separately
ifeq ($(subst $(MACOSX_MINOR),,1234),1234)
DllSuf        = so
else
DllSuf        = dylib
endif
endif

ifeq ($(ARCH),macosxicc)
# MacOS X with Intel icc compiler
MACOSX_MINOR := $(shell sw_vers | sed -n 's/ProductVersion://p' | cut -d . -f 2)
ifeq ($(MACOSX_DEPLOYMENT_TARGET),)
MACOSXTARGET := MACOSX_DEPLOYMENT_TARGET=10.$(MACOSX_MINOR)
else
MACOSXTARGET := MACOSX_DEPLOYMENT_TARGET=$(MACOSX_DEPLOYMENT_TARGET)
endif
ifeq ($(MACOSX_MINOR),5)
MACOSX_MINOR := 4
endif
# The SOFLAGS will be used to create the .dylib,
# the .so will be created separately
ifeq ($(subst $(MACOSX_MINOR),,1234),1234)
DllSuf        = so
else
DllSuf        = dylib
endif
endif

ifeq ($(ARCH),macosx64)
# MacOS X >= 10.4 with gcc 64 bit mode (GNU gcc 4.*)
# Only specific option (-m64) comes from root-config
MACOSX_MINOR := $(shell sw_vers | sed -n 's/ProductVersion://p' | cut -d . -f 2)
ifeq ($(MACOSX_DEPLOYMENT_TARGET),)
MACOSXTARGET := MACOSX_DEPLOYMENT_TARGET=10.$(MACOSX_MINOR)
else
MACOSXTARGET := MACOSX_DEPLOYMENT_TARGET=$(MACOSX_DEPLOYMENT_TARGET)
endif
# The SOFLAGS will be used to create the .dylib,
# the .so will be created separately
ifeq ($(subst $(MACOSX_MINOR),,1234),1234)
DllSuf        = so
else
DllSuf        = dylib
endif
endif

ifeq ($(ARCH),macosxxlc)
# MacOS X with IBM xlC compiler
MACOSX_MINOR := $(shell sw_vers | sed -n 's/ProductVersion://p' | cut -d . -f 2)
ifeq ($(MACOSX_DEPLOYMENT_TARGET),)
MACOSXTARGET := MACOSX_DEPLOYMENT_TARGET=10.$(MACOSX_MINOR)
else
MACOSXTARGET := MACOSX_DEPLOYMENT_TARGET=$(MACOSX_DEPLOYMENT_TARGET)
endif
# The SOFLAGS will be used to create the .dylib,
# the .so will be created separately
DllSuf        = dylib
endif

ifeq ($(ARCH),win32)
# Windows with the VC++ compiler
VC_MAJOR     := $(shell unset VS_UNICODE_OUTPUT; cl.exe 2>&1 | awk '{ if (NR==1) print $$(NF-2) }' | \
                cut -d'.' -f1)
ObjSuf        = obj
SrcSuf        = cxx
ExeSuf        = .exe
DllSuf        = dll
OutPutOpt     = -out:
endif

ifeq ($(ARCH),win32gcc)
# Windows with gcc
DllSuf        = dll
ExeSuf        = .exe
endif
