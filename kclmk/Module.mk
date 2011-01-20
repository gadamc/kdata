# Module.mk for KCLMK module
#
# This is a generic template module that will build all
# files named K*.cxx into libKCLMK, and all other .cxx
# files into individual executables. 
#
# To build this module in the KDATA distribution, add
# 'KCLMK' to Modules.mk in the $KDATA_ROOT directory,
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
#      for any module, but you should replace "KCLMK" throughout with a unique
#      name specific to your module.  You may also have to make changes to the
#      compile and link commands depending on the structure of your module.
#   5) If your executables depend on the library produced by this module, then
#      add $(KCLMK_LIB) to the dependencies and -lKCLMK to the link line for
#      building the executables.
#   6) Type "make" (or gmake) in the $KDATA_ROOT directory to build your new
#      modules.
#
# Author: Adam Cox 08/19/10

MODNAME      := kclmk
MODDIR       := kclmk

KCLMK_FLAGS  := $(CXXFLAGS)
KCLMK_LDFLAGS := $(LDFLAGS)
KCLMK_OPT     := $(OPT)


#KCLMK_OPT  += -g
KCLMK_OPT  := $(filter-out -O2,$(KCLMK_OPT))
KCLMK_LDFLAGS := $(filter-out -O2,$(KCLMK_LDFLAGS))

#adding debugging flags here
#KCLMK_FLAGS += -D_K_DEBUG_ERAEVENTFINDER
#KCLMK_FLAGS += -D_K_DEBUG_FILETRANSFER


KCLMK_DIR    := $(MODDIR)
KCLMK_DIRS   := $(MODDIR)
KCLMK_DIRI   := $(MODDIR)

#only need to put the ERAINCS here because ERA files are NOT copied
# to the projects include directory! This should be changed.
KCLMK_XTRAINCS := $(ERAINCS)  

#list all external module libs that this module depends on
#if this module depends on other modules in this project you MUST
#make sure that this MODNAME is listed AFTER all of the MODNAMEs
#that it depends on.
KCLMK_XTRALIBS := $(ERALIBS) $(KDSLIBS) $(KPTALIBS)
KCLMK_LIBDEP   := $(ERA_LIB) $(KDS_LIB) $(KPTA_LIB)

# Uncomment this to use the LinkDef file when generating the dictionary
#KCLMK_LH     := $(KCLMK_DIRI)/$(MODNAME)_LinkDef.h
KCLMK_DC     := $(KCLMK_DIRS)/$(MODNAME)_Dict.C
KCLMK_DO     := $(KCLMK_DC:.C=.o)
KCLMK_DH     := $(KCLMK_DC:.C=.h)

KCLMK_H      := $(filter-out $(KCLMK_LH) $(KCLMK_DH),$(wildcard $(KCLMK_DIRI)/*.h))
KCLMK_ECXX   := $(wildcard $(KCLMK_DIRS)/K*.cxx)
KCLMK_CXX    := $(filter-out $(KCLMK_ECXX),$(wildcard $(KCLMK_DIRS)/*.cxx))
KCLMK_O      := $(KCLMK_CXX:.cxx=.o)
KCLMK_EO     := $(KCLMK_ECXX:.cxx=.o)
KCLMK_EH     := $(KCLMK_ECXX:.cxx=.h)

KCLMK_EXE    := $(patsubst $(KCLMK_DIRS)/%.cxx,bin/%,$(KCLMK_CXX))

KCLMKLIBS	   := $(patsubst $(LPATH)/lib%.$(SOEXT),-l%,$(KCLMK_LIB))

KCLMK_DEP    := $(KCLMK_O:.o=.d) $(KCLMK_EO:.o=.d)

# only depend on our dictionary if we are building a library
ifneq ($(KCLMK_LIB),)
KCLMK_DEP    += $(KCLMK_DO:.o=.d)
endif

# used in the main Makefile
ALLHDRS      += $(patsubst $(KCLMK_DIRI)/%.h,include/%.h,$(KCLMK_H))
ifneq ($(KCLMK_EO),)
ALLLIBS      += $(KCLMK_LIB)
endif
ALLEXECS     += $(KCLMK_EXE)

# include all dependency files
INCLUDEFILES += $(KCLMK_DEP)

# include local MyConfig.mk file if required
-include $(KCLMK_DIR)/MyConfig.mk

##### local rules #####

# we depend on all of our header files being up to date in the include directory
include/%.h:    $(KCLMK_DIRI)/%.h
		$(COPY_HEADER) $< $@

# rule for compiling our source files
$(KCLMK_DIRS)/%.o:    $(KCLMK_DIRS)/%.cxx
	$(CXX) $(KCLMK_OPT) $(KCLMK_FLAGS) $(ROOTINCS) -I$(KCLMK_XTRAINCS) -o $@ -c $< 

# rule for building executables
bin/%: $(KCLMK_DIRS)/%.o $(KDATAED_LIB) $(KCLMK_LIBDEP)
		@echo "=== Linking $@ ==="
		$(LD) $(KCLMK_LDFLAGS) -o $@ $< $(KDATALIBDIRS) $(ROOTLIBS) $(SYSLIBS) $(KCLMKLIBS) $(KCLMK_XTRALIBS)
                
# rules for building dictionary
$(KCLMK_DO):         $(KCLMK_DC)
	$(CXX) $(NOOPT) $(KCLMK_FLAGS) $(ROOTINCS) -I. -I$(KCLMK_XTRAINCS) -o $@ -c $< 

$(KCLMK_DC):         $(KCLMK_EH) $(KCLMK_LH)
	@echo "Generating dictionary $@..."
	$(ROOTCINT) -f $@ $(ROOTCINTFLAGS) -I$(KCLMK_XTRAINCS)  $(KCLMK_EH) $(KCLMK_LH) 

# rule for building library
$(KCLMK_LIB):        $(KCLMK_EO) $(KCLMK_DO) $(KCLMK_LIBDEP) 
	@echo "Building $@..."
	@$(MAKELIB) $(PLATFORM) "$(LD)" "$(KCLMK_LDFLAGS)" \
	   "$(SOFLAGS)" "$(KCLMK_LIB)" $@  "$(KCLMK_EO) $(KCLMK_DO) $(KCLMK_XTRALIBS)"\
	   "$(ROOTLIBS)  $(KCLMK_FLAGS)"  -I/opt/include -Iinclude 

all-kclmk:       $(KCLMK_LIB)

clean-kclmk:
		@rm -f $(KCLMK_DIRS)/*~ $(KCLMK_DIRS)/*.o
		@rm -f $(KCLMK_DC) $(KCLMK_DH) $(KCLMK_DEP) $(KCLMK_LIB)

clean::         clean-kclmk

#end


