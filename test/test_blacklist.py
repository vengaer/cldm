from cgen import *
from config import *
from makegen import *
from runner import *
from util import *

__TARGET = 'blacklist_test'

def gen_makefile():
    mgen = Makegen(__TARGET, src='main.c')
    mgen.adjust('CFLAGS', '-fPIC', Mod.REMOVE)
    mgen.adjust('LDFLAGS', '-shared', Mod.REMOVE)
    mgen.adjust('LDFLAGS', '-L{}'.format(project_root), Mod.APPEND)
    mgen.adjust('LDLIBS', '-lcldm')
    mgen.export('LD_LIBRARY_PATH', project_root)
    mgen.generate()

def test_blacklist():
    blacklist = ['dlopen',
                 'dlclose',
                 'dlerror',
                 'dlsym'
    ]
    non_blacklisted = ['fprintf',
                       'exit',
                       'atoi'
    ]
    cgen = CGen('main.c')
    cgen.append_include('cldm_blacklist.h', system_header=False) \
        .append_include('stdio.h')

    with cgen.open_function('int', 'main'):
        for fn in blacklist:
            cgen.append_line('printf("%d\\n", cldm_is_blacklisted("{}"));'.format(fn))
        for fn in non_blacklisted:
            cgen.append_line('printf("%d\\n", cldm_is_blacklisted("{}"));'.format(fn))

    cgen.write()
    gen_makefile()

    bl = '1 ' * len(blacklist) + '0 ' * len(non_blacklisted)
    expected = bl.strip().split(' ')

    run(EqMatcher(expected))
