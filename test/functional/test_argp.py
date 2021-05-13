from cgen import *
from cldm import *
from config import *
from makegen import *
from runner import *
from util import *

_BINARY='argp_test'
_HELP_STR = 'cldm -- unit test and mocking framework'
_VER_STR = r'cldm -- version\s*[0-9.]+'

def gen_runcmd(flags):
    return 'LD_LIBRARY_PATH={} {}/{} {}'.format(project_root, working_dir, _BINARY, flags)

def gen_makefile():
    mgen = Makegen(_BINARY)
    mgen.adjust('CFLAGS', '-fPIC', Mod.REMOVE)
    mgen.adjust('LDFLAGS', '-shared', Mod.REMOVE)
    mgen.adjust('LDFLAGS', '-L{}'.format(project_root), Mod.APPEND)
    mgen.adjust('LDLIBS', '-lcldm -lcldm_main')
    mgen.export('LD_LIBRARY_PATH', project_root)
    mgen.generate()

def test_argp_short_help():
    cgen = CGen('tests.c')
    cgen.append_include('cldm.h', system_header=False)
    cgen.write()
    gen_makefile()

    runcmd = gen_runcmd('-h')
    run(ContainsMatcher(_HELP_STR), rvmatcher=RvEqMatcher(0), runcmd=runcmd)

    runcmd = gen_runcmd('-xh')
    run(ContainsMatcher(_HELP_STR), rvmatcher=RvEqMatcher(0), runcmd=runcmd)

    runcmd = gen_runcmd('-xh -- --help')
    run(ContainsMatcher(_HELP_STR), rvmatcher=RvEqMatcher(0), runcmd=runcmd)

    runcmd = gen_runcmd('-- -h')
    run(ContainsNotMatcher(_HELP_STR), rvmatcher=RvDiffMatcher(0), runcmd=runcmd)

def test_argp_short_version():
    cgen = CGen('tests.c')
    cgen.append_include('cldm.h', system_header=False)
    cgen.write()
    gen_makefile()

    runcmd = gen_runcmd('-V')
    run(ContainsMatcher(_VER_STR), rvmatcher=RvEqMatcher(0), runcmd=runcmd)

    runcmd = gen_runcmd('-xV')
    run(ContainsMatcher(_VER_STR), rvmatcher=RvEqMatcher(0), runcmd=runcmd)

    runcmd = gen_runcmd('-xV -- --help')
    run(ContainsMatcher(_VER_STR), rvmatcher=RvEqMatcher(0), runcmd=runcmd)

    runcmd = gen_runcmd('-- -V')
    run(ContainsNotMatcher(_VER_STR), rvmatcher=RvDiffMatcher(0), runcmd=runcmd)

def test_argp_short_fail_fast():
    cgen = CGen('tests.c')
    cgen.append_include('cldm.h', system_header=False)

    with cgen.open_macro('TEST', 'bar'):
        cgen.append_line('ASSERT_EQ(1, 2);')
    with cgen.open_macro('TEST', 'foo'):
        cgen.append_line('ASSERT_EQ(1, 1);')

    cgen.write()
    gen_makefile()

    runcmd = gen_runcmd('')
    run(ContainsMatcher(r'Running\s*foo'), rvmatcher=RvDiffMatcher(0), runcmd=runcmd)

    runcmd = gen_runcmd('-x')
    run(ContainsNotMatcher(r'Running\s*foo'), rvmatcher=RvDiffMatcher(0), runcmd=runcmd)

    runcmd = gen_runcmd('-x -- --help')
    run(ContainsNotMatcher(r'Running\s*foo'), rvmatcher=RvDiffMatcher(0), runcmd=runcmd)

    runcmd = gen_runcmd('-- -x')
    run(ContainsNotMatcher(r'Running\s*foo'), rvmatcher=RvDiffMatcher(0), runcmd=runcmd)

