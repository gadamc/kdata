# Module.mk for KDS module
#
# This is a generic template module that will build all
# files named K*.cxx into libkds, and all other .cxx
# files into individual executables. 
#
# To build this module in the KDATA distribution, add
# 'kds' to Modules.mk in the $KDATA_ROOT directory,
# then type "make" (or gmake) in $KDATA_ROOT.
#
# To use this template as the base for developing a standalone 
# module, follow these steps:
#
#   1) Create a new directory for your development project.
#   2) Make soft links to Makefile and config in $KDATA_ROOT.
#   3) Create a Modules.mk file defining your development modules in the new dir.
#   4) Put each of your development modules in a separate directory with
#      the same name as the module, with a Module.mk and Module_libs.mk file
#      in each directory.  You can use this file and Module_libs.mk as a basis
#      for any module, but you should replace "kds" throughout with a unique
#      name specific to your module.  You may also have to make changes to the
#      compile and link commands depending on the structure of your module.
#   5) If your executables depend on the library produced by this module, then
#      add $(KDS_LIB) to the dependencies and -lkds to the link line for
#      building the executables.
#   6) Type "make" (or gmake) in the $KDATA_ROOT directory to build your new
#      modules.
#
# Author: Adam Cox 08/19/10

MODNAME      := kds
MODDIR       := kds

KDS_FLAGS  := $(CXXFLAGS)
KDS_LDFLAGS := $(LDFLAGS)

#add for debuging purposes
#assignment operator
#KDS_FLAGS  += -D_K_DEBUG_EVENT_ASSIGNOP
#remove the optimization in order to improve debugging if you want
#KDS_OPT  := $(filter-out -O2,$(OPT))
#KDS_LDFLAGS := $(filter-out -O2,$(KDS_LDFLAGS))

###

KDS_DIR    := $(MODDIR)
KDS_DIRS   := $(MODDIR)
KDS_DIRI   := $(MODDIR)

#this library should have no dependencies on other KDATA libraries. 
#therefore, there is no XTRALIBS variable used here.

# Uncomment this to use the LinkDef file when generating the dictionary
KDS_LH     := $(KDS_DIRI)/$(MODNAME)_LinkDef.h
KDS_DC     := $(KDS_DIRS)/$(MODNAME)_Dict.C
KDS_DO     := $(KDS_DC:.C=.$(ObjSuf))
KDS_DH     := $(KDS_DC:.C=.h)

