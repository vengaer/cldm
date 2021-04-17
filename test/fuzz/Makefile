fuzzsrc          := $(wildcard $(module_path)/*.$(cext))

module_build_dir := $(patsubst $(root)/%,$(builddir)/%,$(module_path))

define gen-fuzz-rules
$(foreach __src,$(fuzzsrc),
    $(eval __obj           := $(module_builddir)/$(notdir $(patsubst %.$(cext),%.$(oext),$(__src))))
    $(eval $(cldmfuzz)_obj += $(__obj))
    $(eval
        $(__obj): $(__src)
	        $(QUIET)$(ECHO) "[CC]  $$(notdir $$@)"
	        $(QUIET)$(FUZZCC) -o $$@ $$< $(fuzzcflags) $(CPPFLAGS)))
endef

$(call gen-fuzz-rules)

$(call gen-c-rules,$(fuzzsrc))

ifndef module_name
target  := $(if $(MAKECMDGOALS),$(MAKECMDGOALS),recurse)

.PHONY: $(target)
$(target):
	@$(MAKE) -C .. $(MAKECMDGOALS) --no-print-directory
endif
