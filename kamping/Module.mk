# Module.mk for KAMPING module
#
# This is a generic template module that will build all
# files named K*.cxx into libktmp, and all other .cxx
# files into individual executables. 
#
# To build this module in the KDATA distribution, add
# 'ktmp' to Modules.mk in the $KDATA_ROOT directory,
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
#      for any module, but you should replace "ktmp" throughout with a unique
#      name specific to your module.  You may also have to make changes to the
#      compile and link commands depending on the structure of your module.
#   5) If your executables depend on the library produced by this module, then
#      add $(KAMPING_LIB) to the dependencies and -lktmp to the link line for
#      building the executables.
#   6) Type "make" (or gmake) in the $KDATA_ROOT directory to build your new
#      modules.
#
# Author: Adam Cox 08/19/10

MODNAME      := kamping
MODDIR       := kamping

KAMPING_FLAGS  := $(CXXFLAGS) -I$(KDATA_ROOT)/$(KDATAINCDIR)
KAMPING_LDFLAGS  := $(LDFLAGS)

KAMPING_DIR    := $(MODDIR)
KAMPING_DIRS   := $(MODDIR)
KAMPING_DIRI   := $(MODDIR)

#list all external module libs that this module depends on
#if this module depends on other modules in this project you MUST
#make sure that this MODNAME is listed AFTER all of the MODNAMEs
#that it depends on.
KAMPING_XTRALIBS := $(KPTALIBS) $(KDSLIBS) 
KAMPING_LIBDEP   := $(KPTA_LIB) $(KDS_LIB) 


# Uncomment this to use the LinkDef file when generating the dictionary
KAMPING_LH     := $(KAMPING_DIRI)/$(MODNAME)_LinkDef.h
KAMPING_DC     := $(KAMPING_DIRS)/$(MODNAME)_Dict.C
KAMPING_DO     := $(KAMPING_DC:.C=.$(ObjSuf))
KAMPING_DH     := $(KAMPING_DC:.C=.h)

