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
# This is not a standard module. It is intended to be ROOT independent.
# For now, the only ROOT parts that are used are to create a dictionary
# and link it to the rest of the objects in order to be able to access
# these classes from within ROOT CINT. The idea is that this module
# could be a pulse shape analysis library that is completely indepenent
# of ROOT. 
#
# However, if this restriction is debilitating for development of 
# this library or for physics reasons, we should reconsider how this
# is done. In that case, please contact the KData Czar (Adam Cox) 
# to discuss this. 
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
	$(CXX) $(OPT) $(CXXFLAGS) $(FFTWINCS) -o $@ -c $< 

# rule for building executables
bin/%: $(KPSA_DIRS)/%.o $(KDATAED_LIB) 
		@echo "=== Linking $@ ==="
		$(LD) $(LDFLAGS) -o $@ $< $(KDATALIBDIRS) $(SYSLIBS) $(KPSALIBS) $(FFTWLIBS)
                
# rules for building dictionary
$(KPSA_DO):         $(KPSA_DC)
	$(CXX) $(NOOPT) $(KPSA_FLAGS) $(ROOTINCS) -I. -o $@ -c $< 

$(KPSA_DC):         $(KPSA_EH) $(KPSA_LH)
	@echo "Generating dictionary $@..."
	$(ROOTCINT) -f $@ $(ROOTCINTFLAGS) $(FFTWINCS) $(KPSA_EH) $(KPSA_LH) 

# rule for building library
$(KPSA_LIB):        $(KPSA_EO) $(KPSA_DO) $(KPSA_LIBDEP)
	@echo "Building $@..."
	@$(MAKELIB) $(PLATFORM) "$(LD)" "$(LDFLAGS)" \
	   "$(SOFLAGS)" "$(KPSA_LIB)" $@  "$(KPSA_EO) $(KPSA_DO)" \
	   "$(FFTWLIBS) $(KPSA_FLAGS)"  -I/opt/include -Iinclude 

all-kpsa:       $(KPSA_LIB)

clean-kpsa:
		@rm -f $(KPSA_DIRS)/*~ $(KPSA_DIRS)/*.o
		@rm -f $(KPSA_DC) $(KPSA_DH) $(KPSA_DEP) $(KPSA_LIB)

clean::         clean-kpsa

#end

