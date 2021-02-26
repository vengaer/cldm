import re

from cgen import *
from config import *
from makegen import *
from symbols import *
from util import *

def test_gmock_will_repeatedly():
    target = 'mockups_test'
    symbol_tu = 'syms.c'

    db = read_db()
    cgen = CGen('main.c')
    cgen.append_line('#define CLDM_GMOCK_COMPAT')
    cgen.append_include('cldm.h', system_header=False)
    cgen.append_include('syms.h', system_header=False)
    with cgen.open_function('int', 'main'):
        cgen.append_line('EXPECT_CALL(bar).WillRepeatedly(RETURN(28));')
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

def test_gmock_will_once():
    target = 'mockups_test'
    symbol_tu = 'syms.c'

    db = read_db()
    call = CGen.default_call('ret2', db['symbols']['ret2']['params']);
    cgen = CGen('main.c')
    cgen.append_line('#define CLDM_GMOCK_COMPAT')
    cgen.append_include('cldm.h', system_header=False)
    cgen.append_include('syms.h', system_header=False)
    with cgen.open_function('int', 'main'):
        cgen.append_line('EXPECT_CALL(ret2).WillOnce(RETURN(28));')    \
            .append_line('printf("%d\\n", {});'.format(call))          \
            .append_line('printf("%d\\n", {});'.format(call))
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
    assert output.decode('utf-8').strip().split('\n') == ['28', re.search(r'puts\("(.*)"\)', db['symbols']['ret2']['exec'][0]).group(1), '2']

def test_gmock_will_n_times():
    target = 'mockups_test'
    symbol_tu = 'syms.c'

    db = read_db()
    call = CGen.default_call('ret2', db['symbols']['ret2']['params']);
    cgen = CGen('main.c')
    cgen.append_line('#define CLDM_GMOCK_COMPAT')
    cgen.append_include('cldm.h', system_header=False)
    cgen.append_include('syms.h', system_header=False)
    with cgen.open_function('int', 'main'):
        cgen.append_line('EXPECT_CALL(ret2).WillNTimes(2, RETURN(28));')    \
            .append_line('printf("%d\\n", {});'.format(call))               \
            .append_line('printf("%d\\n", {});'.format(call))               \
            .append_line('printf("%d\\n", {});'.format(call))
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
    assert output.decode('utf-8').strip().split('\n') == ['28', '28', re.search(r'puts\("(.*)"\)', db['symbols']['ret2']['exec'][0]).group(1), '2']

def test_gmock_return():
    target = 'mockups_test'
    symbol_tu = 'syms.c'

    db = read_db()
    call = CGen.default_call('ret2', db['symbols']['ret2']['params']);
    cgen = CGen('main.c')
    cgen.append_line('#define CLDM_GMOCK_COMPAT')
    cgen.append_include('cldm.h', system_header=False)
    cgen.append_include('syms.h', system_header=False)
    with cgen.open_function('int', 'main'):
        cgen.append_line('EXPECT_CALL(ret2).WILL_ONCE(Return(28));')    \
            .append_line('printf("%d\\n", {});'.format(call))          \
            .append_line('printf("%d\\n", {});'.format(call))
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
    assert output.decode('utf-8').strip().split('\n') == ['28', re.search(r'puts\("(.*)"\)', db['symbols']['ret2']['exec'][0]).group(1), '2']

def test_gmock_will_invoke_default():
    target = 'mockups_test'
    symbol_tu = 'syms.c'

    db = read_db()
    call = CGen.default_call('ret2', db['symbols']['ret2']['params']);
    cgen = CGen('main.c')
    cgen.append_line('#define CLDM_GMOCK_COMPAT')
    cgen.append_include('cldm.h', system_header=False)
    cgen.append_include('syms.h', system_header=False)
    with cgen.open_function('int', 'main'):
        cgen.append_line('EXPECT_CALL(ret2).WILL_REPEATEDLY(RETURN(28));')      \
            .append_line('printf("%d\\n", {});'.format(call))                   \
            .append_line('EXPECT_CALL(ret2).WillInvokeDefault();')              \
            .append_line('printf("%d\\n", {});'.format(call))
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
    assert output.decode('utf-8').strip().split('\n') == ['28', re.search(r'puts\("(.*)"\)', db['symbols']['ret2']['exec'][0]).group(1), '2']

