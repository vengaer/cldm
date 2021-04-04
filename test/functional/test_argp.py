from cgen import *
from cldm import *
from config import *
from makegen import *
from runner import *
from util import *

_BINARY='argp_test'

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
    run(ContainsMatcher(r'{}\s*(\[-.\|--.*\]\s*)+.*\[FILE\]\.\.\.'.format(_BINARY)), rvmatcher=RvEqMatcher(0), runcmd=runcmd)

    runcmd = gen_runcmd('-xh')
    run(ContainsMatcher(r'{}\s*(\[-.\|--.*\]\s*)+.*\[FILE\]\.\.\.'.format(_BINARY)), rvmatcher=RvEqMatcher(0), runcmd=runcmd)

    runcmd = gen_runcmd('-xh -- --help')
    run(ContainsMatcher(r'{}\s*(\[-.\|--.*\]\s*)+.*\[FILE\]\.\.\.'.format(_BINARY)), rvmatcher=RvEqMatcher(0), runcmd=runcmd)

    runcmd = gen_runcmd('-- -h')
    run(ContainsNotMatcher(r'{}\s*(\[-.\|--.*\]\s*)+.*\[FILE\]\.\.\.'.format(_BINARY)), rvmatcher=RvEqMatcher(0), runcmd=runcmd)

def test_argp_short_version():
    cgen = CGen('tests.c')
    cgen.append_include('cldm.h', system_header=False)
    cgen.write()
    gen_makefile()

    runcmd = gen_runcmd('-V')
    run(ContainsMatcher(r'cldm\s*version\s*[0-9.]+'.format(_BINARY)), rvmatcher=RvEqMatcher(0), runcmd=runcmd)

    runcmd = gen_runcmd('-xV')
    run(ContainsMatcher(r'cldm\s*version\s*[0-9.]+'.format(_BINARY)), rvmatcher=RvEqMatcher(0), runcmd=runcmd)

    runcmd = gen_runcmd('-xV -- --help')
    run(ContainsMatcher(r'cldm\s*version\s*[0-9.]+'.format(_BINARY)), rvmatcher=RvEqMatcher(0), runcmd=runcmd)

    runcmd = gen_runcmd('-- -V')
    run(ContainsNotMatcher(r'cldm\s*version\s*[0-9.]+'.format(_BINARY)), rvmatcher=RvEqMatcher(0), runcmd=runcmd)

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
    run(ContainsMatcher(r'Running\s*foo'), rvmatcher=RvDiffMatcher(0), runcmd=runcmd)

def test_argp_long_help():
    cgen = CGen('tests.c')
    cgen.append_include('cldm.h', system_header=False)
    cgen.write()
    gen_makefile()

    runcmd = gen_runcmd('--help')
    run(ContainsMatcher(r'{}\s*(\[-.\|--.*\]\s*)+.*\[FILE\]\.\.\.'.format(_BINARY)), rvmatcher=RvEqMatcher(0), runcmd=runcmd)

    runcmd = gen_runcmd('-x --fail-fast --help')
    run(ContainsMatcher(r'{}\s*(\[-.\|--.*\]\s*)+.*\[FILE\]\.\.\.'.format(_BINARY)), rvmatcher=RvEqMatcher(0), runcmd=runcmd)

    runcmd = gen_runcmd('-x --help -- --help')
    run(ContainsMatcher(r'{}\s*(\[-.\|--.*\]\s*)+.*\[FILE\]\.\.\.'.format(_BINARY)), rvmatcher=RvEqMatcher(0), runcmd=runcmd)

    runcmd = gen_runcmd('-x -- --help')
    run(ContainsNotMatcher(r'{}\s*(\[-.\|--.*\]\s*)+.*\[FILE\]\.\.\.'.format(_BINARY)), rvmatcher=RvEqMatcher(0), runcmd=runcmd)

def test_argp_long_version():
    cgen = CGen('tests.c')
    cgen.append_include('cldm.h', system_header=False)
    cgen.write()
    gen_makefile()

    runcmd = gen_runcmd('--version')
    run(ContainsMatcher(r'cldm\s*version\s*[0-9.]+'.format(_BINARY)), rvmatcher=RvEqMatcher(0), runcmd=runcmd)

    runcmd = gen_runcmd('-x --version')
    run(ContainsMatcher(r'cldm\s*version\s*[0-9.]+'.format(_BINARY)), rvmatcher=RvEqMatcher(0), runcmd=runcmd)

    runcmd = gen_runcmd('-x --version -- --help')
    run(ContainsMatcher(r'cldm\s*version\s*[0-9.]+'.format(_BINARY)), rvmatcher=RvEqMatcher(0), runcmd=runcmd)

    runcmd = gen_runcmd('-- --version')
    run(ContainsNotMatcher(r'cldm\s*version\s*[0-9.]+'.format(_BINARY)), rvmatcher=RvEqMatcher(0), runcmd=runcmd)

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

    runcmd = gen_runcmd('--fail-fast -- --help')
    run(ContainsNotMatcher(r'Running\s*foo'), rvmatcher=RvDiffMatcher(0), runcmd=runcmd)

    runcmd = gen_runcmd('-- --fail-fast')
    run(ContainsMatcher(r'Running\s*foo'), rvmatcher=RvDiffMatcher(0), runcmd=runcmd)
