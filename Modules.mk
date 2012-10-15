##### List of KDATA modules to build #####

MODULES      := kds ksamba 

#MODULES     += ktemplate  

#ifneq ($(FFTW_DIR),)
MODULES      += kqpa kpta kamping
#endif

MODULES += $(if $(CURL_DIR),kdatabase,)