KAMPING_H      := $(filter-out $(KAMPING_LH) $(KAMPING_DH),$(wildcard $(KAMPING_DIRI)/*.h))
KAMPING_ECXX   := $(wildcard $(KAMPING_DIRS)/K*.cxx)
KAMPING_CXX    := $(filter-out $(KAMPING_ECXX), $(wildcard $(KAMPING_DIRS)/*.cxx))
KAMPING_O      := $(KAMPING_CXX:.cxx=.$(ObjSuf))
KAMPING_EO     := $(KAMPING_ECXX:.cxx=.$(ObjSuf))
KAMPING_EH     := $(KAMPING_ECXX:.cxx=.h)
KAMPING_DICTH  := $(patsubst $(KAMPING_DIRI)/%.h,$(KDATAINCDIR)/%.h,$(KAMPING_EH))

KAMPING_EXE    := $(patsubst $(KAMPING_DIRS)/%.cxx,$(KDATABINDIR)/%,$(KAMPING_CXX))

KAMPINGLIBS	   := $(patsubst $(LPATH)/lib%.$(DllSuf),-l%,$(KAMPING_LIB))

KAMPING_DEP    := $(KAMPING_O:.$(ObjSuf)=.d) $(KAMPING_EO:.$(ObjSuf)=.d)

# only depend on our dictionary if we are building a library
ifneq ($(KAMPING_LIB),)
KAMPING_DEP    += $(KAMPING_DO:.$(ObjSuf)=.d)
endif

# used in the main Makefile
ALLHDRS      += $(patsubst $(KAMPING_DIRI)/%.h,$(KDATAINCDIR)/%.h,$(KAMPING_H))
ifneq ($(KAMPING_EO),)
ALLLIBS      += $(KAMPING_LIB)
endif
ALLEXECS     += $(KAMPING_EXE)
ALLKDATALIBS += $(KAMPINGLIBS)




# include all dependency files
INCLUDEFILES += $(KAMPING_DEP)

# include local MyConfig.mk file if required
-include $(KAMPING_DIR)/MyConfig.mk

##### local rules #####

# we depend on all of our header files being up to date in the include directory
$(KDATAINCDIR)/%.h:    $(KAMPING_DIRI)/%.h
		$(COPY_HEADER) $< $@

    
# rule for compiling our source files
$(KAMPING_DIRS)/%.$(ObjSuf):    $(KAMPING_DIRS)/%.cxx
	$(CXX) $(KAMPING_FLAGS) -o $@ -c $< 

# rule for building executables
bin/%: $(KAMPING_DIRS)/%.$(ObjSuf) $(KDATAED_LIB) $(KAMPING_LIBDEP)
		@echo "=== Linking $@ ==="
		$(LD) $(KAMPING_LDFLAGS) -o $@ $< $(ROOTLIBS) $(SYSLIBS) $(KDATALIBDIRS) $(KAMPINGLIBS) $(KAMPING_XTRALIBS)
                
# rules for building dictionary
$(KAMPING_DO):         $(KAMPING_DC)
	$(CXX) $(KAMPING_FLAGS) -I. -o $@ -c $< 

$(KAMPING_DC):         $(KAMPING_EH) $(KAMPING_LH)
	@echo "Generating dictionary $@..."
	$(ROOTCINT) -f $@ -c $(KAMPING_DICTH) $(KAMPING_LH) 

# rule for building library
$(KAMPING_LIB):        $(KAMPING_EO) $(KAMPING_DO) $(KAMPING_LIBDEP) $(KAMPING_LIBDEP)
		@echo "Building $@..."
ifeq ($(ARCH),aix5)
		$(MAKESHARED) $(OutPutOpt) $@ $(LIBS) -p 0 $(KAMPING_EO) $(KAMPING_DO)
else
ifeq ($(PLATFORM),macosx)
# We need to make both the .dylib and the .so
		$(LD) $(SOFLAGS)$(KDATALDIR)/$(KAMPING_LIBNAME) $(KAMPING_LDFLAGS) $(KAMPING_EO) $(KAMPING_DO) $(KDATALIBDIRS) $(KAMPING_XTRALIBS) $(OutPutOpt) $@ $(EXPLLINKLIBS)
ifneq ($(subst $(MACOSX_MINOR),,1234),1234)
ifeq ($(MACOSX_MINOR),4)
		ln -sf $@ $(subst .$(DllSuf),.so,$@)
endif
endif
else
ifeq ($(PLATFORM),win32)
		bindexplib $* $(KAMPING_EO) $(KAMPING_DO) > $*.def
		lib -nologo -MACHINE:IX86 $(KAMPING_EO) $(KAMPING_DO) -def:$*.def \
		   $(OutPutOpt)$(EVENTLIB)
		$(LD) $(SOFLAGS) $(KAMPING_LDFLAGS) $(KAMPING_EO) $(KAMPING_DO) $(KDATALIBDIRS) $(KAMPING_XTRALIBS) $*.exp $(LIBS) \
		   $(OutPutOpt)$@
		$(MT_DLL)
else
		$(LD) $(SOFLAGS) $(KAMPING_LDFLAGS) $(KAMPING_EO) $(KAMPING_DO) $(KDATALIBDIRS) $(KAMPING_XTRALIBS) $(OutPutOpt) $@ $(EXPLLINKLIBS)
endif
endif
endif
		@echo "$@ done"

	# @echo "Building $@..."
	# $(LD) $(LDFLAGS) $(SOFLAGS)  -o $@ $(KAMPING_EO) $(KAMPING_DO) $(KDATALIBDIRS) $(KAMPING_XTRALIBS) $(ROOTLIBS) $(KAMPING_FLAGS) 

all-kamping:       $(KAMPING_LIB) 


clean-kamping:
		@rm -f $(KAMPING_DIRS)/*~ $(KAMPING_DIRS)/*.o $(KAMPING_DIRS)/*.obj
		@rm -f $(KAMPING_DC) $(KAMPING_DH) $(KAMPING_DEP) $(KAMPING_LIB)

clean::         clean-kamping


#end

