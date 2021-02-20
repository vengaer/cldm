CC          ?= gcc
LN          ?= ln
MKDIR       ?= mkdir
RM          ?= rm
PYTEST      ?= pytest

sostem      := libmockc
sover       := 0
socompat    := 0

cext        := c
oext        := o
soext       := so

builddir    := build
srcdir      := $(sostem)
testdir     := test

target      := $(sostem).$(soext).$(sover)
link        := $(sostem).$(soext)

CFLAGS      ?= -std=c11 -Wall -Wextra -Wpedantic -fPIC -c
CPPFLAGS    ?= -DLMC_LIBC=$(shell ldd /usr/bin/ls | grep -oP "\s*\K/.*libc\.so(\.\d+)?")
LDFLAGS     ?= -shared -Wl,-soname,$(sostem).$(soext).$(socompat)
LDLIBS      ?= -ldl
LNFLAGS     ?= -sf
MKDIRFLAGS  ?= -p
RMFLAGS     ?= -rf
PYTESTFLAGS ?= -v --rootdir=$(testdir)

QUIET       ?= @

src         := $(wildcard $(srcdir)/*.$(cext))
obj         := $(patsubst $(srcdir)/%.$(cext),$(builddir)/%.$(oext),$(src))

.PHONY: all
all: $(target) $(link)

$(link): $(target)
	$(info [LN] $@)
	$(QUIET)$(LN) $(LNFLAGS) $(abspath $(CURDIR))/$^ $@

$(target): $(obj)
	$(info [LD] $@)
	$(QUIET)$(CC) -o $@ $^ $(LDFLAGS) $(LDLIBS)

$(builddir)/%.$(oext): $(srcdir)/%.$(cext) | $(builddir)
	$(info [CC] $@)
	$(QUIET)$(CC) -o $@ $^ $(CFLAGS) $(CPPFLAGS)

.PHONY: test
test:
	$(QUIET)$(PYTEST) $(PYTESTFLAGS)


$(builddir):
	$(QUIET)$(MKDIR) $(MKDIRFLAGS) $@

.PHONY: clean
clean:
	$(QUIET)$(RM) $(RMFLAGS) $(builddir) $(target) $(link)
