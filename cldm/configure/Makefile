$(call mk-build-build-dir)

$(module_name)_target := $(builddir)/cldm_system
$(module_name)_obj    := $(patsubst $(root)/%.$(cext),$(builddir)/%.$(oext),$(wildcard $(module_path)/*.$(cext)))

$($(module_name)_target): $($(module_name)_obj)
	$(info [LD]  $(notdir $@))
	$(QUIET)$(CC) -o $@ $^

$(system_mk): $($(module_name)_target)
	$(QUIET)$^ $@

$(module_name)_target := $(builddir)/avx2_support
$(module_name)_obj    := $(patsubst $(root)/%.$(asext),$(builddir)/%.$(oext),$(wildcard $(module_path)/*.$(asext)))

$($(module_name)_target): $($(module_name)_obj)
	$(info [LD]  $(notdir $@))
	$(QUIET)$(LD) -o $@ $^

$(avx2_mk): $($(module_name)_target)
	$(QUIET)$^ $@

$(patsubst $(root)/%,$(builddir)/%,$(module_path))/%.$(oext): $(module_path)/%.$(cext)
	$(info [CC]  $(notdir $@))
	$(QUIET)$(CC) -o $@ $< $(CFLAGS) $(CPPFLAGS)

prepdeps  += $(system_mk)
include $(system_mk)

ifeq ($(arch)_$(abi),64-bit_SYSV)
  prepdeps += $(avx2_mk)
  include $(avx2_mk)
endif

ifndef module_name
target := $(if $(MAKECMDGOALS),$(MAKECMDGOALS),recurse)

.PHONY: $(target)
$(target):
	@$(MAKE) -C .. $(MAKECMDGOALS) --no-print-directory
endif
