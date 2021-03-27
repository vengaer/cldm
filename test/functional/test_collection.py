from cgen import *
from cldm import *
from config import *
from makegen import *
from runner import *
from util import *

def gen_makefile():
    mgen = Makegen('collectest')
    mgen.adjust('CFLAGS', '-fPIC', Mod.REMOVE)
    mgen.adjust('LDFLAGS', '-shared', Mod.REMOVE)
    mgen.adjust('LDFLAGS', '-L{}'.format(project_root), Mod.APPEND)
    mgen.adjust('LDLIBS', '-lcldm -lcldm_main')
    mgen.export('LD_LIBRARY_PATH', project_root)
    mgen.generate()

def test_single_file_tests_sorted():
    cgen = CGen('tests.c')
    cgen.append_include('cldm.h', system_header=False)

    with cgen.open_macro('TEST', 'foo'):
        cgen.append_line('ASSERT_EQ(2,2);')
    with cgen.open_macro('TEST', 'bar'):
        cgen.append_line('ASSERT_EQ(2,2);')
    with cgen.open_macro('TEST', 'baz'):
        cgen.append_line('ASSERT_EQ(1,1);')

    cgen.write()
    gen_makefile()

    run(ContainsMatcher(r'Running\s*bar.*Running\s*baz.*Running\s*foo'))

def test_multiple_file_tests_sorted():
    cgen = CGen('tests.c')
    cgen.append_include('cldm.h', system_header=False)

    with cgen.open_macro('TEST', 'foo'):
        cgen.append_line('ASSERT_EQ(2,2);')
    with cgen.open_macro('TEST', 'bar'):
        cgen.append_line('ASSERT_EQ(2,2);')
    with cgen.open_macro('TEST', 'baz'):
        cgen.append_line('ASSERT_EQ(1,1);')

    cgen.write()

    cgen = CGen('test.c')
    cgen.append_include('cldm.h', system_header=False)
    with cgen.open_macro('TEST', 'foobar'):
        cgen.append_line('ASSERT_EQ(2,2);')
    with cgen.open_macro('TEST', 'zed'):
        cgen.append_line('ASSERT_EQ(2,2);')
    with cgen.open_macro('TEST', 'lea'):
        cgen.append_line('ASSERT_EQ(1,1);')

    cgen.write()
    gen_makefile()

    run(ContainsMatcher(r'Running\s*foobar.*Running\s*lea.*Running\s*zed.*Running\s*bar.*Running\s*baz.*Running\s*foo'))

    cgen = CGen('tests.c')
    cgen.append_include('cldm.h', system_header=False)

    with cgen.open_macro('TEST', 'foobar'):
        cgen.append_line('ASSERT_EQ(2,2);')
    with cgen.open_macro('TEST', 'zed'):
        cgen.append_line('ASSERT_EQ(2,2);')
    with cgen.open_macro('TEST', 'lea'):
        cgen.append_line('ASSERT_EQ(1,1);')

    cgen.write()

    cgen = CGen('test.c')
    cgen.append_include('cldm.h', system_header=False)

    with cgen.open_macro('TEST', 'foo'):
        cgen.append_line('ASSERT_EQ(2,2);')
    with cgen.open_macro('TEST', 'bar'):
        cgen.append_line('ASSERT_EQ(2,2);')
    with cgen.open_macro('TEST', 'baz'):
        cgen.append_line('ASSERT_EQ(1,1);')


    cgen.write()
    gen_makefile()

    run(ContainsMatcher(r'Running\s*bar.*Running\s*baz.*Running\s*foo.*Running\s*foobar.*Running\s*lea.*Running\s*zed'))
