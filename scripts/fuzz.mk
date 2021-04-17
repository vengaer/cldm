FUZZCC                   := clang

fuzzdir                   = $(testdir)/fuzz

FUZZTIME                 := 240
FUZZVALPROF              := 1
FUZZTIMEOUT              := 30
FUZZLEN                  := 8192
CORPUSDIR                 = $(fuzzdir)/corpora
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
$(if $(findstring $(1),$(MAKECMDGOALS)),$(if $(CORPORA),,$(error CORPORA is empty)))
endef
