# Module.mk for KPSA module
#
# This is a generic template module that will build all
# files named K*.cxx into libkpsa, and all other .cxx
# files into individual executables. 
#
# To build this module in the KDATA distribution, add
# 'kpsa' to Modules.mk in the $KDATA_ROOT directory,
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
#      for any module, but you should replace "kpsa" throughout with a unique
#      name specific to your module.  You may also have to make changes to the
#      compile and link commands depending on the structure of your module.
#   5) If your executables depend on the library produced by this module, then
#      add $(KPSA_LIB) to the dependencies and -lkpsa to the link line for
#      building the executables.
#   6) Type "make" (or gmake) in the $KDATA_ROOT directory to build your new
#      modules.
#
# Author: Adam Cox 08/19/10

MODNAME      := kpsa
MODDIR       := kpsa

KPSA_FLAGS  := $(CXXFLAGS)

KPSA_DIR    := $(MODDIR)
KPSA_DIRS   := $(MODDIR)
KPSA_DIRI   := $(MODDIR)

# Uncomment this to use the LinkDef file when generating the dictionary
#KPSA_LH     := $(KPSA_DIRI)/$(MODNAME)_LinkDef.h
KPSA_DC     := $(KPSA_DIRS)/$(MODNAME)_Dict.C
KPSA_DO     := $(KPSA_DC:.C=.o)
KPSA_DH     := $(KPSA_DC:.C=.h)

KPSA_H      := $(filter-out $(KPSA_LH) $(KPSA_DH),$(wildcard $(KPSA_DIRI)/*.h))
KPSA_ECXX   := $(wildcard $(KPSA_DIRS)/K*.cxx)
KPSA_CXX    := $(filter-out $(KPSA_ECXX),$(wildcard $(KPSA_DIRS)/*.cxx))
KPSA_O      := $(KPSA_CXX:.cxx=.o)
KPSA_EO     := $(KPSA_ECXX:.cxx=.o)
KPSA_EH     := $(KPSA_ECXX:.cxx=.h)

KPSA_EXE    := $(patsubst $(KPSA_DIRS)/%.cxx,bin/%,$(KPSA_CXX))

KPSALIBS	   := $(patsubst $(LPATH)/lib%.$(SOEXT),-l%,$(KPSA_LIB))

KPSA_DEP    := $(KPSA_O:.o=.d) $(KPSA_EO:.o=.d)

# only depend on our dictionary if we are building a library
ifneq ($(KPSA_LIB),)
KPSA_DEP    += $(KPSA_DO:.o=.d)
endif

# used in the main Makefile
ALLHDRS      += $(patsubst $(KPSA_DIRI)/%.h,include/%.h,$(KPSA_H))
ifneq ($(KPSA_EO),)
ALLLIBS      += $(KPSA_LIB)
endif
ALLEXECS     += $(KPSA_EXE)

# include all dependency files
INCLUDEFILES += $(KPSA_DEP)

# include local MyConfig.mk file if required
-include $(KPSA_DIR)/MyConfig.mk

##### local rules #####

# we depend on all of our header files being up to date in the include directory
include/%.h:    $(KPSA_DIRI)/%.h
		$(COPY_HEADER) $< $@

# rule for compiling our source files
$(KPSA_DIRS)/%.o:    $(KPSA_DIRS)/%.cxx
	$(CXX) $(OPT) $(CXXFLAGS) $(ROOTINCS)  -o $@ -c $< 

# rule for building executables
bin/%: $(KPSA_DIRS)/%.o $(KDATAED_LIB) 
		@echo "=== Linking $@ ==="
		$(LD) $(LDFLAGS) -o $@ $< $(KDATALIBDIRS) $(ROOTLIBS) $(SYSLIBS) $(KPSALIBS)
                
# rules for building dictionary
$(KPSA_DO):         $(KPSA_DC)
	$(CXX) $(NOOPT) $(KPSA_FLAGS) $(ROOTINCS) -I. -o $@ -c $< 

$(KPSA_DC):         $(KPSA_EH) $(KPSA_LH)
	@echo "Generating dictionary $@..."
	$(ROOTCINT) -f $@ $(ROOTCINTFLAGS) $(KPSA_EH) $(KPSA_LH) 

# rule for building library
$(KPSA_LIB):        $(KPSA_EO) $(KPSA_DO) $(KPSA_LIBDEP)
	@echo "Building $@..."
	@$(MAKELIB) $(PLATFORM) "$(LD)" "$(LDFLAGS)" \
	   "$(SOFLAGS)" "$(KPSA_LIB)" $@  "$(KPSA_EO) $(KPSA_DO)" \
	   "$(ROOTLIBS) $(KPSA_FLAGS)"  -I/opt/include -Iinclude 

all-kpsa:       $(KPSA_LIB)

clean-kpsa:
		@rm -f $(KPSA_DIRS)/*~ $(KPSA_DIRS)/*.o
		@rm -f $(KPSA_DC) $(KPSA_DH) $(KPSA_DEP)

clean::         clean-kpsa

#end

