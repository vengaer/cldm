fuzzsrc          := $(wildcard $(module_path)/*.$(cext))

$(call gen-fuzz-rules,$(fuzzsrc))

ifndef module_name
target  := $(if $(MAKECMDGOALS),$(MAKECMDGOALS),recurse)

.PHONY: $(target)
$(target):
	@$(MAKE) -C .. $(MAKECMDGOALS) --no-print-directory
endif
