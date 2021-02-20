import os

from pathlib import Path

from cgen import *
from config import *
from makegen import *
from symbols import *

def test_preload():
    target = 'makegen_test'
    symfile = 'syms.c'

    db = read_db()
    cgen = CGen('main.c')
    cgen.append_include('lmc.h')
    cgen.append_include('syms.h', system_header=False)
    with cgen.with_open_function('int', 'main'):
        cgen.append_line('EXPECT_CALL(bar).WILL_REPEATEDLY(RETURN(28));')
        cgen.append_return(CGen.default_call('bar', db['symbols']['bar']['params']))
    cgen.write()

    cgen = CGen(symfile)
    cgen.generate_matching_symbols()
    cgen.write()

    mgen = Makegen(target, src='main.c')
    mgen.adjust('CFLAGS', '-fPIC', Mod.REMOVE)
    mgen.adjust('LDFLAGS', '-shared', Mod.REMOVE)
    mgen.adjust('LDFLAGS', '-rdynamic -L. -L{} -lmockc'.format(project_root), Mod.APPEND)
    mgen.adjust('LDLIBS', '-ltest', Mod.APPEND)
    mgen.add_rule('libtest.so', '$(builddir)/syms.o', '$(QUIET)$(CC) -o $@ $^ -shared $(LDFLAGS) $(LDLIBS)', '[LD] $@')
    mgen.add_rule('$(builddir)/syms.o', str(working_dir / symfile), '$(QUIET)$(CC) -o $@ $^ $(CFLAGS) $(CPPFLAGS) -fPIC', '[CC] $@')
    mgen.add_prereq(target, 'libtest.so')
    mgen.generate()

    assert os.system('make -C {}'.format(working_dir)) == 0
    assert os.system('LD_PRELOAD={} LD_LIBRARY_PATH={} {}'.format(solib, working_dir, working_dir / target))
