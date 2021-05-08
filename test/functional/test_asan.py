from cgen import *
from cldm import *
from config import *
from makegen import *
from runner import *
from util import *

import os

_BINARY='asan_test'

def gen_makefile():
    mgen = Makegen(_BINARY)
    mgen.adjust('CFLAGS', '-fPIC', Mod.REMOVE)
    mgen.adjust('CFLAGS', '-fsanitize=address', Mod.PREPEND)
    mgen.adjust('LDFLAGS', '-shared', Mod.REMOVE)
    mgen.adjust('LDFLAGS', '-L{}'.format(project_root), Mod.APPEND)
    mgen.adjust('LDFLAGS', '-fsanitize=address', Mod.PREPEND)
    mgen.adjust('LDLIBS', '-lcldm -lcldm_main')
    mgen.export('LD_LIBRARY_PATH', project_root)
    mgen.generate()

def test_asan_compat():
    cgen = CGen('tests.c')
    cgen.append_include('cldm.h', system_header=False)
    with cgen.open_macro('TEST', 'bar'):
        cgen.append_line('ASSERT_EQ(0, 0);')

    cgen.write()
    gen_makefile()
    assert exec_bash('make -j$(nproc) -C {}'.format(working_dir))[0] == 0
    assert os.system('make -sC {} run'.format(working_dir)) == 0
