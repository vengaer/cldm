from cgen import *
from cldm import *
from config import *
from makegen import *
from runner import *
from util import *

_BINARY='cc_test'

def gen_runcmd(flags):
    return 'LD_LIBRARY_PATH={} {}/{} {}'.format(project_root, working_dir, _BINARY, flags)

def do_optimization_test(optlvl):
    cgen = CGen('tests.c')
    cgen.append_include('cldm.h', system_header=False)
    for i in range(10):
        with cgen.open_macro('TEST', 'test{}'.format(i)):
            pass
    cgen.write()
    mgen = Makegen(_BINARY)
    mgen.adjust('CFLAGS', '-fPIC', Mod.REMOVE)
    mgen.adjust('CFLAGS', '-O{}'.format(optlvl), Mod.APPEND)
    mgen.adjust('LDFLAGS', '-shared', Mod.REMOVE)
    mgen.adjust('LDFLAGS', '-L{}'.format(project_root), Mod.APPEND)
    mgen.adjust('LDLIBS', '-lcldm -lcldm_main')
    mgen.export('LD_LIBRARY_PATH', project_root)
    mgen.generate()

    assert build_cldm('-std=c99 -Wall -Wextra -Wpedantic -Werror -c -O{} -fPIC'.format(optlvl))[0] == 0
    run(ContainsMatcher('Successfully finished.*across 10'),rvmatcher=RvEqMatcher(0))

def test_pie():
    cgen = CGen('tests.c')
    cgen.append_include('cldm.h', system_header=False)
    with cgen.open_macro('TEST', 'foo'):
        cgen.append_line('ASSERT_EQ(0, 1);')
    cgen.write()
    mgen = Makegen(_BINARY)
    mgen.adjust('CFLAGS', '-fPIC', Mod.REMOVE)
    mgen.adjust('CFLAGS', '-pie -fPIE', Mod.APPEND)
    mgen.adjust('LDFLAGS', '-shared', Mod.REMOVE)
    mgen.adjust('LDFLAGS', '-L{}'.format(project_root), Mod.APPEND)
    mgen.adjust('LDFLAGS', '-pie', Mod.APPEND)
    mgen.adjust('LDLIBS', '-lcldm -lcldm_main')
    mgen.export('LD_LIBRARY_PATH', project_root)
    mgen.generate()

    run(ContainsMatcher('Running.*foo'), ContainsMatcher('Assertion failure'), RvDiffMatcher(0))

def test_optimization_O0():
    do_optimization_test(0)

def test_optimization_O1():
    do_optimization_test(1)

def test_optimization_O2():
    do_optimization_test(2)

def test_optimization_O3():
    do_optimization_test(3)
