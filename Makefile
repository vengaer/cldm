include     scripts/build_config.mk
include     scripts/cross.mk
include     scripts/fuzz.mk
include     scripts/modules.mk

CC          := $(CROSS_COMPILE)gcc
AS          := $(CROSS_COMPILE)gcc
LD          := $(CROSS_COMPILE)gcc

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

asext       := S
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
system_mk   := $(builddir)/system.mk
proc_mk     := $(builddir)/proc.mk

lcldm       := $(libstem).$(soext).$(sover)
link        := $(libstem).$(soext)
lcldm_main  := $(libstem)_main.$(aext)

cldmtest    := cldmtest
cldmfuzz    := cldmfuzz

cldmgen     := $(libsrcdir)/cldmgen.h
cconfig     := $(libsrcdir)/cldm_config.h
asmconfig   := $(libsrcdir)/asm/system.h

MOCKUPS     ?= $(abspath $(libsrcdir)/mockups.h)

ASFLAGS     := -c -g -I$(libsrcdir)
CFLAGS      := -std=c99 -Wall -Wextra -Wpedantic -fPIC -c -MD -MP -g
CPPFLAGS    := -D_POSIX_C_SOURCE=200112L -DCLDM_VERSION=$(cldmver) -I$(root)
LDFLAGS     := -shared -Wl,-soname,$(libstem).$(soext).$(socompat) $(CROSS_LDFLAGS)
LDLIBS      := -ldl -pthread

testldflags := -L$(root)
testldlibs  := $(patsubst lib%,-l%,$(libstem)) $(patsubst lib%.a,-l%,$(lcldm_main)) -pthread

fuzzcflags  := $(fuzzinstr) $(CFLAGS)
fuzzldflags := -L$(root) $(fuzzinstr)

ARFLAGS     := -rcs
LNFLAGS     := -sf
MKDIRFLAGS  := -p
RMFLAGS     := -rf
ECHOFLAGS   := -e
PYTESTFLAGS := -v --rootdir=$(functestdir)

QUIET       ?= @

module_path := $(root)
module_mk   := Makefile

prepdeps    :=

$(call set-build-config)

.PHONY: all
all: $(link) $(lcldm_main)

ifneq ($(MAKECMDGOALS),clean)
  $(call include-each-module,$(srcdirs))
endif

$(link): $(lcldm)
	$(info [LN]  $@)
	$(QUIET)$(LN) $(LNFLAGS) $(abspath $(CURDIR))/$^ $@

$(lcldm): $(lcldm_obj)
	$(info [LD]  $@)
	$(QUIET)$(CC) -o $@ $^ $(LDFLAGS) $(LDLIBS)

$(cldmtest): $($(cldmtest)_obj) $(link) $(lcldm_main)
	$(info [LD]  $@)
	$(QUIET)$(CC) -o $@ $($(cldmtest)_obj) $(testldflags) $(testldlibs)

$(cldmfuzz): CC     := $(FUZZCC)
$(cldmfuzz): CFLAGS := $(fuzzinstr) $(CFLAGS)
$(cldmfuzz): $($(cldmfuzz)_obj) $(link)
	$(info [LD]  $@)
	$(QUIET)$(FUZZCC) -o $@ $($(cldmfuzz)_obj) $(fuzzldflags)

$(lcldm_main): $(lcldm_main_obj)
	$(info [AR]  $@)
	$(QUIET)$(AR) $(ARFLAGS) $@ $^

$(builddir)/%.$(oext): $(root)/%.$(cext) $(cldmgen) $(cconfig) Makefile
	$(info [CC]  $(notdir $@))
	$(QUIET)$(CC) -o $@ $< $(CFLAGS) $(CPPFLAGS)

$(builddir)/%.$(oext): $(root)/%.$(asext) $(asmconfig) Makefile
	$(info [AS]  $(notdir $@))
	$(QUIET)$(AS) -o $@ $< $(ASFLAGS)

