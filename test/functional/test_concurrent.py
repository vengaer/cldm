from cgen import *
from cldm import *
from config import *
from makegen import *
from runner import *
from util import *

import os

_BINARY='concurrent_test'
_MAX_THREADS = 32

def gen_runcmd(flags):
    return 'LD_LIBRARY_PATH={} {}/{} {}'.format(project_root, working_dir, _BINARY, flags)

def gen_makefile():
    mgen = Makegen(_BINARY)
    mgen.adjust('CFLAGS', '-fPIC', Mod.REMOVE)
    mgen.adjust('LDFLAGS', '-shared', Mod.REMOVE)
    mgen.adjust('LDFLAGS', '-L{}'.format(project_root), Mod.APPEND)
    mgen.adjust('LDLIBS', '-lcldm -lcldm_main')
    mgen.export('LD_LIBRARY_PATH', project_root)
    mgen.generate()

def do_concurrent_setup_test(nthreads):
    cgen = CGen('tests.c')
    cgen.append_include('cldm.h', system_header=False)          \
        .append_include('cldm_thread.h', system_header=False)

    cgen.append_line('static int vals[{}];'.format(nthreads))
    with cgen.open_macro('LOCAL_SETUP'):
        cgen.append_line('vals[cldm_thread_id()] = 1;')
    for i in range(nthreads):
        with cgen.open_macro('TEST', 'test{}'.format(i)):
            cgen.append_line('ASSERT_EQ(vals[cldm_thread_id()], 1);')

    cgen.write()
    gen_makefile()

    assert exec_bash('make -C {}'.format(working_dir))[0] == 0
    assert os.system(gen_runcmd('-j{}'.format(nthreads))) == 0

def do_concurrent_teardown_test(nthreads):
    cgen = CGen('tests.c')
    cgen.append_include('cldm.h', system_header=False)          \
        .append_include('cldm_thread.h', system_header=False)   \
        .append_include('stdlib.h')

    cgen.append_line('static int vals[{}];'.format(nthreads))
    with cgen.open_macro('LOCAL_TEARDOWN'):
        cgen.append_line('vals[cldm_thread_id()] = 0;')
    with cgen.open_macro('GLOBAL_TEARDOWN'):
        with cgen.open_for('int', 'i', 0, str(nthreads)):
            cgen.append_line('if(vals[i]) {')   \
                .append_line('  exit(1);')      \
                .append_line('}')

    for i in range(nthreads):
        with cgen.open_macro('TEST', 'test{}'.format(i)):
            cgen.append_line('vals[cldm_thread_id()] = 1;')

    cgen.write()
    gen_makefile()

    assert exec_bash('make -C {}'.format(working_dir))[0] == 0
    assert os.system(gen_runcmd('-j{}'.format(nthreads))) == 0

def test_concurrent_setup_2():
    do_concurrent_setup_test(2)

def test_concurrent_setup_3():
    do_concurrent_setup_test(3)

def test_concurrent_setup_4():
    do_concurrent_setup_test(4)

def test_concurrent_setup_5():
    do_concurrent_setup_test(5)

def test_concurrent_setup_6():
    do_concurrent_setup_test(6)

def test_concurrent_setup_7():
    do_concurrent_setup_test(7)

def test_concurrent_setup_8():
    do_concurrent_setup_test(8)

def test_concurrent_setup_9():
    do_concurrent_setup_test(9)

def test_concurrent_setup_10():
    do_concurrent_setup_test(10)

def test_concurrent_setup_11():
    do_concurrent_setup_test(11)

def test_concurrent_setup_12():
    do_concurrent_setup_test(12)

def test_concurrent_setup_13():
    do_concurrent_setup_test(13)

def test_concurrent_setup_14():
    do_concurrent_setup_test(14)

def test_concurrent_setup_15():
    do_concurrent_setup_test(15)

def test_concurrent_setup_16():
    do_concurrent_setup_test(16)

def test_concurrent_setup_17():
    do_concurrent_setup_test(17)

def test_concurrent_setup_18():
    do_concurrent_setup_test(18)

def test_concurrent_setup_19():
    do_concurrent_setup_test(19)

def test_concurrent_setup_20():
    do_concurrent_setup_test(20)

def test_concurrent_setup_21():
    do_concurrent_setup_test(21)

def test_concurrent_setup_22():
    do_concurrent_setup_test(22)

def test_concurrent_setup_23():
    do_concurrent_setup_test(23)

def test_concurrent_setup_24():
    do_concurrent_setup_test(24)

def test_concurrent_setup_25():
    do_concurrent_setup_test(25)

def test_concurrent_setup_26():
    do_concurrent_setup_test(26)

def test_concurrent_setup_27():
    do_concurrent_setup_test(27)

def test_concurrent_setup_28():
    do_concurrent_setup_test(28)

def test_concurrent_setup_29():
    do_concurrent_setup_test(29)

def test_concurrent_setup_30():
    do_concurrent_setup_test(30)

def test_concurrent_setup_31():
    do_concurrent_setup_test(31)

def test_concurrent_setup_32():
    do_concurrent_setup_test(32)

def test_concurrent_teardown_2():
    do_concurrent_teardown_test(2)

def test_concurrent_teardown_3():
    do_concurrent_teardown_test(3)

def test_concurrent_teardown_4():
    do_concurrent_teardown_test(4)

def test_concurrent_teardown_5():
    do_concurrent_teardown_test(5)

def test_concurrent_teardown_6():
    do_concurrent_teardown_test(6)

def test_concurrent_teardown_7():
    do_concurrent_teardown_test(7)

def test_concurrent_teardown_8():
    do_concurrent_teardown_test(8)

def test_concurrent_teardown_9():
    do_concurrent_teardown_test(9)

def test_concurrent_teardown_10():
    do_concurrent_teardown_test(10)

def test_concurrent_teardown_11():
    do_concurrent_teardown_test(11)

def test_concurrent_teardown_12():
    do_concurrent_teardown_test(12)

def test_concurrent_teardown_13():
    do_concurrent_teardown_test(13)

def test_concurrent_teardown_14():
    do_concurrent_teardown_test(14)

def test_concurrent_teardown_15():
    do_concurrent_teardown_test(15)

def test_concurrent_teardown_16():
    do_concurrent_teardown_test(16)

def test_concurrent_teardown_17():
    do_concurrent_teardown_test(17)

def test_concurrent_teardown_18():
    do_concurrent_teardown_test(18)

def test_concurrent_teardown_19():
    do_concurrent_teardown_test(19)

def test_concurrent_teardown_20():
    do_concurrent_teardown_test(20)

def test_concurrent_teardown_21():
    do_concurrent_teardown_test(21)

def test_concurrent_teardown_22():
    do_concurrent_teardown_test(22)

def test_concurrent_teardown_23():
    do_concurrent_teardown_test(23)

def test_concurrent_teardown_24():
    do_concurrent_teardown_test(24)

def test_concurrent_teardown_25():
    do_concurrent_teardown_test(25)

def test_concurrent_teardown_26():
    do_concurrent_teardown_test(26)

def test_concurrent_teardown_27():
    do_concurrent_teardown_test(27)

def test_concurrent_teardown_28():
    do_concurrent_teardown_test(28)

def test_concurrent_teardown_29():
    do_concurrent_teardown_test(29)

def test_concurrent_teardown_30():
    do_concurrent_teardown_test(30)

def test_concurrent_teardown_31():
    do_concurrent_teardown_test(31)

def test_concurrent_teardown_32():
    do_concurrent_teardown_test(32)
