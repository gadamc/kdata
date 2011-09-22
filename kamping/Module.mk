# Module.mk for KAMPING module
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
#      add $(KAMPING_LIB) to the dependencies and -lktmp to the link line for
#      building the executables.
#   6) Type "make" (or gmake) in the $KDATA_ROOT directory to build your new
#      modules.
#
# Author: Adam Cox 08/19/10

MODNAME      := kamping
MODDIR       := kamping

KAMPING_FLAGS  := $(CXXFLAGS)

KAMPING_DIR    := $(MODDIR)
KAMPING_DIRS   := $(MODDIR)
KAMPING_DIRI   := $(MODDIR)


# Uncomment this to use the LinkDef file when generating the dictionary
#KAMPING_LH     := $(KAMPING_DIRI)/$(MODNAME)_LinkDef.h
KAMPING_DC     := $(KAMPING_DIRS)/$(MODNAME)_Dict.C
KAMPING_DO     := $(KAMPING_DC:.C=.o)
KAMPING_DH     := $(KAMPING_DC:.C=.h)

KAMPING_H      := $(filter-out $(KAMPING_LH) $(KAMPING_DH),$(wildcard $(KAMPING_DIRI)/*.h))
KAMPING_ECXX   := $(wildcard $(KAMPING_DIRS)/K*.cxx)
KAMPING_CXX    := $(filter-out $(KAMPING_ECXX),$(wildcard $(KAMPING_DIRS)/*.cxx))
KAMPING_O      := $(KAMPING_CXX:.cxx=.o)
KAMPING_EO     := $(KAMPING_ECXX:.cxx=.o)
KAMPING_EH     := $(KAMPING_ECXX:.cxx=.h)
KAMPING_DICTH  := $(KAMPING_EH:.h=.h+)

KAMPING_EXE    := $(patsubst $(KAMPING_DIRS)/%.cxx,bin/%,$(KAMPING_CXX))

KAMPINGLIBS	   := $(patsubst $(LPATH)/lib%.$(SOEXT),-l%,$(KAMPING_LIB))

KAMPING_DEP    := $(KAMPING_O:.o=.d) $(KAMPING_EO:.o=.d)

# only depend on our dictionary if we are building a library
ifneq ($(KAMPING_LIB),)
KAMPING_DEP    += $(KAMPING_DO:.o=.d)
endif

# used in the main Makefile
ALLHDRS      += $(patsubst $(KAMPING_DIRI)/%.h,include/%.h,$(KAMPING_H))
ifneq ($(KAMPING_EO),)
ALLLIBS      += $(KAMPING_LIB)
endif
ALLEXECS     += $(KAMPING_EXE)
ALLKDATALIBS += $(KAMPINGLIBS)

KAMPING_XTRALIBS := $(KDSLIBS) $(KPTALIBS)


# include all dependency files
INCLUDEFILES += $(KAMPING_DEP)

# include local MyConfig.mk file if required
-include $(KAMPING_DIR)/MyConfig.mk

##### local rules #####

# we depend on all of our header files being up to date in the include directory
include/%.h:    $(KAMPING_DIRI)/%.h
		$(COPY_HEADER) $< $@

    
# rule for compiling our source files
$(KAMPING_DIRS)/%.o:    $(KAMPING_DIRS)/%.cxx
	$(CXX) $(OPT) $(KAMPING_FLAGS) $(ROOTINCS)  -o $@ -c $< 

# rule for building executables
bin/%: $(KAMPING_DIRS)/%.o $(KDATAED_LIB) 
		@echo "=== Linking $@ ==="
		$(LD) $(LDFLAGS) -o $@ $< $(KDATALIBDIRS) $(ROOTLIBS) $(SYSLIBS) $(KAMPINGLIBS) $(KAMPING_XTRALIBS)
                
# rules for building dictionary
$(KAMPING_DO):         $(KAMPING_DC)
	$(CXX) $(NOOPT) $(KAMPING_FLAGS) $(ROOTINCS) -I. -o $@ -c $< 

$(KAMPING_DC):         $(KAMPING_EH) $(KAMPING_LH)
	@echo "Generating dictionary $@..."
	$(ROOTCINT) -f $@ $(ROOTCINTFLAGS) $(KAMPING_DICTH) $(KAMPING_LH) 

# rule for building library
$(KAMPING_LIB):        $(KAMPING_EO) $(KAMPING_DO) $(KAMPING_LIBDEP)
	@echo "Building $@..."
	@$(MAKELIB) $(PLATFORM) "$(LD)" "$(LDFLAGS)" \
	   "$(SOFLAGS)" "$(KAMPING_LIB)" $@  "$(KAMPING_EO) $(KAMPING_DO)" \
	   "$(ROOTLIBS) $(KAMPING_FLAGS)"  -I/opt/include -Iinclude 

all-kamping:       $(KAMPING_LIB) 


clean-kamping:
		@rm -f $(KAMPING_DIRS)/*~ $(KAMPING_DIRS)/*.o
		@rm -f $(KAMPING_DC) $(KAMPING_DH) $(KAMPING_DEP) $(KAMPING_LIB)

clean::         clean-kamping


#end

