# Module.mk for KRAW module
#
# This is a generic template module that will build all
# files named K*.cxx into libkraw, and all other .cxx
# files into individual executables. 
#
# To build this module in the KDATA distribution, add
# 'kraw' to Modules.mk in the $KDATA_ROOT directory,
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
#      for any module, but you should replace "kraw" throughout with a unique
#      name specific to your module.  You may also have to make changes to the
#      compile and link commands depending on the structure of your module.
#   5) If your executables depend on the library produced by this module, then
#      add $(KRAW_LIB) to the dependencies and -lkraw to the link line for
#      building the executables.
#   6) Type "make" (or gmake) in the $KDATA_ROOT directory to build your new
#      modules.
#
# Author: Adam Cox 08/19/10

MODNAME      := kraw
MODDIR       := kraw

KRAW_FLAGS  := $(CXXFLAGS)

KRAW_DIR    := $(MODDIR)
KRAW_DIRS   := $(MODDIR)
KRAW_DIRI   := $(MODDIR)

# Uncomment this to use the LinkDef file when generating the dictionary
#KRAW_LH     := $(KRAW_DIRI)/$(MODNAME)_LinkDef.h
KRAW_DC     := $(KRAW_DIRS)/$(MODNAME)_Dict.C
KRAW_DO     := $(KRAW_DC:.C=.o)
KRAW_DH     := $(KRAW_DC:.C=.h)

KRAW_H      := $(filter-out $(KRAW_LH) $(KRAW_DH),$(wildcard $(KRAW_DIRI)/*.h))
KRAW_ECXX   := $(wildcard $(KRAW_DIRS)/K*.cxx)
KRAW_CXX    := $(filter-out $(KRAW_ECXX),$(wildcard $(KRAW_DIRS)/*.cxx))
KRAW_O      := $(KRAW_CXX:.cxx=.o)
KRAW_EO     := $(KRAW_ECXX:.cxx=.o)
KRAW_EH     := $(KRAW_ECXX:.cxx=.h)

KRAW_EXE    := $(patsubst $(KRAW_DIRS)/%.cxx,bin/%,$(KRAW_CXX))

KRAWLIBS	   := $(patsubst $(LPATH)/lib%.$(SOEXT),-l%,$(KRAW_LIB))

KRAW_DEP    := $(KRAW_O:.o=.d) $(KRAW_EO:.o=.d)

# only depend on our dictionary if we are building a library
ifneq ($(KRAW_LIB),)
KRAW_DEP    += $(KRAW_DO:.o=.d)
endif

# used in the main Makefile
ALLHDRS      += $(patsubst $(KRAW_DIRI)/%.h,include/%.h,$(KRAW_H))
ifneq ($(KRAW_EO),)
ALLLIBS      += $(KRAW_LIB)
endif
ALLEXECS     += $(KRAW_EXE)

# include all dependency files
INCLUDEFILES += $(KRAW_DEP)

# include local MyConfig.mk file if required
-include $(KRAW_DIR)/MyConfig.mk

##### local rules #####

# we depend on all of our header files being up to date in the include directory
include/%.h:    $(KRAW_DIRI)/%.h
		$(COPY_HEADER) $< $@

# rule for compiling our source files
$(KRAW_DIRS)/%.o:    $(KRAW_DIRS)/%.cxx
	$(CXX) $(OPT) $(CXXFLAGS) $(ROOTINCS) -I$(ERAINCS)  -o $@ -c $< 

# rule for building executables
bin/%: $(KRAW_DIRS)/%.o $(KDATAED_LIB) $(ERA_LIB)
		@echo "=== Linking $@ ==="
		$(LD) $(LDFLAGS) -o $@ $< $(KDATALIBDIRS) $(ROOTLIBS) $(SYSLIBS) $(KRAWLIBS) $(ERALIBS)
                
# rules for building dictionary
$(KRAW_DO):         $(KRAW_DC)
	$(CXX) $(NOOPT) $(KRAW_FLAGS) $(ROOTINCS) -I. -I$(ERAINCS) -o $@ -c $< 

$(KRAW_DC):         $(KRAW_EH) $(KRAW_LH)
	@echo "Generating dictionary $@..."
	$(ROOTCINT) -f $@ $(ROOTCINTFLAGS) -I$(ERAINCS) $(KRAW_EH) $(KRAW_LH) 

# rule for building library
$(KRAW_LIB):        $(KRAW_EO) $(KRAW_DO) $(KRAW_LIBDEP) $(ERA_LIB)
	@echo "Building $@..."
	@$(MAKELIB) $(PLATFORM) "$(LD)" "$(LDFLAGS)" \
	   "$(SOFLAGS)" "$(KRAW_LIB)" $@  "$(KRAW_EO) $(KRAW_DO)" \
	   "$(ROOTLIBS) $(ERALIBS) $(KRAW_FLAGS)"  -I/opt/include -Iinclude 

all-kraw:       $(KRAW_LIB)

clean-kraw:
		@rm -f $(KRAW_DIRS)/*~ $(KRAW_DIRS)/*.o
		@rm -f $(KRAW_DC) $(KRAW_DH) $(KRAW_DEP) $(KRAW_LIB)

clean::         clean-kraw

#end

