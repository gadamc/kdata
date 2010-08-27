##### List of KDATA modules to build #####

MODULES      := kds era kera

ifneq ($(FFTW_DIR),)
MODULES      += kpsa
endif

