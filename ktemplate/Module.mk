# Module.mk for KTMP module
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
#      add $(KTMP_LIB) to the dependencies and -lktmp to the link line for
#      building the executables.
#   6) Type "make" (or gmake) in the $KDATA_ROOT directory to build your new
#      modules.
#
# Author: Adam Cox 08/19/10

MODNAME      := ktemplate
MODDIR       := ktemplate

KTMP_FLAGS  := $(CXXFLAGS)
KTMP_LDFLAGS  := $(LDFLAGS)

KTMP_DIR    := $(MODDIR)
KTMP_DIRS   := $(MODDIR)
KTMP_DIRI   := $(MODDIR)


#list all external module libs that this module depends on
#if this module depends on other modules in this project you MUST
#make sure that this MODNAME is listed AFTER all of the MODNAMEs
#that it depends on.
KTMP_XTRALIBS := 
KTMP_LIBDEP   := 

# Uncomment this to use the LinkDef file when generating the dictionary
#KTMP_LH     := $(KTMP_DIRI)/$(MODNAME)_LinkDef.h
KTMP_DC     := $(KTMP_DIRS)/$(MODNAME)_Dict.C
KTMP_DO     := $(KTMP_DC:.C=.$(ObjSuf))
KTMP_DH     := $(KTMP_DC:.C=.h)

KTMP_H      := $(filter-out $(KTMP_LH) $(KTMP_DH),$(wildcard $(KTMP_DIRI)/*.h))
KTMP_ECXX   := $(wildcard $(KTMP_DIRS)/K*.cxx)
KTMP_CXX    := $(filter-out $(KTMP_ECXX),$(wildcard $(KTMP_DIRS)/*.cxx))
KTMP_O      := $(KTMP_CXX:.cxx=.$(ObjSuf))
KTMP_EO     := $(KTMP_ECXX:.cxx=.$(ObjSuf))
KTMP_EH     := $(KTMP_ECXX:.cxx=.h)
KTMP_DICTH  := $(patsubst $(KTMP_DIRI)/%.h,$(KDATAINCDIR)/%.h+,$(KTMP_EH))

KTMP_EXE    := $(patsubst $(KTMP_DIRS)/%.cxx,$(KDATABINDIR)/%,$(KTMP_CXX))

KTMPLIBS	   := $(patsubst $(LPATH)/lib%.$(DllSuf),-l%,$(KTMP_LIB))

KTMP_DEP    := $(KTMP_O:.$(ObjSuf)=.d) $(KTMP_EO:.$(ObjSuf)=.d)

# only depend on our dictionary if we are building a library
ifneq ($(KTMP_LIB),)
KTMP_DEP    += $(KTMP_DO:.$(ObjSuf)=.d)
endif

# used in the main Makefile
ALLHDRS      += $(patsubst $(KTMP_DIRI)/%.h,include/%.h,$(KTMP_H))
ifneq ($(KTMP_EO),)
ALLLIBS      += $(KTMP_LIB)
endif
ALLEXECS     += $(KTMP_EXE)
ALLKDATALIBS += $(KTMPLIBS)

KTMP_XTRALIBS := $(KDSLIBS) $(KPTALIBS)


# include all dependency files
INCLUDEFILES += $(KTMP_DEP)

# include local MyConfig.mk file if required
-include $(KTMP_DIR)/MyConfig.mk

##### local rules #####

# we depend on all of our header files being up to date in the include directory
$(KDATAINCDIR)/%.h:    $(KTMP_DIRI)/%.h
	$(COPY_HEADER) $< $@

# rule for compiling our source files
$(KTMP_DIRS)/%.$(ObjSuf):    $(KTMP_DIRS)/%.cxx
	$(CXX) $(KTMP_FLAGS) -o $@ -c $< 

# rule for building executables
$(KDATABINDIR)/%: $(KTMP_DIRS)/%.$(ObjSuf) $(KDATAED_LIB) $(KTMP_LIBDEP)
		@echo "=== Linking $@ ==="
		$(LD) $(KTMP_LDFLAGS) -o $@ $< $(KDATALIBDIRS) $(KTMPLIBS) $(ROOTLIBS) $(SYSLIBS) $(KTMP_XTRALIBS)
                
# rules for building dictionary
$(KTMP_DO):         $(KTMP_DC) 
	$(CXX) $(KTMP_FLAGS) -I. -o $@ -c $< 

$(KTMP_DC):         $(KTMP_EH) $(KTMP_LH)
	@echo "Generating dictionary $@..."
	$(ROOTCINT) -f $@ -c $(KTMP_DICTH) $(KTMP_LH) 

# rule for building library
$(KTMP_LIB):        $(KTMP_EO) $(KTMP_DO) $(KTMP_LIBDEP)
		@echo "Building $@..."
ifeq ($(ARCH),aix5)
		$(MAKESHARED) $(OutPutOpt) $@ $(LIBS) -p 0 $(KTMP_EO) $(KTMP_DO)
else
ifeq ($(PLATFORM),macosx)
# We need to make both the .dylib and the .so
		$(LD) $(SOFLAGS)$(KDATALDIR)/$(KTMP_LIBNAME) $(KTMP_LDFLAGS) $(KTMP_EO) $(KTMP_DO) $(OutPutOpt) $@ $(EXPLLINKLIBS)
ifneq ($(subst $(MACOSX_MINOR),,1234),1234)
ifeq ($(MACOSX_MINOR),4)
		ln -sf $@ $(subst .$(DllSuf),.so,$@)
endif
endif
else
ifeq ($(PLATFORM),win32)
		bindexplib $* $(KTMP_EO) $(KTMP_DO) > $*.def
		lib -nologo -MACHINE:IX86 $(KTMP_EO) $(KTMP_DO) -def:$*.def \
		   $(OutPutOpt)$(EVENTLIB)
		$(LD) $(SOFLAGS) $(KTMP_LDFLAGS) $(KTMP_EO) $(KTMP_DO) $*.exp $(LIBS) \
		   $(OutPutOpt)$@
		$(MT_DLL)
else
		$(LD) $(SOFLAGS) $(KTMP_LDFLAGS) $(KTMP_EO) $(KTMP_DO) $(OutPutOpt) $@ $(EXPLLINKLIBS)
endif
endif
endif
		@echo "$@ done"


all-ktemplate:       $(KTMP_LIB) 


clean-ktemplate:
		@rm -f $(KTMP_DIRS)/*~ $(KTMP_DIRS)/*.o $(KTMP_DIRS)/*.obj
		@rm -f $(KTMP_DC) $(KTMP_DH) $(KTMP_DEP) $(KTMP_LIB)

clean::         clean-ktemplate

#end