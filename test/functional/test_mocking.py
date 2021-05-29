from cgen import *
from config import *
from makegen import *
from runner import *
from symbols import *
from util import *

_BINARY = 'mock_test'
_SYMBOL_TU = 'syms.c'
_RUNCMD = 'LD_PRELOAD={} LD_LIBRARY_PATH={} {}/{}'.format(solib, working_dir, working_dir, _BINARY)

def gen_makefile():
    mgen = Makegen(_BINARY, src='test.c')
    mgen.default(mgen, _BINARY, _SYMBOL_TU)
    mgen.adjust('LDLIBS', '-lcldm_main', Mod.PREPEND)
    mgen.generate()

def gen_symbols():
    cgen = CGen(_SYMBOL_TU)
    cgen.generate_matching_symbols()
    cgen.write()

def success_string(nassertions):
    return 'Successfully finished {} assertions'.format(nassertions)

def test_preload():
    db = read_db()
    cgen = CGen('test.c')
    cgen.append_include('cldm.h', system_header=False)
    cgen.append_include('syms.h', system_header=False)
    with cgen.open_macro('TEST', 'foo'):
        cgen.append_line('WHEN_CALLED(bar).SHOULD_REPEATEDLY(RETURN(28));')   \
            .append_line('ASSERT_EQ(28, {});'.format(CGen.default_call('bar', db['symbols']['bar']['params'])))
    cgen.write()

    gen_symbols()
    gen_makefile()

    run(ContainsMatcher(success_string(1)), runcmd=_RUNCMD)

def test_symbol_fallback():
    db = read_db()
    cgen = CGen('test.c')
    cgen.append_include('cldm.h', system_header=False)      \
        .append_include('syms.h', system_header=False)

    with cgen.open_macro('TEST', 'foo'):
        cgen.append_line('ASSERT_EQ(ret2(), {});'.format(db['symbols']['ret2']['return']))
    cgen.write()

    gen_symbols()
    gen_makefile()

    run(ContainsMatcher(success_string(1)), runcmd=_RUNCMD)

def test_invoke():
    db = read_db()
    cgen = CGen('test.c')
    cgen.append_include('cldm.h', system_header=False)      \
        .append_include('syms.h', system_header=False)

    cgen.append_line('int mockfoo_called  = 0;')
    with cgen.open_function('void', 'mockfoo', db['symbols']['foo']['params']):
        cgen.append_line('(void)a0;')                       \
            .append_line('(void)a1;')                       \
            .append_line('mockfoo_called = 1;')

    with cgen.open_macro('TEST', 'bar'):
        cgen.append_line('WHEN_CALLED(foo).SHOULD_REPEATEDLY(INVOKE(mockfoo));')                      \
            .append_line('{};'.format(CGen.default_call('foo', db['symbols']['foo']['params'])))    \
            .append_line('ASSERT_EQ(mockfoo_called, 1);')
    cgen.write()

    gen_symbols()
    gen_makefile()

    run(ContainsMatcher(success_string(1)), runcmd=_RUNCMD)

def test_increment_counter():
    db = read_db()
    cgen = CGen('test.c')
    cgen.append_include('cldm.h', system_header=False)      \
        .append_include('syms.h', system_header=False)

    with cgen.open_macro('TEST', 'foo'):
        cgen.append_line('WHEN_CALLED(ret2).SHOULD_REPEATEDLY(INCREMENT_COUNTER(0));')    \
            .append_line('ASSERT_EQ(1, ret2());')                                       \
            .append_line('ASSERT_EQ(2, ret2());')                                       \
            .append_line('ASSERT_EQ(3, ret2());')
    cgen.write()

    gen_symbols()
    gen_makefile()

    run(ContainsMatcher(success_string(3)), runcmd=_RUNCMD)

def test_invoke_with_fallback():
    db = read_db()
    cgen = CGen('test.c')
    cgen.append_include('cldm.h', system_header=False)      \
        .append_include('syms.h', system_header=False)

    cgen.append_line('int mockfoo_called = 0;')

    with cgen.open_function('void', 'mockfoo', db['symbols']['foo']['params']):
        cgen.append_line('(void)a0;')                       \
            .append_line('(void)a1;')                       \
            .append_line('mockfoo_called = 1;')

    with cgen.open_macro('TEST', 'foo'):
        cgen.append_line('WHEN_CALLED(foo).SHOULD_REPEATEDLY(INVOKE(mockfoo));')                                              \
            .append_line('{};'.format(CGen.default_call('foo', db['symbols']['foo']['params'])))                            \
            .append_line('foo({});'.format(', '.join(['({}){{ 0 }}'.format(a) for a in db['symbols']['foo']['params']])))   \
            .append_line('ASSERT_EQ(mockfoo_called, 1);')                                                                   \
            .append_line('ASSERT_EQ(ret2(), 2);')
    cgen.write()

    gen_symbols()
    gen_makefile()

    run(ContainsMatcher(success_string(2)), runcmd=_RUNCMD)

