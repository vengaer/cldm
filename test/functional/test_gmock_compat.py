from cgen import *
from config import *
from makegen import *
from runner import *
from symbols import *
from util import *

_BINARY = 'gmock_compat_test'
_SYMBOL_TU = 'syms.c'
_RUNCMD = 'LD_PRELOAD={} LD_LIBRARY_PATH={} {}/{}'.format(solib, working_dir, working_dir, _BINARY)

def gen_makefile():
    mgen = Makegen(_BINARY, src='main.c')
    mgen.default(mgen, _BINARY, _SYMBOL_TU)
    mgen.adjust('LDLIBS', '-lcldm_main', Mod.PREPEND)
    mgen.generate()

def test_gmock_will_repeatedly():
    db = read_db()
    cgen = CGen('main.c')
    cgen.append_line('#define CLDM_GMOCK_COMPAT')
    cgen.append_include('cldm.h', system_header=False)
    cgen.append_include('syms.h', system_header=False)
    with cgen.open_macro('TEST', 'foo'):
        cgen.append_line('EXPECT_CALL(bar).WillRepeatedly(RETURN(28));')
        cgen.append_line('ASSERT_EQ({}, 28);'.format(CGen.default_call('bar', db['symbols']['bar']['params'])))

    cgen.write()

    cgen = CGen(_SYMBOL_TU)
    cgen.generate_matching_symbols()
    cgen.write()

    gen_makefile()

    run(ContainsMatcher('Successfully finished 1 assertion'), runcmd=_RUNCMD)

def test_gmock_will_once():
    db = read_db()
    call = CGen.default_call('ret2', db['symbols']['ret2']['params']);
    cgen = CGen('main.c')
    cgen.append_line('#define CLDM_GMOCK_COMPAT')
    cgen.append_include('cldm.h', system_header=False)
    cgen.append_include('syms.h', system_header=False)
    with cgen.open_macro('TEST', 'foo'):
        cgen.append_line('EXPECT_CALL(ret2).WillOnce(RETURN(28));')     \
            .append_line('ASSERT_EQ(28, {});'.format(call))             \
            .append_line('ASSERT_EQ(2, {});'.format(call))
    cgen.write()

    cgen = CGen(_SYMBOL_TU)
    cgen.generate_matching_symbols()
    cgen.write()

    gen_makefile()

    run(ContainsMatcher('Successfully finished 2 assertions'), runcmd=_RUNCMD)

def test_gmock_will_n_times():
    db = read_db()
    call = CGen.default_call('ret2', db['symbols']['ret2']['params']);
    cgen = CGen('main.c')
    cgen.append_line('#define CLDM_GMOCK_COMPAT')
    cgen.append_include('cldm.h', system_header=False)
    cgen.append_include('syms.h', system_header=False)
    with cgen.open_macro('TEST', 'foo'):
        cgen.append_line('EXPECT_CALL(ret2).WillNTimes(2, RETURN(28));')    \
            .append_line('ASSERT_EQ({}, 28);'.format(call))                 \
            .append_line('ASSERT_EQ({}, 28);'.format(call))                 \
            .append_line('ASSERT_EQ({}, 2);'.format(call))
    cgen.write()

    cgen = CGen(_SYMBOL_TU)
    cgen.generate_matching_symbols()
    cgen.write()

    gen_makefile()

    run(ContainsMatcher('Successfully finished 3 assertions'), runcmd=_RUNCMD)

def test_gmock_return():
    db = read_db()
    call = CGen.default_call('ret2', db['symbols']['ret2']['params']);
    cgen = CGen('main.c')
    cgen.append_line('#define CLDM_GMOCK_COMPAT')
    cgen.append_include('cldm.h', system_header=False)
    cgen.append_include('syms.h', system_header=False)
    with cgen.open_macro('TEST', 'foo'):
        cgen.append_line('EXPECT_CALL(ret2).WILL_ONCE(Return(28));')    \
            .append_line('ASSERT_EQ({}, 28);'.format(call))             \
            .append_line('ASSERT_EQ({}, 2);'.format(call))
    cgen.write()

    cgen = CGen(_SYMBOL_TU)
    cgen.generate_matching_symbols()
    cgen.write()

    gen_makefile()

    run(ContainsMatcher('Successfully finished 2 assertions'), runcmd=_RUNCMD)

