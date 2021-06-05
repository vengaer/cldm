from cgen import *
from cldm import *
from config import *
from makegen import *
from runner import *
from util import *

def gen_makefile():
    mgen = Makegen('collectest')
    mgen.adjust('CFLAGS', '-fPIC', Mod.REMOVE)
    mgen.adjust('CFLAGS', '-std=c99', Mod.REMOVE)
    mgen.adjust('CFLAGS', '-std=c11', Mod.PREPEND)
    mgen.adjust('LDFLAGS', '-shared', Mod.REMOVE)
    mgen.adjust('LDFLAGS', '-L{}'.format(project_root), Mod.APPEND)
    mgen.adjust('LDLIBS', '-lcldm -lcldm_main')
    mgen.export('LD_LIBRARY_PATH', project_root)
    mgen.generate()

def test_failures_expanded():
    cgen = CGen('tests.c')
    cgen.append_include('cldm.h', system_header=False)

    with cgen.open_macro('TEST', 'foo'):
        cgen.append_line('int i = 3;')
        cgen.append_line('int j = 12;')
        cgen.append_line('ASSERT_EQ(i, j);')

    cgen.write()
    gen_makefile()
    build_cldm('-std=c11 -Wall -Wextra -Wpedantic -Werror -c -g -fPIC')

    run(ContainsMatcher(r'Running\s*foo'), ContainsMatcher(r'Assertion\s*failure\s*in\s*foo.*i\s*==\s*j.*with\s*expansion.*3\s*==\s*12\s*'), RvDiffMatcher(0))
