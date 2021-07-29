aarch64_target   := $(builddir)/procfeature
aarch64_obj      := $(patsubst $(root)/%.$(asext),$(builddir)/%.$(oext),$(wildcard $(module_path)/*.$(asext)))

$(aarch64_target): $(aarch64_obj)
	$(info [LD]  $(notdir $@))
	$(QUIET)$(LD) -o $@ $<

$(proc_mk): $(aarch64_target)
	$(QUIET)$(CROSS_EMU) $< $@

include $(proc_mk)

ifndef module_name
target := $(if $(MAKECMDGOALS),$(MAKECMDGOALS),recurse)

.PHONY: $(target)
$(target):
	@$(MAKE) -C .. $(MAKECMDGOALS) --no-print-directory
endif
