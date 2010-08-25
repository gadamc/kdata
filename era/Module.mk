# Makefile for the ERA module of KData.
# Derived from the example makefile from the ROOT framework
# written by Rene Brun and Fons Rademakers
# using the ROOT libraries on all supported platforms.
#
# NOTE: This is not a standard Module.mk file for a module within KData, but is specially written
# for the ERA since the ERA was written independently of KData
# and can thus remain independent of KData should KData not survive! :)
#
# This makefile builds a libEra.so which can be loaded from ROOT CINT and used
# to read the ROOT event files produced in the initial ERA processing
# However, as of now (21.8.10), the c-functions in EdwUtils and EdwParams are not part of the library
# and to use this library from the ROOT CINT, one must load this file 
# root[0] .L /path/to/kdata/era/EdwUtils.cxx
# root[0] .L /path/to/kdata/era/EdwParams.cxx
# In addition, since many functions within libEra depend upon the c-funtions in EdwUtils.cxx
# and these don't appear to be in the library, I suspect that any external 
# program that links to this library might break.
# or you must make sure to #include EdwUtils.cxx explicitly in your external program.
# I'm not sure if this is true though, since I have not yet tested it. 
#
#       : Adam Cox, 21/8/2010
#

#------------------------------------------------------------------------------


#select if you want to compile with the debugging switch
DEBUG = -g 
#DEBUG = 

# the Module_libs.mk file is needed for ExeSuf, ObjSuf, DllSuf, OutputOpt, ROOTCFLAGS, ROOTLDFLAGS
# ROOTLIBS, UNDEFOPT, SYSLIBS, LDFLAGS, SOFLAGS have been defined elsewhere 
include era/Module_libs.mk

-include ../MyConfig.mk
SRCDIR = era/


#
# ANY CLASS THAT BEGINS WITH "Edw" THAT YOU ADD TO THE kera/ DIRECTORY WILL BE COMPILED
# 
CLASSES := $(basename $(notdir $(wildcard $(SRCDIR)Edw*.cxx)))

# FILTER OUT ANY CLASSES THAT YOU ARE NOT READY TO ADD FOR COMPILATION
#	
#CLASSES := $(filter-out EdwMcEventBase, $(CLASSES))
#CLASSES := $(filter-out EdwTestEventClass, $(CLASSES))
#CLASSES := $(filter-out EdwTreeReader, $(CLASSES))
#CLASSES := $(filter-out EdwTreeWriter, $(CLASSES))
#CLASSES := $(filter-out EdwTreeIO, $(CLASSES))
#CLASSES := $(filter-out EdwAnalysis, $(CLASSES))
CLASSES := $(filter-out EdwParams, $(CLASSES))

CLASSES += Pulse FitPulse OffsetFFT SambaToRoot


EXE = edwrootana

FILLEVENT    = bin/$(EXE)$(ExeSuf)
FILLEVENTO   = $(addprefix $(SRCDIR),$(EXE).$(ObjSuf))
PROGRAMS     = $(FILLEVENT) 
OBJS        += $(FILLEVENTO)


############################
############################
############################

#automatically extract the headers and object file names
EDWHEADERS = $(addsuffix .h, $(CLASSES))
#NEDWHEADERS = $(addprefix $(SRCDIR), $(EDWHEADERS)) 

# used in the main Makefile
#ALLHDRS      += $(patsubst $(SRCDIR)/%.h,include/%.h,$(EDWHEADERS))
#
#ALLEXECS     += $(FILLEVENT)


#automatically generate the ERA_LinkDef.h file! Don't call it Edw_LinkDef
LINKDEF := $(SRCDIR)ERA_LinkDef.h

COBJS = $(addprefix $(SRCDIR),$(addsuffix .$(ObjSuf), $(CLASSES)))
OBJS          = $(COBJS)

## Library and Executable
LIBEDWDSO       = lib/libEra.$(DllSuf)

ifeq ($(PLATFORM),win32)
EVENTLIB      = lib/libEra.lib
else
EVENTLIB      = $(shell pwd)/$(LIBEDWDSO)
endif

# include all dependency files
#INCLUDEFILES += $(addsuffix .o, $(CLASSES)) $(addsuffix .d, $(CLASSES))

