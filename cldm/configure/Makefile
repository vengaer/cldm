$(call mk-build-build-dir)

$(module_name)_target := $(builddir)/cldm_configure
$(module_name)_obj    := $(patsubst $(root)/%.$(cext),$(builddir)/%.$(oext),$(wildcard $(module_path)/*.$(cext)))

$($(module_name)_target): $($(module_name)_obj)
	$(info [LD]  $(notdir $@))
	$(QUIET)$(CC) -o $@ $^

$(patsubst $(root)/%,$(builddir)/%,$(module_path))/%.$(oext): $(moduel_path)/%.$(cext)
	$(info [CC]  $@)
	$(QUIET)$(CC) $(filter-out -fPIC,$(CFLAGS)) $(CPPFLAGS) $@ $<

$(config_mk): $($(module_name)_target)
	$(QUIET)$^ $@

builddeps += $(eval include $(config_mk))

ifndef module_name
target := $(if $(MAKECMDGOALS),$(MAKECMDGOALS),recurse)

.PHONY: $(target)
$(target):
	@$(MAKE) -C .. $(MAKECMDGOALS) --no-print-directory
endif
