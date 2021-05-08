import os
import time

from pathlib import Path

from cgen import *
from config import *
from makegen import *
from symbols import *
from util import *

__TARGET = 'makegen_test'

def test_alignof():
    typelist = ['char', 'short', 'int', 'long', 'long long', 'void *']
    expected = '1 ' * len(typelist)
    expected = expected.strip().split(' ')

    cgen = CGen('main.c')
    cgen.append_include('cldm_macro.h', system_header=False)            \
        .append_include('stdio.h')                                      \
        .append_include('stdalign.h')                                   \
        .append_line('#define ARRSIZE(a) (sizeof(a) / sizeof(a[0]))')

    with cgen.open_function('int', 'main'):
        with cgen.open_scope('int result[] =', semicolon=True):
            for t in typelist[:len(typelist) - 1]:
                cgen.append_line(f'alignof({t}) == cldm_alignof({t}),')
            cgen.append_line('alignof({t}) == cldm_alignof({t})'.format(t=typelist[len(typelist) - 1]))

        with cgen.open_for('unsigned', 'i', 0, 'ARRSIZE(result)'):
            cgen.append_line('printf("%d\\n", result[i]);')
    cgen.write()

    mgen = Makegen(__TARGET, src='main.c')
    mgen.adjust('CFLAGS', '-std=c11 -Wall -Wextra -Wpedantic -g -c -Werror', Mod.REPLACE)
    mgen.adjust('LDFLAGS', 'fPIC', Mod.REMOVE)
    mgen.adjust('LDFLAGS', '-shared', Mod.REMOVE)
    mgen.generate()

    assert exec_bash('make -C {}'.format(working_dir))[0] == 0
    retval, output, _ = exec_bash(str(working_dir / __TARGET))
    assert retval == 0
    output = output.decode('utf-8').split('\n')
    output = output[:len(output) - 1]
    assert output == expected

