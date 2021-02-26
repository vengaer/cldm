import os

from pathlib import Path

from cgen import *
from config import *
from makegen import *
from util import *

def test_makefile_generation():
    target = 'makegen_test'
    mgen = Makegen(target)
    mgen.adjust('CFLAGS', '-fPIC', Mod.REMOVE)
    mgen.adjust('LDFLAGS', '-shared', Mod.REMOVE)
    mgen.generate()

    cgen = CGen('main.c')
    with cgen.open_function('int', 'main'):
        pass
    cgen.write()

    assert exec_bash('make -C {}'.format(working_dir))[0] == 0
    assert exec_bash(working_dir / target)[0] == 0
