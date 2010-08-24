##### List of KDATA modules to build #####

MODULES      := era kds kpsa

#kds requires era now if its going to use TRefs to get to pulse traces. 

#ifneq ($(FFTW_DIR),)
#MODULES      +=
#endif

