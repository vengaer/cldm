$(call mk-build-build-dir)

system_target := $(builddir)/cldm_system
system_obj    := $(patsubst $(root)/%.$(cext),$(builddir)/%.$(oext),$(wildcard $(module_path)/*.$(cext)))

$(system_target): $(system_obj)
	$(info [LD]  $(notdir $@))
	$(QUIET)$(CC) -o $@ $<

$(system_mk): $(system_target)
	$(QUIET)$< $@

avx2_target   := $(builddir)/avx2_support
avx2_obj      := $(patsubst $(root)/%.$(asext),$(builddir)/%.$(oext),$(module_path)/cldm_avx2_support.$(asext))

$(avx2_target): $(avx2_obj)
	$(info [LD]  $(notdir $@))
	$(QUIET)$(LD) -o $@ $<

$(avx2_mk): $(avx2_target)
	$(QUIET)$< $@

$(patsubst $(root)/%,$(builddir)/%,$(module_path))/%.$(oext): $(module_path)/%.$(cext) Makefile
	$(info [CC]  $(notdir $@))
	$(QUIET)$(CC) -o $@ $< $(CFLAGS) $(CPPFLAGS)

include $(system_mk)

ifeq ($(arch)_$(abi),64-bit_SYSV)
  include $(avx2_mk)
endif

ifndef module_name
target := $(if $(MAKECMDGOALS),$(MAKECMDGOALS),recurse)

.PHONY: $(target)
$(target):
	@$(MAKE) -C .. $(MAKECMDGOALS) --no-print-directory
endif
