# Makefile for the Edw Data Structure and Analyis Toolkit (EAT).
# Derived from the example makefile from the ROOT framework
# written by Rene Brun and Fons Rademakers
# using the ROOT libraries on all supported platforms.
#
#
#
# Author: Fons Rademakers, 29/2/2000
#       : Adam Cox, 19/3/2010
#

include kera/Module_libs.mk
-include ../MyConfig.mk
SRCDIR = kera/

#------------------------------------------------------------------------------


#select if you want to compile with the debugging switch
DEBUG = -g 
#-D_EDW_DEBUG_EVENT_ASSIGNOP
#DEBUG = 

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

#fillEvents -- CHANGE THIS NAME HERE TO COMPILE A DIFFERENT EXECUTABLE

#EXE = fillinG4Events

EXE = edwrootana
#EXE = fillinG4Events

FILLEVENT    = bin/$(EXE)$(ExeSuf)
FILLEVENTO   = $(addprefix $(SRCDIR),$(EXE).$(ObjSuf))
PROGRAMS     = $(FILLEVENT) 
OBJS        += $(FILLEVENTO)


#fillEventTest  -- THIS DOESN'T WORK. 
#EXE = fillEventTest
#FILLEVENTTEST    = bin/$(EXE)$(ExeSuf)
#FILLEVENTTESTO   = $(addprefix $(SRCDIR),$(EXE).$(ObjSuf))
#PROGRAMS      += $(FILLEVENTTEST) 
#OBJS        += $(FILLEVENTTESTO)

############################
############################
############################

#automatically generate the ERA_LinkDef.h file! Don't call it Edw_LinkDef
LINKDEF := $(SRCDIR)ERA_LinkDef.h

#automatically extract the headers and object file names
EDWHEADERS = $(addsuffix .h, $(CLASSES))
#NEDWHEADERS = $(addprefix $(SRCDIR), $(EDWHEADERS)) 

COBJS = $(addprefix $(SRCDIR),$(addsuffix .$(ObjSuf), $(CLASSES)))
OBJS          = $(COBJS)

## Library and Executable
LIBEDWDSO       = lib/libEra.$(DllSuf)

ifeq ($(PLATFORM),win32)
EVENTLIB      = lib/libEra.lib
else
EVENTLIB      = $(shell pwd)/$(LIBEDWDSO)
endif

#ROOT Dictionary -- dont call this EdwDict... 
EDSDICT = $(SRCDIR)ERADict
EDSDICTO = $(EDSDICT).$(ObjSuf)
EDSDICTS = $(EDSDICT).$(SrcSuf)
OBJS   += $(EDSDICTO) 
#BOOSTDIR = /usr/local/include
#CXXFLAGS += -I$(BOOSTDIR) #include the directly that holds the BOOST libraries

#------------------------------------------------------------------------------

.SUFFIXES: .$(SrcSuf) .$(ObjSuf) .$(DllSuf)

all:            $(PROGRAMS)

$(LIBEDWDSO):     $(COBJS) $(EDSDICTO) 

ifeq ($(PLATFORM),macosx)
# We need to make both the .dylib and the .so
		$(LD) $(SOFLAGS)$@ $(LDFLAGS) $(LIBS) $^ $(OutPutOpt) $@
ifneq ($(subst $(MACOSX_MINOR),,1234),1234)
ifeq ($(MACOSX_MINOR),4)
		ln -sf $@ $(subst .$(DllSuf),.so,$@)
else
		$(LD) -bundle -undefined $(UNDEFOPT) $(LDFLAGS) $(LIBS) $^ \
			$(OutPutOpt) $(subst .$(DllSuf),.so,$@)
endif
endif
else
ifeq ($(PLATFORM),win32)
		bindexplib $* $^ > $*.def
		lib -nologo -MACHINE:IX86 $^ -def:$*.def \
			$(OutPutOpt)$(EVENTLIB)
		$(LD) $(SOFLAGS) $(LDFLAGS) $^ $*.exp $(LIBS) \
			$(OutPutOpt)$@
		$(MT_DLL)
else
	$(LD) $(SOFLAGS) $(LDFLAGS) $(LIBS) $^ $(OutPutOpt) $@ $(EXPLLINKLIBS)
endif
endif
		@echo "$@ done"

$(FILLEVENT):       $(LIBEDWDSO) $(FILLEVENTO)
		$(LD) $(LDFLAGS) $(FILLEVENTO) $(COBJS) $(EDSDICTO) $(LIBS) $(OutPutOpt)$@	
		$(MT_EXE)
		@echo "$@ done"


clean-KERA: 
		@rm -f $(SRCDIR)*.o $(SRCDIR)*.d $(LINKDEF)* $(EDSDICT)* $(SRCDIR)core

clean::     clean-KERA


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
		if [ "$${i}" == "EdwUtils" ]; then \
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
	$(CXX)  $(CXXFLAGS) $(OutPutOpt) $@ $(DEBUG) -c $<

