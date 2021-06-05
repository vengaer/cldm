import os
import time

from pathlib import Path

from cgen import *
from config import *
from makegen import *
from runner import *
from symbols import *
from util import *

_BINARY = 'makegen_test'

def test_alignof():
    typelist = ['char', 'short', 'int', 'long', 'long long', 'void *']
    expected = '1 ' * len(typelist)
    expected = expected.strip().split(' ')

    cgen = CGen('main.c')
    cgen.append_include('cldm_macro.h', system_header=False)
    cgen.append_include('stdio.h')
    cgen.append_include('stdalign.h')
    cgen.append_line('#define ARRSIZE(a) (sizeof(a) / sizeof(a[0]))')

    with cgen.open_function('int', 'main'):
        with cgen.open_scope('int result[] =', semicolon=True):
            for t in typelist[:len(typelist) - 1]:
                cgen.append_line(f'alignof({t}) == cldm_alignof({t}),')
            cgen.append_line('alignof({t}) == cldm_alignof({t})'.format(t=typelist[len(typelist) - 1]))

        with cgen.open_for('unsigned', 'i', 0, 'ARRSIZE(result)'):
            cgen.append_line('printf("%d\\n", result[i]);')
    cgen.write()

    mgen = Makegen(_BINARY, src='main.c')
    mgen.adjust('CFLAGS', '-std=c11 -Wall -Wextra -Wpedantic -g -c -Werror', Mod.REPLACE)
    mgen.adjust('LDFLAGS', 'fPIC', Mod.REMOVE)
    mgen.adjust('LDFLAGS', '-shared', Mod.REMOVE)
    mgen.generate()

    assert exec_bash('make -C {}'.format(working_dir))[0] == 0
    retval, output, _ = exec_bash(str(working_dir / _BINARY))
    assert retval == 0
    output = output.decode('utf-8').split('\n')
    output = output[:len(output) - 1]
    assert output == expected

def test_sequential_strscpy():
    string = 'Out to work my train at the railway station, Shotgun Betty stoppin\' dead at my shoes.' \
             'Out to run the train at the railway station, Shotgun Betty stoppin\' dead at my shoes'
    cgen = CGen('tests.c')
    cgen.append_include('cldm.h', system_header=False)
    cgen.append_include('cldm_byteseq.h', system_header=False)
    cgen.append_include('string.h')

    with cgen.open_macro('TEST', 'sequential_strscpy'):
        cgen.append_line('enum { SIZE = 256 };')
        cgen.append_line('char dst[SIZE];')
        cgen.append_line('char const *src = "{}";'.format(string))
        cgen.append_line('ASSERT_EQ(cldm_strscpy(dst, src, sizeof(dst)), (long long)strlen(src));')
        cgen.append_line('ASSERT_STREQ(dst, src);')

    with cgen.open_macro('TEST', 'sequential_strscpy_termination'):
        cgen.append_line('enum { SIZE = 16 };')
        cgen.append_line('char dst[SIZE];')
        cgen.append_line('char const *src = "{}";'.format(string))
        cgen.append_line('ASSERT_EQ(cldm_strscpy(dst, src, sizeof(dst)), -7);')
        cgen.append_line('ASSERT_STRNEQ(src, dst, sizeof(dst) - 1);')
        cgen.append_line('ASSERT_EQ(dst[sizeof(dst) - 1], 0);')

    with cgen.open_macro('TEST', 'sequential_strscpy_empty'):
        cgen.append_line('enum { SIZE = 32 };')
        cgen.append_line('char dst[SIZE];')
        cgen.append_line('char const *src = "";')
        cgen.append_line('ASSERT_EQ(cldm_strscpy(dst, src, sizeof(dst)), 0);')
        cgen.append_line('ASSERT_EQ(dst[0], 0);')

    with cgen.open_macro('TEST', 'sequential_strscpy_dstsize_zero'):
        cgen.append_line('enum { SIZE = 32 };')
        cgen.append_line('char dst[SIZE] = { 0x18 };')
        cgen.append_line('char const *src = "asdf";')
        cgen.append_line('ASSERT_EQ(cldm_strscpy(dst, src, 0), -7);')
        cgen.append_line('ASSERT_EQ(dst[0], 0x18);')
    cgen.write()

    mgen = Makegen(_BINARY)
    mgen.adjust('CFLAGS', '-fPIC', Mod.REMOVE)
    mgen.adjust('LDFLAGS', '-shared', Mod.REMOVE)
    mgen.adjust('LDFLAGS', '-L{}'.format(project_root), Mod.APPEND)
    mgen.adjust('LDLIBS', '-lcldm -lcldm_main')
    mgen.export('LD_LIBRARY_PATH', project_root)
    mgen.generate()

    exec_bash('make -j$(nproc) avx2_support=n -B -C {}'.format(project_root))
    run(ContainsMatcher('Successfully finished 9 assertions'), rvmatcher=RvEqMatcher(0))
