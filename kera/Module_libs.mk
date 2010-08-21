# -*- mode: makefile -*-
#
# Makefile containing platform dependencies for ROOT based projects.
#
# Copyright (c) 2000 Rene Brun and Fons Rademakers
#
# Author: Fons Rademakers, 29/2/2000

ROOTCONFIG   := root-config

ARCH         := $(shell $(ROOTCONFIG) --arch)
PLATFORM     := $(shell $(ROOTCONFIG) --platform)
ALTCC        := $(shell $(ROOTCONFIG) --cc)
ALTCXX       := $(shell $(ROOTCONFIG) --cxx)
ALTF77       := $(shell $(ROOTCONFIG) --f77)
ALTLD        := $(shell $(ROOTCONFIG) --ld)

#CXX           =
ObjSuf        = o
SrcSuf        = cxx
ExeSuf        = 
DllSuf        = so
OutPutOpt     = -o # keep whitespace after "-o"

ifeq (debug,$(findstring debug,$(ROOTBUILD)))
OPT           = -g
OPT2          = -g
else
ifneq ($(findstring debug, $(strip $(shell $(ROOTCONFIG) --config))),)
OPT           = -g
OPT2          = -g
else
OPT           = -O
OPT2          = -O2
endif
endif

ROOTCFLAGS   := $(shell $(ROOTCONFIG) --cflags)
ROOTLDFLAGS  := $(shell $(ROOTCONFIG) --ldflags)
ROOTLIBS     := $(shell $(ROOTCONFIG) --libs)
ROOTGLIBS    := $(shell $(ROOTCONFIG) --glibs)
HASTHREAD    := $(shell $(ROOTCONFIG) --has-thread)
ROOTDICTTYPE := $(shell $(ROOTCONFIG) --dicttype)
#NOSTUBS      := $(shell $(ROOTCONFIG) --nostubs)
ROOTCINT     := rootcint

# Stub Functions Generation
ifeq ($(NOSTUBS),yes)
   ROOTCINT = export CXXFLAGS="$(CXXFLAGS)"; $(ROOTSYS)/core/utils/src/rootcint_nostubs.sh -$(ROOTDICTTYPE)
endif

ifeq ($(ARCH),hpuxacc)
# HP-UX 10.x with aCC
CXX           = aCC
CXXFLAGS      = $(OPT) +Z
LD            = aCC
LDFLAGS       = $(OPT) -z
SOFLAGS       = -b
endif

ifeq ($(ARCH),hpuxia64acc)
# HP-UX 11i 1.5 (IA-64) with aCC
CXX           = aCC
CXXFLAGS      = +DD64 $(OPT) +Z
LD            = aCC
LDFLAGS       = +DD64 $(OPT) -z
SOFLAGS       = -b
endif

ifeq ($(ARCH),hpuxgcc)
# HP-UX 10.x with g++
CXXFLAGS      = $(OPT) -fPIC
CXX           = g++
LD            = g++
LDFLAGS       = $(OPT)
SOFLAGS       = -fPIC -shared
endif

ifeq ($(ARCH),hurddeb)
# GNU/Hurd
CXX           = g++
CXXFLAGS      = $(OPT2) -Wall -fPIC
LD            = g++
LDFLAGS       = $(OPT2)
SOFLAGS       = -shared
endif

ifeq ($(ARCH),aix)
# IBM AIX xlC 4.x
CXX           = xlC
CXXFLAGS      = $(OPT)
LD            = xlC
LDFLAGS       = $(OPT)
SOFLAGS       =
DllSuf        = a
endif

ifeq ($(ARCH),aix5)
# IBM AIX xlC 5.x
CXX           = xlC
CXXFLAGS      = $(OPT)
LD            = xlC
LDFLAGS       = $(OPT)
SOFLAGS       =
DllSuf        = a
endif

ifeq ($(ARCH),aixgcc)
# IBM AIX with GCC
CXX           = g++
CXXFLAGS      = $(OPT)
LD            = g++
LDFLAGS       = $(OPT)
SOFLAGS       = -shared
DllSuf        = a
EXPLLINKLIBS  = $(ROOTLIBS) $(ROOTGLIBS)
endif

ifeq ($(ARCH),solaris)
# Solaris CC
CXX           = /opt/SUNWspro/bin/CC
CXXFLAGS      = $(OPT) -KPIC
LD            = /opt/SUNWspro/bin/CC
LDFLAGS       = $(OPT)
SOFLAGS       = -G
endif

ifeq ($(ARCH),solarisCC5)
# Solaris CC 5.0
CXX           = CC
CXXFLAGS      = $(OPT) -KPIC
LD            = CC
LDFLAGS       = $(OPT)
SOFLAGS       = -G
endif

