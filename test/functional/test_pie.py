from cgen import *
from cldm import *
from config import *
from makegen import *
from runner import *
from util import *

_BINARY='pie_test'

def gen_runcmd(flags):
    return 'LD_LIBRARY_PATH={} {}/{} {}'.format(project_root, working_dir, _BINARY, flags)

def gen_makefile():
    mgen = Makegen(_BINARY)
    mgen.adjust('CFLAGS', '-pie -fPIE', Mod.REMOVE)
    mgen.adjust('LDFLAGS', '-shared', Mod.REMOVE)
    mgen.adjust('LDFLAGS', '-L{}'.format(project_root), Mod.APPEND)
    mgen.adjust('LDFLAGS', '-pie', Mod.APPEND)
    mgen.adjust('LDLIBS', '-lcldm -lcldm_main')
    mgen.export('LD_LIBRARY_PATH', project_root)
    mgen.generate()

def test_pie():
    cgen = CGen('tests.c')
    cgen.append_include('cldm.h', system_header=False)
    with cgen.open_macro('TEST', 'foo'):
        cgen.append_line('ASSERT_EQ(0, 1);')
    cgen.write()
    gen_makefile()

    run(ContainsMatcher('Running.*foo'), ContainsMatcher('Assertion failure'), RvDiffMatcher(0))
