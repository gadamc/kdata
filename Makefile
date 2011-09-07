###############################################################################
# Makefile for KDATA System
#
# Author: Adam Cox, 08/19/10 (borrowed from ROOT and QSNO, then modified)
#
# This Makefile relies on a successful install and build of the ROOT system,
# and the correct definition of the following environment variables:
#
#   KDATA_ROOT      - KDATA release directory (usually `pwd`)
#   ROOTSYS        - root system directory
#   FFTW_DIR       - fftw libraries directory (if required)
#   
#   CURL_DIR       - libcurl directory (required for kdatabase)
#
#
# All of the above environment variables, as well as PATH and LD_LIBRARY_PATH
# are set via the "source config/setup_kdata.csh" command in the KDATA root
# directory.  You should add this command to your shell startup script.
# The setup_kdata.csh file is generated by the install_kdata script when KDATA is
# first installed. (There is also a setup_kdata.sh file available for those
# of you who run a sh-type shell.)
#
# Make targets:
#
#   all          - builds all KDATA libraries and executables (default)
#   libs         - builds all KDATA libraries
#   clean        - removes object, dependency, dictionary, core and temp files
#   clean-MODULE - cleans only the specified module
#   reallyclean  - clean + also removes bin, lib and include directories
#   distclean    - reallyclean + also removes install-generated files
#   showbuild    - show definitions for build
#
# Files included by this Makefile (in order):
#
#   ROOTSYS/config/Makefile.config - defines architecture (ARCH)
#   ROOTSYS/config/Makefile.ARCH   - archictecture-dependent definitions
#   config/Makefile.ARCH           - KData-specific architecture-dependent defs
#   MyConfig.mk                    - local system-specific definitions
#   Modules.mk                     - defines modules to build
#   MODULE/Module_libs.mk          - module library definitions
#   MODULE/Module.mk               - module-specific definitions
#   MODULE/MyConfig.mk             - system-specific module definitions
#   MyRules.mk                     - any additional system-specific rules
#   INCLUDEFILES                   - module-defined dependency files
#
# Note: You should never have to change this master Makefile.  Changes required
# for specific systems should be made in the MyConfig.mk and MyRules.mk files,
# and new modules should be added in the Modules.mk file.
#
# This scheme allows to easily develop your modules separate from the 
# distribution without having to copy all of the KDATA files.  Here are the
# steps required to set up a separate development environment:
#
#   1) Create a new directory for your development project and cd to this dir.
#   2) Create a Modules.mk file defining your development modules in this dir.
#   3) Make soft links to Makefile and config in $KDATA_ROOT.
#   4) Put each of your development modules in a separate directory, with a
#      Module.mk and Module_libs.mk file in each directory.
#   5) Type "make" (or gmake) in the $KDATA_ROOT directory to build your
#      modules.
#   6) Before running the new code, you will have to add the development
#      bin directory to the start of the PATH environment variable, and the
#      development lib directory to the start of LD_LIBRARY_PATH.
#
# The ktemplate module can be used as a starting point for developing new
# modules.  It contains a generalized Module.mk file and some example code
# to generate a simple library.
#
###############################################################################

##### Include list of modules to build #####
-include Modules.mk

# skip unnecessary stuff if this is a "clean" type target
CLEAN_MODULES := $(patsubst %,clean-%,$(MODULES))
ifeq ($(filter $(MAKECMDGOALS),clean reallyclean distclean $(CLEAN_MODULES)),)

##### the ROOT config file (specifies our architecture) #####

include $(ROOTSYS)/config/Makefile.config

##### include ROOT make file for this architecture #####

include $(ROOTSYS)/config/Makefile.$(ARCH)

##### default additions to basic ROOT system definitions #####


LPATH          := $(KDATA_ROOT)/lib
KDATALIBDIRS    := -L$(LPATH)
ERADIR       := ./era

# add KDATA_ROOT directories if not building modules from there
ifneq ($(shell pwd),$(KDATA_ROOT))
EXTRA_CXXFLAGS += -I$(KDATA_ROOT)/include 
endif

ROOTINCS       := -I$(ROOTSYS)/include
XMLIBS         := $(patsubst -lX11,-lXm -lXmu -lXt -lX11,$(XLIBS))
ROOTLIBS       := $(shell $(ROOTSYS)/bin/root-config $(ROOT_LINK_NEW) --glibs) -lMinuit -lPyROOT -lGeomPainter -lMatrix -lGeom
ROOTLIBS       += -Wl,-rpath,$(ROOTSYS)/lib

FFTWINCS       := -I$(FFTW_DIR)/../include -I$(FFTW_DIR)/../include/fftw -I$(FFTW_DIR)/../include/rfftw
FFTWLIBS       := -L$(FFTW_DIR) -lfftw3 -lm
FFTWLIBS       += -Wl,-rpath,$(FFTW_DIR)

