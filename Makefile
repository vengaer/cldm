include     scripts/modules.mk

CC          ?= gcc

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

cldmgen     := $(libsrcdir)/cldmgen.h
config      := $(libsrcdir)/cldm_config.h

MOCKUPS     ?= $(abspath $(libsrcdir)/mockups.h)

CFLAGS      := -std=c11 -Wall -Wextra -Wpedantic -fPIC -c -MD -MP -g
CPPFLAGS     = -D_GNU_SOURCE -DCLDM_VERSION=$(cldmver) -I$(root)
LDFLAGS     := -shared -Wl,-soname,$(libstem).$(soext).$(socompat)
LDLIBS      := -ldl

TLDFLAGS    := -L$(root)
TLDLIBS     := $(patsubst lib%,-l%,$(libstem)) $(patsubst lib%.a,-l%,$(lcldm_main))

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
	$(QUIET)$(CC) -o $@ $(cldmtest_obj) $(TLDFLAGS) $(TLDLIBS)

$(lcldm_main): $(lcldm_main_obj)
	$(info [AR]  $@)
	$(QUIET)$(AR) $(ARFLAGS) $@ $^

$(builddir)/%.$(oext): $(root)/%.$(cext) $(cldmgen) $(config)
	$(info [CC]  $(notdir $@))
	$(QUIET)$(CC) -o $@ $< $(CFLAGS) $(CPPFLAGS)

$(cldmgen): $(MOCKUPS)
	$(info [GEN] $(notdir $@))
	$(QUIET)$(ECHO) $(ECHOFLAGS) '#ifndef CLDMGEN_H\n#define CLDMGEN_H\n#include "$^"\n#endif /* CLDMGEN_H */' > $@

$(config): $(prepare)
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

.PHONY: test
test: unit functional

.PHONY: check
check: test

.PHONY: clean
clean:
	$(QUIET)$(RM) $(RMFLAGS) $(builddir) $(lcldm) $(lcldm_main) $(link) $(help) $(cldmgen) $(cldmtest) $(config)

-include $(obj:.$(oext)=.$(dext))
