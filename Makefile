include     scripts/modules.mk
include     scripts/fuzz.mk
include     scripts/build_config.mk

CC          ?= gcc
AS          := nasm

AR          ?= ar
LN          ?= ln
MKDIR       ?= mkdir
RM          ?= rm
ECHO        ?= echo
TOUCH       ?= touch
PYTEST      ?= pytest

cldmver     := 0.1.0

libstem     := libcldm
sover       := 0
socompat    := 0

asext       := asm
cext        := c
oext        := o
dext        := d
soext       := so
aext        := a

root        := $(abspath $(CURDIR))

builddir    := $(root)/build
libsrcdir   := $(root)/$(patsubst lib%,%,$(libstem))
testdir     := $(root)/test
srcdirs     := $(libsrcdir) $(testdir)

# Generated during prepare step
config_mk   := $(builddir)/config.mk

lcldm       := $(libstem).$(soext).$(sover)
link        := $(libstem).$(soext)
lcldm_main  := $(libstem)_main.$(aext)

cldmtest    := cldmtest
cldmfuzz    := cldmfuzz

cldmgen     := $(libsrcdir)/cldmgen.h
cldm_config := $(libsrcdir)/cldm_config.h

MOCKUPS     ?= $(abspath $(libsrcdir)/mockups.h)

ASFLAGS     := -felf64 -g -Fdwarf
CFLAGS      := -std=c99 -Wall -Wextra -Wpedantic -fPIC -c -MD -MP -g
CPPFLAGS     = -D_GNU_SOURCE -DCLDM_VERSION=$(cldmver) -I$(root)
LDFLAGS     := -shared -Wl,-soname,$(libstem).$(soext).$(socompat)
LDLIBS      := -ldl

testldflags := -L$(root)
testldlibs  := $(patsubst lib%,-l%,$(libstem)) $(patsubst lib%.a,-l%,$(lcldm_main))

fuzzcflags  := $(fuzzinstr) $(CFLAGS)
fuzzldflags := -L$(root) $(fuzzinstr)
fuzzldlibs  := $(patsubst lib%,-l%,$(libstem))

ARFLAGS     := -rcs
LNFLAGS     := -sf
MKDIRFLAGS  := -p
RMFLAGS     := -rf
ECHOFLAGS   := -e
PYTESTFLAGS := -v --rootdir=$(functestdir)

QUIET       ?= @

module_path := $(root)
module_mk   := Makefile

prepare     := $(builddir)/.prepare.stamp

prepdeps    :=

$(call set-build-config)

.PHONY: all
all: $(link) $(lcldm_main)

ifneq ($(MAKECMDGOALS),clean)
  $(call include-each-module,$(srcdirs))
endif

$(prepare): $(prepdeps)
	$(QUIET)$(TOUCH) $@

$(link): $(lcldm)
	$(info [LN]  $@)
	$(QUIET)$(LN) $(LNFLAGS) $(abspath $(CURDIR))/$^ $@

$(lcldm): $(lcldm_obj)
	$(info [LD]  $@)
	$(QUIET)$(CC) -o $@ $^ $(LDFLAGS) $(LDLIBS)

$(cldmtest): $(cldmtest_obj) $(link) $(lcldm_main)
	$(info [LD]  $@)
	$(QUIET)$(CC) -o $@ $(cldmtest_obj) $(testldflags) $(testldlibs)

$(cldmfuzz): $(cldmfuzz_obj) $(link)
	$(info [LD]  $@)
	$(QUIET)$(FUZZCC) -o $@ $(cldmfuzz_obj) $(fuzzldflags) $(fuzzldlibs)

$(lcldm_main): $(lcldm_main_obj)
	$(info [AR]  $@)
	$(QUIET)$(AR) $(ARFLAGS) $@ $^

$(builddir)/%.$(oext): $(root)/%.$(cext) $(cldmgen) $(cldm_config)
	$(info [CC]  $(notdir $@))
	$(QUIET)$(CC) -o $@ $< $(CFLAGS) $(CPPFLAGS)

$(builddir)/%.$(oext): $(root)/%.$(asext)
	$(info [AS]  $(notdir $@))
	$(QUIET)$(AS) -o $@ $< $(ASFLAGS)

$(cldmgen): $(MOCKUPS)
	$(info [GEN] $(notdir $@))
	$(QUIET)$(ECHO) $(ECHOFLAGS) '#ifndef CLDMGEN_H\n#define CLDMGEN_H\n#include "$^"\n#endif /* CLDMGEN_H */' > $@

$(cldm_config): $(prepare)
	$(info [GEN] $(notdir $@))
	$(QUIET)$(ECHO) $(ECHOFLAGS) '#ifndef CLDM_CONFIG_H\n#define CLDM_CONFIG_H\n' > $@
	$(QUIET)$(ECHO) $(ECHOFLAGS) '$(if $(filter y,$(has_generic)),#define CLDM_HAS_GENERIC)\n' >> $@
	$(QUIET)$(ECHO) $(ECHOFLAGS) '#define CLDM_ARCH_$(patsubst %-bit,%,$(arch))\n' >> $@
	$(QUIET)$(ECHO) $(ECHOFLAGS) '#define CLDM_ABI_$(abi)\n' >> $@
	$(QUIET)$(ECHO) $(ECHOFLAGS) '#endif /* CLDM_CONFIG_H */' >> $@

$(MOCKUPS):
	$(info [GEN] $(notdir $@))
	$(QUIET)$(TOUCH) $@

.PHONY: functional
functional:
	$(QUIET)$(PYTEST) $(PYTESTFLAGS)

.PHONY: unit
unit: $(cldmtest)
	$(QUIET)LD_LIBRARY_PATH=$(root) ./$<

.PHONY: fuzz
fuzz: $(cldmfuzz)

.PHONY: test
test: unit functional

.PHONY: check
check: test

.PHONY: fuzzrun
fuzzrun: $(call require-fuzztarget,fuzzrun)
fuzzrun: $(cldmfuzz)
	$(QUIET)LD_LIBRARY_PATH=$(root) ./$^ $(FUZZFLAGS)
	$(QUIET)llvm-profdata $(PROFFLAGS)
	$(QUIET)llvm-cov $(COVFLAGS)
	$(QUIET)llvm-cov $(COVREPFLAGS)

.PHONY: fuzzmerge
fuzzmerge: $(call require-corpora,fuzzmerge)
fuzzmerge: $(call require-fuzztarget,fuzzmerge)
fuzzmerge: $(cldmfuzz)
	$(QUIET)LD_LIBRARY_PATH=$(root) ./$^ $(MERGEFLAGS)

.PHONY: clean
clean:
	$(QUIET)$(RM) $(RMFLAGS) $(builddir) $(lcldm) $(lcldm_main) $(link) $(cldmgen) $(cldmtest) $(cldmfuzz) $(cldm_config) crash-*

-include $(obj:.$(oext)=.$(dext))
