# Module.mk for KQPA module
#
# This is a generic qpa module that will build all
# files named K*.cxx into libKQPA, and all other .cxx
# files into individual executables. 
#
# To build this module in the KDATA distribution, add
# 'KQPA' to Modules.mk in the $KDATA_ROOT directory,
# then type "make" (or gmake) in $KDATA_ROOT.
#
# To use this qpa as the base for developing a standalone
# module, follow these steps:
#
#   1) Create a new directory for your development project.
#   2) Make soft links to Makefile and config in $KDATA_ROOT.
#   3) Create a Modules.mk file defining your development modules in the new dir.
#   4) Put each of your development modules in a separate directory with
#      the same name as the module, with a Module.mk and Module_libs.mk file
#      in each directory.  You can use this file and Module_libs.mk as a basis
#      for any module, but you should replace "KQPA" throughout with a unique
#      name specific to your module.  You may also have to make changes to the
#      compile and link commands depending on the structure of your module.
#   5) If your executables depend on the library produced by this module, then
#      add $(KQPA_LIB) to the dependencies and -lKQPA to the link line for
#      building the executables.
#   6) Type "make" (or gmake) in the $KDATA_ROOT directory to build your new
#      modules.
#
# Author: Adam Cox 08/19/10

MODNAME      := kqpa
MODDIR       := kqpa

KQPA_FLAGS  := $(CXXFLAGS)
KQPA_LDFLAGS := $(LDFLAGS)
KQPA_OPT     := $(OPT)



KQPA_OPT  := $(filter-out -O2,$(KQPA_OPT))
KQPA_LDFLAGS := $(filter-out -O2,$(KQPA_LDFLAGS))

#adding debugging flags here
#KQPA_FLAGS += -D_K_DEBUG_ERAEVENTFINDER
#KQPA_FLAGS += -D_K_DEBUG_FILETRANSFER


KQPA_DIR    := $(MODDIR)
KQPA_DIRS   := $(MODDIR)
KQPA_DIRI   := $(MODDIR)

#only need to put the ERAINCS here because ERA files are NOT copied
# to the projects include directory! This should be changed.
KQPA_XTRAINCS := $(ERAINCS)  

#list all external module libs that this module depends on
#if this module depends on other modules in this project you MUST
#make sure that this MODNAME is listed AFTER all of the MODNAMEs
#that it depends on.
KQPA_XTRALIBS := $(ERALIBS) $(KDSLIBS) $(KPTALIBS)
KQPA_LIBDEP   := $(ERA_LIB) $(KDS_LIB) $(KPTA_LIB)

# Uncomment this to use the LinkDef file when generating the dictionary
#KQPA_LH     := $(KQPA_DIRI)/$(MODNAME)_LinkDef.h
KQPA_DC     := $(KQPA_DIRS)/$(MODNAME)_Dict.C
KQPA_DO     := $(KQPA_DC:.C=.o)
KQPA_DH     := $(KQPA_DC:.C=.h)

KQPA_H      := $(filter-out $(KQPA_LH) $(KQPA_DH),$(wildcard $(KQPA_DIRI)/*.h))
KQPA_ECXX   := $(wildcard $(KQPA_DIRS)/K*.cxx)
KQPA_CXX    := $(filter-out $(KQPA_ECXX),$(wildcard $(KQPA_DIRS)/*.cxx))
KQPA_O      := $(KQPA_CXX:.cxx=.o)
KQPA_EO     := $(KQPA_ECXX:.cxx=.o)
KQPA_EH     := $(KQPA_ECXX:.cxx=.h)
KQPA_DICTH  := $(KQPA_EH:.h=.h+)

KQPA_EXE    := $(patsubst $(KQPA_DIRS)/%.cxx,bin/%,$(KQPA_CXX))

KQPALIBS	   := $(patsubst $(LPATH)/lib%.$(SOEXT),-l%,$(KQPA_LIB))

KQPA_DEP    := $(KQPA_O:.o=.d) $(KQPA_EO:.o=.d)

# only depend on our dictionary if we are building a library
ifneq ($(KQPA_LIB),)
KQPA_DEP    += $(KQPA_DO:.o=.d)
endif

# used in the main Makefile
ALLHDRS      += $(patsubst $(KQPA_DIRI)/%.h,include/%.h,$(KQPA_H))
ifneq ($(KQPA_EO),)
ALLLIBS      += $(KQPA_LIB)
endif
ALLEXECS     += $(KQPA_EXE)

# include all dependency files
INCLUDEFILES += $(KQPA_DEP)

# include local MyConfig.mk file if required
-include $(KQPA_DIR)/MyConfig.mk

##### local rules #####

# we depend on all of our header files being up to date in the include directory
include/%.h:    $(KQPA_DIRI)/%.h
		$(COPY_HEADER) $< $@

# rule for compiling our source files
$(KQPA_DIRS)/%.o:    $(KQPA_DIRS)/%.cxx
	$(CXX) $(KQPA_OPT) $(KQPA_FLAGS) $(ROOTINCS) -I$(KQPA_XTRAINCS) -o $@ -c $< 

# rule for building executables
bin/%: $(KQPA_DIRS)/%.o $(KDATAED_LIB) $(KQPA_LIBDEP)
		@echo "=== Linking $@ ==="
		$(LD) $(KQPA_LDFLAGS) -o $@ $< $(KDATALIBDIRS) $(ROOTLIBS) $(SYSLIBS) $(KQPALIBS) $(KQPA_XTRALIBS)
                
# rules for building dictionary
$(KQPA_DO):         $(KQPA_DC)
	$(CXX) $(NOOPT) $(KQPA_FLAGS) $(ROOTINCS) -I. -I$(KQPA_XTRAINCS) -o $@ -c $< 

$(KQPA_DC):         $(KQPA_EH) $(KQPA_LH)
	@echo "Generating dictionary $@..."
	$(ROOTCINT) -f $@ $(ROOTCINTFLAGS) -I$(KQPA_XTRAINCS)  $(KQPA_DICTH) $(KQPA_LH) 

# rule for building library
$(KQPA_LIB):        $(KQPA_EO) $(KQPA_DO) $(KQPA_LIBDEP)
	@echo "Building $@..."
	@$(MAKELIB) $(PLATFORM) "$(LD)" "$(KQPA_LDFLAGS)" \
	   "$(SOFLAGS)" "$(KQPA_LIB)" $@  "$(KQPA_EO) $(KQPA_DO) $(KQPA_XTRALIBS)"\
	   "$(ROOTLIBS)  $(KQPA_FLAGS)"  -I/opt/include -Iinclude 

all-kqpa:       $(KQPA_LIB) 
  
clean-kqpa:
		@rm -f $(KQPA_DIRS)/*~ $(KQPA_DIRS)/*.o
		@rm -f $(KQPA_DC) $(KQPA_DH) $(KQPA_DEP) $(KQPA_LIB)

clean::         clean-kqpa


	
#prepare::       prepare-kqpa

#end