ifeq ($(ARCH),solaris64CC5)
# Solaris CC 5.0 64-bit
CXX           = CC
CXXFLAGS      = $(OPT) -KPIC
LD            = CC
LDFLAGS       = $(OPT)
SOFLAGS       = -G
endif

ifeq ($(ARCH),solarisgcc)
# Solaris gcc
CXX           = g++
CXXFLAGS      = $(OPT) -fPIC
LD            = g++
LDFLAGS       = $(OPT)
SOFLAGS       = -shared
endif

ifeq ($(ARCH),sgicc)
# SGI
CXX           = CC -n32  -I/usr/include/CC.sgi
CXXFLAGS      = $(OPT)
LD            = CC -n32 -LANG:std  -I/usr/include/CC.sgi
LDFLAGS       = $(OPT)
SOFLAGS       = -shared
endif

ifeq ($(ARCH),sgicc64)
# SGI
CXX           = CC -64  -I/usr/include/CC.sgi
CXXFLAGS      = $(OPT)
LD            = CC -64 -LANG:std -I/usr/include/CC.sgi
LDFLAGS       = $(OPT)
SOFLAGS       = -shared
endif

ifeq ($(ARCH),sgigcc)
# SGI 6.x with gcc
CXX           = g++
CXXFLAGS      = $(OPT) -Wall -fPIC
LD            = g++
LDFLAGS       = $(OPT) -Wl,-u,__builtin_new -Wl,-u,__builtin_delete -Wl,-u,__nw__FUiPv
SOFLAGS       = -shared
endif

ifeq ($(ARCH),sgin32gcc)
# SGI 6.x with gcc for n32 ABI
CXX           = g++
CXXFLAGS      = $(OPT) -Wall -fPIC
LD            = g++
LDFLAGS       = $(OPT) -L/usr/lib32 -Wl,-woff,134 -lgen
SOFLAGS       = -shared
endif

ifeq ($(ARCH),alphagcc)
# Alpha/OSF with gcc
CXX           = g++
CXXFLAGS      = $(OPT2) -Wall -fPIC
LD            = g++
LDFLAGS       = $(OPT2)
SOFLAGS       = -Wl,-expect_unresolved,* -shared
endif

ifeq ($(ARCH),alphacxx6)
# Alpha/OSF with cxx6
CXX           = cxx
CXXFLAGS      = $(OPT)
LD            = cxx
LDFLAGS       = $(OPT)
SOFLAGS       = -shared -nocxxstd -Wl,-expect_unresolved,*,-msym
endif

ifeq ($(ARCH),linux)
# Linux with egcs, gcc 2.9x, gcc 3.x
CXX           = g++
CXXFLAGS      = $(OPT2) -Wall -fPIC -I/usr/include/
LD            = g++
LDFLAGS       = $(OPT2)
SOFLAGS       = -shared
endif

ifeq ($(ARCH),linuxicc)
# Linux with Intel icc compiler
ICC_MAJOR    := $(shell icc -v 2>&1 | awk '{ if (NR==1) print $$2 }' | \
                cut -d'.' -f1)
ICC_MINOR    := $(shell icc -v 2>&1 | awk '{ if (NR==1) print $$2 }' | \
                cut -d'.' -f2)
CXX           = icc
CXXFLAGS      = $(OPT) -fPIC -wd1476
LD            = icpc
LDFLAGS       = $(OPT)
SOFLAGS       = -shared
endif

ifeq ($(ARCH),linuxppcgcc)
# PPC Linux with gcc and glibc
CXX           = g++
CXXFLAGS      = $(OPT2) -Wall -fPIC
LD            = g++
LDFLAGS       = $(OPT2)
SOFLAGS       = -shared
endif

ifeq ($(ARCH),linuxia64gcc)
# Itanium Linux with gcc 2.9x
CXX           = g++
CXXFLAGS      = $(OPT2) -Wall -fPIC
LD            = g++
LDFLAGS       = $(OPT2)
SOFLAGS       = -shared
endif

ifeq ($(ARCH),linuxia64ecc)
# Itanium Linux with Intel icc (was ecc)
ICC_MAJOR    := $(shell icc -v 2>&1 | awk '{ if (NR==1) print $$2 }' | \
                cut -d'.' -f1)
ICC_MINOR    := $(shell icc -v 2>&1 | awk '{ if (NR==1) print $$2 }' | \
                cut -d'.' -f2)
CXX           = icc
CXXFLAGS      = $(OPT) -fPIC -wd1476 -ftz
LD            = icpc
LDFLAGS       = $(OPT)
SOFLAGS       = -shared
endif

ifeq ($(ARCH),linuxx8664gcc)
# AMD Opteron and Intel EM64T (64 bit mode) Linux with gcc 3.x
CXX           = g++
CXXFLAGS      = $(OPT2) -Wall -fPIC -I/usr/include
LD            = g++
LDFLAGS       = $(OPT2)
SOFLAGS       = -shared
endif

