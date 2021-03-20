import os
import time

from pathlib import Path

from cgen import *
from config import *
from makegen import *
from symbols import *
from util import *

__TARGET = 'mem_test'

def gen_makefile():
    mgen = Makegen(__TARGET, src='main.c')
    mgen.adjust('CFLAGS', '-fPIC', Mod.REMOVE)
    mgen.adjust('LDFLAGS', '-shared', Mod.REMOVE)
    mgen.adjust('LDFLAGS', '-L{}'.format(project_root), Mod.APPEND)
    mgen.adjust('LDLIBS', '-lcldm')
    mgen.export('LD_LIBRARY_PATH', project_root)
    mgen.generate()

def run(expected_output):
    assert exec_bash('make -C {}'.format(working_dir))[0] == 0
    retval, output, _ = exec_bash('make -sC {} run'.format(working_dir))
    assert retval == 0
    output = output.decode('utf-8').split('\n')
    output = output[:len(output) - 1]
    assert output == expected_output

def test_mmove():
    size = 11
    cgen = CGen('main.c')
    cgen.append_include('cldm_mem.h', system_header=False)  \
        .append_include('stdio.h')                          \
        .append_include('string.h')

    with cgen.open_function('int', 'main'):
        with cgen.open_scope('char src[] =', semicolon=True):
            for i in range(size - 1):
                cgen.append_line(f'{i},')
            cgen.append_line('{}'.format(size - 1))
        cgen.append_line('char dst[sizeof(src)];')                              \
            .append_line('void *addr = cldm_mmove(dst, src, sizeof(src));')     \
            .append_line('printf("%d\\n", addr == dst);')                       \
            .append_line('printf("%d\\n", memcmp(dst, src, sizeof(src)));')     \
            .append_return(0)
    cgen.write()
    gen_makefile()

    run(['1', '0'])



def test_mmove_unaligned():
    size = 11

    cgen = CGen('main.c')
    cgen.append_include('cldm_mem.h', system_header=False)              \
        .append_include('stdio.h')                                      \
        .append_include('string.h')                                     \
        .append_line('#define ARRSIZE(a) (sizeof(a) / sizeof(a[0]))')

    with cgen.open_function('int', 'main'):
        # At least one of src0 and src1 is unaligned
        with cgen.open_scope('unsigned src0[] =', semicolon=True):
            for i in range(size - 1):
                cgen.append_line(f'{i},')
            cgen.append_line('{}'.format(size - 1))
        with cgen.open_scope('unsigned src1[] =', semicolon=True):
            for i in range(size - 1):
                cgen.append_line(f'{i},')
            cgen.append_line('{}'.format(size - 1))
        cgen.append_line('unsigned dst0[ARRSIZE(src0)];')                       \
            .append_line('unsigned dst1[ARRSIZE(src0)];')                       \
            .append_line('void *addr0 = cldm_mmove(dst0, src0, sizeof(src0));') \
            .append_line('void *addr1 = cldm_mmove(dst1, src1, sizeof(src1));') \
            .append_line('printf("%d\\n", addr0 == dst0);')                     \
            .append_line('printf("%d\\n", memcmp(dst0, src0, sizeof(src0)));')  \
            .append_line('printf("%d\\n", addr1 == dst1);')                     \
            .append_line('printf("%d\\n", memcmp(dst1, src1, sizeof(src1)));')
    cgen.write()

    gen_makefile()

    run(['1', '0', '1', '0'])

def test_mmove_aligned():
    size = 9
    cgen = CGen('main.c')
    cgen.append_include('cldm_mem.h', system_header=False)  \
        .append_include('stdio.h')                          \
        .append_include('string.h')                         \
        .append_include('stdalign.h')

    with cgen.open_function('int', 'main'):
        with cgen.open_scope('alignas(unsigned) char src[] =', semicolon=True):
            for i in range(size - 1):
                cgen.append_line(f'{i},')
            cgen.append_line('{}'.format(size - 1))
        cgen.append_line('alignas(unsigned) char dst[sizeof(src)];')                    \
            .append_line('void *addr = cldm_mmove(dst, src, sizeof(src));')             \
            .append_line('printf("%d\\n", addr == dst);')                               \
            .append_line('printf("%d\\n", memcmp(dst, src, sizeof(src)));')             \
            .append_line('printf("%zu\\n", ((size_t)src) % cldm_alignof(unsigned));')   \
            .append_line('printf("%zu\\n", ((size_t)dst) % cldm_alignof(unsigned));')   \
            .append_return(0)
    cgen.write()

    gen_makefile()

    run(['1', '0', '0', '0'])

