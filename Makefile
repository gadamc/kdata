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

include Makefile.arch
-include ../MyConfig.mk
SRCDIR = src/

#------------------------------------------------------------------------------


#select if you want to compile with the debugging switch
DEBUG = -g 
#-D_EDW_DEBUG_EVENT_ASSIGNOP
#DEBUG = 

#
# ANY CLASS THAT BEGINS WITH "Edw" THAT YOU ADD TO THE src/ DIRECTORY WILL BE COMPILED
# 
CLASSES := $(basename $(notdir $(wildcard $(SRCDIR)K*.cxx)))

# FILTER OUT ANY CLASSES THAT YOU ARE NOT READY TO ADD FOR COMPILATION
#	

CLASSES := $(filter-out KTimeStamp, $(CLASSES))

#fillEvents -- CHANGE THIS NAME HERE TO COMPILE A DIFFERENT EXECUTABLE

#EXE = fillinG4Events

#EXE = skimNoNoiseBolos
EXE = breakupKDataFile
#EXE = fillinG4Events
#EXE = allPassOne_GSEventNumbers

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

#automatically generate the EDS_LinkDef.h file! Don't call it Edw_LinkDef
LINKDEF := $(SRCDIR)KEDS_LinkDef.h

#automatically extract the headers and object file names
KEDSHEADERS = $(addsuffix .h, $(CLASSES))
#NEDWHEADERS = $(addprefix $(SRCDIR), $(EDWHEADERS)) 

COBJS = $(addprefix $(SRCDIR),$(addsuffix .$(ObjSuf), $(CLASSES)))
OBJS          = $(COBJS)

## Library and Executable
LIBKEDSO       = lib/libKData.$(DllSuf)

ifeq ($(PLATFORM),win32)
EVENTLIB      = lib/libKData.lib
else
EVENTLIB      = $(shell pwd)/$(LIBKEDSO)
endif

#ROOT Dictionary -- dont start this with a K... 
KEDSDICT = $(SRCDIR)DataDict
KEDSDICTO = $(KEDSDICT).$(ObjSuf)
KEDSDICTS = $(KEDSDICT).$(SrcSuf)
OBJS   += $(KEDSDICTO) 
#BOOSTDIR = /usr/local/include
#CXXFLAGS += -I$(BOOSTDIR) #include the directly that holds the BOOST libraries

#------------------------------------------------------------------------------

.SUFFIXES: .$(SrcSuf) .$(ObjSuf) .$(DllSuf)

all:            $(PROGRAMS)

$(LIBKEDSO):     $(COBJS) $(KEDSDICTO) 
 
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

$(FILLEVENT):       $(LIBKEDSO) $(FILLEVENTO) 
		$(LD) $(LDFLAGS) $(FILLEVENTO) $(COBJS) $(KEDSDICTO) $(LIBS) $(OutPutOpt)$@	
		$(MT_EXE)
		@echo "$@ done"
		
#$(FILLEVENTTEST):       $(LIBKEDSO) $(FILLEVENTTESTO)
#		$(LD) $(LDFLAGS) $(FILLEVENTTESTO) $(COBJS) $(EDSDICTO) $(LIBS) $(OutPutOpt)$@	
#		$(MT_EXE)
#		@echo "$@ done"

clean:
		@rm -f $(OBJS) $(TRACKMATHSRC) core $(LINKDEF)* $(KEDSDICT)* lib/* bin/* $(SRCDIR)*.o

distclean:      clean
		@rm -f $(PROGRAMS) $(LIBKEDSO) $(EVENTLIB) *.def *.exp \
		   *.root *.ps *.so *.lib *.dll *.d *.log .def so_locations \
		   files/* 

.SUFFIXES: .$(SrcSuf)

### 
$(KEDSDICTS): $(addprefix $(SRCDIR),$(KEDSHEADERS)) $(LINKDEF)
	@echo "Generating dictionary $@..."
	$(ROOTCINT) -f $@ -c -I./$(SRCDIR) $(KEDSHEADERS) $(LINKDEF)
	@echo "$@ done"

	
$(LINKDEF) : $(addsuffix, .$(ObjSuf), $(addprefix $(SRCDIR), $(CLASSES))) $(addprefix $(SRCDIR),$(KEDSHEADERS))
	@echo "Writing Linkdef file $@..."
	@echo "#ifdef __CINT__" > $@.tmp
	@echo "" >> $@.tmp
	@echo "#pragma link off all globals;" >> $@.tmp
	@echo "#pragma link off all classes;" >> $@.tmp
	@echo "#pragma link off all functions;" >> $@.tmp
	@echo "" >> $@.tmp
	@for i in $(CLASSES); do \
		echo "#pragma link C++ class $$i+;" >> $@.tmp; \
	done
	@echo "" >> $@.tmp
	@echo "#endif" >> $@.tmp
	@mv $@.tmp $@


.$(SrcSuf).$(ObjSuf):
	$(CXX)  $(CXXFLAGS) $(OutPutOpt) $@ $(DEBUG) -c $<

