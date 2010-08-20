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

MODNAME      := KDS
MODDIR       := kds

KDS_FLAGS  := $(CXXFLAGS)

KDS_DIR    := $(MODDIR)
KDS_DIRS   := $(MODDIR)
KDS_DIRI   := $(MODDIR)

# Uncomment this to use the LinkDef file when generating the dictionary
#KDS_LH     := $(KDS_DIRI)/$(MODNAME)_LinkDef.h
KDS_DC     := $(KDS_DIRS)/$(MODNAME)_Dict.C
KDS_DO     := $(KDS_DC:.C=.o)
KDS_DH     := $(KDS_DC:.C=.h)

KDS_H      := $(filter-out $(KDS_LH) $(KDS_DH),$(wildcard $(KDS_DIRI)/*.h))
KDS_ECXX   := $(wildcard $(KDS_DIRS)/K*.cxx)
KDS_CXX    := $(filter-out $(KDS_ECXX),$(wildcard $(KDS_DIRS)/*.cxx))
KDS_O      := $(KDS_CXX:.cxx=.o)
KDS_EO     := $(KDS_ECXX:.cxx=.o)
KDS_EH     := $(KDS_ECXX:.cxx=.h)

KDS_EXE    := $(patsubst $(KDS_DIRS)/%.cxx,bin/%,$(KDS_CXX))

KDSLIBS	   := $(patsubst $(LPATH)/lib%.$(SOEXT),-l%,$(KDS_LIB))

KDS_DEP    := $(KDS_O:.o=.d) $(KDS_EO:.o=.d)

# only depend on our dictionary if we are building a library
ifneq ($(KDS_LIB),)
KDS_DEP    += $(KDS_DO:.o=.d)
endif

# used in the main Makefile
ALLHDRS      += $(patsubst $(KDS_DIRI)/%.h,include/%.h,$(KDS_H))
ifneq ($(KDS_EO),)
ALLLIBS      += $(KDS_LIB)
endif
ALLEXECS     += $(KDS_EXE)

# include all dependency files
INCLUDEFILES += $(KDS_DEP)

# include local MyConfig.mk file if required
-include $(KDS_DIR)/MyConfig.mk

##### local rules #####

# we depend on all of our header files being up to date in the include directory
include/%.h:    $(KDS_DIRI)/%.h
		$(COPY_HEADER) $< $@

# rule for compiling our source files
$(KDS_DIRS)/%.o:    $(KDS_DIRS)/%.cxx
	$(CXX) $(OPT) $(CXXFLAGS) $(ROOTINCS)  -o $@ -c $< 

# rule for building executables
bin/%: $(KDS_DIRS)/%.o $(KDATAED_LIB) 
		@echo "=== Linking $@ ==="
		$(LD) $(LDFLAGS) -o $@ $< $(KDATALIBDIRS) $(ROOTLIBS) $(SYSLIBS) $(KDSLIBS)
                
# rules for building dictionary
$(KDS_DO):         $(KDS_DC)
	$(CXX) $(NOOPT) $(KDS_FLAGS) $(ROOTINCS) -I. -o $@ -c $< 

$(KDS_DC):         $(KDS_EH) $(KDS_LH)
	@echo "Generating dictionary $@..."
	$(ROOTCINT) -f $@ $(ROOTCINTFLAGS) $(KDS_EH) $(KDS_LH) 

# rule for building library
$(KDS_LIB):        $(KDS_EO) $(KDS_DO) $(KDS_LIBDEP)
	@echo "Building $@..."
	@$(MAKELIB) $(PLATFORM) "$(LD)" "$(LDFLAGS)" \
	   "$(SOFLAGS)" "$(KDS_LIB)" $@  "$(KDS_EO) $(KDS_DO)" \
	   "$(ROOTLIBS) $(KDS_FLAGS)"  -I/opt/include -Iinclude 

all-KDS:       $(KDS_LIB)

clean-KDS:
		@rm -f $(KDS_DIRS)/*~ $(KDS_DIRS)/*.o
		@rm -f $(KDS_DC) $(KDS_DH) $(KDS_DEP)

clean::         clean-KDS

#end

