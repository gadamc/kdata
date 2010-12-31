##### List of KDATA modules to build #####

MODULES      := kds era ksamba 

#kmuon

### kmuon is a set of tools for analyzing the muon veto data found in KData files, and thus depends on the 'kds' module. 


### kera depends on kpta now, so it must come afterwards in this list. 
ifneq ($(FFTW_DIR),)
MODULES      += kpta kera
endif

