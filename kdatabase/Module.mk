# Module.mk for KDATABASE module
#
# This is a generic template module that will build all
# files named K*.cxx into libKDATABASE, and all other .cxx
# files into individual executables. 
#
# To build this module in the KDATA distribution, add
# 'KDATABASE' to Modules.mk in the $KDATA_ROOT directory,
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
#      for any module, but you should replace "KDATABASE" throughout with a unique
#      name specific to your module.  You may also have to make changes to the
#      compile and link commands depending on the structure of your module.
#   5) If your executables depend on the library produced by this module, then
#      add $(KDATABASE_LIB) to the dependencies and -lKDATABASE to the link line for
#      building the executables.
#   6) Type "make" (or gmake) in the $KDATA_ROOT directory to build your new
#      modules.
#
# Author: Adam Cox 08/19/10

MODNAME      := kdatabase
MODDIR       := kdatabase

KDATABASE_FLAGS  := $(CXXFLAGS)
KDATABASE_LDFLAGS := $(LDFLAGS)
KDATABASE_OPT     := $(OPT)

#KDATABASE_OPT  += -g
KDATABASE_OPT  := $(filter-out -O2,$(KDATABASE_OPT))
KDATABASE_LDFLAGS := $(filter-out -O2,$(KDATABASE_LDFLAGS))

#adding debugging flags here
#KDATABASE_FLAGS += -D_K_DEBUG_ERAEVENTFINDER
#KDATABASE_FLAGS += -D_K_DEBUG_FILETRANSFER


KDATABASE_DIR    := $(MODDIR)
KDATABASE_DIRS   := $(MODDIR)
KDATABASE_DIRI   := $(MODDIR)

#only need to put the ERAINCS here because ERA files are NOT copied
# to the projects include directory! This should be changed.
KDATABASE_XTRAINCS := $(ERAINCS) 

#list all external module libs that this module depends on
#if this module depends on other modules in this project you MUST
#make sure that this MODNAME is listed AFTER all of the MODNAMEs
#that it depends on.
KDATABASE_XTRALIBS := $(ERALIBS) $(KDSLIBS) $(KPTALIBS)
KDATABASE_LIBDEP   := $(ERA_LIB) $(KDS_LIB) $(KPTA_LIB)

# Uncomment this to use the LinkDef file when generating the dictionary
#KDATABASE_LH     := $(KDATABASE_DIRI)/$(MODNAME)_LinkDef.h
KDATABASE_DC     := $(KDATABASE_DIRS)/$(MODNAME)_Dict.C
KDATABASE_DO     := $(KDATABASE_DC:.C=.o)
KDATABASE_DH     := $(KDATABASE_DC:.C=.h)

KDATABASE_H      := $(filter-out $(KDATABASE_LH) $(KDATABASE_DH),$(wildcard $(KDATABASE_DIRI)/*.h))
KDATABASE_ECXX   := $(wildcard $(KDATABASE_DIRS)/K*.cxx) $(wildcard $(KDATABASE_DIRS)/JSON*.cxx)
KDATABASE_CXX    := $(filter-out $(KDATABASE_ECXX),$(wildcard $(KDATABASE_DIRS)/*.cxx))
KDATABASE_O      := $(KDATABASE_CXX:.cxx=.o)
KDATABASE_EO     := $(KDATABASE_ECXX:.cxx=.o)
KDATABASE_EH     := $(KDATABASE_ECXX:.cxx=.h)
KDATABASE_DICTH  := $(KDATABASE_EH:.h=.h+)

KDATABASE_EXE    := $(patsubst $(KDATABASE_DIRS)/%.cxx,bin/%,$(KDATABASE_CXX))

KDATABASELIBS	   := $(patsubst $(LPATH)/lib%.$(SOEXT),-l%,$(KDATABASE_LIB))

KDATABASE_DEP    := $(KDATABASE_O:.o=.d) $(KDATABASE_EO:.o=.d)

# only depend on our dictionary if we are building a library
ifneq ($(KDATABASE_LIB),)
KDATABASE_DEP    += $(KDATABASE_DO:.o=.d)
endif

# used in the main Makefile
ALLHDRS      += $(patsubst $(KDATABASE_DIRI)/%.h,include/%.h,$(KDATABASE_H))
ifneq ($(KDATABASE_EO),)
ALLLIBS      += $(KDATABASE_LIB)
endif
ALLEXECS     += $(KDATABASE_EXE)

# include all dependency files
INCLUDEFILES += $(KDATABASE_DEP)

# include local MyConfig.mk file if required
-include $(KDATABASE_DIR)/MyConfig.mk

##### local rules #####

# we depend on all of our header files being up to date in the include directory
include/%.h:    $(KDATABASE_DIRI)/%.h
	$(COPY_HEADER) $< $@

# rule for compiling our source files
$(KDATABASE_DIRS)/%.o:    $(KDATABASE_DIRS)/%.cxx
	$(CXX) $(KDATABASE_OPT) $(KDATABASE_FLAGS) $(ROOTINCS) $(JANSSONINCS) $(CURLINCS) -I$(KDATABASE_XTRAINCS) -o $@ -c $< 

# rule for building executables
bin/%: $(KDATABASE_DIRS)/%.o $(KDATAED_LIB) $(KDATABASE_LIBDEP)
		@echo "=== Linking $@ ==="
		$(LD) $(KDATABASE_LDFLAGS) -o $@ $< $(KDATALIBDIRS) $(JANSSONLIBS) $(CURLLIBS) $(ROOTLIBS) $(SYSLIBS) $(KDATABASELIBS) $(KDATABASE_XTRALIBS)
                
# rules for building dictionary
$(KDATABASE_DO):         $(KDATABASE_DC)
	$(CXX) $(NOOPT) $(KDATABASE_FLAGS) $(ROOTINCS) -I. -I$(KDATABASE_XTRAINCS) -o $@ -c $< 

$(KDATABASE_DC):         $(KDATABASE_EH) $(KDATABASE_LH)
	@echo "Generating dictionary $@..."
	$(ROOTCINT) -f $@ $(ROOTCINTFLAGS) -I$(KDATABASE_XTRAINCS)  $(KDATABASE_DICTH) $(KDATABASE_LH) 

# rule for building library
$(KDATABASE_LIB):        $(KDATABASE_EO) $(KDATABASE_DO) $(KDATABASE_LIBDEP) 
	@echo "Building $@..."
	@$(MAKELIB) $(PLATFORM) "$(LD)" "$(KDATABASE_LDFLAGS)" \
	   "$(SOFLAGS)" "$(KDATABASE_LIB)" $@  "$(KDATABASE_EO) $(KDATABASE_DO) $(KDATABASE_XTRALIBS) "\
	   "$(JANSSONLIBS) $(CURLLIBS) $(ROOTLIBS) $(KDATABASE_FLAGS)"  -I/opt/include -Iinclude 

all-kdatabase:       $(KDATABASE_LIB) 

clean-kdatabase:
		@rm -f $(KDATABASE_DIRS)/*~ $(KDATABASE_DIRS)/*.o
		@rm -f $(KDATABASE_DC) $(KDATABASE_DH) $(KDATABASE_DEP) $(KDATABASE_LIB)

clean::         clean-kdatabase
  
#prepare::       prepare-kdatabase
#end