def test_gmock_will_invoke_default():
    db = read_db()
    call = CGen.default_call('ret2', db['symbols']['ret2']['params']);
    cgen = CGen('main.c')
    cgen.append_line('#define CLDM_GMOCK_COMPAT')
    cgen.append_include('cldm.h', system_header=False)
    cgen.append_include('syms.h', system_header=False)
    with cgen.open_macro('TEST', 'foo'):
        cgen.append_line('EXPECT_CALL(ret2).WILL_REPEATEDLY(RETURN(28));')      \
            .append_line('ASSERT_EQ({}, 28);'.format(call))                     \
            .append_line('EXPECT_CALL(ret2).WillInvokeDefault();')              \
            .append_line('ASSERT_EQ({}, 2);'.format(call))
    cgen.write()

    cgen = CGen(_SYMBOL_TU)
    cgen.generate_matching_symbols()
    cgen.write()

    gen_makefile()

    run(ContainsMatcher('Successfully finished 2 assertions'), runcmd=_RUNCMD)

def test_gmock_invoke():
    db = read_db()
    cgen = CGen('main.c')
    cgen.append_line('#define CLDM_GMOCK_COMPAT')           \
        .append_include('cldm.h', system_header=False)      \
        .append_include('syms.h', system_header=False)

    cgen.append_line('int mockfoo_called = 0;')

    with cgen.open_function('void', 'mockfoo', db['symbols']['foo']['params']):
        cgen.append_line('(void)a0;')                       \
            .append_line('(void)a1;')                       \
            .append_line('mockfoo_called = 1;')

    with cgen.open_macro('TEST', 'bar'):
        cgen.append_line('EXPECT_CALL(foo).WILL_REPEATEDLY(Invoke(mockfoo));')                      \
            .append_line('ASSERT_EQ(mockfoo_called, 0);')                                           \
            .append_line('{};'.format(CGen.default_call('foo', db['symbols']['foo']['params'])))    \
            .append_line('ASSERT_EQ(mockfoo_called, 1);')
    cgen.write()

    cgen = CGen(_SYMBOL_TU)
    cgen.generate_matching_symbols()
    cgen.write()

    gen_makefile()

    run(ContainsMatcher('Successfully finished 2 assertions'), runcmd=_RUNCMD)

def test_gmock_return_arg():
    db = read_db()
    cgen = CGen('main.c')
    cgen.append_line('#define CLDM_GMOCK_COMPAT')           \
        .append_include('cldm.h', system_header=False)      \
        .append_include('syms.h', system_header=False)

    with cgen.open_macro('TEST', 'foo'):
        cgen.append_line('EXPECT_CALL(retarg).WILL_REPEATEDLY(ReturnArg(2));')  \
            .append_line('ASSERT_EQ(retarg("foo", 0, 10), 10);')                \
            .append_line('ASSERT_EQ(retarg("foo", 0, 8), 8);')
    cgen.write()

    cgen = CGen(_SYMBOL_TU)
    cgen.generate_matching_symbols()
    cgen.write()

    gen_makefile()

    run(ContainsMatcher('Successfully finished 2 assertions'), runcmd=_RUNCMD)

