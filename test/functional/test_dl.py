from cgen import *
from cldm import *
from config import *
from makegen import *
from runner import *
from util import *

_BINARY = 'dl_test'
_SOURCEFILE = 'test.c'
_SYMBOL_TU = 'syms.c'
_RUNCMD = 'LD_PRELOAD={so} LD_LIBRARY_PATH={wd} {wd}/{binary}'.format(so=solib, wd=working_dir, binary=_BINARY)

def gen_makefile():
    mgen = Makegen(_BINARY, src=_SOURCEFILE)
    mgen.default(mgen, _BINARY, _SYMBOL_TU)
    mgen.adjust('LDLIBS', '-lcldm_main -ltest', Mod.PREPEND)
    mgen.generate()

def gen_symbols():
    cgen = CGen(_SYMBOL_TU)
    cgen.generate_matching_symbols()
    cgen.write()

def test_dl_cycle():
    cgen = CGen(_SOURCEFILE)
    cgen.append_include('cldm.h', system_header=False)
    cgen.append_include('syms.h', system_header=False)

    cgen.append_line('int whirlb(int);')
    with cgen.open_function('int', 'whirlf', ['int']):
        cgen.append_line('if(a0) {')
        cgen.append_line('  return a0;')
        cgen.append_line('}')
        cgen.append_return('pad1(1);')

    with cgen.open_function('int', 'whirlb', ['int']):
        cgen.append_return('pad0(a0);')

    with cgen.open_macro('TEST', 'whirl'):
        cgen.append_line('WHEN_CALLED(pad0).SHOULD_REPEATEDLY(INVOKE(whirlf));')
        cgen.append_line('WHEN_CALLED(pad1).SHOULD_REPEATEDLY(INVOKE(whirlb));')
        cgen.append_line('ASSERT_EQ(pad0(0), 1);')

    cgen.write()

    gen_makefile()
    gen_symbols()

    run(ContainsMatcher('Successfully finished'), runcmd=_RUNCMD)

def test_dl_realloc():
    cgen = CGen(_SOURCEFILE)
    cgen.append_include('cldm.h', system_header=False)
    cgen.append_include('syms.h', system_header=False)

    with cgen.open_macro('TEST', 'foo'):
        for i in range(33):
            cgen.append_line(f'ASSERT_EQ({i}, pad{i}({i}));')

    cgen.write()

    gen_makefile()
    gen_symbols()

    run(ContainsMatcher('Successfully finished'), runcmd=_RUNCMD)
