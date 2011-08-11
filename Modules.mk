##### List of KDATA modules to build #####

MODULES      := kds era ksamba kebap

### kera depends on kpta now, so it must come afterwards in this list. 
ifneq ($(FFTW_DIR),)
MODULES      += kpta kera kqpa ktemplate

endif

MODULES += $(if $(CURL_DIR),kdatabase,)
#MODULES += $(if $(and $(CURL_DIR),$(FFTW_DIR)),kdsplay,)
