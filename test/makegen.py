from enum import Enum
from pathlib import Path

from config import *

def gen_default_makefile(mgen, target, symbol_tu):
    mgen.adjust('CFLAGS', '-fPIC', Mod.REMOVE)
    mgen.adjust('LDFLAGS', '-shared', Mod.REMOVE)
    mgen.adjust('LDFLAGS', '-L. -L{} -lcldm'.format(project_root), Mod.APPEND)
    mgen.adjust('LDLIBS', '-ltest', Mod.APPEND)
    mgen.add_rule('libtest.so', '$(builddir)/syms.o', '$(QUIET)$(CC) -o $@ $^ -shared $(LDFLAGS) $(LDLIBS)', '[LD] $@')
    mgen.add_rule('$(builddir)/syms.o', str(working_dir / symbol_tu), '$(QUIET)$(CC) -o $@ $^ $(CFLAGS) $(CPPFLAGS) -fPIC', '[CC] $@')
    mgen.add_prereq(target, 'libtest.so')


class Mod(Enum):
    APPEND = 0
    PREPEND = 1
    REPLACE = 2
    REMOVE = 3

class Makegen():
    __CC = 'gcc'
    __CFLAGS = '-std=c11 -Wall -Wextra -Wpedantic -fPIC -g -c'
    __CPPFLAGS = '-I {}'.format(libdir)
    __LDFLAGS = '-shared'
    __LDLIBS = ''
    __BUILDDIR = 'build'

    __CEXT = 'c'
    __OEXT = 'o'

    def __init__(self, target, cc=__CC, cflags=__CFLAGS, cppflags=__CPPFLAGS, ldflags=__LDFLAGS, ldlibs=__LDLIBS,
                       builddir=__BUILDDIR, srcdir=working_dir, src=None):
        self.target = target
        self.cc = cc
        self.cflags = cflags
        self.cppflags = cppflags
        self.ldflags = ldflags
        self.ldlibs = ldlibs
        self.builddir = str(working_dir / builddir)
        self.srcdir=working_dir
        if src:
            self.src = srcdir / src
        else:
            self.src = '$(wildcard {}/*.{})'.format(srcdir, self.__CEXT)

        self.aux_rules = []

    def adjust(self, varname, value, mode=Mod.APPEND):
        varname = varname.lower()
        if mode == Mod.APPEND:
            setattr(self, varname, getattr(self, varname) + ' ' + ' '.join([v for v in value.split(' ')]))
        elif mode == Mod.PREPEND:
            setattr(self, varname, ' '.join([v for v in value.split(' ')]) + ' ' + getattr(self, varname))
        elif mode == Mod.REPLACE:
            setattr(self, varname,  ' '.join([v for v in value.split(' ')]))
        elif mode == Mod.REMOVE:
            setattr(self, varname, getattr(self, varname).replace(value, ''))
        else:
            raise ValueError('Unknown adjustment {}'.format(value))

    def add_rule(self, target, prereq, command, info=None):
        self.aux_rules.append(target + ': ' + ' '.join([p for p in prereq.split(' ')]) + '\n')
        if info:
            self.aux_rules.append('\t$(info ' + info + ')\n')
        self.aux_rules.append('\t' + command + '\n')

    def add_prereq(self, target, prereq):
        self.aux_rules.append(target + ': ' + prereq + '\n')

    def generate(self, filename=str(working_dir / 'Makefile')):
        contents =  f'CC       := {self.cc}\n'                                                                  \
                    f'CFLAGS   := {self.cflags}\n'                                                              \
                    f'CPPFLAGS := {self.cppflags}\n'                                                            \
                    f'LDFLAGS  := {self.ldflags}\n'                                                             \
                    f'LDLIBS   := {self.ldlibs}\n'                                                              \
                     'QUIET    ?= @ \n'                                                                         \
                    f'builddir := {self.builddir}\n'                                                            \
                    f'srcdir   := {self.srcdir}\n'                                                              \
                    f'src      := {self.src}\n'                                                                 \
                    f'obj      := $(patsubst $(srcdir)/%.{self.__CEXT}, $(builddir)/%.{self.__OEXT}, $(src))\n' \
                    f'target   := {self.target}\n'                                                              \
                     '.PHONY: all\n'                                                                            \
                     'all: $(target)\n'                                                                         \
                     '$(target): $(obj)\n'                                                                      \
                     '\t$(info [LD] $@)\n'                                                                      \
                     '\t$(QUIET)$(CC) -o $@ $^ $(LDFLAGS) $(LDLIBS)\n'                                          \
                    f'$(builddir)/%.{self.__OEXT}: $(srcdir)/%.{self.__CEXT} | $(builddir)\n'                   \
                     '\t$(info [CC] $@)\n'                                                                      \
                     '\t$(QUIET)$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ $^\n'                                         \
                     '$(builddir):\n'                                                                           \
                     '\t$(QUIET)mkdir -p $@\n'                                                                  \
                     '.PHONY: clean\n'                                                                          \
                     'clean:\n'                                                                                 \
                     '\t$(QUIET)rm -rf $(builddir) $(target)\n'                                                 \
                     '{}'.format(''.join([l for l in self.aux_rules]))

        with open(filename, 'w') as fd:
            fd.write(contents)