#JANSSONINCS    := -I$(JANSSON_DIR)/../include 
#JANSSONLIBS    := -L$(JANSSON_DIR) -ljansson
#JANSSONLIBS    += -Wl,-rpath,$(JANSSON_DIR)

ifneq ($(strip $(CURL_DIR)),)
CURLINCS       := $(shell $(CURL_DIR)/curl-config --cflags)
CURLLIBS       := -L$(shell $(CURL_DIR)/curl-config --prefix)/lib $(shell $(CURL_DIR)/curl-config --libs)
CURLLIBS       += -Wl,-rpath,$(shell $(CURL_DIR)/curl-config --prefix)/lib
endif


#special paths to the local ERA libraries. Needed for modules that depend upon ERA. 
ERA_LIB := $(LPATH)/libEra.$(SOEXT)
ERALIBS	   := $(KDATALIBDIRS) $(patsubst $(LPATH)/lib%.$(SOEXT),-l%,$(ERA_LIB))
ERAINCS  :=  $(ERADIR)

ALLKDATALIBS += $(ERALIBS)

##### Utilities #####

ROOTCINT       := $(ROOTSYS)/bin/rootcint
AR             := ar -rl

##### f77 options #####

ifeq ($(F77LD),)
F77LD        := $(LD)
endif
ifeq ($(F77OPT),)
F77OPT       := $(OPT)
endif
ifeq ($(F77LDFLAGS),)
F77LDFLAGS   := $(LDFLAGS)
endif

##### utilities #####

MAKEDEP         = $(KDATA_ROOT)/scripts/depend.sh
MAKELIB         = $(ROOTSYS)/build/unix/makelib.sh $(MKLIBOPTIONS)
#MAKELIB         = $(ROOTSYS)/build/unix/makelib.sh 

# warn when overwriting headers if this is our first pass
# (enabling us to quickly spot duplicate header names)
ifeq ($(wildcard include/dummy.d),)
COPY_HEADER     = cp -i
else
COPY_HEADER     = cp -f
endif

MKDIR = mkdir -p

##### additional KDATA architecture-specific definitions #####

include config/Makefile.$(ARCH)

EXTRALIBS += -lutil

EXTRA_CFLAGS   += $(KDATA_FLAGS)
EXTRA_CXXFLAGS += $(KDATA_FLAGS)
ROOTCINTFLAGS  := -c $(EXTRA_CXXFLAGS)
ROOTLIBS       += $(EXTRALIBS)
SYSLIBS        += $(EXTRALIBS)
F77LIBS        += $(EXTRALIBS)

##### Allow local macros #####

-include MyConfig.mk           # allow local definitions

endif

##### RULES #####

RMKDEP          = bin/rmkdepend

.SUFFIXES: .cxx .d .C
.PRECIOUS: include/%.h include/%.py

##### TARGETS #####

.PHONY:         all libs prepare clean reallyclean distclean showbuild\
                $(patsubst %,all-%,$(MODULES)) \
                $(patsubst %,prepare-%,$(MODULES)) \
                $(patsubst %,clean-%,$(MODULES)) \
                $(patsubst %,reallyclean-%,$(MODULES)) \
                $(patsubst %,distclean-%,$(MODULES)) 

all:            kdataexecs 
	@echo Done making all

##### Module Makefiles #####

# include the Module_libs.mk and Module.mk files for each specified module
# (the libraries are included first so they will all be defined for use
#  in the link rules for each module)
-include $(patsubst %,%/Module_libs.mk,$(MODULES))
-include $(patsubst %,%/Module.mk,$(MODULES))
-include $(KDATA_ROOT)/programs/Module.mk

-include MyRules.mk            # allow local rules

# include dependency files from each module
# (do this silently because they won't exist the first time we compile)
# - don't include them when cleaning, because they will cause targets to be built
ifeq ($(filter $(MAKECMDGOALS),clean reallyclean distclean showbuild $(CLEAN_MODULES)),)
-include $(INCLUDEFILES)
-include include/dummy.d          # must be last include
endif

libs:           kdatalibs
	@echo Done making libs


kdatalibs:       $(ALLLIBS) 

# the executables depend on the libraries being built first
kdataexecs:      kdatalibs $(ALLEXECS)

# define generic rules last to allow modules to override them
# (the first matching rule is the one that is used)
%.o: %.cxx
	$(CXX) $(OPT) $(CXXFLAGS) $(ROOTINCS) -o $@ -c $<

%.o: %.c
	$(CC) $(OPT) $(CFLAGS) -o $@ -c $<


include/dummy.d: prepare $(RMKDEP) $(ALLHDRS)
	@(if [ ! -f $@ ] ; then \
	   touch $@; \
	fi)

#%.d: %.c
#	$(MAKEDEP) $@ "$(CFLAGS)" $< > $@

