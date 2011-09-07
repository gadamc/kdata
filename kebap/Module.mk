# Module.mk for kEeventBuilder module
#
# This is a generic template module that will build all
# files named K*.cxx into libkeb, and all other .cxx
# files into individual executables. 
#
# To build this module in the KDATA distribution, add
# 'KEventBuilder' to Modules.mk in the $KDATA_ROOT directory,
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
#      for any module, but you should replace "KEB" throughout with a unique
#      name specific to your module.  You may also have to make changes to the
#      compile and link commands depending on the structure of your module.
#   5) If your executables depend on the library produced by this module, then
#      add $(KEB_LIB) to the dependencies and -lkeb to the link line for
#      building the executables.
#   6) Type "make" (or gmake) in the $KDATA_ROOT directory to build your new
#      modules.
#
# Author: Adam Cox 08/19/10

MODNAME      := kebap
MODDIR       := kebap

KEB_FLAGS  := $(CXXFLAGS)

KEB_DIR    := $(MODDIR)
KEB_DIRS   := $(MODDIR)
KEB_DIRI   := $(MODDIR)


# Uncomment this to use the LinkDef file when generating the dictionary
#KEB_LH     := $(KEB_DIRI)/$(MODNAME)_LinkDef.h
KEB_DC     := $(KEB_DIRS)/$(MODNAME)_Dict.C
KEB_DO     := $(KEB_DC:.C=.o)
KEB_DH     := $(KEB_DC:.C=.h)

KEB_H      := $(filter-out $(KEB_LH) $(KEB_DH),$(wildcard $(KEB_DIRI)/*.h))
KEB_ECXX   := $(wildcard $(KEB_DIRS)/K*.cxx)
KEB_ECXX   := $(filter-out KMergeRoot.cxx, $(KEB_ECXX))
KEB_CXX    := $(filter-out $(KEB_ECXX),$(wildcard $(KEB_DIRS)/*.cxx))
KEB_O      := $(KEB_CXX:.cxx=.o)
KEB_EO     := $(KEB_ECXX:.cxx=.o)
KEB_EH     := $(KEB_ECXX:.cxx=.h)

KEB_EXE    := $(patsubst $(KEB_DIRS)/%.cxx,bin/%,$(KEB_CXX))

KEBLIBS	   := $(patsubst $(LPATH)/lib%.$(SOEXT),-l%,$(KEB_LIB))

KEB_DEP    := $(KEB_O:.o=.d) $(KEB_EO:.o=.d)

#list all external module libs that this module depends on
#if this module depends on other modules in this project you MUST
#make sure that this MODNAME is listed AFTER all of the MODNAMEs
#that it depends on.
KEB_XTRALIBS :=  $(KDSLIBS)
KEB_LIBDEP   :=  $(KDS_LIB)

# only depend on our dictionary if we are building a library
ifneq ($(KEB_LIB),)
KEB_DEP    += $(KEB_DO:.o=.d)
endif

# used in the main Makefile
ALLHDRS      += $(patsubst $(KEB_DIRI)/%.h,include/%.h,$(KEB_H))
ifneq ($(KEB_EO),)
ALLLIBS      += $(KEB_LIB)
endif
ALLEXECS     += $(KEB_EXE)
ALLKDATALIBS += $(KEBLIBS)



# include all dependency files
INCLUDEFILES += $(KEB_DEP)

# include local MyConfig.mk file if required
-include $(KEB_DIR)/MyConfig.mk

##### local rules #####

# we depend on all of our header files being up to date in the include directory
include/%.h:    $(KEB_DIRI)/%.h
		$(COPY_HEADER) $< $@
		@echo "Executaples to my knowldge..."

    
# rule for compiling our source files
$(KEB_DIRS)/%.o:    $(KEB_DIRS)/%.cxx
	@echo "Compiling source files..."
	$(CXX) $(OPT) $(KEB_FLAGS) $(ROOTINCS)  -o $@ -c $< 
    

# rule for building executables
bin/%: $(KEB_DIRS)/%.o $(KDATAED_LIB) $(KEB_LIBDEP)
		@echo "=== Linking $@ ==="
		$(LD) $(LDFLAGS) -o $@ $< $(KDATALIBDIRS) $(ROOTLIBS) $(SYSLIBS) $(KEBLIBS) $(KEB_XTRALIBS)
                
# rules for building dictionary
$(KEB_DO):         $(KEB_DC)
	@echo "Building dictionary $@..."
	$(CXX) $(NOOPT) $(KEB_FLAGS) $(ROOTINCS) -I. -o $@ -c $< 

$(KEB_DC):         $(KEB_EH) $(KEB_LH)
	@echo "Generating dictionary $@..."
	$(ROOTCINT) -f $@ $(ROOTCINTFLAGS) $(KEB_EH) $(KEB_LH) 

# rule for building library
$(KEB_LIB):        $(KEB_EO) $(KEB_DO) $(KEB_LIBDEP)
	@echo "Building $@..."
	@$(MAKELIB) $(PLATFORM) "$(LD)" "$(LDFLAGS)" \
	"$(SOFLAGS)" "$(KEB_LIB)" $@  "$(KEB_EO) $(KEB_DO) $(KEB_XTRALLIBS)" \
	"$(ROOTLIBS) $(KEB_FLAGS)"  -I/opt/include -Iinclude 

all-kebap:       $(KEB_LIB) 


clean-kebap:
	@rm -f $(KEB_DIRS)/*~ $(KEB_DIRS)/*.o
	@rm -f $(KEB_DC) $(KEB_DH) $(KEB_DEP) $(KEB_LIB)

clean::         clean-kebap


#end