def test_argp_short_capture_none():
    cgen = CGen('tests.c')
    cgen.append_include('cldm.h', system_header=False)  \
        .append_include('stdio.h')

    with cgen.open_macro('TEST', 'bar'):
        cgen.append_line('puts("text");')  \
            .append_line('ASSERT_EQ(1, 1);')
    cgen.write()
    gen_makefile()

    runcmd = gen_runcmd('')
    run(ContainsMatcher(r'Running\s*bar.*pass.*text'), rvmatcher=RvEqMatcher(0), runcmd=runcmd)

    runcmd = gen_runcmd('-s')
    run(ContainsMatcher(r'Running\s*bar.*text.*pass'), rvmatcher=RvEqMatcher(0), runcmd=runcmd)

    runcmd = gen_runcmd('-s -- --help')
    run(ContainsNotMatcher(r'Running\s*bar.*text.*pass'), rvmatcher=RvDiffMatcher(0), runcmd=runcmd)

    runcmd = gen_runcmd('-- -s')
    run(ContainsNotMatcher(r'Running\s*bar.*pass.*text'), rvmatcher=RvDiffMatcher(0), runcmd=runcmd)

def test_argp_short_capture_with_none():
    cgen = CGen('tests.c')
    cgen.append_include('cldm.h', system_header=False)  \
        .append_include('stdio.h')

    with cgen.open_macro('TEST', 'bar'):
        cgen.append_line('puts("text");')               \
            .append_line('fputs("stderr\\n", stderr);') \
            .append_line('ASSERT_EQ(1, 0);')
    cgen.write()
    gen_makefile()

    runcmd = gen_runcmd('-c none')
    run(ContainsMatcher(r'Running\s*bar.*text.*fail'), ContainsMatcher('stderr'), RvDiffMatcher(0), runcmd=runcmd)

    runcmd = gen_runcmd('-cnone')
    run(ContainsMatcher(r'Running\s*bar.*text.*fail'), ContainsMatcher('stderr'), RvDiffMatcher(0), runcmd=runcmd)

    runcmd = gen_runcmd('-- -cnone')
    run(ContainsNotMatcher(r'Running\s*bar.*text.*fail'), rvmatcher=RvDiffMatcher(0), runcmd=runcmd)

def test_argp_short_capture_with_stdout():
    cgen = CGen('tests.c')
    cgen.append_include('cldm.h', system_header=False)  \
        .append_include('stdio.h')

    with cgen.open_macro('TEST', 'bar'):
        cgen.append_line('puts("text");')               \
            .append_line('fputs("stderr\\n", stderr);') \
            .append_line('ASSERT_EQ(1, 0);')
    cgen.write()
    gen_makefile()

    runcmd = gen_runcmd('-c stdout')
    run(ContainsMatcher(r'Running\s*bar.*fail.*text'), ContainsMatcher('stderr'), RvDiffMatcher(0), runcmd=runcmd)

    runcmd = gen_runcmd('-cstdout')
    run(ContainsMatcher(r'Running\s*bar.*fail.*text'), ContainsMatcher('stderr'), RvDiffMatcher(0), runcmd=runcmd)

    runcmd = gen_runcmd('-- -cstdout')
    run(ContainsNotMatcher(r'Running\s*bar.*fail.*text'), rvmatcher=RvDiffMatcher(0), runcmd=runcmd)

def test_argp_short_capture_with_all():
    cgen = CGen('tests.c')
    cgen.append_include('cldm.h', system_header=False)  \
        .append_include('stdio.h')

    with cgen.open_macro('TEST', 'bar'):
        cgen.append_line('puts("text");')               \
            .append_line('fputs("stderr\\n", stderr);') \
            .append_line('ASSERT_EQ(1, 0);')
    cgen.write()
    gen_makefile()

    runcmd = gen_runcmd('-c all')
    run(ContainsMatcher(r'Running\s*bar.*fail.*text'), ContainsMatcher('stderr'), RvDiffMatcher(0), runcmd=runcmd)

    runcmd = gen_runcmd('-call')
    run(ContainsMatcher(r'Running\s*bar.*fail.*text'), ContainsMatcher('stderr'), RvDiffMatcher(0), runcmd=runcmd)

    runcmd = gen_runcmd('-- -call')
    run(ContainsNotMatcher(r'Running\s*bar.*fail.*text'), rvmatcher=RvDiffMatcher(0), runcmd=runcmd)

def test_argp_long_help():
    cgen = CGen('tests.c')
    cgen.append_include('cldm.h', system_header=False)
    cgen.write()
    gen_makefile()

    runcmd = gen_runcmd('--help')
    run(ContainsMatcher(_HELP_STR), rvmatcher=RvEqMatcher(0), runcmd=runcmd)

    runcmd = gen_runcmd('-x --fail-fast --help')
    run(ContainsMatcher(_HELP_STR), rvmatcher=RvEqMatcher(0), runcmd=runcmd)

    runcmd = gen_runcmd('-x --help --')
    run(ContainsMatcher(_HELP_STR), rvmatcher=RvEqMatcher(0), runcmd=runcmd)

    runcmd = gen_runcmd('-x -- --help')
    run(ContainsNotMatcher(_HELP_STR), rvmatcher=RvDiffMatcher(0), runcmd=runcmd)

