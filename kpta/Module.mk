# Module.mk for KPTA module
#
# This is a generic template module that will build all
# files named K*.cxx into libkpta, and all other .cxx
# files into individual executables. 
#
# To build this module in the KDATA distribution, add
# 'kpta' to Modules.mk in the $KDATA_ROOT directory,
# then type "make" (or gmake) in $KDATA_ROOT.
#
# This module is unique in KData because it does not use any of the
# ROOT libraries. It only requires FFTW. 
#
# However, if this restriction is debilitating for development of 
# this library or for physics reasons, we should reconsider how this
# is done.  
#
# Author: Adam Cox 08/19/10

MODNAME      := kpta
MODDIR       := kpta

KPTA_FLAGS  := $(CXXFLAGS)
KPTA_LDFLAGS := $(LDFLAGS)

KPTA_DIR    := $(MODDIR)
KPTA_DIRS   := $(MODDIR)
KPTA_DIRI   := $(MODDIR)

#xtra libs
KPTA_XTRALIBS  := $(FFTWLIBS)

# Uncomment this to use the LinkDef file when generating the dictionary
KPTA_LH     := $(KPTA_DIRI)/$(MODNAME)_LinkDef.h
KPTA_DC     := $(KPTA_DIRS)/$(MODNAME)_Dict.C
KPTA_DO     := $(KPTA_DC:.C=.$(ObjSuf))
KPTA_DH     := $(KPTA_DC:.C=.h)

KPTA_H      := $(filter-out $(KPTA_LH) $(KPTA_DH),$(wildcard $(KPTA_DIRI)/*.h))
KPTA_ECXX   := $(wildcard $(KPTA_DIRS)/K*.cxx)
KPTA_CXX    := $(filter-out $(KPTA_ECXX),$(wildcard $(KPTA_DIRS)/*.cxx))
KPTA_O      := $(KPTA_CXX:.cxx=.$(ObjSuf))
KPTA_EO     := $(KPTA_ECXX:.cxx=.$(ObjSuf))
KPTA_EH     := $(KPTA_ECXX:.cxx=.h)
KPTA_DICTH  := $(patsubst $(KPTA_DIRI)/%.h,$(KDATAINCDIR)/%.h,$(KPTA_EH)) 

KPTA_EXE    := $(patsubst $(KPTA_DIRS)/%.cxx,bin/%,$(KPTA_CXX))

KPTALIBS	:= $(patsubst $(LPATH)/lib%.$(DllSuf),-l%,$(KPTA_LIB))

KPTA_DEP    := $(KPTA_O:.$(ObjSuf)=.d) $(KPTA_EO:.$(ObjSuf)=.d)

# only depend on our dictionary if we are building a library
ifneq ($(KPTA_LIB),)
KPTA_DEP    += $(KPTA_DO:.$(ObjSuf)=.d)
endif

# used in the main Makefile
ALLHDRS      += $(patsubst $(KPTA_DIRI)/%.h,$(KDATAINCDIR)/%.h,$(KPTA_H))
ifneq ($(KPTA_EO),)
ALLLIBS      += $(KPTA_LIB)
endif
ALLEXECS     += $(KPTA_EXE)
ALLKDATALIBS += $(KPTALIBS)



# include all dependency files
INCLUDEFILES += $(KPTA_DEP)

# include local MyConfig.mk file if required
-include $(KPTA_DIR)/MyConfig.mk

##### local rules #####

# we depend on all of our header files being up to date in the include directory
$(KDATAINCDIR)/%.h:    $(KPTA_DIRI)/%.h
	$(COPY_HEADER) $< $@
    
# rule for compiling our source files
$(KPTA_DIRS)/%.$(ObjSuf):    $(KPTA_DIRS)/%.cxx
	$(CXX) $(KPTA_FLAGS) $(FFTWINCS)  -o $@ -c $< 

# rule for building executables
$(KDATABINDIR)/%: $(KPTA_DIRS)/%.$(ObjSuf) $(KDATAED_LIB) 
		@echo "=== Linking $@ ==="
		$(LD) $(KPTA_LDFLAGS) -o $@ $< $(KDATALIBDIRS)  $(KPTALIBS) $(ROOTLIBS) $(SYSLIBS) $(KPTA_XTRALIBS)
                
# rules for building dictionary
$(KPTA_DO):         $(KPTA_DC)
	@echo "building dictionary... $@"
	$(CXX) $(KPTA_FLAGS) -I. -o $@ -c $< 

$(KPTA_DC):         $(KPTA_EH) $(KPTA_LH)
	@echo "Generating dictionary $@..."
	$(ROOTCINT) -f $@ -c $(KPTA_DICTH) $(KPTA_LH) 

# rule for building library
$(KPTA_LIB):        $(KPTA_EO) $(KPTA_DO) $(KPTA_LIBDEP)
		@echo "Building $@..."
ifeq ($(ARCH),aix5)
		$(MAKESHARED) $(OutPutOpt) $@ $(LIBS) -p 0 $(KPTA_EO) $(KSAMBA_DO)
else
ifeq ($(PLATFORM),macosx)
# We need to make both the .dylib and the .so
		$(LD) $(SOFLAGS)$(KDATALDIR)/$(KPTA_LIBNAME) $(KPTA_LDFLAGS) $(KPTA_EO) $(KPTA_DO) $(KDATALIBDIRS) $(KPTA_XTRALIBS) $(OutPutOpt) $@ $(EXPLLINKLIBS)
ifneq ($(subst $(MACOSX_MINOR),,1234),1234)
ifeq ($(MACOSX_MINOR),4)
		ln -sf $@ $(subst .$(DllSuf),.so,$@)
endif
endif
else
ifeq ($(PLATFORM),win32)
		bindexplib $* $(KPTA_EO) $(KPTA_DO) > $*.def
		lib -nologo -MACHINE:IX86 $(KPTA_EO) $(KPTA_DO) -def:$*.def \
		   $(OutPutOpt)$(EVENTLIB)
		$(LD) $(SOFLAGS) $(KPTA_LDFLAGS) $(KPTA_EO) $(KPTA_DO) $(KDATALIBDIRS) $(KPTA_XTRALIBS) $*.exp $(LIBS) \
		   $(OutPutOpt)$@
		$(MT_DLL)
else
		$(LD) $(SOFLAGS) $(KPTA_LDFLAGS) $(KPTA_EO) $(KPTA_DO) $(KDATALIBDIRS) $(KPTA_XTRALIBS) $(OutPutOpt) $@ $(EXPLLINKLIBS)
endif
endif
endif
		@echo "$@ done"

	# @echo "Building $@..."
	# $(LD) $(LDFLAGS) $(SOFLAGS)  -o $@ $(KPTA_EO) $(KPTA_DO) $(FFTWLIBS) $(ROOTLIBS) $(KPTA_FLAGS) 
	# @echo "Done building library kpta"

all-kpta:       $(KPTA_LIB)

clean-kpta:
		@rm -f $(KPTA_DIRS)/*~ $(KPTA_DIRS)/*.o $(KPTA_DIRS)/*.obj
		@rm -f $(KPTA_DC) $(KPTA_DH) $(KPTA_DEP) $(KPTA_LIB)

clean::         clean-kpta

#end

