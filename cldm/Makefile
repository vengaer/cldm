trivial_module := y
required_by    := lcldm cldmfuzz

$(call include-module,main)
$(call include-module,configure)
$(call include-module,asm)

ifndef module_name
target := $(if $(MAKECMDGOALS),$(MAKECMDGOALS),recurse)

.PHONY: $(target)
$(target):
	@$(MAKE) -C .. $(MAKECMDGOALS) --no-print-directory
endif