$(cldmgen): $(MOCKUPS)
	$(info [GEN] $(notdir $@))
	$(QUIET)$(ECHO) $(ECHOFLAGS) '#ifndef CLDMGEN_H\n#define CLDMGEN_H\n#include "$^"\n#endif /* CLDMGEN_H */' > $@

$(cconfig): $(system_mk) $(proc_mk) Makefile
	$(info [GEN] $(notdir $@))
	$(QUIET)$(ECHO) $(ECHOFLAGS) '#ifndef CLDM_CONFIG_H\n#define CLDM_CONFIG_H\n' > $@
	$(QUIET)$(ECHO) $(ECHOFLAGS) '#define CLDM_ARCH_$(patsubst %-bit,%,$(arch))' >> $@
	$(QUIET)$(ECHO) $(ECHOFLAGS) '#define CLDM_ABI_$(abi)' >> $@
	$(QUIET)$(ECHO) $(ECHOFLAGS) '#define CLDM_PGSIZE $(pagesize)' >> $@
	$(QUIET)$(ECHO) $(ECHOFLAGS) '#define CLDM_L1_DCACHE_LINESIZE $(l1_dcache)' >> $@
	$(QUIET)$(if $(filter y,$(avx2_support)), $(ECHO) $(ECHOFLAGS) '#define CLDM_HAS_AVX2' >> $@)
	$(QUIET)$(if $(filter y,$(advsimd_support)), $(ECHO) $(ECHOFLAGS) '#define CLDM_HAS_ADVSIMD' >> $@)
	$(QUIET)$(if $(filter y,$(advsimd_hp_support)), $(ECHO) $(ECHOFLAGS) '#define CLDM_HAS_ADVSIMD_HALF' >> $@)
	$(QUIET)$(if $(filter y,$(has_generic)),  $(ECHO) $(ECHOFLAGS) '#define CLDM_HAS_GENERIC' >> $@)
	$(QUIET)$(if $(filter y,$(has_noreturn)), $(ECHO) $(ECHOFLAGS) '#define CLDM_HAS_NORETURN' >> $@)
	$(QUIET)$(ECHO) $(ECHOFLAGS) '\n#endif /* CLDM_CONFIG_H */' >> $@

$(asmconfig): $(system_mk) Makefile
	$(info [GEN] $(notdir $@))
	$(QUIET)$(ECHO) $(ECHOFLAGS) '#ifndef SYSTEM_H\n#define SYSTEM_H\n' > $@
	$(QUIET)$(ECHO) $(ECHOFLAGS) '#define PGSIZE $(pagesize)' >> $@
	$(QUIET)$(ECHO) $(ECHOFLAGS) '#define L1_DCACHE_LINESIZE $(l1_dcache)' >> $@
	$(QUIET)$(ECHO) $(ECHOFLAGS) '#define HAS_ADVSIMD_HALF' >> $@
	$(QUIET)$(ECHO) $(ECHOFLAGS) '\n#endif' >> $@

$(MOCKUPS):
	$(info [GEN] $(notdir $@))
	$(QUIET)$(TOUCH) $@

.PHONY: functional
functional:
	$(QUIET)$(PYTEST) $(PYTESTFLAGS)

.PHONY: unit
unit: $(cldmtest)
	$(QUIET)LD_LIBRARY_PATH=$(root) $(UNITPREFIX) $(CROSS_EMU) ./$< $(UNITSUFFIX)

.PHONY: vgunit
vgunit: UNITPREFIX := valgrind --suppressions=.cldm.supp
vgunit: unit

.PHONY: fuzz
fuzz: $(cldmfuzz)

.PHONY: test
test: unit functional

.PHONY: check
check: test

.PHONY: vgcheck
vgcheck: vgunit functional

.PHONY: vgparunit
vgparunit: UNITSUFFIX := -j$(if $(THREADS),$(THREADS),16)
vgparunit: vgunit

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
	$(QUIET)$(RM) $(RMFLAGS) $(builddir) $(lcldm) $(lcldm_main) $(link) $(cldmgen) $(cldmtest) $(cldmfuzz) $(cconfig) $(asmconfig) crash-*

-include $(obj:.$(oext)=.$(dext))
