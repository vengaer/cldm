$(call conditional-include-module,x86_64,avx2_support)
$(call conditional-include-module,aarch64,advsimd_support)

ifndef module_name
target := $(if $(MAKECMDGOALS),$(MAKECMDGOALS),recurse)

.PHONY: $(target)
$(target):
	@$(MAKE) -C .. $(MAKECMDGOALS) --no-print-directory
endif