def test_gmock_invoke():
    target = 'mockups_test'
    symbol_tu = 'syms.c'
    mockmsg = 'mockfoo called'

    db = read_db()
    cgen = CGen('main.c')
    cgen.append_line('#define CLDM_GMOCK_COMPAT')           \
        .append_include('cldm.h', system_header=False)      \
        .append_include('syms.h', system_header=False)      \
        .append_include('stdio.h')

    with cgen.open_function('void', 'mockfoo', ['int', 'char']):
        cgen.append_line('(void)a0;')                       \
            .append_line('(void)a1;')                       \
            .append_line('puts("{}");'.format(mockmsg))

    with cgen.open_function('int', 'main'):
        cgen.append_line('EXPECT_CALL(foo).WILL_REPEATEDLY(Invoke(mockfoo));')  \
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

def test_gmock_return_arg():
    target = 'mockups_test'
    symbol_tu = 'syms.c'

    db = read_db()
    cgen = CGen('main.c')
    cgen.append_line('#define CLDM_GMOCK_COMPAT')           \
        .append_include('cldm.h', system_header=False)      \
        .append_include('syms.h', system_header=False)      \
        .append_include('stdio.h')

    with cgen.open_function('int', 'main'):
        cgen.append_line('EXPECT_CALL(retarg).WILL_REPEATEDLY(ReturnArg(2));')  \
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

def test_gmock_return_pointee():
    target = 'mockups_test'
    symbol_tu = 'syms.c'

    db = read_db()
    cgen = CGen('main.c')
    cgen.append_line('#define CLDM_GMOCK_COMPAT')           \
        .append_include('cldm.h', system_header=False)      \
        .append_include('syms.h', system_header=False)      \
        .append_include('stdio.h')

    with cgen.open_function('int', 'main'):
        cgen.append_line('int i = 10, j = 12;')                                         \
            .append_line('EXPECT_CALL(retpointee).WILL_REPEATEDLY(ReturnPointee(0));')  \
            .append_line('printf("%d\\n", retpointee(&i, &j));')                        \
            .append_line('EXPECT_CALL(retpointee).WILL_REPEATEDLY(ReturnPointee(1));')  \
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

def test_increment_counter():
    target = 'mockups_test'
    symbol_tu = 'syms.c'

    db = read_db()
    cgen = CGen('main.c')
    cgen.append_line('#define CLDM_GMOCK_COMPAT')           \
        .append_include('cldm.h', system_header=False)      \
        .append_include('syms.h', system_header=False)      \
        .append_include('stdio.h')

    with cgen.open_function('int', 'main'):
        cgen.append_line('EXPECT_CALL(ret2).WILL_REPEATEDLY(IncrementCounter(0));')     \
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

def test_gmock_assign():
    target = 'mockups_test'
    symbol_tu = 'syms.c'

    db = read_db()
    cgen = CGen('main.c')
    cgen.append_line('#define CLDM_GMOCK_COMPAT')           \
        .append_include('cldm.h', system_header=False)      \
        .append_include('syms.h', system_header=False)      \
        .append_include('stdio.h')

    with cgen.open_function('int', 'main'):
        cgen.append_line('int i = 10, j = 12;')                                         \
            .append_line('EXPECT_CALL(baz).WILL_REPEATEDLY(Assign(i, j));')             \
            .append_line('printf("%d\\n", i);')                                         \
            .append_line('baz();')                                                      \
            .append_line('printf("%d\\n", i);')                                         \
            .append_line('EXPECT_CALL(baz).WILL_REPEATEDLY(Assign(i, 13, int));')       \
            .append_line('baz();')                                                      \
            .append_line('printf("%d\\n", i);')                                         \
            .append_line('EXPECT_CALL(baz).WILL_REPEATEDLY(Assign(i, (int){ 14 }));')   \
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
