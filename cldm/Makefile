trivial_module := y
required_by    := lcldm

$(call include-module,main)
$(call include-module,configure)
$(call conditional-include-module,avx2,avx2_support)

ifndef module_name
target := $(if $(MAKECMDGOALS),$(MAKECMDGOALS),recurse)

.PHONY: $(target)
$(target):
	@$(MAKE) -C .. $(MAKECMDGOALS) --no-print-directory
endif
