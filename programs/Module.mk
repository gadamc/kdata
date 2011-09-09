# Module.mk for KPROG module
#
# All .cxx files found in this directory (KDATA_ROOT/programs) 
# will be compiled and linked to the ROOT and KDATA Libraries
#

# Author: Adam Cox Sep 7 2011

MODDIR       := programs
KPROG_FLAGS  := $(CXXFLAGS)

KPROG_DIRS   := $(MODDIR)

KPROG_CXX    := $(wildcard $(KPROG_DIRS)/*.cxx)
KPROG_EXE    := $(patsubst $(KPROG_DIRS)/%.cxx,bin/%,$(KPROG_CXX))

ALLEXECS     += $(KPROG_EXE)
 

##### local rules #####

# rule for compiling our source files
$(KPROG_DIRS)/%.o:    $(KPROG_DIRS)/%.cxx
	$(CXX) $(OPT) $(KPROG_FLAGS) $(ROOTINCS) $(FFTWINCS) $(CURLINCS) -I$(ERAINCS) -o $@ -c $< 

# rule for building executables
bin/%: $(KPROG_DIRS)/%.o $(KDATAED_LIB) 
		@echo "=== Linking $@ ==="
		$(LD) $(LDFLAGS) -o $@ $< $(ALLKDATALIBS) $(ROOTLIBS) $(SYSLIBS) $(FFTWLIBS) $(CURLLIBS)
                

clean-programs:
		@rm -f $(KPROG_DIRS)/*~ $(KPROG_DIRS)/*.o $(KPROG_EXE)

clean::         clean-programs


#end