#%.d: %.cxx
#	$(MAKEDEP) $@ "$(CXXFLAGS)" $< > $@

#%.d: %.C
#	$(MAKEDEP) $@ "$(CXXFLAGS)" $< > $@

$(RMKDEP): $(ROOTSYS)/bin/rmkdepend
	@$(INSTALLDIR) bin
	@cp $(ROOTSYS)/bin/rmkdepend bin

# stuff we need to do in preparation for building KDATA
prepare::
	@$(INSTALLDIR) include
	@$(INSTALLDIR) lib
	@rsync -aq --exclude=.svn/ --exclude=.DS_Store KDataPy lib/.
	@chmod -R +x lib/KDataPy
	@config/checkenv

clean::
	@rm -f *~ core

reallyclean::	clean
	@rm -rf include bin lib

ifeq ($(shell pwd),$(KDATA_ROOT))
distclean::	reallyclean
	@rm -rf config/setup_kdata.sh config/setup_kdata.csh
else
distclean::	reallyclean
	@echo "Made reallyclean -- Can only make distclean in KDATA_ROOT"
endif

showbuild:
	@echo "------------------------------------------------"
	@echo "KDATA_ROOT          = $(KDATA_ROOT)"
	@echo "ROOTSYS            = $(ROOTSYS)"
	@echo "FFTW_DIR           = $(FFTW_DIR)"
#	@echo "YAJL_DIR           = $(YAJL_DIR)"
#	@echo "JANSSON_DIR        = $(JANSSON_DIR)"
	@echo "CURL_DIR           = $(CURL_DIR)"
	@echo ""
	@echo "MODULES            = $(MODULES)"
	@echo "ALLLIBS            = $(ALLLIBS)"
	@echo "ALLKDATALIBS"      = $(ALLKDATALIBS)
	@echo "ALLEXECS           = $(ALLEXECS)"
	@echo ""
	@echo "ARCH               = $(ARCH)"
	@echo "PLATFORM           = $(PLATFORM)"
	@echo "OPT                = $(OPT)"
	@echo ""
	@echo "CXX                = $(CXX)"
	@echo "CC                 = $(CC)"
	@echo "F77                = $(F77)"
	@echo "CPP                = $(CPP)"
	@echo "LD                 = $(LD)"
	@echo "F77LD              = $(F77LD)"
	@echo "AR                 = $(AR)"
	@echo ""
	@echo "CXXFLAGS           = $(CXXFLAGS)"
	@echo "CINTCXXFLAGS       = $(CINTCXXFLAGS)"
	@echo "EXTRA_CXXFLAGS     = $(EXTRA_CXXFLAGS)"
	@echo "CFLAGS             = $(CFLAGS)"
	@echo "ROOTCINTFLAGS      = $(ROOTCINTFLAGS)"
	@echo "EXTRA_CFLAGS       = $(EXTRA_CFLAGS)"
	@echo "F77FLAGS           = $(F77FLAGS)"
	@echo "F77OPT             = $(F77OPT)"
	@echo "LDFLAGS            = $(LDFLAGS)"
	@echo "EXTRA_LDFLAGS      = $(EXTRA_LDFLAGS)"
	@echo "SOFLAGS            = $(SOFLAGS)"
	@echo "SOEXT              = $(SOEXT)"
	@echo ""
	@echo "FFTWINCS           = $(FFTWINCS)"
#	@echo "YAJLINCS           = $(YAJLINCS)"
#	@echo "JANSSONINCS        = $(JANSSONINCS)"
	@echo "CURLINCS           = $(CURLINCS)"
	@echo ""
	@echo "KDATALIBDIRS       = $(KDATALIBDIRS)"
	@echo ""
	@echo "ROOTLIBS           = $(ROOTLIBS)"
	@echo "SYSLIBS            = $(SYSLIBS)"
	@echo "XLIBS              = $(XLIBS)"
	@echo "F77LIBS            = $(F77LIBS)"
	@echo "FFTWLIBS           = $(FFTWLIBS)"
#	@echo "YAJLLIBS           = $(YAJLLIBS)"
#	@echo "JANSSONLIBS        = $(JANSSONLIBS)"
	@echo "CURLLIBS           = $(CURLLIBS)"
	@echo ""
	@echo "OSTHREADLIB        = $(OSTHREADLIB)"
	@echo ""
	@echo "INSTALL            = $(INSTALL)"
	@echo "RMKDEP             = $(RMKDEP)"
	@echo "MAKEDEP            = $(MAKEDEP)"
	@echo "MAKELIB            = $(MAKELIB)"
	@echo "COPY_HEADER        = $(COPY_HEADER)"
	@echo " "
	@echo "KDATA_FLAGS         = $(KDATA_FLAGS)"
	@echo "KDATA_EXTRA_FLAGS   = $(KDATA_EXTRA_FLAGS)"


# end
