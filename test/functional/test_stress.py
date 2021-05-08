from cgen import *
from cldm import *
from config import *
from makegen import *
from runner import *
from util import *

import os

_BINARY='stress_test'

def gen_makefile():
    mgen = Makegen(_BINARY)
    mgen.adjust('CFLAGS', '-fPIC', Mod.REMOVE)
    mgen.adjust('LDFLAGS', '-shared', Mod.REMOVE)
    mgen.adjust('LDFLAGS', '-L{}'.format(project_root), Mod.APPEND)
    mgen.adjust('LDLIBS', '-lcldm -lcldm_main')
    mgen.export('LD_LIBRARY_PATH', project_root)
    mgen.generate()

def test_10000_single_file():
    cgen = CGen('tests.c')
    cgen.append_include('cldm.h', system_header=False)
    for i in range(10000):
        with cgen.open_macro('TEST', 'test{}'.format(i)):
            cgen.append_line(f'ASSERT_EQ({i},{i});')

    cgen.write()
    gen_makefile()
    assert exec_bash('make -C {}'.format(working_dir))[0] == 0
    assert os.system('make -sC {} run'.format(working_dir)) == 0

def test_10000_multiple_files():
    for i in range(10):
        cgen = CGen('tests{}.c'.format(i))
        cgen.append_include('cldm.h', system_header=False)
        for j in range(1000):
            with cgen.open_macro('TEST', 'test{}_{}'.format(i, j)):
                cgen.append_line(f'ASSERT_EQ({i},{i});')
        cgen.write()

    gen_makefile()
    assert exec_bash('make -j$(nproc) -C {}'.format(working_dir))[0] == 0
    assert os.system('make -sC {} run'.format(working_dir)) == 0
