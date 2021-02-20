from cgen import *
from config import *
from makegen import *
from symbols import *
from util import *

def test_readme_ex0():
    target = 'makegen_test'
    symbol_tu = 'syms.c'

    db = read_db()
    cgen = CGen('main.c')
    cgen.append_include('lmc.h', system_header=False)   \
        .append_include('assert.h')                     \
        .append_include('stdlib.h')

    with cgen.with_open_function('int', 'main'):
        cgen.append_line('EXPECT_CALL(atoi).WILL_ONCE(RETURN(8));')         \
            .append_line('assert(atoi("2") == 8);')                         \
            .append_line('assert(atoi("2") == 2);')                         \
            .append_line('EXPECT_CALL(atoi).WILL_REPEATEDLY(RETURN(7));')   \
            .append_line('assert(atoi("2") == 7);')                         \
            .append_line('assert(atoi("6") == 7);')
    cgen.write()

    cgen = CGen(symbol_tu)
    cgen.generate_matching_symbols()
    cgen.write()

    mgen = Makegen(target, src='main.c')
    gen_default_makefile(mgen, target, symbol_tu)
    mgen.generate()

    assert exec_bash('make -C {}'.format(working_dir))[0] == 0
    assert exec_bash('LD_PRELOAD={} LD_LIBRARY_PATH={} {}'.format(solib, working_dir, working_dir / target))[0] == 0
