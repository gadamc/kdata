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

KTMP_DIR    := $(MODDIR)
KTMP_DIRS   := $(MODDIR)
KTMP_DIRI   := $(MODDIR)


# Uncomment this to use the LinkDef file when generating the dictionary
#KTMP_LH     := $(KTMP_DIRI)/$(MODNAME)_LinkDef.h
KTMP_DC     := $(KTMP_DIRS)/$(MODNAME)_Dict.C
KTMP_DO     := $(KTMP_DC:.C=.o)
KTMP_DH     := $(KTMP_DC:.C=.h)

KTMP_H      := $(filter-out $(KTMP_LH) $(KTMP_DH),$(wildcard $(KTMP_DIRI)/*.h))
KTMP_ECXX   := $(wildcard $(KTMP_DIRS)/K*.cxx)
KTMP_CXX    := $(filter-out $(KTMP_ECXX),$(wildcard $(KTMP_DIRS)/*.cxx))
KTMP_O      := $(KTMP_CXX:.cxx=.o)
KTMP_EO     := $(KTMP_ECXX:.cxx=.o)
KTMP_EH     := $(KTMP_ECXX:.cxx=.h)
KTMP_DICTH  := $(KTMP_EH:.h=.h+)

KTMP_EXE    := $(patsubst $(KTMP_DIRS)/%.cxx,bin/%,$(KTMP_CXX))

KTMPLIBS	   := $(patsubst $(LPATH)/lib%.$(SOEXT),-l%,$(KTMP_LIB))

KTMP_DEP    := $(KTMP_O:.o=.d) $(KTMP_EO:.o=.d)

# only depend on our dictionary if we are building a library
ifneq ($(KTMP_LIB),)
KTMP_DEP    += $(KTMP_DO:.o=.d)
endif

# used in the main Makefile
ALLHDRS      += $(patsubst $(KTMP_DIRI)/%.h,include/%.h,$(KTMP_H))
ifneq ($(KTMP_EO),)
ALLLIBS      += $(KTMP_LIB)
endif
ALLEXECS     += $(KTMP_EXE)

KTMP_XTRALIBS := $(KDSLIBS) $(KPTALIBS)


# include all dependency files
INCLUDEFILES += $(KTMP_DEP)

# include local MyConfig.mk file if required
-include $(KTMP_DIR)/MyConfig.mk

##### local rules #####

# we depend on all of our header files being up to date in the include directory
include/%.h:    $(KTMP_DIRI)/%.h
		$(COPY_HEADER) $< $@

    
# rule for compiling our source files
$(KTMP_DIRS)/%.o:    $(KTMP_DIRS)/%.cxx
	$(CXX) $(OPT) $(KTMP_FLAGS) $(ROOTINCS)  -o $@ -c $< 

# rule for building executables
bin/%: $(KTMP_DIRS)/%.o $(KDATAED_LIB) 
		@echo "=== Linking $@ ==="
		$(LD) $(LDFLAGS) -o $@ $< $(KDATALIBDIRS) $(ROOTLIBS) $(SYSLIBS) $(KTMPLIBS) $(KTMP_XTRALIBS)
                
# rules for building dictionary
$(KTMP_DO):         $(KTMP_DC)
	$(CXX) $(NOOPT) $(KTMP_FLAGS) $(ROOTINCS) -I. -o $@ -c $< 

$(KTMP_DC):         $(KTMP_EH) $(KTMP_LH)
	@echo "Generating dictionary $@..."
	$(ROOTCINT) -f $@ $(ROOTCINTFLAGS) $(KTMP_DICTH) $(KTMP_LH) 

# rule for building library
$(KTMP_LIB):        $(KTMP_EO) $(KTMP_DO) $(KTMP_LIBDEP)
	@echo "Building $@..."
	@$(MAKELIB) $(PLATFORM) "$(LD)" "$(LDFLAGS)" \
	   "$(SOFLAGS)" "$(KTMP_LIB)" $@  "$(KTMP_EO) $(KTMP_DO)" \
	   "$(ROOTLIBS) $(KTMP_FLAGS)"  -I/opt/include -Iinclude 

all-ktemplate:       $(KTMP_LIB) 


clean-ktemplate:
		@rm -f $(KTMP_DIRS)/*~ $(KTMP_DIRS)/*.o
		@rm -f $(KTMP_DC) $(KTMP_DH) $(KTMP_DEP) $(KTMP_LIB)

clean::         clean-ktemplate


#end

