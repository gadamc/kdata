# Module.mk for KPTA module
#
# This is a generic template module that will build all
# files named K*.cxx into libkpta, and all other .cxx
# files into individual executables. 
#
# To build this module in the KDATA distribution, add
# 'kpta' to Modules.mk in the $KDATA_ROOT directory,
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

MODNAME      := kpta
MODDIR       := kpta

KPTA_FLAGS  := $(CXXFLAGS)

KPTA_DIR    := $(MODDIR)
KPTA_DIRS   := $(MODDIR)
KPTA_DIRI   := $(MODDIR)

# Uncomment this to use the LinkDef file when generating the dictionary
#KPTA_LH     := $(KPTA_DIRI)/$(MODNAME)_LinkDef.h
KPTA_DC     := $(KPTA_DIRS)/$(MODNAME)_Dict.C
KPTA_DO     := $(KPTA_DC:.C=.o)
KPTA_DH     := $(KPTA_DC:.C=.h)

KPTA_H      := $(filter-out $(KPTA_LH) $(KPTA_DH),$(wildcard $(KPTA_DIRI)/*.h))
KPTA_ECXX   := $(wildcard $(KPTA_DIRS)/K*.cxx)
KPTA_CXX    := $(filter-out $(KPTA_ECXX),$(wildcard $(KPTA_DIRS)/*.cxx))
KPTA_O      := $(KPTA_CXX:.cxx=.o)
KPTA_EO     := $(KPTA_ECXX:.cxx=.o)
KPTA_EH     := $(KPTA_ECXX:.cxx=.h)

KPTA_EXE    := $(patsubst $(KPTA_DIRS)/%.cxx,bin/%,$(KPTA_CXX))

KPTALIBS	   := $(patsubst $(LPATH)/lib%.$(SOEXT),-l%,$(KPTA_LIB))

KPTA_DEP    := $(KPTA_O:.o=.d) $(KPTA_EO:.o=.d)

# only depend on our dictionary if we are building a library
ifneq ($(KPTA_LIB),)
KPTA_DEP    += $(KPTA_DO:.o=.d)
endif

# used in the main Makefile
ALLHDRS      += $(patsubst $(KPTA_DIRI)/%.h,include/%.h,$(KPTA_H))
ifneq ($(KPTA_EO),)
ALLLIBS      += $(KPTA_LIB)
endif
ALLEXECS     += $(KPTA_EXE)

# include all dependency files
INCLUDEFILES += $(KPTA_DEP)

# include local MyConfig.mk file if required
-include $(KPTA_DIR)/MyConfig.mk

##### local rules #####

# we depend on all of our header files being up to date in the include directory
include/%.h:    $(KPTA_DIRI)/%.h
		$(COPY_HEADER) $< $@

# rule for compiling our source files
$(KPTA_DIRS)/%.o:    $(KPTA_DIRS)/%.cxx
	$(CXX) $(OPT) $(KPTA_FLAGS) $(FFTWINCS) -o $@ -c $< 

# rule for building executables
bin/%: $(KPTA_DIRS)/%.o $(KDATAED_LIB) 
		@echo "=== Linking $@ ==="
		$(LD) $(LDFLAGS) -o $@ $< $(KDATALIBDIRS) $(SYSLIBS) $(KPTALIBS) $(FFTWLIBS)
                
# rules for building dictionary
$(KPTA_DO):         $(KPTA_DC)
	$(CXX) $(NOOPT) $(KPTA_FLAGS) $(ROOTINCS) -I. -o $@ -c $< 

$(KPTA_DC):         $(KPTA_EH) $(KPTA_LH)
	@echo "Generating dictionary $@..."
	$(ROOTCINT) -f $@ $(ROOTCINTFLAGS) $(KPTA_EH) $(KPTA_LH) 

# rule for building library
$(KPTA_LIB):        $(KPTA_EO) $(KPTA_DO) $(KPTA_LIBDEP)
	@echo "Building $@..."
	@$(MAKELIB) $(PLATFORM) "$(LD)" "$(LDFLAGS)" \
	   "$(SOFLAGS)" "$(KPTA_LIB)" $@  "$(KPTA_EO) $(KPTA_DO)" \
	   "$(FFTWLIBS) $(KPTA_FLAGS)"  -I/opt/include -Iinclude 

all-kpta:       $(KPTA_LIB)

clean-kpta:
		@rm -f $(KPTA_DIRS)/*~ $(KPTA_DIRS)/*.o
		@rm -f $(KPTA_DC) $(KPTA_DH) $(KPTA_DEP) $(KPTA_LIB)

clean::         clean-kpta

#end

