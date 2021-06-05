from cgen import *
from cldm import *
from config import *
from makegen import *
from runner import *
from symbols import *
from util import *

_BINARY = 'readme_test'
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

def test_readme_example_usage():
    db = read_db()
    cgen = CGen('test.c')
    cgen.append_include('cldm.h', system_header=False)
    cgen.append_include('assert.h')
    cgen.append_include('stdlib.h')

    with cgen.open_macro('TEST', 'foo'):
        cgen.append_line('WHEN_CALLED(atoi).SHOULD_ONCE(RETURN(8));')
        cgen.append_line('assert(atoi("2") == 8);')
        cgen.append_line('assert(atoi("2") == 2);')
        cgen.append_line('WHEN_CALLED(atoi).SHOULD_REPEATEDLY(RETURN(7));')
        cgen.append_line('assert(atoi("2") == 7);')
        cgen.append_line('assert(atoi("6") == 7);')
    cgen.write()

    gen_symbols()
    gen_makefile()

    run(runcmd=_RUNCMD)

def test_readme_build():
    cgen = CGen(symfile)
    cgen.append_include('cldm.h', system_header=False)
    cgen.append_line('MOCK_FUNCTION(int *, get_resource, int);')
    cgen.write()

    cgen = CGen('resource.c')
    cgen.append_line('static int resource_array[32] = { 1, 2, 3 };')
    with cgen.open_function('int *', 'get_resource', ['int']):
        cgen.append_return('&resource_array[a0]')
    cgen.write()

    cgen = CGen('resource.h')
    cgen.append_line('int *get_resource(int a0);')
    cgen.write()

    cgen = CGen('test.c')
    cgen.append_include('cldm.h', system_header=False)
    cgen.append_include('resource.h', system_header=False)
    cgen.append_include('assert.h')
    with cgen.open_macro('TEST', 'foo'):
        cgen.append_line('int i = 12;')
        cgen.append_line('WHEN_CALLED(get_resource).SHOULD_ONCE(RETURN(&i));')
        cgen.append_line('int *res = get_resource(0);')
        cgen.append_line('assert(res == &i);')

    cgen.write()

    assert build_cldm()[0] == 0
    assert exec_bash('gcc -shared -fPIC -o {d}/libresource.so {d}/resource.c'.format(d=working_dir))[0] == 0
    assert exec_bash('gcc -o {d}/a.out {d}/test.c -L{d} -L{root} -lresource -lcldm -lcldm_main -I{root}/cldm'.format(d=working_dir, root=project_root))[0] == 0
    assert exec_bash('LD_PRELOAD={root}/libcldm.so LD_LIBRARY_PATH={wd} {wd}/a.out'.format(root=project_root, wd=working_dir))[0] == 0

def test_readme_incorrect_build():
    cgen = CGen(symfile)
    cgen.append_include('cldm.h', system_header=False)
    cgen.append_line('MOCK_FUNCTION(int *, get_resource, int);')
    cgen.write()

    cgen = CGen('resource.c')
    cgen.append_line('static int resource_array[32] = { 1, 2, 3 };')
    with cgen.open_function('int *', 'get_resource', ['int']):
        cgen.append_return('&resource_array[a0]')
    cgen.write()

    cgen = CGen('resource.h')
    cgen.append_line('int *get_resource(int a0);')
    cgen.write()

    cgen = CGen('test.c')
    cgen.append_include('cldm.h', system_header=False)
    cgen.append_include('resource.h', system_header=False)
    cgen.append_include('assert.h')
    with cgen.open_function('int', 'main'):
        cgen.append_line('int i = 12;')
        cgen.append_line('WHEN_CALLED(get_resource).SHOULD_ONCE(RETURN(&i));')
        cgen.append_line('int *res = get_resource(0);')
        cgen.append_line('assert(res == &i);')
        cgen.append_return(0)
    cgen.write()

    assert build_cldm()[0] == 0
    assert exec_bash('gcc -o {d}/a.out {d}/test.c {d}/resource.c -L{root} -lcldm -I{root}/cldm'.format(d=working_dir, root=project_root))[0] == 0
    assert exec_bash('LD_PRELOAD={root}/libcldm.so LD_LIBRARY_PATH={wd} {wd}/a.out'.format(root=project_root, wd=working_dir))[0] != 0

def test_readme_assign():
    db = read_db()
    cgen = CGen('test.c')
    cgen.append_include('cldm.h', system_header=False)
    cgen.append_include('syms.h', system_header=False)
    cgen.append_include('string.h')

    with cgen.open_macro('TEST', 'foo'):
        with cgen.open_union('uic'):
            cgen.append_line('int as_int;')
            cgen.append_line('char as_bytes[sizeof(long long)];')

        cgen.append_line('long long i;')
        cgen.append_line('union uic u;')
        cgen.append_line('memset(&u.as_bytes, 1, sizeof(u));')
        cgen.append_line('u.as_int = 10;')
        cgen.append_line('WHEN_CALLED(baz).SHOULD_REPEATEDLY(ASSIGN(i, u.as_int));')
        cgen.append_line('baz();')
        cgen.append_line('ASSERT_NE(i, 10);')
        cgen.append_line('WHEN_CALLED(baz).SHOULD_REPEATEDLY(ASSIGN(i, u.as_int, long long));')
        cgen.append_line('baz();')
        cgen.append_line('ASSERT_EQ(i, 10);')
    cgen.write()

    gen_symbols()
    gen_makefile()

    run(ContainsMatcher('Successfully finished 2 assertions'), runcmd=_RUNCMD)