def test_argp_long_usage():
    cgen = CGen('tests.c')
    cgen.append_include('cldm.h', system_header=False)
    cgen.write()
    gen_makefile()

    runcmd = gen_runcmd('--usage')
    run(ContainsMatcher(_HELP_STR), rvmatcher=RvEqMatcher(0), runcmd=runcmd)

    runcmd = gen_runcmd('-x --fail-fast --usage')
    run(ContainsMatcher(_HELP_STR), rvmatcher=RvEqMatcher(0), runcmd=runcmd)

    runcmd = gen_runcmd('-x --usage --')
    run(ContainsMatcher(_HELP_STR), rvmatcher=RvEqMatcher(0), runcmd=runcmd)

    runcmd = gen_runcmd('-x -- --usage')
    run(ContainsNotMatcher(_HELP_STR), rvmatcher=RvDiffMatcher(0), runcmd=runcmd)

def test_argp_long_version():
    cgen = CGen('tests.c')
    cgen.append_include('cldm.h', system_header=False)
    cgen.write()
    gen_makefile()

    runcmd = gen_runcmd('--version')
    run(ContainsMatcher(_VER_STR), rvmatcher=RvEqMatcher(0), runcmd=runcmd)

    runcmd = gen_runcmd('-x --version')
    run(ContainsMatcher(_VER_STR), rvmatcher=RvEqMatcher(0), runcmd=runcmd)

    runcmd = gen_runcmd('-x --version --')
    run(ContainsMatcher(_VER_STR), rvmatcher=RvEqMatcher(0), runcmd=runcmd)

    runcmd = gen_runcmd('-- --version')
    run(ContainsNotMatcher(_VER_STR), rvmatcher=RvDiffMatcher(0), runcmd=runcmd)

def test_argp_long_fail_fast():
    cgen = CGen('tests.c')
    cgen.append_include('cldm.h', system_header=False)

    with cgen.open_macro('TEST', 'bar'):
        cgen.append_line('ASSERT_EQ(1, 2);')
    with cgen.open_macro('TEST', 'foo'):
        cgen.append_line('ASSERT_EQ(1, 1);')

    cgen.write()
    gen_makefile()

    runcmd = gen_runcmd('')
    run(ContainsMatcher(r'Running\s*foo'), rvmatcher=RvDiffMatcher(0), runcmd=runcmd)

    runcmd = gen_runcmd('--fail-fast')
    run(ContainsNotMatcher(r'Running\s*foo'), rvmatcher=RvDiffMatcher(0), runcmd=runcmd)

    runcmd = gen_runcmd('--fail-fast --')
    run(ContainsNotMatcher(r'Running\s*foo'), rvmatcher=RvDiffMatcher(0), runcmd=runcmd)

    runcmd = gen_runcmd('-- --fail-fast')
    run(ContainsNotMatcher(r'Running\s*foo'), rvmatcher=RvDiffMatcher(0), runcmd=runcmd)

def test_argp_long_capture_none():
    cgen = CGen('tests.c')
    cgen.append_include('cldm.h', system_header=False)  \
        .append_include('stdio.h')

    with cgen.open_macro('TEST', 'bar'):
        cgen.append_line('puts("text");')  \
            .append_line('ASSERT_EQ(1, 1);')
    cgen.write()
    gen_makefile()

    runcmd = gen_runcmd('')
    run(ContainsMatcher(r'Running\s*bar.*pass.*text'), rvmatcher=RvEqMatcher(0), runcmd=runcmd)

    runcmd = gen_runcmd('--capture-none')
    run(ContainsMatcher(r'Running\s*bar.*text.*pass'), rvmatcher=RvEqMatcher(0), runcmd=runcmd)

    runcmd = gen_runcmd('--capture-none --')
    run(ContainsMatcher(r'Running\s*bar.*text.*pass'), rvmatcher=RvEqMatcher(0), runcmd=runcmd)

    runcmd = gen_runcmd('-- --capture-none')
    run(ContainsNotMatcher(r'Running\s*bar.*pass.*text'), rvmatcher=RvDiffMatcher(0), runcmd=runcmd)