ifeq ($(ARCH),linuxppc64gcc)
# PPC64 Linux with gcc 3.x
CXX           = g++
CXXFLAGS      = $(OPT) -Wall -fPIC
LD            = g++
LDFLAGS       = $(OPT)
SOFLAGS       = -shared
endif

ifeq ($(ARCH),linuxx8664icc)
# AMD Opteron and Intel EM64T (64 bit mode) Linux with Intel icc compiler
CXX           = icc
CXXFLAGS      = $(OPT) -fPIC -wd1476 -wd1572
LD            = icpc
LDFLAGS       = $(OPT)
SOFLAGS       = -shared
endif

ifeq ($(ARCH),linuxalphagcc)
# Alpha Linux with gcc
CXX           = g++
CXXFLAGS      = $(OPT2) -Wall -fPIC
LD            = g++
LDFLAGS       = $(OPT2)
SOFLAGS       = -shared
endif

ifeq ($(ARCH),linuxmips)
# GNU/Linux on mips (BE/LE, O32/N32/N64) with gcc
CXX           = g++
CXXFLAGS      = $(OPT2) -Wall -fPIC
LD            = g++
LDFLAGS       = $(OPT2)
SOFLAGS       = -shared
endif

ifeq ($(ARCH),linuxhppa)
# GNU/Linux on hppa with gcc
CXX           = g++
CXXFLAGS      = $(OPT2) -Wall -fPIC
LD            = g++
LDFLAGS       = $(OPT2)
SOFLAGS       = -shared
endif

ifeq ($(ARCH),linuxarm)
# ARM Linux with egcs
CXX           = g++
CXXFLAGS      = $(OPT) -Wall -fPIC
LD            = g++
LDFLAGS       = $(OPT)
SOFLAGS       = -shared
endif

ifeq ($(ARCH),freebsd4)
# FreeBSD with glibc
CXX           = g++
CXXFLAGS      = $(OPT) -W -Wall -fPIC
LD            = $(CXX)
LDFLAGS       = $(OPT)
SOFLAGS       = -shared -Wl,-x
endif

ifeq ($(ARCH),freebsd5)
# FreeBSD with glibc
CXX           = g++
CXXFLAGS      = $(OPT) -W -Wall -fPIC
LD            = $(CXX)
LDFLAGS       = $(OPT)
SOFLAGS       = -shared -Wl,-x
endif

ifeq ($(ARCH),freebsd7)
# FreeBSD with libc
CXX           = g++
CXXFLAGS      = $(OPT) -W -Wall -fPIC
LD            = $(CXX)
LDFLAGS       = $(OPT)
SOFLAGS       = -shared -Wl,-x
endif

ifeq ($(ARCH),openbsd)
# OpenBSD with libc
CXX           = g++
CXXFLAGS      = $(OPT) -pipe -W -Wall -fPIC
LD            = g++
LDFLAGS       = $(OPT)
SOFLAGS       = -shared -Wl,-x
endif

ifeq ($(ARCH),macosx)
# MacOS X with cc (GNU cc 2.95.2 and gcc 3.3)
MACOSX_MINOR := $(shell sw_vers | sed -n 's/ProductVersion://p' | cut -d . -f 2)
ifeq ($(MACOSX_DEPLOYMENT_TARGET),)
MACOSXTARGET := MACOSX_DEPLOYMENT_TARGET=10.$(MACOSX_MINOR)
else
MACOSXTARGET := MACOSX_DEPLOYMENT_TARGET=$(MACOSX_DEPLOYMENT_TARGET)
endif
CXX           = g++
CXXFLAGS      = $(OPT2) -pipe -Wall -W -Woverloaded-virtual
LD            = $(MACOSXTARGET) g++
LDFLAGS       = $(OPT2)
# The SOFLAGS will be used to create the .dylib,
# the .so will be created separately
ifeq ($(subst $(MACOSX_MINOR),,1234),1234)
DllSuf        = so
else
DllSuf        = dylib
endif
UNDEFOPT      = dynamic_lookup
ifneq ($(subst $(MACOSX_MINOR),,12),12)
UNDEFOPT      = suppress
LD            = g++
endif
SOFLAGS       = -dynamiclib -single_module -undefined $(UNDEFOPT) -install_name $(CURDIR)/
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
CXX           = icc
CXXFLAGS      = $(OPT) -fPIC -wd1476
LD            = $(MACOSXTARGET) icpc
LDFLAGS       = $(OPT)
# The SOFLAGS will be used to create the .dylib,
# the .so will be created separately
ifeq ($(subst $(MACOSX_MINOR),,1234),1234)
DllSuf        = so
else
DllSuf        = dylib
endif
SOFLAGS       = -dynamiclib -single_module -undefined dynamic_lookup -install_name $(CURDIR)/
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
CXX           = g++
CXXFLAGS      = $(OPT2) -pipe -Wall -W -Woverloaded-virtual
LD            = $(MACOSXTARGET) g++
LDFLAGS       = $(OPT2)
# The SOFLAGS will be used to create the .dylib,
# the .so will be created separately
ifeq ($(subst $(MACOSX_MINOR),,1234),1234)
DllSuf        = so
else
DllSuf        = dylib
endif
SOFLAGS       = -dynamiclib -single_module -undefined dynamic_lookup -install_name $(CURDIR)/
endif

