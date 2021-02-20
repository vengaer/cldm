import os
import re

from pathlib import Path

from cgen import *
from config import *
from makegen import *
from symbols import *
from util import *

def gen_default_makefile(mgen, target, symbol_tu):
    mgen.adjust('CFLAGS', '-fPIC', Mod.REMOVE)
    mgen.adjust('LDFLAGS', '-shared', Mod.REMOVE)
    mgen.adjust('LDFLAGS', '-rdynamic -L. -L{} -lmockc'.format(project_root), Mod.APPEND)
    mgen.adjust('LDLIBS', '-ltest', Mod.APPEND)
    mgen.add_rule('libtest.so', '$(builddir)/syms.o', '$(QUIET)$(CC) -o $@ $^ -shared $(LDFLAGS) $(LDLIBS)', '[LD] $@')
    mgen.add_rule('$(builddir)/syms.o', str(working_dir / symbol_tu), '$(QUIET)$(CC) -o $@ $^ $(CFLAGS) $(CPPFLAGS) -fPIC', '[CC] $@')
    mgen.add_prereq(target, 'libtest.so')

def test_preload():
    target = 'makegen_test'
    symbol_tu = 'syms.c'

    db = read_db()
    cgen = CGen('main.c')
    cgen.append_include('lmc.h', system_header=False)
    cgen.append_include('syms.h', system_header=False)
    with cgen.with_open_function('int', 'main'):
        cgen.append_line('EXPECT_CALL(bar).WILL_REPEATEDLY(RETURN(28));')
        cgen.append_return(CGen.default_call('bar', db['symbols']['bar']['params']))
    cgen.write()

    cgen = CGen(symbol_tu)
    cgen.generate_matching_symbols()
    cgen.write()

    mgen = Makegen(target, src='main.c')
    gen_default_makefile(mgen, target, symbol_tu)
    mgen.generate()

    assert exec_bash('make -C {}'.format(working_dir))[0] == 0
    assert exec_bash('LD_PRELOAD={} LD_LIBRARY_PATH={} {}'.format(solib, working_dir, working_dir / target))[0] == 28

def test_gmock_compat():
    target = 'makegen_test'
    symbol_tu = 'syms.c'

    db = read_db()
    cgen = CGen('main.c')
    cgen.append_line('#define LMC_GMOCK_COMPAT')
    cgen.append_include('lmc.h', system_header=False)
    cgen.append_include('syms.h', system_header=False)
    with cgen.with_open_function('int', 'main'):
        cgen.append_line('EXPECT_CALL(bar).WillRepeatedly(Return(28));')
        cgen.append_return(CGen.default_call('bar', db['symbols']['bar']['params']))
    cgen.write()

    cgen = CGen(symbol_tu)
    cgen.generate_matching_symbols()
    cgen.write()

    mgen = Makegen(target, src='main.c')
    gen_default_makefile(mgen, target, symbol_tu)
    mgen.generate()

    assert exec_bash('make -C {}'.format(working_dir))[0] == 0
    assert exec_bash('LD_PRELOAD={} LD_LIBRARY_PATH={} {}'.format(solib, working_dir, working_dir / target))[0] == 28

def test_symbol_fallback():
    target = 'makegen_test'
    symbol_tu = 'syms.c'

    db = read_db()
    cgen = CGen('main.c')
    cgen.append_include('lmc.h', system_header=False)       \
        .append_include('syms.h', system_header=False)      \
        .append_include('stdio.h')

    with cgen.with_open_function('int', 'main'):
        cgen.append_return('ret2()')
    cgen.write()

    cgen = CGen(symbol_tu)
    cgen.generate_matching_symbols()
    cgen.write()

    mgen = Makegen(target, src='main.c')
    gen_default_makefile(mgen, target, symbol_tu)
    mgen.generate()

    assert exec_bash('make -C {}'.format(working_dir))[0] == 0
    rv, output, _ = exec_bash('LD_PRELOAD={} LD_LIBRARY_PATH={} {}'.format(solib, working_dir, working_dir / target))
    assert rv == db['symbols']['ret2']['return']
    assert output.decode('utf-8').replace('\n', '') == re.search(r'puts\("(.*)"\)', db['symbols']['ret2']['exec'][0]).group(1)


def test_invoke():
    target = 'makegen_test'
    symbol_tu = 'syms.c'
    mockmsg = 'mockfoo called'

    db = read_db()
    cgen = CGen('main.c')
    cgen.append_include('lmc.h', system_header=False)       \
        .append_include('syms.h', system_header=False)      \
        .append_include('stdio.h')

    with cgen.with_open_function('void', 'mockfoo', ['int', 'char']):
        cgen.append_line('puts("{}");'.format(mockmsg))

    with cgen.with_open_function('int', 'main'):
        cgen.append_line('EXPECT_CALL(foo).WILL_REPEATEDLY(INVOKE(mockfoo));')  \
            .append_line('foo({});'.format(', '.join(['({}){{ 0 }}'.format(a) for a in db['symbols']['foo']['params']])))
    cgen.write()

    cgen = CGen(symbol_tu)
    cgen.generate_matching_symbols()
    cgen.write()

    mgen = Makegen(target, src='main.c')
    gen_default_makefile(mgen, target, symbol_tu)
    mgen.generate()

    assert exec_bash('make -C {}'.format(working_dir))[0] == 0
    rv, output, _ = exec_bash('LD_PRELOAD={} LD_LIBRARY_PATH={} {}'.format(solib, working_dir, working_dir / target))
    assert rv == 0
    assert output.decode('utf-8').replace('\n', '') == mockmsg