def test_mmove_hit_all_loops():
    size = 11
    cgen = CGen('main.c')
    cgen.append_include('cldm_mem.h', system_header=False)  \
        .append_include('stdio.h')                          \
        .append_include('string.h')                         \
        .append_include('stdalign.h')

    with cgen.open_function('int', 'main'):
        with cgen.open_scope('alignas(unsigned) char src[] =', semicolon=True):
            for i in range(size - 1):
                cgen.append_line(f'{i},')
            cgen.append_line('{}'.format(size - 1))
        cgen.append_line('alignas(unsigned) char dst[sizeof(src)];')                    \
            .append_line('void *addr = cldm_mmove(&dst[1], &src[1], sizeof(src) - 1);') \
            .append_line('printf("%d\\n", addr == &dst[1]);')                           \
            .append_line('printf("%d\\n", memcmp(&dst[1], &src[1], sizeof(src) - 1));') \
            .append_return(0)
    cgen.write()

    gen_makefile()

    run(['1', '0'])

def test_mmove_no_overflow():
    size = 12
    cgen = CGen('main.c')
    cgen.append_include('cldm_mem.h', system_header=False)  \
        .append_include('stdio.h')                          \
        .append_include('string.h')                         \
        .append_include('stdalign.h')

    with cgen.open_function('int', 'main'):
        with cgen.open_scope('alignas(unsigned) char src[] =', semicolon=True):
            for i in range(size - 1):
                cgen.append_line(f'{i},')
            cgen.append_line('{}'.format(size - 1))
        cgen.append_line('alignas(unsigned) char dst[sizeof(src)] = { 0 };')            \
            .append_line('void *addr = cldm_mmove(&dst[1], &src[1], sizeof(src) - 2);') \
            .append_line('printf("%d\\n", addr == &dst[1]);')                           \
            .append_line('printf("%d\\n", memcmp(&dst[1], &src[1], sizeof(src) - 2));') \
            .append_line('printf("%d\\n", (int)dst[sizeof(dst) - 1]);')                 \
            .append_return(0)
    cgen.write()

    gen_makefile()

    run(['1', '0', '0'])

def test_mcpy():
    size = 11
    cgen = CGen('main.c')
    cgen.append_include('cldm_mem.h', system_header=False)  \
        .append_include('stdio.h')                          \
        .append_include('string.h')

    with cgen.open_function('int', 'main'):
        with cgen.open_scope('char src[] =', semicolon=True):
            for i in range(size - 1):
                cgen.append_line(f'{i},')
            cgen.append_line('{}'.format(size - 1))
        cgen.append_line('char dst[sizeof(src)];')                              \
            .append_line('void *addr = cldm_mcpy(dst, src, sizeof(src));')      \
            .append_line('printf("%d\\n", addr == dst);')                       \
            .append_line('printf("%d\\n", memcmp(dst, src, sizeof(src)));')     \
            .append_return(0)
    cgen.write()

    gen_makefile()

    run(['1', '0'])


