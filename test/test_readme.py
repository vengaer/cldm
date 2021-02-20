from cgen import *
from config import *
from makegen import *
from symbols import *
from util import *

def test_readme_ex0():
    target = 'readme_test'
    symbol_tu = 'syms.c'

    db = read_db()
    cgen = CGen('main.c')
    cgen.append_include('cmock.h', system_header=False) \
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

def test_readme_ex1():
    target = 'readme_test'

    cgen = CGen(symfile)
    cgen.append_include('cmock.h', system_header=False)                         \
        .append_line('MOCK_FUNCTION(int *, get_resource, int);')
    cgen.write()

    cgen = CGen('resource.c')
    cgen.append_line('static int resource_array[32] = { 1, 2, 3 };')
    with cgen.with_open_function('int *', 'get_resource', ['int']):
        cgen.append_return('&resource_array[a0]')
    cgen.write()

    cgen = CGen('resource.h')
    cgen.append_line('int *get_resource(int a0);')                              \
        .write()

    cgen = CGen('main.c')
    cgen.append_include('cmock.h', system_header=False)                         \
        .append_include('resource.h', system_header=False)                      \
        .append_include('assert.h')
    with cgen.with_open_function('int', 'main'):
        cgen.append_line('int i = 12;')                                         \
            .append_line('EXPECT_CALL(get_resource).WILL_ONCE(RETURN(&i));')    \
            .append_line('int *res = get_resource(0);')                         \
            .append_line('assert(res == &i);')                                  \
            .append_return(0)
    cgen.write()

    assert exec_bash('make -B -C {}'.format(project_root))[0] == 0
    assert exec_bash('gcc -shared -fPIC -o {d}/libresource.so {d}/resource.c'.format(d=working_dir))[0] == 0
    assert exec_bash('gcc -o {d}/a.out {d}/main.c -L{d} -L{root} -lresource -lcmock -I{root}/cmock'.format(d=working_dir, root=project_root))[0] == 0
    assert exec_bash('LD_PRELOAD={root}/libcmock.so LD_LIBRARY_PATH={wd} {wd}/a.out'.format(root=project_root, wd=working_dir))[0] == 0

def test_readme_ex2():
    target = 'readme_test'

    cgen = CGen(symfile)
    cgen.append_include('cmock.h', system_header=False)                         \
        .append_line('MOCK_FUNCTION(int *, get_resource, int);')
    cgen.write()

    cgen = CGen('resource.c')
    cgen.append_line('static int resource_array[32] = { 1, 2, 3 };')
    with cgen.with_open_function('int *', 'get_resource', ['int']):
        cgen.append_return('&resource_array[a0]')
    cgen.write()

    cgen = CGen('resource.h')
    cgen.append_line('int *get_resource(int a0);')                              \
        .write()

    cgen = CGen('main.c')
    cgen.append_include('cmock.h', system_header=False)                         \
        .append_include('resource.h', system_header=False)                      \
        .append_include('assert.h')
    with cgen.with_open_function('int', 'main'):
        cgen.append_line('int i = 12;')                                         \
            .append_line('EXPECT_CALL(get_resource).WILL_ONCE(RETURN(&i));')    \
            .append_line('int *res = get_resource(0);')                         \
            .append_line('assert(res == &i);')                                  \
            .append_return(0)
    cgen.write()

    assert exec_bash('make -B -C {}'.format(project_root))[0] == 0
    assert exec_bash('gcc -o {d}/a.out {d}/main.c {d}/resource.c -L{d} -L{root} -lcmock -I{root}/cmock'.format(d=working_dir, root=project_root))[0] == 0
    assert exec_bash('LD_PRELOAD={root}/libcmock.so LD_LIBRARY_PATH={wd} {wd}/a.out'.format(root=project_root, wd=working_dir))[0] != 0