def test_increment_counter():
    target = 'makegen_test'
    symbol_tu = 'syms.c'

    db = read_db()
    cgen = CGen('main.c')
    cgen.append_include('lmc.h', system_header=False)       \
        .append_include('syms.h', system_header=False)      \
        .append_include('stdio.h')

    with cgen.with_open_function('int', 'main'):
        cgen.append_line('EXPECT_CALL(ret2).WILL_REPEATEDLY(INCREMENT_COUNTER(0));')    \
            .append_line('printf("%d\\n", ret2());')                                    \
            .append_line('printf("%d\\n", ret2());')                                    \
            .append_line('printf("%d\\n", ret2());')
    cgen.write()

    cgen = CGen(symbol_tu)
    cgen.generate_matching_symbols()
    cgen.write()

    mgen = Makegen(target, src='main.c')
    gen_default_makefile(mgen, target, symbol_tu)
    mgen.generate()

    assert exec_bash('make -C {}'.format(working_dir))[0] == 0
    rv, output, _ = exec_bash('LD_PRELOAD={} LD_LIBRARY_PATH={} {}'.format(solib, working_dir, working_dir / target))
    assert rv == 0
    assert output.decode('utf-8').replace('\n', '') == '123'

def test_invoke_with_fallback():
    target = 'makegen_test'
    symbol_tu = 'syms.c'
    mockmsg = 'mockfoo called'

    db = read_db()
    cgen = CGen('main.c')
    cgen.append_include('lmc.h', system_header=False)       \
        .append_include('syms.h', system_header=False)      \
        .append_include('stdio.h')

    with cgen.with_open_function('void', 'mockfoo', ['int', 'char']):
        cgen.append_line('puts("{}");'.format(mockmsg))

    with cgen.with_open_function('int', 'main'):
        cgen.append_line('EXPECT_CALL(foo).WILL_REPEATEDLY(INVOKE(mockfoo));')                                              \
            .append_line('foo({});'.format(', '.join(['({}){{ 0 }}'.format(a) for a in db['symbols']['foo']['params']])))   \
            .append_return('ret2()')
    cgen.write()

    cgen = CGen(symbol_tu)
    cgen.generate_matching_symbols()
    cgen.write()

    mgen = Makegen(target, src='main.c')
    gen_default_makefile(mgen, target, symbol_tu)
    mgen.generate()

    assert exec_bash('make -C {}'.format(working_dir))[0] == 0
    rv, output, _ = exec_bash('LD_PRELOAD={} LD_LIBRARY_PATH={} {}'.format(solib, working_dir, working_dir / target))
    assert rv == db['symbols']['ret2']['return']
    assert output.decode('utf-8').strip().split('\n') == [mockmsg, re.search(r'puts\("(.*)"\)', db['symbols']['ret2']['exec'][0]).group(1)]

def test_will_once():
    target = 'makegen_test'
    symbol_tu = 'syms.c'

    db = read_db()
    cgen = CGen('main.c')
    cgen.append_include('lmc.h', system_header=False)       \
        .append_include('syms.h', system_header=False)      \
        .append_include('stdio.h')

    with cgen.with_open_function('int', 'main'):
        cgen.append_line('EXPECT_CALL(ret2).WILL_ONCE(RETURN(5));') \
            .append_line('printf("%d\\n", ret2());')                \
            .append_line('printf("%d\\n", ret2());')
    cgen.write()

    cgen = CGen(symbol_tu)
    cgen.generate_matching_symbols()
    cgen.write()

    mgen = Makegen(target, src='main.c')
    gen_default_makefile(mgen, target, symbol_tu)
    mgen.generate()

    assert exec_bash('make -C {}'.format(working_dir))[0] == 0
    rv, output, _ = exec_bash('LD_PRELOAD={} LD_LIBRARY_PATH={} {}'.format(solib, working_dir, working_dir / target))
    assert rv == 0
    assert output.decode('utf-8').strip().split('\n') == ['5', re.search(r'puts\("(.*)"\)', db['symbols']['ret2']['exec'][0]).group(1), '2']

def test_atoi_mock():
    target = 'makegen_test'
    symbol_tu = 'syms.c'

    db = read_db()
    cgen = CGen('main.c')
    cgen.append_include('lmc.h', system_header=False)       \
        .append_include('syms.h', system_header=False)      \
        .append_include('stdio.h')                          \
        .append_include('stdlib.h')

    with cgen.with_open_function('int', 'main'):
        cgen.append_line('EXPECT_CALL(atoi).WILL_ONCE(RETURN(5));') \
            .append_line('printf("%d\\n", atoi("1"));')             \
            .append_line('printf("%d\\n", atoi("1"));')             \
            .append_return('atoi("1")')
    cgen.write()

    cgen = CGen(symbol_tu)
    cgen.generate_matching_symbols()
    cgen.write()

    mgen = Makegen(target, src='main.c')
    gen_default_makefile(mgen, target, symbol_tu)
    mgen.generate()

    assert exec_bash('make -C {}'.format(working_dir))[0] == 0
    rv, output, _ = exec_bash('LD_PRELOAD={} LD_LIBRARY_PATH={} {}'.format(solib, working_dir, working_dir / target))
    print(output)
    assert rv == 1
    assert output.decode('utf-8').strip().split('\n') == ['5', '1']

def test_readme_example():
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
