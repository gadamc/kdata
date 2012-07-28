# Module.mk for KSAMBA module
# 
#
# To build this module in the KDATA distribution, add
# 'ksamba' to Modules.mk in the $KDATA_ROOT directory,
# then type "make" (or gmake) in $KDATA_ROOT.
#
# To use this samba as the base for developing a standalone
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
#      add $(KSAMBA_LIB) to the dependencies and -lktmp to the link line for
#      building the executables.
#   6) Type "make" (or gmake) in the $KDATA_ROOT directory to build your new
#      modules.
#
# Author: Adam Cox 08/19/10

MODNAME      := ksamba
MODDIR       := ksamba

KSAMBA_FLAGS  := $(CXXFLAGS) -I$(KDATA_ROOT)/$(KDATAINCDIR)
KSAMBA_LDFLAGS := $(LDFLAGS) 

KSAMBA_DIR    := $(MODDIR)
KSAMBA_DIRS   := $(MODDIR)
KSAMBA_DIRI   := $(MODDIR)


#list all external module libs that this module depends on
#if this module depends on other modules in this project you MUST
#make sure that this MODNAME is listed AFTER all of the MODNAMEs
#that it depends on.
KSAMBA_XTRALIBS := $(KDSLIBS) 
KSAMBA_LIBDEP   := $(KDS_LIB)

# Uncomment this to use the LinkDef file when generating the dictionary
#KSAMBA_LH     := $(KSAMBA_DIRI)/$(MODNAME)_LinkDef.h
KSAMBA_DC     := $(KSAMBA_DIRS)/$(MODNAME)_Dict.C
KSAMBA_DO     := $(KSAMBA_DC:.C=.$(ObjSuf))
KSAMBA_DH     := $(KSAMBA_DC:.C=.h)

