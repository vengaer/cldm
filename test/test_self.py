import os

from pathlib import Path

from cgen import *
from config import *
from makegen import *

def test_makefile_generation():
    target = 'makegen_test'
    mgen = Makegen(target)
    mgen.adjust('CFLAGS', '-fPIC', Mod.REMOVE)
    mgen.adjust('LDFLAGS', '-shared', Mod.REMOVE)
    mgen.generate()

    cgen = CGen('main.c')
    cgen.open_function('int', 'main')
    cgen.close_function()
    cgen.write()

    assert os.system('make -C {}'.format(working_dir)) == 0
    assert os.system(working_dir / target) == 0
