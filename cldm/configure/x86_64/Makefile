avx2_target   := $(builddir)/procfeature
avx2_obj      := $(patsubst $(root)/%.$(asext),$(builddir)/%.$(oext),$(wildcard $(module_path)/*.$(asext)))

$(avx2_target): $(avx2_obj)
	$(info [LD]  $(notdir $@))
	$(QUIET)$(LD) -o $@ $<

$(proc_mk): $(avx2_target)
	$(QUIET)$< $@

include $(proc_mk)

ifndef module_name
target := $(if $(MAKECMDGOALS),$(MAKECMDGOALS),recurse)

.PHONY: $(target)
$(target):
	@$(MAKE) -C .. $(MAKECMDGOALS) --no-print-directory
endif