KSAMBA_H      := $(filter-out $(KSAMBA_LH) $(KSAMBA_DH),$(wildcard $(KSAMBA_DIRI)/*.h))
KSAMBA_ECXX   := $(wildcard $(KSAMBA_DIRS)/K*.cxx)
KSAMBA_CXX    := $(filter-out $(KSAMBA_ECXX),$(wildcard $(KSAMBA_DIRS)/*.cxx))
KSAMBA_O      := $(KSAMBA_CXX:.cxx=.$(ObjSuf))
KSAMBA_EO     := $(KSAMBA_ECXX:.cxx=.$(ObjSuf))
KSAMBA_EH     := $(KSAMBA_ECXX:.cxx=.h)
KSAMBA_DICTH  := $(patsubst $(KSAMBA_DIRI)/%.h,$(KDATAINCDIR)/%.h+,$(KSAMBA_EH))

KSAMBA_EXE    := $(patsubst $(KSAMBA_DIRS)/%.cxx,$(KDATABINDIR)/%,$(KSAMBA_CXX))

KSAMBALIBS	   := $(patsubst $(LPATH)/lib%.$(DllSuf),-l%,$(KSAMBA_LIB))

KSAMBA_DEP    := $(KSAMBA_O:.$(ObjSuf)=.d) $(KSAMBA_EO:.$(ObjSuf)=.d)

# only depend on our dictionary if we are building a library
ifneq ($(KSAMBA_LIB),)
KSAMBA_DEP    += $(KSAMBA_DO:.$(ObjSuf)=.d)
endif

# used in the main Makefile
ALLHDRS      += $(patsubst $(KSAMBA_DIRI)/%.h,$(KDATAINCDIR)/%.h,$(KSAMBA_H))
ifneq ($(KSAMBA_EO),)
ALLLIBS      += $(KSAMBA_LIB)
endif
ALLEXECS     += $(KSAMBA_EXE)
ALLKDATALIBS += $(KSAMBALIBS)

# include all dependency files
INCLUDEFILES += $(KSAMBA_DEP)

# include local MyConfig.mk file if required
-include $(KSAMBA_DIR)/MyConfig.mk

##### local rules #####

# we depend on all of our header files being up to date in the include directory
$(KDATAINCDIR)/%.h:    $(KSAMBA_DIRI)/%.h 
	$(COPY_HEADER) $< $@
    
# rule for compiling our source files
$(KSAMBA_DIRS)/%.$(ObjSuf):    $(KSAMBA_DIRS)/%.cxx
	$(CXX) $(KSAMBA_FLAGS) -o $@ -c $< 

# rule for building executables
$(KDATABINDIR)/%: $(KSAMBA_DIRS)/%.$(ObjSuf) $(KDATAED_LIB) $(KSAMBA_LIBDEP)
		@echo "=== Linking $@ ==="
		$(LD) $(KSAMBA_LDFLAGS) -o $@ $< $(KDATALIBDIRS) $(KSAMBALIBS) $(ROOTLIBS) $(SYSLIBS) $(KSAMBA_XTRALIBS)
                
# rules for building dictionary
$(KSAMBA_DO):         $(KSAMBA_DC)
	$(CXX) $(KSAMBA_FLAGS) -I. -o $@ -c $< 

$(KSAMBA_DC):         $(KSAMBA_EH) $(KSAMBA_LH)
	@echo "Generating dictionary $@..."
	$(ROOTCINT) -f $@ -c $(KSAMBA_DICTH) $(KSAMBA_LH) 

# rule for building library
$(KSAMBA_LIB):        $(KSAMBA_EO) $(KSAMBA_DO) $(KSAMBA_LIBDEP) $(KSAMBA_LIBDEP)
		@echo "Building $@..."
ifeq ($(ARCH),aix5)
		$(MAKESHARED) $(OutPutOpt) $@ $(LIBS) -p 0 $(KSAMBA_EO) $(KSAMBA_DO)
else
ifeq ($(PLATFORM),macosx)
# We need to make both the .dylib and the .so
		$(LD) $(SOFLAGS)$(KDATALDIR)/$(KSAMBA_LIBNAME) $(KSAMBA_LDFLAGS) $(KSAMBA_EO) $(KSAMBA_DO) $(KDATALIBDIRS) $(KSAMBA_XTRALIBS) $(OutPutOpt) $@ $(EXPLLINKLIBS)
ifneq ($(subst $(MACOSX_MINOR),,1234),1234)
ifeq ($(MACOSX_MINOR),4)
		ln -sf $@ $(subst .$(DllSuf),.so,$@)
endif
endif
else
ifeq ($(PLATFORM),win32)
		bindexplib $* $(KSAMBA_EO) $(KSAMBA_DO) > $*.def
		lib -nologo -MACHINE:IX86 $(KSAMBA_EO) $(KSAMBA_DO) -def:$*.def \
		   $(OutPutOpt)$(EVENTLIB)
		$(LD) $(SOFLAGS) $(KSAMBA_LDFLAGS) $(KSAMBA_EO) $(KSAMBA_DO) $(KDATALIBDIRS) $(KSAMBA_XTRALIBS) $*.exp $(LIBS) \
		   $(OutPutOpt)$@
		$(MT_DLL)
else
		$(LD) $(SOFLAGS) $(KSAMBA_LDFLAGS) $(KSAMBA_EO) $(KSAMBA_DO) $(KDATALIBDIRS) $(KSAMBA_XTRALIBS) $(OutPutOpt) $@ $(EXPLLINKLIBS)
endif
endif
endif
		@echo "$@ done"

#	@echo "Building $@..."
#	$(LD) $(LDFLAGS) $(SOFLAGS) -o $@ $(KSAMBA_EO) $(KSAMBA_DO) $(KDATALIBDIRS) $(KSAMBA_XTRALIBS) $(ROOTLIBS) $(KSAMBA_FLAGS) 
	

all-ksamba:       $(KSAMBA_LIB)

clean-ksamba:
		@rm -f $(KSAMBA_DIRS)/*~ $(KSAMBA_DIRS)/*.o $(KSAMBA_DIRS)/*.obj
		@rm -f $(KSAMBA_DC) $(KSAMBA_DH) $(KSAMBA_DEP) $(KSAMBA_LIB)

clean::         clean-ksamba

#end

