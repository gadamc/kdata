##### List of KDATA modules to build #####

MODULES      := kds ksamba ktemplate

#ifneq ($(FFTW_DIR),)
MODULES      += kqpa kpta kamping
#endif

MODULES += $(if $(CURL_DIR),kdatabase,)
