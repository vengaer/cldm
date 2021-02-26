import re

from pathlib import Path

from cgen import *
from cldm import *
from config import *
from makegen import *
from symbols import *
from util import *

def test_preload():
    target = 'mockups_test'
    symbol_tu = 'syms.c'

    db = read_db()
    cgen = CGen('main.c')
    cgen.append_include('cldm.h', system_header=False)
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

def test_symbol_fallback():
    target = 'mockups_test'
    symbol_tu = 'syms.c'

    db = read_db()
    cgen = CGen('main.c')
    cgen.append_include('cldm.h', system_header=False)      \
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
    target = 'mockups_test'
    symbol_tu = 'syms.c'
    mockmsg = 'mockfoo called'

    db = read_db()
    cgen = CGen('main.c')
    cgen.append_include('cldm.h', system_header=False)      \
        .append_include('syms.h', system_header=False)      \
        .append_include('stdio.h')

    with cgen.with_open_function('void', 'mockfoo', ['int', 'char']):
        cgen.append_line('(void)a0;')                       \
            .append_line('(void)a1;')                       \
            .append_line('puts("{}");'.format(mockmsg))

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
    target = 'mockups_test'
    symbol_tu = 'syms.c'

    db = read_db()
    cgen = CGen('main.c')
    cgen.append_include('cldm.h', system_header=False)      \
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
    target = 'mockups_test'
    symbol_tu = 'syms.c'
    mockmsg = 'mockfoo called'

    db = read_db()
    cgen = CGen('main.c')
    cgen.append_include('cldm.h', system_header=False)      \
        .append_include('syms.h', system_header=False)      \
        .append_include('stdio.h')

    with cgen.with_open_function('void', 'mockfoo', ['int', 'char']):
        cgen.append_line('(void)a0;')                       \
            .append_line('(void)a1;')                       \
            .append_line('puts("{}");'.format(mockmsg))

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
    target = 'mockups_test'
    symbol_tu = 'syms.c'

    db = read_db()
    cgen = CGen('main.c')
    cgen.append_include('cldm.h', system_header=False)      \
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
    target = 'mockups_test'
    symbol_tu = 'syms.c'

    db = read_db()
    cgen = CGen('main.c')
    cgen.append_include('cldm.h', system_header=False)      \
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
    assert rv == 1
    assert output.decode('utf-8').strip().split('\n') == ['5', '1']


def test_max_params():
    target = 'mockups_test'

    typelist = 'int ' * 127
    typelist = [t for t in typelist.strip().split(' ')]
    cgen = CGen(symfile)
    cgen.append_include('cldm.h', system_header=False)  \
        .append_line('MOCK_FUNCTION(int, foo, {});'.format(', '.join(typelist)))
    cgen.write()

    cgen = CGen('syms.c')
    with cgen.with_open_function('int', 'foo', typelist):
        for i, _ in enumerate(typelist):
            cgen.append_line('(void)a{};'.format(i))
        cgen.append_return('8')
    cgen.write()

    cgen = CGen('syms.h')
    cgen.append_line('int foo({});'.format(', '.join([t + ' a{}'.format(i) for i, t in enumerate(typelist)])))
    cgen.write()

    cgen = CGen('main.c')
    cgen.append_include('cldm.h', system_header=False)              \
        .append_include('syms.h', system_header=False)
    with cgen.with_open_function('int', 'main'):
        cgen.append_line('EXPECT_CALL(foo).WILL_ONCE(RETURN(38));') \
            .append_return(CGen.default_call('foo', typelist))
    cgen.write()

    mgen = Makegen(target, src='main.c')
    gen_default_makefile(mgen, target, 'syms.c')
    mgen.generate()

    assert build_cldm()[0] == 0
    assert exec_bash('make -C {}'.format(working_dir))[0] == 0
    assert exec_bash('LD_PRELOAD={} LD_LIBRARY_PATH={} {}'.format(solib, working_dir, working_dir / target))[0] == 38

