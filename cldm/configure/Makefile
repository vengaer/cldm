$(call mk-build-build-dir)

system_target := $(builddir)/sysfeature
system_obj    := $(patsubst $(root)/%.$(cext),$(builddir)/%.$(oext),$(wildcard $(module_path)/*.$(cext)))

$(system_target): $(system_obj)
	$(info [LD]  $(notdir $@))
	$(QUIET)$(CC) -o $@ $<

$(system_mk): $(system_target)
	$(QUIET)$(CROSS_EMU) $< $@

$(patsubst $(root)/%,$(builddir)/%,$(module_path))/%.$(oext): $(module_path)/%.$(cext) Makefile
	$(info [CC]  $(notdir $@))
	$(QUIET)$(CC) -o $@ $< $(CFLAGS) $(CPPFLAGS)

include $(system_mk)

$(if $(ARCH),$(eval machine := $(ARCH)))

ifeq ($(arch)_$(abi),64-bit_SYSV)
  $(if $(wildcard $(module_path)/$(machine)), $(call include-module,$(machine)))
endif


ifndef module_name
target := $(if $(MAKECMDGOALS),$(MAKECMDGOALS),recurse)

.PHONY: $(target)
$(target):
	@$(MAKE) -C .. $(MAKECMDGOALS) --no-print-directory
endif
