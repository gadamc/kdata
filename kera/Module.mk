# Module.mk for KERA module
#
# This is a generic template module that will build all
# files named K*.cxx into libKERA, and all other .cxx
# files into individual executables. 
#
# To build this module in the KDATA distribution, add
# 'KERA' to Modules.mk in the $KDATA_ROOT directory,
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
#      for any module, but you should replace "KERA" throughout with a unique
#      name specific to your module.  You may also have to make changes to the
#      compile and link commands depending on the structure of your module.
#   5) If your executables depend on the library produced by this module, then
#      add $(KERA_LIB) to the dependencies and -lKERA to the link line for
#      building the executables.
#   6) Type "make" (or gmake) in the $KDATA_ROOT directory to build your new
#      modules.
#
# Author: Adam Cox 08/19/10

MODNAME      := kera
MODDIR       := kera

KERA_FLAGS  := $(CXXFLAGS)

KERA_DIR    := $(MODDIR)
KERA_DIRS   := $(MODDIR)
KERA_DIRI   := $(MODDIR)



# Uncomment this to use the LinkDef file when generating the dictionary
#KERA_LH     := $(KERA_DIRI)/$(MODNAME)_LinkDef.h
KERA_DC     := $(KERA_DIRS)/$(MODNAME)_Dict.C
KERA_DO     := $(KERA_DC:.C=.o)
KERA_DH     := $(KERA_DC:.C=.h)

KERA_H      := $(filter-out $(KERA_LH) $(KERA_DH),$(wildcard $(KERA_DIRI)/*.h))
KERA_ECXX   := $(wildcard $(KERA_DIRS)/K*.cxx)
KERA_CXX    := $(filter-out $(KERA_ECXX),$(wildcard $(KERA_DIRS)/*.cxx))
KERA_O      := $(KERA_CXX:.cxx=.o)
KERA_EO     := $(KERA_ECXX:.cxx=.o)
KERA_EH     := $(KERA_ECXX:.cxx=.h)

KERA_EXE    := $(patsubst $(KERA_DIRS)/%.cxx,bin/%,$(KERA_CXX))

KERALIBS	   := $(patsubst $(LPATH)/lib%.$(SOEXT),-l%,$(KERA_LIB))

KERA_DEP    := $(KERA_O:.o=.d) $(KERA_EO:.o=.d)

# only depend on our dictionary if we are building a library
ifneq ($(KERA_LIB),)
KERA_DEP    += $(KERA_DO:.o=.d)
endif

# used in the main Makefile
ALLHDRS      += $(patsubst $(KERA_DIRI)/%.h,include/%.h,$(KERA_H))
ifneq ($(KERA_EO),)
ALLLIBS      += $(KERA_LIB)
endif
ALLEXECS     += $(KERA_EXE)

# include all dependency files
INCLUDEFILES += $(KERA_DEP)

# include local MyConfig.mk file if required
-include $(KERA_DIR)/MyConfig.mk

##### local rules #####

# we depend on all of our header files being up to date in the include directory
include/%.h:    $(KERA_DIRI)/%.h
		$(COPY_HEADER) $< $@

# rule for compiling our source files
$(KERA_DIRS)/%.o:    $(KERA_DIRS)/%.cxx
	$(CXX) $(OPT) $(CXXFLAGS) $(ROOTINCS) -I$(ERAINCS) -o $@ -c $< 

# rule for building executables
bin/%: $(KERA_DIRS)/%.o $(KDATAED_LIB) $(ERA_LIB)
		@echo "=== Linking $@ ==="
		$(LD) $(LDFLAGS) -o $@ $< $(KDATALIBDIRS) $(ROOTLIBS) $(SYSLIBS) $(KERALIBS) $(ERALIBS)
                
# rules for building dictionary
$(KERA_DO):         $(KERA_DC)
	$(CXX) $(NOOPT) $(KERA_FLAGS) $(ROOTINCS) -I. -I$(ERAINCS) -o $@ -c $< 

$(KERA_DC):         $(KERA_EH) $(KERA_LH)
	@echo "Generating dictionary $@..."
	$(ROOTCINT) -f $@ $(ROOTCINTFLAGS) -I$(ERAINCS)  $(KERA_EH) $(KERA_LH) 

# rule for building library
$(KERA_LIB):        $(KERA_EO) $(KERA_DO) $(KERA_LIBDEP) $(ERA_LIB)
	@echo "Building $@..."
	@$(MAKELIB) $(PLATFORM) "$(LD)" "$(LDFLAGS)" \
	   "$(SOFLAGS)" "$(KERA_LIB)" $@  "$(KERA_EO) $(KERA_DO) $(ERALIBS)"\
	   "$(ROOTLIBS)  $(KERA_FLAGS)"  -I/opt/include -Iinclude 

all-kera:       $(KERA_LIB)

clean-kera:
		@rm -f $(KERA_DIRS)/*~ $(KERA_DIRS)/*.o
		@rm -f $(KERA_DC) $(KERA_DH) $(KERA_DEP) $(KERA_LIB)

clean::         clean-kera

#end

