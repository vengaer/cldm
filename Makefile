CC         ?= gcc
LN         ?= ln
MKDIR      ?= mkdir
RM         ?= rm

sostem     := libmockc
sover      := 0
socompat   := 0

cext       := c
oext       := o
soext      := so

builddir   := build
srcdir     := $(sostem)

target     := $(sostem).$(soext).$(sover)
link       := $(sostem).$(soext)

CFLAGS     ?= -std=c11 -Wall -Wextra -Wpedantic -fPIC -c
CPPFLAGS   ?=
LDFLAGS    ?= -shared -Wl,-soname,$(sostem).$(soext).$(socompat)
LDLIBS     ?= -ldl
LNFLAGS    ?= -sf
MKDIRFLAGS ?= -p
RMFLAGS    ?= -rf

QUIET      ?= @

src        := $(wildcard $(srcdir)/*.$(cext))
obj        := $(patsubst $(srcdir)/%.$(cext),$(builddir)/%.$(oext),$(src))

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

$(builddir):
	$(QUIET)$(MKDIR) $(MKDIRFLAGS) $@

.PHONY: clean
clean:
	$(QUIET)$(RM) $(RMFLAGS) $(builddir) $(target) $(link)
