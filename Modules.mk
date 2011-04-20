##### List of KDATA modules to build #####

MODULES      := kds era ktemplate ksamba

### kera depends on kpta now, so it must come afterwards in this list. 
ifneq ($(FFTW_DIR),)
MODULES      += kpta kera kqpa
endif

MODULES += $(if $(and $(CURL_DIR),$(JANSSON_DIR)),kdatabase,)

#MODULES += $(if $(and $(CURL_DIR),$(JANSSON_DIR),$(FFTW_DIR)),kdsplay,)