def test_mcpy_unaligned():
    size = 11

    cgen = CGen('main.c')
    cgen.append_include('cldm_mem.h', system_header=False)              \
        .append_include('stdio.h')                                      \
        .append_include('string.h')                                     \
        .append_line('#define ARRSIZE(a) (sizeof(a) / sizeof(a[0]))')

    with cgen.open_function('int', 'main'):
        # At least one of src0 and src1 is unaligned
        with cgen.open_scope('unsigned src0[] =', semicolon=True):
            for i in range(size - 1):
                cgen.append_line(f'{i},')
            cgen.append_line('{}'.format(size - 1))
        with cgen.open_scope('unsigned src1[] =', semicolon=True):
            for i in range(size - 1):
                cgen.append_line(f'{i},')
            cgen.append_line('{}'.format(size - 1))
        cgen.append_line('unsigned dst0[ARRSIZE(src0)];')                       \
            .append_line('unsigned dst1[ARRSIZE(src0)];')                       \
            .append_line('void *addr0 = cldm_mcpy(dst0, src0, sizeof(src0));')  \
            .append_line('void *addr1 = cldm_mcpy(dst1, src1, sizeof(src1));')  \
            .append_line('printf("%d\\n", addr0 == dst0);')                     \
            .append_line('printf("%d\\n", memcmp(dst0, src0, sizeof(src0)));')  \
            .append_line('printf("%d\\n", addr1 == dst1);')                     \
            .append_line('printf("%d\\n", memcmp(dst1, src1, sizeof(src1)));')
    cgen.write()

    gen_makefile()

    run(['1', '0', '1', '0'])

def test_mcpy_aligned():
    size = 9
    cgen = CGen('main.c')
    cgen.append_include('cldm_mem.h', system_header=False)  \
        .append_include('stdio.h')                          \
        .append_include('string.h')                         \
        .append_include('stdalign.h')

    with cgen.open_function('int', 'main'):
        with cgen.open_scope('alignas(unsigned) char src[] =', semicolon=True):
            for i in range(size - 1):
                cgen.append_line(f'{i},')
            cgen.append_line('{}'.format(size - 1))
        cgen.append_line('alignas(unsigned) char dst[sizeof(src)];')                    \
            .append_line('void *addr = cldm_mcpy(dst, src, sizeof(src));')              \
            .append_line('printf("%d\\n", addr == dst);')                               \
            .append_line('printf("%d\\n", memcmp(dst, src, sizeof(src)));')             \
            .append_line('printf("%zu\\n", ((size_t)src) % cldm_alignof(unsigned));')   \
            .append_line('printf("%zu\\n", ((size_t)dst) % cldm_alignof(unsigned));')   \
            .append_return(0)
    cgen.write()

    gen_makefile()

    run(['1', '0', '0', '0'])

def test_mcpy_hit_all_loops():
    size = 11
    cgen = CGen('main.c')
    cgen.append_include('cldm_mem.h', system_header=False)  \
        .append_include('stdio.h')                          \
        .append_include('string.h')                         \
        .append_include('stdalign.h')

    with cgen.open_function('int', 'main'):
        with cgen.open_scope('alignas(unsigned) char src[] =', semicolon=True):
            for i in range(size - 1):
                cgen.append_line(f'{i},')
            cgen.append_line('{}'.format(size - 1))
        cgen.append_line('alignas(unsigned) char dst[sizeof(src)];')                    \
            .append_line('void *addr = cldm_mcpy(&dst[1], &src[1], sizeof(src) - 1);')  \
            .append_line('printf("%d\\n", addr == &dst[1]);')                           \
            .append_line('printf("%d\\n", memcmp(&dst[1], &src[1], sizeof(src) - 1));') \
            .append_return(0)
    cgen.write()

    gen_makefile()

    run(['1', '0'])

def test_mcpy_no_overflow():
    size = 12
    cgen = CGen('main.c')
    cgen.append_include('cldm_mem.h', system_header=False)  \
        .append_include('stdio.h')                          \
        .append_include('string.h')                         \
        .append_include('stdalign.h')

    with cgen.open_function('int', 'main'):
        with cgen.open_scope('alignas(unsigned) char src[] =', semicolon=True):
            for i in range(size - 1):
                cgen.append_line(f'{i},')
            cgen.append_line('{}'.format(size - 1))
        cgen.append_line('alignas(unsigned) char dst[sizeof(src)] = { 0 };')            \
            .append_line('void *addr = cldm_mmove(&dst[1], &src[1], sizeof(src) - 2);') \
            .append_line('printf("%d\\n", addr == &dst[1]);')                           \
            .append_line('printf("%d\\n", memcmp(&dst[1], &src[1], sizeof(src) - 2));') \
            .append_line('printf("%d\\n", (int)dst[sizeof(dst) - 1]);')                 \
            .append_return(0)
    cgen.write()

    gen_makefile()

    run(['1', '0', '0'])
