import os

from pathlib import Path

from cgen import *
from config import *
from makegen import *
from util import *

__TARGET = 'ntbs_test'

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

def test_find_substr():
    cgen = CGen('main.c')
    cgen.append_include('cldm_ntbs.h', system_header=False) \
        .append_include('stdio.h')

    with cgen.open_function('int', 'main'):
        cgen.append_line('printf("%d\\n", !!cldm_ntbs_find_substr("a string containing PATTERN", "PATTERN"));')     \
            .append_line('printf("%d\\n", !!cldm_ntbs_find_substr("a string without pat", "PATTERN"));')            \
            .append_line('printf("%d\\n", !!cldm_ntbs_find_substr("", "string"));')                                 \
            .append_line('printf("%d\\n", !!cldm_ntbs_find_substr("string", ""));')                                 \
            .append_line('printf("%d\\n", !!cldm_ntbs_find_substr("another string", "other str"));')

    cgen.write()
    gen_makefile()

    run(['1', '0', '0', '1', '1'])

def test_find_substr_address():
    string = '"a string to be searched"'
    substr = '"string to be"'
    cgen = CGen('main.c')
    cgen.append_include('cldm_ntbs.h', system_header=False) \
        .append_include('stdio.h')                          \
        .append_include('string.h')

    with cgen.open_function('int', 'main'):
        cgen.append_line('char const *addr = cldm_ntbs_find_substr({}, {});'.format(string, substr))    \
            .append_line('int res = strncmp(addr, {}, {});'.format(substr, len(substr) - 2))                \
            .append_line('printf("%d\\n", res);')

    cgen.write()
    gen_makefile()

    run(['0'])