def test_will_n_times():
    target = 'mockups_test'
    symbol_tu = 'syms.c'

    db = read_db()
    cgen = CGen('main.c')
    cgen.append_include('cldm.h', system_header=False)      \
        .append_include('syms.h', system_header=False)      \
        .append_include('stdio.h')

    with cgen.with_open_function('int', 'main'):
        cgen.append_line('EXPECT_CALL(ret2).WILL_N_TIMES(2, RETURN(5));')   \
            .append_line('printf("%d\\n", ret2());')                        \
            .append_line('printf("%d\\n", ret2());')                        \
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
    assert output.decode('utf-8').strip().split('\n') == ['5', '5', re.search(r'puts\("(.*)"\)', db['symbols']['ret2']['exec'][0]).group(1), '2']

def test_will_invoke_default():
    target = 'mockups_test'
    symbol_tu = 'syms.c'

    db = read_db()
    cgen = CGen('main.c')
    cgen.append_include('cldm.h', system_header=False)      \
        .append_include('syms.h', system_header=False)      \
        .append_include('stdio.h')

    with cgen.with_open_function('int', 'main'):
        cgen.append_line('EXPECT_CALL(ret2).WILL_REPEATEDLY(RETURN(5));')   \
            .append_line('printf("%d\\n", ret2());')                        \
            .append_line('printf("%d\\n", ret2());')                        \
            .append_line('EXPECT_CALL(ret2).WILL_INVOKE_DEFAULT();')        \
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
    assert output.decode('utf-8').strip().split('\n') == ['5', '5', re.search(r'puts\("(.*)"\)', db['symbols']['ret2']['exec'][0]).group(1), '2']

def test_return_arg():
    target = 'mockups_test'
    symbol_tu = 'syms.c'

    db = read_db()
    cgen = CGen('main.c')
    cgen.append_include('cldm.h', system_header=False)      \
        .append_include('syms.h', system_header=False)      \
        .append_include('stdio.h')

    with cgen.with_open_function('int', 'main'):
        cgen.append_line('EXPECT_CALL(retarg).WILL_REPEATEDLY(RETURN_ARG(2));') \
            .append_line('printf("%d\\n", retarg("foo", 0, 10));')              \
            .append_line('printf("%d\\n", retarg("bar", 0, 8));')
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
    assert output.decode('utf-8').strip().split('\n') == ['10', '8']

def test_return_pointee():
    target = 'mockups_test'
    symbol_tu = 'syms.c'

    db = read_db()
    cgen = CGen('main.c')
    cgen.append_include('cldm.h', system_header=False)      \
        .append_include('syms.h', system_header=False)      \
        .append_include('stdio.h')

    with cgen.with_open_function('int', 'main'):
        cgen.append_line('int i = 10, j = 12;')                                         \
            .append_line('EXPECT_CALL(retpointee).WILL_REPEATEDLY(RETURN_POINTEE(0));') \
            .append_line('printf("%d\\n", retpointee(&i, &j));')                        \
            .append_line('EXPECT_CALL(retpointee).WILL_REPEATEDLY(RETURN_POINTEE(1));') \
            .append_line('printf("%d\\n", retpointee(&i, &j));')
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
    assert output.decode('utf-8').strip().split('\n') == ['10', '12']

def test_assign():
    target = 'mockups_test'
    symbol_tu = 'syms.c'

    db = read_db()
    cgen = CGen('main.c')
    cgen.append_include('cldm.h', system_header=False)      \
        .append_include('syms.h', system_header=False)      \
        .append_include('stdio.h')

    with cgen.with_open_function('int', 'main'):
        cgen.append_line('int i = 10, j = 12;')                                         \
            .append_line('EXPECT_CALL(baz).WILL_REPEATEDLY(ASSIGN(i, j));')             \
            .append_line('printf("%d\\n", i);')                                         \
            .append_line('baz();')                                                      \
            .append_line('printf("%d\\n", i);')                                         \
            .append_line('EXPECT_CALL(baz).WILL_REPEATEDLY(ASSIGN(i, 13, int));')       \
            .append_line('baz();')                                                      \
            .append_line('printf("%d\\n", i);')                                         \
            .append_line('EXPECT_CALL(baz).WILL_REPEATEDLY(ASSIGN(i, (int){ 14 }));')   \
            .append_line('baz();')                                                      \
            .append_line('printf("%d\\n", i);')
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
    assert output.decode('utf-8').strip().split('\n') == ['10', '12', '13', '14']
