include     scripts/modules.mk

CC          ?= gcc

AR          ?= ar
LN          ?= ln
MKDIR       ?= mkdir
RM          ?= rm
ECHO        ?= echo
TOUCH       ?= touch
PYTEST      ?= pytest


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
srcdir      := $(root)/$(patsubst lib%,%,$(libstem))

lcldm       := $(libstem).$(soext).$(sover)
link        := $(libstem).$(soext)
lcldm_main  := $(libstem)_main.$(aext)

cldmgen     := $(srcdir)/cldmgen.h

MOCKUPS     ?= $(abspath $(srcdir)/mockups.h)

CFLAGS      := -std=c99 -Wall -Wextra -Wpedantic -fPIC -c -MD -MP -g
CPPFLAGS    := -D_GNU_SOURCE
LDFLAGS     := -shared -Wl,-soname,$(libstem).$(soext).$(socompat)
LDLIBS      := -ldl
ARFLAGS     := -rcs
LNFLAGS     := -sf
MKDIRFLAGS  := -p
RMFLAGS     := -rf
ECHOFLAGS   := -e
PYTESTFLAGS := -v --rootdir=$(functestdir)

QUIET       ?= @

module_path := $(root)
module_mk   := Makefile

$(call include-module,$(notdir $(srcdir)))

.PHONY: all
all: $(link) $(lcldm_main)

$(link): $(lcldm)
	$(info [LN]  $@)
	$(QUIET)$(LN) $(LNFLAGS) $(abspath $(CURDIR))/$^ $@

$(lcldm): $(lcldm_obj)
	$(info [LD]  $@)
	$(QUIET)$(CC) -o $@ $^ $(LDFLAGS) $(LDLIBS)

$(lcldm_main): $(lcldm_main_obj)
	$(info [AR]  $@)
	$(QUIET)$(AR) $(ARFLAGS) $@ $^

$(builddir)/%.$(oext): $(root)/%.$(cext) $(cldmgen)
	$(info [CC]  $(notdir $@))
	$(QUIET)$(CC) -o $@ $< $(CFLAGS) $(CPPFLAGS)

$(cldmgen): $(MOCKUPS)
	$(info [GEN] $(notdir $@))
	$(QUIET)$(ECHO) $(ECHOFLAGS) '#ifndef CLDM_CONFIG_H\n#define CLDM_CONFIG_H\n#include "$^"\n#endif' > $@

$(MOCKUPS):
	$(info $@ not found, generating empty one)
	$(info [GEN] $(notdir $@))
	$(QUIET)$(TOUCH) $@

.PHONY: functional_tests
functional_tests:
	$(QUIET)$(PYTEST) $(PYTESTFLAGS)

.PHONY: test
test: functional_tests

.PHONY: check
check: test

.PHONY: clean
clean:
	$(QUIET)$(RM) $(RMFLAGS) $(builddir) $(lcldm) $(lcldm_main) $(link) $(help) $(cldmgen)

-include $(obj:.$(oext)=.$(dext))
