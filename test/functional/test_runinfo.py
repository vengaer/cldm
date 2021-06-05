from cgen import *
from cldm import *
from config import *
from makegen import *
from runner import *
from util import *

_BINARY='runinfo_test'

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

def test_runinfo_setup():
    cgen = CGen('tests.c')
    cgen.append_include('cldm.h', system_header=False)
    cgen.append_include('stdio.h')

    with cgen.open_macro('UNBOUND_SETUP', 'ubsetup'):
        cgen.append_line('puts("Unbound setup");')

    with cgen.open_macro('TEST', [ 'foo', '{ .setup = ubsetup }' ]):
        cgen.append_line('puts("Test");')

    cgen.write()
    gen_makefile()

    run(ContainsMatcher(r'Unbound setup.*Test'), rvmatcher=RvEqMatcher(0))

def test_runinfo_teardown():
    cgen = CGen('tests.c')
    cgen.append_include('cldm.h', system_header=False)
    cgen.append_include('stdio.h')

    with cgen.open_macro('UNBOUND_TEARDOWN', 'ubteardown'):
        cgen.append_line('puts("Unbound teardown");')

    with cgen.open_macro('TEST', [ 'foo', '{ .teardown = ubteardown }' ]):
        cgen.append_line('puts("Test");')

    cgen.write()
    gen_makefile()

    run(ContainsMatcher(r'Test.*Unbound teardown'), rvmatcher=RvEqMatcher(0))

def test_runinfo_setup_teardown():
    cgen = CGen('tests.c')
    cgen.append_include('cldm.h', system_header=False)
    cgen.append_include('stdio.h')

    with cgen.open_macro('UNBOUND_SETUP', 'ubsetup'):
        cgen.append_line('puts("Unbound setup");')

    with cgen.open_macro('UNBOUND_TEARDOWN', 'ubteardown'):
        cgen.append_line('puts("Unbound teardown");')

    with cgen.open_macro('TEST', [ 'foo', '{ .setup = ubsetup, .teardown = ubteardown }' ]):
        cgen.append_line('puts("Test");')

    cgen.write()
    gen_makefile()

    run(ContainsMatcher(r'Unbound setup.*Test.*Unbound teardown'), rvmatcher=RvEqMatcher(0))
