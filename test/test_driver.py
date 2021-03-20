from cgen import *
from cldm import *
from config import *
from makegen import *
from runner import *
from util import *

__TARGET = 'driver_test'
__TESTFILE = 'test.c'

def gen_makefile(src):
    mgen = Makegen('driver_test', src=src)
    mgen.adjust('CFLAGS', '-fPIC', Mod.REMOVE)
    mgen.adjust('LDFLAGS', '-shared', Mod.REMOVE)
    mgen.adjust('LDFLAGS', '-L{}'.format(project_root), Mod.APPEND)
    mgen.adjust('LDLIBS', '-lcldm -lcldm_main')
    mgen.export('LD_LIBRARY_PATH', project_root)
    mgen.generate()


def test_collection():
    cgen = CGen(__TESTFILE)
    cgen.append_include('cldm.h', system_header=False)

    with cgen.open_macro('TEST', 'foo'):
        pass
    with cgen.open_macro('TEST', 'bar'):
        pass
    with cgen.open_macro('TEST', 'baz'):
        pass

    cgen.write()
    gen_makefile(__TESTFILE)

    run(ContainsMatcher('Collected 3 tests'))

def test_setup_detection():
    cgen = CGen(__TESTFILE)
    cgen.append_include('cldm.h', system_header=False)

    with cgen.open_macro('TEST_SETUP'):
        pass
    with cgen.open_macro('TEST', 'baz'):
        pass

    cgen.write()
    gen_makefile(__TESTFILE)

    run(ContainsMatcher('Detected local setup'))

def test_teardown_detection():
    cgen = CGen(__TESTFILE)
    cgen.append_include('cldm.h', system_header=False)

    with cgen.open_macro('TEST_TEARDOWN'):
        pass
    with cgen.open_macro('TEST', 'baz'):
        pass

    cgen.write()
    gen_makefile(__TESTFILE)

    run(ContainsMatcher('Detected local teardown'))

def test_global_setup_detection():
    cgen = CGen(__TESTFILE)
    cgen.append_include('cldm.h', system_header=False)

    with cgen.open_macro('GLOBAL_SETUP'):
        pass
    with cgen.open_macro('TEST', 'baz'):
        pass

    cgen.write()
    gen_makefile(__TESTFILE)

    run(ContainsMatcher('Detected global setup'))

def test_global_teardown_detection():
    cgen = CGen(__TESTFILE)
    cgen.append_include('cldm.h', system_header=False)

    with cgen.open_macro('GLOBAL_TEARDOWN'):
        pass
    with cgen.open_macro('TEST', 'baz'):
        pass

    cgen.write()
    gen_makefile(__TESTFILE)

    run(ContainsMatcher('Detected global teardown'))

def test_runner():
    cgen = CGen(__TESTFILE)
    cgen.append_include('cldm.h', system_header=False)  \
        .append_include('stdio.h')

    with cgen.open_macro('TEST', 'foo'):
        cgen.append_line('puts(__func__);')
    with cgen.open_macro('TEST', 'bar'):
        cgen.append_line('puts(__func__);')

    cgen.write()
    gen_makefile(__TESTFILE)

    run(ContainsMatcher('Captured stdout:.*{p}foo.*{p}bar'.format(p=test_prefix)))

def test_setup_invoked_before_tests():
    cgen = CGen(__TESTFILE)
    cgen.append_include('cldm.h', system_header=False)  \
        .append_include('stdio.h')

    with cgen.open_macro('TEST_SETUP'):
        cgen.append_line('puts(__func__);')
    with cgen.open_macro('TEST', 'foo'):
        cgen.append_line('puts(__func__);')
    with cgen.open_macro('TEST', 'bar'):
        cgen.append_line('puts(__func__);')

    cgen.write()
    gen_makefile(__TESTFILE)

    run(ContainsMatcher('{s}.*{p}(foo|bar).*{s}.*{p}(foo|bar)'.format(s=local_setup, p=test_prefix)))

def test_teardown_invoked_after_tests():
    cgen = CGen(__TESTFILE)
    cgen.append_include('cldm.h', system_header=False)  \
        .append_include('stdio.h')

    with cgen.open_macro('TEST_TEARDOWN'):
        cgen.append_line('puts(__func__);')
    with cgen.open_macro('TEST', 'foo'):
        cgen.append_line('puts(__func__);')
    with cgen.open_macro('TEST', 'bar'):
        cgen.append_line('puts(__func__);')

    cgen.write()
    gen_makefile(__TESTFILE)

    run(ContainsMatcher(r'{p}(foo|bar).*{t}.*{p}(foo|bar).*{t}'.format(p=test_prefix, t=local_teardown)))

def test_global_setup_invoked_once():
    cgen = CGen(__TESTFILE)
    cgen.append_include('cldm.h', system_header=False)  \
        .append_include('stdio.h')

    with cgen.open_macro('GLOBAL_SETUP'):
        cgen.append_line('puts(__func__);')
    with cgen.open_macro('TEST', 'foo'):
        cgen.append_line('puts(__func__);')
    with cgen.open_macro('TEST', 'bar'):
        cgen.append_line('puts(__func__);')

    cgen.write()
    gen_makefile(__TESTFILE)

    run(ContainsOnceMatcher('{gs}.*{p}(foo|bar).*{p}(foo|bar)'.format(gs=global_setup, p=test_prefix), [global_setup]))

def test_global_teardown_invoked_once():
    cgen = CGen(__TESTFILE)
    cgen.append_include('cldm.h', system_header=False)  \
        .append_include('stdio.h')

    with cgen.open_macro('GLOBAL_TEARDOWN'):
        cgen.append_line('puts(__func__);')
    with cgen.open_macro('TEST', 'foo'):
        cgen.append_line('puts(__func__);')
    with cgen.open_macro('TEST', 'bar'):
        cgen.append_line('puts(__func__);')

    cgen.write()
    gen_makefile(__TESTFILE)

    run(ContainsOnceMatcher('{p}(foo|bar).*{p}(foo|bar).*{gt}'.format(p=test_prefix, gt=global_teardown), [global_teardown]))

def test_setup_teardown():
    cgen = CGen(__TESTFILE)
    cgen.append_include('cldm.h', system_header=False)  \
        .append_include('stdio.h')

    with cgen.open_macro('GLOBAL_SETUP'):
        cgen.append_line('puts(__func__);')
    with cgen.open_macro('GLOBAL_TEARDOWN'):
        cgen.append_line('puts(__func__);')
    with cgen.open_macro('TEST_SETUP'):
        cgen.append_line('puts(__func__);')
    with cgen.open_macro('TEST_TEARDOWN'):
        cgen.append_line('puts(__func__);')
    with cgen.open_macro('TEST', 'foo'):
        cgen.append_line('puts(__func__);')
    with cgen.open_macro('TEST', 'bar'):
        cgen.append_line('puts(__func__);')

    cgen.write()
    gen_makefile(__TESTFILE)

    run(ContainsOnceMatcher('{gs}.*{ts}.*{p}(foo|bar).*{tt}.*{ts}.*{p}(foo|bar).*{tt}.*{gt}'
        .format(gs=global_setup, ts=local_setup, p=test_prefix, tt=local_teardown, gt=global_teardown), [global_setup, global_teardown]))
