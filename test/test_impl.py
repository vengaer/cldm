import os
import time

from pathlib import Path

from cgen import *
from config import *
from makegen import *
from symbols import *
from util import *

def test_for_each_word():
    target = 'makegen_test'
    string = 'a list of words to test'

    cgen = CGen('main.c')
    cgen.append_line('#define CMOCK_GENERATE_SYMBOLS')    \
        .append_include('cmock.h', system_header=False) \
        .append_include('stdio.h')
    with cgen.with_open_function('int', 'main'):
        cgen.append_line('char words[{}];'.format(len(string) + 2)) \
            .append_line('strcpy(words, "{}");'.format(string))     \
            .append_line('char const *iter;')                       \
            .append_line('cmock_for_each_word(iter, words) {')      \
            .append_line('printf("%s\\n", iter);')                  \
            .append_line('}')                                       \
            .append_return(0)
    cgen.write()

    mgen = Makegen(target, src='main.c')
    mgen.adjust('CFLAGS', '-fPIC', Mod.REMOVE)
    mgen.adjust('LDFLAGS', '-shared', Mod.REMOVE)
    mgen.adjust('LDFLAGS', '-L{}'.format(project_root), Mod.APPEND)
    mgen.adjust('LDLIBS', '-lcmock -ldl', Mod.APPEND)
    mgen.generate()

    retval = exec_bash('make -C {}'.format(working_dir))[0] == 0
    retval, output, _ = exec_bash('LD_LIBRARY_PATH={} {}'.format(project_root, working_dir / target))
    assert retval == 0
    output = output.decode('utf-8').split('\n')
    output = output[:len(output) - 1]
    assert string.split(' ') == output