KDS_H      := $(filter-out $(KDS_LH) $(KDS_DH),$(wildcard $(KDS_DIRI)/*.h))
KDS_ECXX   := $(wildcard $(KDS_DIRS)/K*.cxx)
KDS_CXX    := $(filter-out $(KDS_ECXX),$(wildcard $(KDS_DIRS)/*.cxx))
#KDS_CXX    := $(filter-out $(KDS_CXX),$(KDS_DIRS)/mergeKEdsTree.cxx)

KDS_O      := $(KDS_CXX:.cxx=.$(ObjSuf))
KDS_EO     := $(KDS_ECXX:.cxx=.$(ObjSuf))
KDS_EH     := $(KDS_ECXX:.cxx=.h)
KDS_DICTH  := $(patsubst $(KDS_DIRI)/%.h,$(KDATAINCDIR)/%.h,$(KDS_EH)) 


KDS_EXE    := $(patsubst $(KDS_DIRS)/%.cxx,$(KDATABINDIR)/%,$(KDS_CXX))

KDSLIBS	   := $(patsubst $(LPATH)/lib%.$(DllSuf),-l%,$(KDS_LIB))

KDS_DEP    := $(KDS_O:.$(ObjSuf)=.d) $(KDS_EO:.$(ObjSuf)=.d)

# only depend on our dictionary if we are building a library
ifneq ($(KDS_LIB),)
KDS_DEP    += $(KDS_DO:.$(ObjSuf)=.d)
endif

# used in the main Makefile
ALLHDRS      += $(patsubst $(KDS_DIRI)/%.h,$(KDATAINCDIR)/%.h,$(KDS_H))
ifneq ($(KDS_EO),)
ALLLIBS      += $(KDS_LIB)
endif
ALLEXECS     += $(KDS_EXE)
ALLKDATALIBS += $(KDSLIBS)


# include all dependency files
INCLUDEFILES += $(KDS_DEP)

# include local MyConfig.mk file if required
-include $(KDS_DIR)/MyConfig.mk

##### local rules #####

# we depend on all of our header files being up to date in the include directory
$(KDATAINCDIR)/%.h:    $(KDS_DIRI)/%.h
	$(COPY_HEADER) $< $@

# rule for compiling our source files
$(KDS_DIRS)/%.$(ObjSuf):    $(KDS_DIRS)/%.cxx 
	$(CXX) $(KDS_FLAGS) -o $@ -c $< 

# rule for building executables
$(KDATABINDIR)/%: $(KDS_DIRS)/%.$(ObjSuf) $(KDATAED_LIB) 
	@echo "=== Linking executable $@ ==="
	$(LD) $(KDS_LDFLAGS) -o $@ $< $(KDATALIBDIRS) $(KDSLIBS) $(ROOTLIBS) $(SYSLIBS) 
                
# rules for building dictionary
$(KDS_DO):         $(KDS_DC)
	$(CXX) $(KDS_FLAGS) -I. -o $@ -c $< 

$(KDS_DC):         $(KDS_EH) $(KDS_LH)
	@echo "Generating dictionary $@..."
	$(ROOTCINT) -f $@ -c $(KDS_DICTH) $(KDS_LH) 

# rule for building library
$(KDS_LIB):        $(KDS_EO) $(KDS_DO) $(KDS_LIBDEP) 
		@echo "Building $@..."
ifeq ($(ARCH),aix5)
		$(MAKESHARED) $(OutPutOpt) $@ $(LIBS) -p 0 $(KDS_EO) $(KDS_DO)
else
ifeq ($(PLATFORM),macosx)
# We need to make both the .dylib and the .so
		$(LD) $(SOFLAGS)$(KDATALDIR)/$(KDS_LIBNAME) $(KDS_LDFLAGS) $(KDS_EO) $(KDS_DO) $(OutPutOpt) $@ $(EXPLLINKLIBS)
ifneq ($(subst $(MACOSX_MINOR),,1234),1234)
ifeq ($(MACOSX_MINOR),4)
		ln -sf $@ $(subst .$(DllSuf),.so,$@)
endif
endif
else
ifeq ($(PLATFORM),win32)
		bindexplib $* $(KDS_EO) $(KDS_DO) > $*.def
		lib -nologo -MACHINE:IX86 $(KDS_EO) $(KDS_DO) -def:$*.def \
		   $(OutPutOpt)$(EVENTLIB)
		$(LD) $(SOFLAGS) $(KDS_LDFLAGS) $(KDS_EO) $(KDS_DO) $*.exp $(LIBS) \
		   $(OutPutOpt)$@
		$(MT_DLL)
else
		$(LD) $(SOFLAGS) $(KDS_LDFLAGS) $(KDS_EO) $(KDS_DO) $(OutPutOpt) $@ $(EXPLLINKLIBS)
endif
endif
endif
		@echo "$@ done"

#	$(LD) $(KDS_LDFLAGS) $(SOFLAGS) -o $@  $(KDS_EO) $(KDS_DO) $(ROOTLIBS) $(KDS_FLAGS) 
#	@echo "done building kpta library"

all-kds:       $(KDS_LIB)

clean-kds:
		@rm -f $(KDS_DIRS)/*~  $(KDS_DIRS)/*.o $(KDS_DIRS)/*.obj $(KDS_DIRS)/*.d
		@rm -f $(KDS_DC) $(KDS_DH) $(KDS_DEP) $(KDS_LIB)

clean::      clean-kds

#end