def test_will_once():
    db = read_db()
    cgen = CGen('test.c')
    cgen.append_include('cldm.h', system_header=False)      \
        .append_include('syms.h', system_header=False)

    with cgen.open_macro('TEST', 'foo'):
        cgen.append_line('WHEN_CALLED(ret2).SHOULD_ONCE(RETURN(5));') \
            .append_line('ASSERT_EQ(5, ret2());')                   \
            .append_line('ASSERT_EQ(2, ret2());')
    cgen.write()

    gen_symbols()
    gen_makefile()

    run(ContainsMatcher(success_string(2)), runcmd=_RUNCMD)

def test_atoi_mock():
    db = read_db()
    cgen = CGen('test.c')
    cgen.append_include('cldm.h', system_header=False)      \
        .append_include('syms.h', system_header=False)      \
        .append_include('stdlib.h')

    with cgen.open_macro('TEST', 'foo'):
        cgen.append_line('WHEN_CALLED(atoi).SHOULD_ONCE(RETURN(5));') \
            .append_line('ASSERT_EQ(atoi("1"), 5);')                \
            .append_line('ASSERT_EQ(atoi("1"), 1);')
    cgen.write()

    gen_symbols()
    gen_makefile()

    run(ContainsMatcher(success_string(2)), runcmd=_RUNCMD)


def test_max_params():
    typelist = 'int ' * 127
    typelist = [t for t in typelist.strip().split(' ')]
    cgen = CGen(symfile)
    cgen.append_include('cldm.h', system_header=False)  \
        .append_line('MOCK_FUNCTION(int, foo, {});'.format(', '.join(typelist)))
    cgen.write()

    cgen = CGen('syms.c')
    with cgen.open_function('int', 'foo', typelist):
        for i, _ in enumerate(typelist):
            cgen.append_line('(void)a{};'.format(i))
        cgen.append_return('8')
    cgen.write()

    cgen = CGen('syms.h')
    cgen.append_line('int foo({});'.format(', '.join([t + ' a{}'.format(i) for i, t in enumerate(typelist)])))
    cgen.write()

    cgen = CGen('test.c')
    cgen.append_include('cldm.h', system_header=False)              \
        .append_include('syms.h', system_header=False)
    with cgen.open_macro('TEST', 'ironing_board'):
        cgen.append_line('WHEN_CALLED(foo).SHOULD_ONCE(RETURN(38));') \
            .append_line('ASSERT_EQ(38, {});'.format(CGen.default_call('foo', typelist)))
    cgen.write()

    gen_makefile()

    build_cldm()
    run(ContainsMatcher(success_string(1)), runcmd=_RUNCMD)

def test_will_n_times():
    db = read_db()
    cgen = CGen('test.c')
    cgen.append_include('cldm.h', system_header=False)      \
        .append_include('syms.h', system_header=False)

    with cgen.open_macro('TEST', 'foo'):
        cgen.append_line('WHEN_CALLED(ret2).SHOULD_N_TIMES(2, RETURN(5));')   \
            .append_line('ASSERT_EQ(5, ret2());')                           \
            .append_line('ASSERT_EQ(5, ret2());')                           \
            .append_line('ASSERT_EQ(2, ret2());')
    cgen.write()

    gen_symbols()
    gen_makefile()

    run(ContainsMatcher(success_string(3)), runcmd=_RUNCMD)

def test_will_invoke_default():
    db = read_db()
    cgen = CGen('test.c')
    cgen.append_include('cldm.h', system_header=False)      \
        .append_include('syms.h', system_header=False)

    with cgen.open_macro('TEST', 'foo'):
        cgen.append_line('WHEN_CALLED(ret2).SHOULD_REPEATEDLY(RETURN(5));')   \
            .append_line('ASSERT_EQ(5, ret2());')                           \
            .append_line('ASSERT_EQ(5, ret2());')                           \
            .append_line('WHEN_CALLED(ret2).SHOULD_INVOKE_DEFAULT();')        \
            .append_line('ASSERT_EQ(2, ret2());')
    cgen.write()

    gen_symbols()
    gen_makefile()

    run(ContainsMatcher(success_string(3)), runcmd=_RUNCMD)

