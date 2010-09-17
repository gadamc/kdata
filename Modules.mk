##### List of KDATA modules to build #####

MODULES      := kds era 

### kera depends on kpsa now, so it must come afterwards in this list. 
ifneq ($(FFTW_DIR),)
MODULES      += kpsa kera
endif

