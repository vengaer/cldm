from cgen import *
from cldm import *
from config import *
from makegen import *
from symbols import *
from util import *

def test_readme_ex0():
    target = 'readme_test'
    symbol_tu = 'syms.c'

    db = read_db()
    cgen = CGen('main.c')
    cgen.append_include('cldm.h', system_header=False)  \
        .append_include('assert.h')                     \
        .append_include('stdlib.h')

    with cgen.open_function('int', 'main'):
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

def test_readme_ex1():
    target = 'readme_test'

    cgen = CGen(symfile)
    cgen.append_include('cldm.h', system_header=False)                          \
        .append_line('MOCK_FUNCTION(int *, get_resource, int);')
    cgen.write()

    cgen = CGen('resource.c')
    cgen.append_line('static int resource_array[32] = { 1, 2, 3 };')
    with cgen.open_function('int *', 'get_resource', ['int']):
        cgen.append_return('&resource_array[a0]')
    cgen.write()

    cgen = CGen('resource.h')
    cgen.append_line('int *get_resource(int a0);')                              \
        .write()

    cgen = CGen('main.c')
    cgen.append_include('cldm.h', system_header=False)                          \
        .append_include('resource.h', system_header=False)                      \
        .append_include('assert.h')
    with cgen.open_function('int', 'main'):
        cgen.append_line('int i = 12;')                                         \
            .append_line('EXPECT_CALL(get_resource).WILL_ONCE(RETURN(&i));')    \
            .append_line('int *res = get_resource(0);')                         \
            .append_line('assert(res == &i);')                                  \
            .append_return(0)
    cgen.write()

    assert build_cldm()[0] == 0
    assert exec_bash('gcc -shared -fPIC -o {d}/libresource.so {d}/resource.c'.format(d=working_dir))[0] == 0
    assert exec_bash('gcc -o {d}/a.out {d}/main.c -L{d} -L{root} -lresource -lcldm -I{root}/cldm'.format(d=working_dir, root=project_root))[0] == 0
    assert exec_bash('LD_PRELOAD={root}/libcldm.so LD_LIBRARY_PATH={wd} {wd}/a.out'.format(root=project_root, wd=working_dir))[0] == 0

def test_readme_ex2():
    target = 'readme_test'

    cgen = CGen(symfile)
    cgen.append_include('cldm.h', system_header=False)                          \
        .append_line('MOCK_FUNCTION(int *, get_resource, int);')
    cgen.write()

    cgen = CGen('resource.c')
    cgen.append_line('static int resource_array[32] = { 1, 2, 3 };')
    with cgen.open_function('int *', 'get_resource', ['int']):
        cgen.append_return('&resource_array[a0]')
    cgen.write()

    cgen = CGen('resource.h')
    cgen.append_line('int *get_resource(int a0);')                              \
        .write()

    cgen = CGen('main.c')
    cgen.append_include('cldm.h', system_header=False)                          \
        .append_include('resource.h', system_header=False)                      \
        .append_include('assert.h')
    with cgen.open_function('int', 'main'):
        cgen.append_line('int i = 12;')                                         \
            .append_line('EXPECT_CALL(get_resource).WILL_ONCE(RETURN(&i));')    \
            .append_line('int *res = get_resource(0);')                         \
            .append_line('assert(res == &i);')                                  \
            .append_return(0)
    cgen.write()

    assert build_cldm()[0] == 0
    assert exec_bash('gcc -o {d}/a.out {d}/main.c {d}/resource.c -L{root} -lcldm -I{root}/cldm'.format(d=working_dir, root=project_root))[0] == 0
    assert exec_bash('LD_PRELOAD={root}/libcldm.so LD_LIBRARY_PATH={wd} {wd}/a.out'.format(root=project_root, wd=working_dir))[0] != 0

def test_readme_ex3():
    target = 'readme_test'
    symbol_tu = 'syms.c'

    db = read_db()
    cgen = CGen('main.c')
    cgen.append_include('cldm.h', system_header=False)      \
        .append_include('syms.h', system_header=False)      \
        .append_include('stdio.h')                          \
        .append_include('string.h')

    with cgen.open_function('int', 'main'):
        cgen.append_line('long long i;')                                                        \
            .append_line('union { int as_int; char as_bytes[sizeof(long long)]; } u;')          \
            .append_line('memset(&u.as_bytes, 1, sizeof(u));')                                  \
            .append_line('u.as_int = 10;')                                                      \
            .append_line('EXPECT_CALL(baz).WILL_REPEATEDLY(ASSIGN(i, u.as_int));')              \
            .append_line('baz();')                                                              \
            .append_line('printf("%lld\\n", i);')                                               \
            .append_line('EXPECT_CALL(baz).WILL_REPEATEDLY(ASSIGN(i, u.as_int, long long));')   \
            .append_line('baz();')                                                              \
            .append_line('printf("%lld\\n", i);')
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
    output = output.decode('utf-8').strip().split('\n')
    assert output[0] != '10'
    assert output[1] == '10'