def test_return_param():
    db = read_db()
    cgen = CGen('test.c')
    cgen.append_include('cldm.h', system_header=False)      \
        .append_include('syms.h', system_header=False)

    with cgen.open_macro('TEST', 'foo'):
        cgen.append_line('WHEN_CALLED(retarg).SHOULD_REPEATEDLY(RETURN_PARAM(2));') \
            .append_line('ASSERT_EQ(10, retarg("foo", 0, 10));')                \
            .append_line('ASSERT_EQ(8, retarg("foo", 0, 8));')
    cgen.write()

    gen_symbols()
    gen_makefile()

    run(ContainsMatcher(success_string(2)), runcmd=_RUNCMD)

def test_return_pointee():
    db = read_db()
    cgen = CGen('test.c')
    cgen.append_include('cldm.h', system_header=False)      \
        .append_include('syms.h', system_header=False)

    with cgen.open_macro('TEST', 'foo'):
        cgen.append_line('int i = 10, j = 12;')                                         \
            .append_line('WHEN_CALLED(retpointee).SHOULD_REPEATEDLY(RETURN_POINTEE(0));') \
            .append_line('ASSERT_EQ(10, retpointee(&i, &j));')                          \
            .append_line('WHEN_CALLED(retpointee).SHOULD_REPEATEDLY(RETURN_POINTEE(1));') \
            .append_line('ASSERT_EQ(12, retpointee(&i, &j));')
    cgen.write()

    gen_symbols()
    gen_makefile()

    run(ContainsMatcher(success_string(2)), runcmd=_RUNCMD)

def test_assign():
    db = read_db()
    cgen = CGen('test.c')
    cgen.append_include('cldm.h', system_header=False)      \
        .append_include('syms.h', system_header=False)

    with cgen.open_macro('TEST', 'foo'):
        cgen.append_line('int i = 10, j = 12;')                                         \
            .append_line('WHEN_CALLED(baz).SHOULD_REPEATEDLY(ASSIGN(i, j));')             \
            .append_line('ASSERT_EQ(10, i);')                                           \
            .append_line('baz();')                                                      \
            .append_line('ASSERT_EQ(12, i);')                                           \
            .append_line('WHEN_CALLED(baz).SHOULD_REPEATEDLY(ASSIGN(i, 13, int));')       \
            .append_line('baz();')                                                      \
            .append_line('ASSERT_EQ(13, i);')                                           \
            .append_line('WHEN_CALLED(baz).SHOULD_REPEATEDLY(ASSIGN(i, (int){ 14 }));')   \
            .append_line('baz();')                                                      \
            .append_line('ASSERT_EQ(14, i);')
    cgen.write()

    gen_symbols()
    gen_makefile()

    run(ContainsMatcher(success_string(4)), runcmd=_RUNCMD)

def test_assign_param():
    db = read_db()
    cgen = CGen('test.c')
    cgen.append_include('cldm.h', system_header=False)      \
        .append_include('syms.h', system_header=False)

    with cgen.open_macro('TEST', 'foo'):
        cgen.append_line('int i = 10;')                                         \
            .append_line('WHEN_CALLED(foo).SHOULD_REPEATEDLY(ASSIGN_PARAM(0, i));') \
            .append_line('ASSERT_EQ(10, i);')                                   \
            .append_line('foo(12, 13);')                                        \
            .append_line('ASSERT_EQ(12, i);')
    cgen.write()

    gen_symbols()
    gen_makefile()

    run(ContainsMatcher(success_string(2)), runcmd=_RUNCMD)

def test_force_disable():
    db = read_db()
    cgen = CGen('test.c')
    cgen.append_include('cldm.h', system_header=False)      \
        .append_include('syms.h', system_header=False)      \
        .append_include('stdlib.h')

    with cgen.open_macro('TEST', 'foo'):
        cgen.append_line('cldm_mock_disable_all();')                \
            .append_line('WHEN_CALLED(atoi).SHOULD_ONCE(RETURN(5));') \
            .append_line('ASSERT_EQ(1, atoi("1"));')
    cgen.write()

    gen_symbols()
    gen_makefile()

    run(ContainsMatcher(success_string(1)), runcmd=_RUNCMD)