#ROOT Dictionary -- dont call this EdwDict... 
EDSDICT = $(SRCDIR)ERADict
EDSDICTO = $(EDSDICT).$(ObjSuf)
EDSDICTS = $(EDSDICT).$(SrcSuf)
OBJS   += $(EDSDICTO) 

ERALDFLAGS := $(LDFLAGS) $(ROOTLDFLAGS)

#------------------------------------------------------------------------------

.SUFFIXES: .$(SrcSuf) .$(ObjSuf) .$(DllSuf)

all:            $(PROGRAMS)

all-era:        all

# we depend on all of our header files being up to date in the include directory
#include/%.h:    $(SRCDIR)/%.h
#	$(COPY_HEADER) $< $@
	
$(LIBEDWDSO):     $(COBJS) $(EDSDICTO) 

ifeq ($(PLATFORM),macosx)
# We need to make both the .dylib and the .so
		$(LD) $(SOFLAGS)$@ $(ERALDFLAGS) $(ROOTLIBS) $(SYSLIBS) $^ $(OutPutOpt) $@
ifneq ($(subst $(MACOSX_MINOR),,1234),1234)
ifeq ($(MACOSX_MINOR),4)
		ln -sf $@ $(subst .$(DllSuf),.so,$@)
else
		$(LD) -bundle -undefined $(UNDEFOPT) $(ERALDFLAGS) $(ROOTLIBS) $(SYSLIBS) $^ \
			$(OutPutOpt) $(subst .$(DllSuf),.so,$@)
endif
endif
else
ifeq ($(PLATFORM),win32)
		bindexplib $* $^ > $*.def
		lib -nologo -MACHINE:IX86 $^ -def:$*.def \
			$(OutPutOpt)$(EVENTLIB)
		$(LD) $(SOFLAGS) $(ERALDFLAGS) $^ $*.exp $(ROOTLIBS) $(SYSLIBS) \
			$(OutPutOpt)$@
		$(MT_DLL)
else
	$(LD) $(SOFLAGS) $(ERALDFLAGS) $(ROOTLIBS)  $(SYSLIBS) $^ $(OutPutOpt) $@ 
endif
endif
		@echo "$@ done"

$(FILLEVENT):       $(LIBEDWDSO) $(FILLEVENTO)
		$(LD) $(ERALDFLAGS) $(FILLEVENTO) $(COBJS) $(EDSDICTO) $(ROOTLIBS) $(SYSLIBS) $(OutPutOpt)$@	
		$(MT_EXE)
		@echo "$@ done"


clean-era: 
		@rm -f $(SRCDIR)*.o $(SRCDIR)*.d $(LINKDEF)* $(EDSDICT)* $(SRCDIR)core $(LIBEDWDSO) $(FILLEVENT)

clean::     clean-era


.SUFFIXES: .$(SrcSuf)

### 
$(EDSDICTS): $(addprefix $(SRCDIR),$(EDWHEADERS)) $(LINKDEF)
	@echo "Generating dictionary $@..."
	$(ROOTCINT) -f $@ -c -I./$(SRCDIR) $(filter-out EdwUtils.h, $(EDWHEADERS)) $(LINKDEF)
	@echo "$@ done"


$(LINKDEF) : $(addsuffix, .$(ObjSuf), $(addprefix $(SRCDIR), $(CLASSES))) $(addprefix $(SRCDIR),$(EDWHEADERS))
	@echo "Writing Linkdef file $@..."
	@echo "#ifdef __CINT__" > $@.tmp
	@echo "" >> $@.tmp
	@echo "#pragma link off all globals;" >> $@.tmp
	@echo "#pragma link off all classes;" >> $@.tmp
	@echo "#pragma link off all functions;" >> $@.tmp
	@echo "" >> $@.tmp
	@for i in $(CLASSES); do \
		if [ "$${i}" = "EdwUtils" ]; then \
			echo "removing EdwUtils";\
		else \
			echo "#pragma link C++ class $$i+;" >> $@.tmp; \
		fi \
	done
	@echo "#pragma link C++ class FloatVect+;" >> $@.tmp
	@echo "" >> $@.tmp
	@echo "#endif" >> $@.tmp
	@mv $@.tmp $@



.$(SrcSuf).$(ObjSuf):
	$(CXX)  $(CXXFLAGS) $(ROOTCFLAGS) $(OutPutOpt) $@ $(DEBUG) -c $<

