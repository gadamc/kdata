##### List of KDATA modules to build #####

MODULES      := kds era ktemplate ksamba

### kera depends on kpta now, so it must come afterwards in this list. 
ifneq ($(FFTW_DIR),)
MODULES      += kpta kera kqpa 
endif