def test_argp_long_capture_with_none():
    cgen = CGen('tests.c')
    cgen.append_include('cldm.h', system_header=False)  \
        .append_include('stdio.h')

    with cgen.open_macro('TEST', 'bar'):
        cgen.append_line('puts("text");')               \
            .append_line('fputs("stderr\\n", stderr);') \
            .append_line('ASSERT_EQ(1, 0);')
    cgen.write()
    gen_makefile()

    runcmd = gen_runcmd('--capture=none')
    run(ContainsMatcher(r'Running\s*bar.*text.*fail'), ContainsMatcher('stderr'), RvDiffMatcher(0), runcmd=runcmd)

    runcmd = gen_runcmd('-- --capture=none')
    run(ContainsNotMatcher(r'Running\s*bar.*text.*fail'), rvmatcher=RvDiffMatcher(0), runcmd=runcmd)

def test_argp_long_capture_with_stdout():
    cgen = CGen('tests.c')
    cgen.append_include('cldm.h', system_header=False)  \
        .append_include('stdio.h')

    with cgen.open_macro('TEST', 'bar'):
        cgen.append_line('puts("text");')               \
            .append_line('fputs("stderr\\n", stderr);') \
            .append_line('ASSERT_EQ(1, 0);')
    cgen.write()
    gen_makefile()

    runcmd = gen_runcmd('--capture=stdout')
    run(ContainsMatcher(r'Running\s*bar.*fail.*text'), ContainsMatcher('stderr'), RvDiffMatcher(0), runcmd=runcmd)

    runcmd = gen_runcmd('-- --capture=stdout')
    run(ContainsNotMatcher(r'Running\s*bar.*fail.*text'), rvmatcher=RvDiffMatcher(0), runcmd=runcmd)

def test_argp_long_capture_with_all():
    cgen = CGen('tests.c')
    cgen.append_include('cldm.h', system_header=False)  \
        .append_include('stdio.h')

    with cgen.open_macro('TEST', 'bar'):
        cgen.append_line('puts("text");')               \
            .append_line('fputs("stderr\\n", stderr);') \
            .append_line('ASSERT_EQ(1, 0);')
    cgen.write()
    gen_makefile()

    runcmd = gen_runcmd('--capture=all')
    run(ContainsMatcher(r'Running\s*bar.*fail.*text'), ContainsMatcher('stderr'), RvDiffMatcher(0), runcmd=runcmd)

    runcmd = gen_runcmd('-- --capture=all')
    run(ContainsNotMatcher(r'Running\s*bar.*fail.*text'), rvmatcher=RvDiffMatcher(0), runcmd=runcmd)

def test_argp_long_switch_without_value():
    cgen = CGen('tests.c')
    cgen.append_include('cldm.h', system_header=False)  \
        .append_include('stdio.h')

    with cgen.open_macro('TEST', 'bar'):
        cgen.append_line('puts("text");')               \
            .append_line('fputs("stderr\\n", stderr);') \
            .append_line('ASSERT_EQ(1, 0);')
    cgen.write()
    gen_makefile()

    runcmd = gen_runcmd('--capture= all')
    run(stderr_matcher=ContainsMatcher(r'--capture requires an argument'), rvmatcher=RvDiffMatcher(0), runcmd=runcmd)

    runcmd = gen_runcmd('--capture=')
    run(stderr_matcher=ContainsNotMatcher(r'--capture requires and argument'), rvmatcher=RvDiffMatcher(0), runcmd=runcmd)

def test_argp_positional_param_invocation():
    cgen = CGen('bar_test.c')
    cgen.append_include('cldm.h', system_header=False)

    with cgen.open_macro('TEST', 'bar'):
        cgen.append_line('ASSERT_EQ(1, 1);')
    cgen.write()

    cgen = CGen('foo_test.c')
    cgen.append_include('cldm.h', system_header=False)

    with cgen.open_macro('TEST', 'foo'):
        cgen.append_line('ASSERT_EQ(2, 2);')
    cgen.write()
    gen_makefile()

    runcmd = gen_runcmd('')
    run(ContainsMatcher(r'Running\s*bar.*Running\s*foo'), rvmatcher=RvEqMatcher(0), runcmd=runcmd)

    runcmd = gen_runcmd('bar_test.c')
    run(ContainsNotMatcher(r'Running\s*foo'), rvmatcher=RvEqMatcher(0), runcmd=runcmd)
    run(ContainsMatcher(r'Running\s*bar'), rvmatcher=RvEqMatcher(0), runcmd=runcmd)

