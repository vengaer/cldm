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

builddir    := build
srcdir      := $(patsubst lib%,%,$(libstem))
testdir     := test

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
PYTESTFLAGS := -v --rootdir=$(testdir)

QUIET       ?= @

help        := .help.stamp

mainsrc     := $(wildcard $(srcdir)/*_main.$(cext))
mainobj     := $(patsubst $(srcdir)/%.$(cext),$(builddir)/%.$(oext),$(mainsrc))
src         := $(filter-out $(mainsrc),$(wildcard $(srcdir)/*.$(cext)))
obj         := $(patsubst $(srcdir)/%.$(cext),$(builddir)/%.$(oext),$(src))

.PHONY: all
all: $(lcldm) $(link) $(lcldm_main)

$(link): $(lcldm)
	$(info [LN]  $@)
	$(QUIET)$(LN) $(LNFLAGS) $(abspath $(CURDIR))/$^ $@

$(lcldm): $(obj)
	$(info [LD]  $@)
	$(QUIET)$(CC) -o $@ $^ $(LDFLAGS) $(LDLIBS)

$(lcldm_main): $(mainobj)
	$(info [AR]  $@)
	$(QUIET)$(AR) $(ARFLAGS) $@ $^

$(builddir)/%.$(oext): $(srcdir)/%.$(cext) $(cldmgen) | $(builddir)
	$(info [CC]  $@)
	$(QUIET)$(CC) -o $@ $< $(CFLAGS) $(CPPFLAGS)

$(cldmgen): $(MOCKUPS)
	$(info [GEN] $@)
	$(QUIET)$(ECHO) $(ECHOFLAGS) '#ifndef CLDM_CONFIG_H\n#define CLDM_CONFIG_H\n#include "$^"\n#endif' > $@

.PHONY: test
test:
	$(QUIET)$(PYTEST) $(PYTESTFLAGS)

$(builddir):
	$(QUIET)$(MKDIR) $(MKDIRFLAGS) $@

.PHONY: clean
clean:
	$(QUIET)$(RM) $(RMFLAGS) $(builddir) $(lcldm) $(link) $(help) $(cldmgen)

$(help):
	$(info +========+)
	$(info |  cldm  |)
	$(info +========+)
	$(info Linker-based function mocking)
	$(info )
	$(info Building:)
	$(info Create $(MOCKUPS) and specify which functions to mock)
	$(info Run 'make' (without any targets))
	$(info The path to mockups file may be modified by passing the MOCKUPS variable)
	$(info )
	$(info Testing (requires pytest):)
	$(info Run 'make test')
	$(info )
	$(info For more info: gitlab.com/vilhelmengstrom/cldm)
	$(QUIET)$(TOUCH) $@
	$(QUIET)$(RM) $@

.PHONY: help
help: $(help)

-include $(obj:.$(oext)=.$(dext))
