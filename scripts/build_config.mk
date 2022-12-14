build_config :=

# $(call set-build-config)
define set-build-config
$(if $(findstring $(MAKECMDGOALS),test check),
    $(eval build_config := test),
  $(if $(findstring $(MAKECMDGOALS),fuzz fuzzrun fuzzmerge $(cldmfuzz)),
      $(eval build_config := fuzz),
    $(if $(findstring $(MAKECMDGOALS),unit $(cldmtest)),
        $(eval build_config := unit),
      $(if $(findstring $(MAKECMDGOALS),functional),
          $(eval build_config := functional),
        $(eval build_config := libcldm)))))
$(call set-build-config-flags)
endef

# $(call set-build-config-flags)
define set-build-config-flags
$(if $(findstring fuzz,$(build_config)),
    $(eval CPPFLAGS += -DCLDM_SUPPRESS_LOGGING)
    $(eval builddir := $(builddir)/fuzz))
endef