def test_argp_positional_param_all_tests():
    cgen = CGen('bar_test.c')
    cgen.append_include('cldm.h', system_header=False)

    with cgen.open_macro('TEST', 'bar'):
        cgen.append_line('ASSERT_EQ(1, 1);')
    with cgen.open_macro('TEST', 'baz'):
        cgen.append_line('ASSERT_TRUE(1);')
    cgen.write()

    cgen = CGen('foo_test.c')
    cgen.append_include('cldm.h', system_header=False)

    with cgen.open_macro('TEST', 'foo'):
        cgen.append_line('ASSERT_EQ(2, 2);')
    cgen.write()

    cgen = CGen('gen_test.c')
    cgen.append_include('cldm.h', system_header=False)

    with cgen.open_macro('TEST', 'virginti'):
        cgen.append_line('ASSERT_EQ(1,1);')
    cgen.write()
    gen_makefile()

    runcmd = gen_runcmd('gen_test.c bar_test.c foo_test.c')
    run(ContainsMatcher(r'Collected\s*4\s*tests.*Running\s*virginti.*Running\s*bar.*Running\s*baz.*Running\s*foo'), rvmatcher=RvEqMatcher(0), runcmd=runcmd)

def test_argp_positional_param_fail_fast():
    cgen = CGen('test.c')
    cgen.append_include('cldm.h', system_header=False)

    with cgen.open_macro('TEST', 'bar'):
        cgen.append_line('ASSERT_EQ(1, 2);')
    with cgen.open_macro('TEST', 'foo'):
        cgen.append_line('ASSERT_TRUE(1);')
    cgen.write()
    cgen = CGen('goo_test.c')
    cgen.append_include('cldm.h', system_header=False)

    with cgen.open_macro('TEST', 'goo'):
        cgen.append_line('ASSERT_EQ(2, 2);')
    cgen.write()

    gen_makefile()

    runcmd = gen_runcmd('-x test.c')
    run(ContainsMatcher(r'Running\s*bar'), rvmatcher=RvDiffMatcher(0), runcmd=runcmd)
    run(ContainsNotMatcher(r'Running\s*foo.*Running\s*goo'), rvmatcher=RvDiffMatcher(0), runcmd=runcmd)

def test_argp_positional_param_order_override():
    cgen = CGen('bar_test.c')
    cgen.append_include('cldm.h', system_header=False)

    with cgen.open_macro('TEST', 'bar'):
        cgen.append_line('ASSERT_EQ(1, 1);')
    with cgen.open_macro('TEST', 'baz'):
        cgen.append_line('ASSERT_TRUE(1);')
    cgen.write()

    cgen = CGen('foo_test.c')
    cgen.append_include('cldm.h', system_header=False)

    with cgen.open_macro('TEST', 'foo'):
        cgen.append_line('ASSERT_EQ(2, 2);')
    cgen.write()

    cgen = CGen('gen_test.c')
    cgen.append_include('cldm.h', system_header=False)

    with cgen.open_macro('TEST', 'virginti'):
        cgen.append_line('ASSERT_EQ(1,1);')
    cgen.write()
    gen_makefile()

    runcmd = gen_runcmd('gen_test.c bar_test.c foo_test.c')
    run(ContainsMatcher(r'Running\s*virginti.*Running\s*bar.*Running\s*baz.*Running\s*foo'), rvmatcher=RvEqMatcher(0), runcmd=runcmd)

    runcmd = gen_runcmd('foo_test.c gen_test.c bar_test.c')
    run(ContainsMatcher(r'Running\s*foo.*Running\s*virginti.*Running\s*bar.*Running\s*baz'), rvmatcher=RvEqMatcher(0), runcmd=runcmd)

def test_argp_invalid_positional_param():
    file = 'baz.c'
    cgen = CGen('test.c')
    cgen.append_include('cldm.h', system_header=False)

    with cgen.open_macro('TEST', 'bar'):
        cgen.append_line('ASSERT_EQ(1, 2);')
    with cgen.open_macro('TEST', 'foo'):
        cgen.append_line('ASSERT_TRUE(1);')
    cgen.write()

    gen_makefile()

    runcmd = gen_runcmd(file)
    run(DummyMatcher(), stderr_matcher=ContainsMatcher(r"File '{}' not found".format(file)), rvmatcher=RvDiffMatcher(0), runcmd=runcmd)