def test_gmock_return_pointee():
    db = read_db()
    cgen = CGen('main.c')
    cgen.append_line('#define CLDM_GMOCK_COMPAT')           \
        .append_include('cldm.h', system_header=False)      \
        .append_include('syms.h', system_header=False)

    with cgen.open_macro('TEST', 'foo'):
        cgen.append_line('int i = 10, j = 12;')                                         \
            .append_line('EXPECT_CALL(retpointee).WILL_REPEATEDLY(ReturnPointee(0));')  \
            .append_line('ASSERT_EQ(10, retpointee(&i, &j));')                          \
            .append_line('EXPECT_CALL(retpointee).WILL_REPEATEDLY(ReturnPointee(1));')  \
            .append_line('ASSERT_EQ(12, retpointee(&i, &j));')
    cgen.write()

    cgen = CGen(_SYMBOL_TU)
    cgen.generate_matching_symbols()
    cgen.write()

    gen_makefile()

    run(ContainsMatcher('Successfully finished 2 assertions'), runcmd=_RUNCMD)

def test_increment_counter():
    db = read_db()
    cgen = CGen('main.c')
    cgen.append_line('#define CLDM_GMOCK_COMPAT')           \
        .append_include('cldm.h', system_header=False)      \
        .append_include('syms.h', system_header=False)

    with cgen.open_macro('TEST', 'foo'):
        cgen.append_line('EXPECT_CALL(ret2).WILL_REPEATEDLY(IncrementCounter(0));')     \
            .append_line('ASSERT_EQ(1, ret2());')                                       \
            .append_line('ASSERT_EQ(2, ret2());')                                       \
            .append_line('ASSERT_EQ(3, ret2());')
    cgen.write()

    cgen = CGen(_SYMBOL_TU)
    cgen.generate_matching_symbols()
    cgen.write()

    gen_makefile()

    run(ContainsMatcher('Successfully finished 3 assertions'), runcmd=_RUNCMD)

def test_gmock_assign():
    db = read_db()
    cgen = CGen('main.c')
    cgen.append_line('#define CLDM_GMOCK_COMPAT')           \
        .append_include('cldm.h', system_header=False)      \
        .append_include('syms.h', system_header=False)

    with cgen.open_macro('TEST', 'foo'):
        cgen.append_line('int i = 10, j = 12;')                                         \
            .append_line('EXPECT_CALL(baz).WILL_REPEATEDLY(Assign(i, j));')             \
            .append_line('ASSERT_EQ(10, i);')                                           \
            .append_line('baz();')                                                      \
            .append_line('ASSERT_EQ(12, i);')                                           \
            .append_line('EXPECT_CALL(baz).WILL_REPEATEDLY(Assign(i, 13, int));')       \
            .append_line('baz();')                                                      \
            .append_line('ASSERT_EQ(13, i);')                                           \
            .append_line('EXPECT_CALL(baz).WILL_REPEATEDLY(Assign(i, (int){ 14 }));')   \
            .append_line('baz();')                                                      \
            .append_line('ASSERT_EQ(14, i);')
    cgen.write()

    cgen = CGen(_SYMBOL_TU)
    cgen.generate_matching_symbols()
    cgen.write()

    gen_makefile()

    run(ContainsMatcher('Successfully finished 4 assertions'), runcmd=_RUNCMD)

def test_gmock_assign_arg():
    db = read_db()
    cgen = CGen('main.c')
    cgen.append_line('#define CLDM_GMOCK_COMPAT')           \
        .append_include('cldm.h', system_header=False)      \
        .append_include('syms.h', system_header=False)

    with cgen.open_macro('TEST', 'foo'):
        cgen.append_line('int i = 10;')                                         \
            .append_line('EXPECT_CALL(foo).WILL_REPEATEDLY(AssignArg(0, i));')  \
            .append_line('ASSERT_EQ(i, 10);')                                   \
            .append_line('foo(12, 13);')                                        \
            .append_line('ASSERT_EQ(i, 12);')
    cgen.write()

    cgen = CGen(_SYMBOL_TU)
    cgen.generate_matching_symbols()
    cgen.write()

    gen_makefile()

    run(ContainsMatcher('Successfully finished 2 assertions'), runcmd=_RUNCMD)