ifeq ($(ARCH),macosxxlc)
# MacOS X with IBM xlC compiler
MACOSX_MINOR := $(shell sw_vers | sed -n 's/ProductVersion://p' | cut -d . -f 2)
ifeq ($(MACOSX_DEPLOYMENT_TARGET),)
MACOSXTARGET := MACOSX_DEPLOYMENT_TARGET=10.$(MACOSX_MINOR)
else
MACOSXTARGET := MACOSX_DEPLOYMENT_TARGET=$(MACOSX_DEPLOYMENT_TARGET)
endif
CXX           = xlC
CXXFLAGS      = $(OPT)
LD            = $(MACOSXTARGET) xlC
LDFLAGS       = $(OPT) -Wl,-bind_at_load
# The SOFLAGS will be used to create the .dylib,
# the .so will be created separately
DllSuf        = dylib
UNDEFOPT      = dynamic_lookup
ifneq ($(subst $(MACOSX_MINOR),,12),12)
UNDEFOPT      = suppress
LD            = xlC
endif
SOFLAGS       = -qmkshrobj -single_module -undefined $(UNDEFOPT) -install_name $(CURDIR)/
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
CXX           = cl
ifeq (debug,$(findstring debug,$(ROOTBUILD)))
CXXOPT        = -Z7
LDOPT         = -debug
else
ifneq ($(findstring debug, $(strip $(shell $(ROOTCONFIG) --config))),)
CXXOPT        = -Z7
LDOPT         = -debug
else
CXXOPT        = -O2
LDOPT         = -opt:ref
endif
endif
ROOTINCDIR   := -I$(shell cygpath -m `$(ROOTCONFIG) --incdir`)
CXXFLAGS      = $(CXXOPT) -nologo $(ROOTINCDIR) -FIw32pragma.h
LD            = link
LDFLAGS       = $(LDOPT) -nologo
SOFLAGS       = -DLL

EXPLLINKLIBS  = $(ROOTLIBS) $(ROOTGLIBS)
ifneq (,$(findstring $(VC_MAJOR),14 15))
MT_EXE        = mt -nologo -manifest $@.manifest -outputresource:$@\;1; rm -f $@.manifest
MT_DLL        = mt -nologo -manifest $@.manifest -outputresource:$@\;2; rm -f $@.manifest
else
MT_EXE        =
MT_DLL        =
endif
endif

ifeq ($(ARCH),win32gcc)
# Windows with gcc
DllSuf        = dll
ExeSuf        = .exe
CXX           = g++
CXXFLAGS      = $(OPT) -pipe -Wall -Woverloaded-virtual -I/usr/X11R6/include
LD            = g++
LDFLAGS       = $(OPT) -Wl,--enable-auto-import \
                -Wl,--enable-runtime-pseudo-reloc \
		-L/usr/X11R6/lib
SOFLAGS       = -shared -Wl,--enable-auto-image-base \
                -Wl,--export-all-symbols
EXPLLINKLIBS  = $(ROOTLIBS) $(ROOTGLIBS)
endif

ifeq ($(CXX),)
$(error $(ARCH) invalid architecture)
endif

CXXFLAGS     += $(ROOTCFLAGS)
LDFLAGS      += $(ROOTLDFLAGS)
LIBS          = $(ROOTLIBS) $(SYSLIBS)
GLIBS         = $(ROOTGLIBS) $(SYSLIBS)

ifneq ($(ALTCC),)
   CC  = $(ALTCC)
endif
ifneq ($(ALTCXX),)
   CXX = $(ALTCXX)
endif
ifneq ($(ALTF77),)
   F77 = $(ALTF77)
endif
ifneq ($(ALTLD),)
   LD  = $(ALTLD)
endif

ifneq ($(findstring g++, $(CXX)),)
GCC_MAJOR := $(shell $(CXX) -dumpversion 2>&1 | cut -d'.' -f1)
GCC_MINOR := $(shell $(CXX) -dumpversion 2>&1 | cut -d'.' -f2)
endif
