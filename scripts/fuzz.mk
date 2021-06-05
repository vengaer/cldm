FUZZCC                   := clang
FUZZTARGET               := CLDM_FUZZTARGET

fuzzdir                   = $(testdir)/fuzz

FUZZTIME                 ?= 240
FUZZVALPROF              := 1
FUZZTIMEOUT              := 30
FUZZLEN                  := 8192
CORPUSDIR                 = $(fuzzdir)/corpora/$(CLDM_FUZZTARGET)
FUZZFLAGS                 = -max_len=$(FUZZLEN) -max_total_time=$(FUZZTIME) -use_value_profile=$(FUZZVALPROF) \
                            -timeout=$(FUZZTIMEOUT) $(CORPUSDIR)

export LLVM_PROFILE_FILE  = $(builddir)/.fuzz.profraw

# CORPORA should be passed when invoking make
MERGEFLAGS                = -merge=1 $(CORPUSDIR) $(CORPORA)

PROFDATA                  = $(builddir)/.fuzz.profdata
PROFFLAGS                 = merge -sparse $(LLVM_PROFILE_FILE) -o $(PROFDATA)

COVFLAGS                  = show $(cldmfuzz) -instr-profile=$(PROFDATA)
COVREPFLAGS               = report $(cldmfuzz) -instr-profile=$(PROFDATA)

fuzzinstr                := -fsanitize=fuzzer,address,undefined -fprofile-instr-generate -fcoverage-mapping

# $(call require-corpora,RULE)
define require-corpora
$(if $(findstring -_-$(MAKECMDGOALS)-_-,-_-$(1)-_-),$(if $(CORPORA),,$(error CORPORA is empty)))
endef

# $(call require-fuzztarget,RULE)
define require-fuzztarget
$(if $(findstring -_-$(MAKECMDGOALS)-_-,-_-$(1)-_-),$(if $($(FUZZTARGET)),,$(error $(FUZZTARGET) is empty)))
endef

# $(call gen-fuzz-rules,SRC)
define gen-fuzz-rules
$(foreach __src,$(1),
    $(eval __obj           := $(module_builddir)/$(notdir $(patsubst %.$(cext),%.$(oext),$(__src))))
    $(eval $(cldmfuzz)_obj += $(__obj))
    $(eval
        $(__obj): $(__src) $(cconfig) Makefile
	        $(QUIET)$(ECHO) "[CC]  $$(notdir $$@)"
	        $(QUIET)$(FUZZCC) -o $$@ $$< $(fuzzcflags) $(CPPFLAGS)))
endef
